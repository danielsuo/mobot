/*
% install lOSMesa
% compile
% mex WarpMeshColor.cpp -lGLU -lOSMesa
% or
% mex WarpMeshColor.cpp -lGLU -lOSMesa -I/media/Data/usr/Mesa-9.1.2/include
% on mac:
% mex WarpMeshColor.cpp -lGLU -lOSMesa -I/opt/X11/include/ -L/opt/X11/lib/
*/

/*

This code is to render a Mesh given a 3x4 camera matrix with an image resolution widthxheight. The rendering result is an ID map for facets, edges and vertices. This can usually used for occlusion testing in texture mapping a model from an image, such as the texture mapping in the following two papers.

--Jianxiong Xiao http://mit.edu/jxiao/

Citation:

[1] J. Xiao, T. Fang, P. Zhao, M. Lhuillier, and L. Quan
Image-based Street-side City Modeling
ACM Transaction on Graphics (TOG), Volume 28, Number 5
Proceedings of ACM SIGGRAPH Asia 2009

[2] J. Xiao, T. Fang, P. Tan, P. Zhao, E. Ofek, and L. Quan
Image-based Facade Modeling
ACM Transaction on Graphics (TOG), Volume 27, Number 5
Proceedings of ACM SIGGRAPH Asia 2008

*/

#include <string.h>
#include <math.h>

// Off-screen rendering
#include <GL/osmesa.h>

// GL utilities (e.g., coordinate mapping)
#include <GL/glu.h>

// MATLAB Executable
#include "mex.h" 

// Convert uint8 RGB 3-array into single uint
unsigned int uchar2uint(unsigned char* in){
  unsigned int out = (((unsigned int)(in[0])) << 16) + (((unsigned int)(in[1])) << 8) + ((unsigned int)(in[2]));
  return out;
}

// Convert a single uint into a uint8 RGB 3-array
void uint2uchar(unsigned int in, unsigned char* out){
  out[0] = (in & 0x00ff0000) >> 16;
  out[1] = (in & 0x0000ff00) >> 8;
  out[2] =  in & 0x000000ff;
}

/* Arguments:
 * - nlhs: number of left-hand side arguments (outputs)
 *   - arg0: 3x4 Projection matrix, 
 *   - arg1: width*height*4 double matrix,
 * - plhs: pointers to left-hand side arguments
 * - nrhs: number of right-hand side arguments (inputs)
 * - prhs: pointers to right-hand side arguments
 *
 * Return values:
 * - imageWarped: TODO you will need to transpose the result in Matlab manually. 
 *                see the demo
 * - depth TODO
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  // --------------------------------------------------------------------------
  // Step 0: initialize parameters and variables
  // --------------------------------------------------------------------------
  
  // Store arguments from Matlab
  double* projection = mxGetPr(prhs[0]);    // arg0: 3x4 Projection matrix, 
  double*     vertex = mxGetPr(prhs[1]);    // arg1: num_rows x num_cols x 4 double vertices matrix

  // Get number of rows and columns
  unsigned int  num_cols = mxGetN(prhs[1])/4;
  unsigned int  num_rows = mxGetM(prhs[1]);

  // --------------------------------------------------------------------------
  // Step 1: setup off-screen binding. See header file for more information
  // https://github.com/freedreno/mesa/blob/master/include/GL/osmesa.h
  // --------------------------------------------------------------------------  
  OSMesaContext ctx = OSMesaCreateContextExt(
    OSMESA_BGR,   /* format: Specifies the format of the pixel data. The
                     following symbolic values are accepted:
                     #define OSMESA_COLOR_INDEX GL_COLOR_INDEX
                     #define OSMESA_RGBA   GL_RGBA
                     #define OSMESA_BGRA   0x1
                     #define OSMESA_ARGB   0x2
                     #define OSMESA_RGB    GL_RGB
                     #define OSMESA_BGR    0x4
                     #define OSMESA_RGB_565    0x5

                     NOTE: strange hack not sure why it is not OSMESA_RGB
                  */
    32,           // depthBits: size of depth buffer
    0,            // stencilBits: size of stencil buffer
    0,            // aaccumBits: size of accumulation buffer

    NULL          // sharelist: OSMesaContext that specifies the context with
                  // which to share display lists. NULL indicates that no
                  // sharing is to take place.
  );

  // Inititalize imageWarp output buffer
  unsigned char * pbuffer = new unsigned char [3 * num_cols * num_rows];

  // Bind the buffer to the context and make it current
  if (!OSMesaMakeCurrent(
        ctx,                // ctx: Context to bind
        (void*)pbuffer,     // buffer: Buffer to bind to
        GL_UNSIGNED_BYTE,   // type: Data typefor pixel components
        num_cols,           // width: Width of buffer in pixels
        num_rows            // height: Height of buffer in pixels
      )) {
    mexErrMsgTxt("OSMesaMakeCurrent failed!: ");
  }

  // Y coordinates increase downward
  OSMesaPixelStore(OSMESA_Y_UP, 0);

  // --------------------------------------------------------------------------
  // Step 2: Setup basic OpenGL setting
  // --------------------------------------------------------------------------
  // Enable depth test: If enabled, do depth comparisons and update the depth
  // buffer. Note that even if the depth buffer exists and the depth mask is
  // non-zero, the depth buffer is not updated if the depth test is disabled.
  glEnable(GL_DEPTH_TEST);

  // Disable lighting: If enabled and no vertex shader is active, use the
  // current lighting parameters to compute the vertex color or index.
  // Otherwise, simply associate the current color or index with each vertex.
  glDisable(GL_LIGHTING);

  // Enable face culling: If enabled, cull polygons based on their winding in
  // window coordinates.
  glEnable(GL_CULL_FACE);

  // Cull back face
  glCullFace(GL_BACK);

  // Rasterize polygons by filling front-facing polygons
  glPolygonMode(GL_FRONT, GL_FILL);

  // Clear buffer to values set by glClearColor, glClearDepthf, and glClearStencil
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Create the viewport
  glViewport(0, 0, num_cols, num_rows);

  // --------------------------------------------------------------------------
  // Step 3: Set projection matrices
  // --------------------------------------------------------------------------
  double scale = 1.0;
  double final_matrix[16];

  // Set projection parameters
  float m_near = 0.3; // TODO
  float m_far  = 1e8;  // TODO

  // new way: faster way by reuse computation and symbolic derive. See
  // sym_derive.m to check the math.
  double inv_width_scale  = 1.0/(num_cols*scale);
  double inv_height_scale = 1.0/(num_rows*scale);
  double inv_width_scale_1 =inv_width_scale - 1.0;
  double inv_height_scale_1_s = -(inv_height_scale - 1.0);
  double inv_width_scale_2 = inv_width_scale*2.0;
  double inv_height_scale_2_s = -inv_height_scale*2.0;
  double m_far_a_m_near = m_far + m_near;
  double m_far_s_m_near = m_far - m_near;
  double m_far_d_m_near = m_far_a_m_near/m_far_s_m_near;
  
  final_matrix[ 0]= projection[2+0*3]*inv_width_scale_1 + projection[0+0*3]*inv_width_scale_2;
  final_matrix[ 1]= projection[2+0*3]*inv_height_scale_1_s + projection[1+0*3]*inv_height_scale_2_s;
  final_matrix[ 2]= projection[2+0*3]*m_far_d_m_near;
  final_matrix[ 3]= projection[2+0*3];
  final_matrix[ 4]= projection[2+1*3]*inv_width_scale_1 + projection[0+1*3]*inv_width_scale_2;
  final_matrix[ 5]= projection[2+1*3]*inv_height_scale_1_s + projection[1+1*3]*inv_height_scale_2_s; 
  final_matrix[ 6]= projection[2+1*3]*m_far_d_m_near;    
  final_matrix[ 7]= projection[2+1*3];
  final_matrix[ 8]= projection[2+2*3]*inv_width_scale_1 + projection[0+2*3]*inv_width_scale_2; 
  final_matrix[ 9]= projection[2+2*3]*inv_height_scale_1_s + projection[1+2*3]*inv_height_scale_2_s;
  final_matrix[10]= projection[2+2*3]*m_far_d_m_near;
  final_matrix[11]= projection[2+2*3];
  final_matrix[12]= projection[2+3*3]*inv_width_scale_1 + projection[0+3*3]*inv_width_scale_2;
  final_matrix[13]= projection[2+3*3]*inv_height_scale_1_s + projection[1+3*3]*inv_height_scale_2_s;  
  final_matrix[14]= projection[2+3*3]*m_far_d_m_near - (2*m_far*m_near)/m_far_s_m_near;
  final_matrix[15]= projection[2+3*3];
  
  // matrix is ready. use it
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(final_matrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // --------------------------------------------------------------------------
  // Step 4: render the mesh with encoded color from their ID
  // --------------------------------------------------------------------------
  unsigned char colorBytes[3];
  
  unsigned int layerSize = num_cols* num_rows;
  
  double zThreshold = 0.1;
          
  for (unsigned int c = 0; c < num_cols-1; ++c) {
      
    double* x00 = vertex+ num_rows*c;   double* x01 = x00 + num_rows;
    double* x10 = x00 + 1;              double* x11 = x01 + 1;
    
    for (unsigned int r = 0; r < num_rows-1; ++r) {
        double* y00 = x00+layerSize;    double* y01 = x01+layerSize;
        double* y10 = x10+layerSize;    double* y11 = x11+layerSize;
        double* z00 = y00+layerSize;    double* z10 = y10+layerSize;
        double* z01 = y01+layerSize;    double* z11 = y11+layerSize;
        double* l00 = z00+layerSize;    double* l10 = z10+layerSize;
        double* l01 = z01+layerSize;    double* l11 = z11+layerSize;
                
        if (*z00 == 0.0){
            if (*z01 != 0.0 && *z10 != 0.0 && *z11 != 0.0 && fabs(*z01-*z10)<zThreshold && fabs(*z11-*z10)<zThreshold && fabs(*z01-*z11)<zThreshold ){
                glBegin(GL_TRIANGLES);
                uint2uchar(static_cast<unsigned int>(*l11),colorBytes);
                glColor3ubv(colorBytes);
                glVertex3d(*x11,*y11,*z11);
                uint2uchar(static_cast<unsigned int>(*l10),colorBytes);
                glColor3ubv(colorBytes);
                glVertex3d(*x10,*y10,*z10);
                uint2uchar(static_cast<unsigned int>(*l01),colorBytes);
                glColor3ubv(colorBytes);
                glVertex3d(*x01,*y01,*z01);
                glEnd();
            }
        }else{
            if (*z11 == 0.0){
                if (*z01!= 0.0 && *z10!= 0.0 && *z00!= 0.0 && fabs(*z00-*z01)<zThreshold && fabs(*z01-*z10)<zThreshold && fabs(*z10-*z00)<zThreshold ){
                    glBegin(GL_TRIANGLES);
                    uint2uchar(static_cast<unsigned int>(*l00),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x00,*y00,*z00);
                    uint2uchar(static_cast<unsigned int>(*l01),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x01,*y01,*z01);
                    uint2uchar(static_cast<unsigned int>(*l10),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x10,*y10,*z10);
                    glEnd();                   
                }
            }else{
                if (*z01!=0.0 && fabs(*z00-*z01)<zThreshold && fabs(*z01-*z11)<zThreshold && fabs(*z11-*z00)<zThreshold ){
                    glBegin(GL_TRIANGLES);
                    uint2uchar(static_cast<unsigned int>(*l00),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x00,*y00,*z00);
                    uint2uchar(static_cast<unsigned int>(*l01),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x01,*y01,*z01);
                    uint2uchar(static_cast<unsigned int>(*l11),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x11,*y11,*z11);
                    glEnd();                     
                }
                if (*z10!=0.0 && fabs(*z00-*z11)<zThreshold && fabs(*z11-*z10)<zThreshold && fabs(*z10-*z00)<zThreshold ){
                    glBegin(GL_TRIANGLES);
                    uint2uchar(static_cast<unsigned int>(*l00),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x00,*y00,*z00);
                    uint2uchar(static_cast<unsigned int>(*l11),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x11,*y11,*z11);
                    uint2uchar(static_cast<unsigned int>(*l10),colorBytes);
                    glColor3ubv(colorBytes);
                    glVertex3d(*x10,*y10,*z10);
                    glEnd();                     
                }
            }
        }
        
        // update
        x00 = x10++;
        x01 = x11++;
    }
  }

  glFinish(); // done rendering
 
  // --------------------------------------------------------------------------
  // Step 5: convert the result from color to interger array
  // --------------------------------------------------------------------------
  plhs[0] = mxCreateNumericMatrix(num_cols, num_rows, mxUINT32_CLASS, mxREAL);
  unsigned int* result = (unsigned int*) mxGetData(plhs[0]);

  unsigned int* resultCur = result;
  unsigned int* resultEnd = result + num_cols * num_rows;
  unsigned char * pbufferCur = pbuffer;
  while(resultCur != resultEnd){
    *resultCur = uchar2uint(pbufferCur);
    
    pbufferCur += 3;
    ++resultCur;
  }
   
  unsigned int* pDepthBuffer;
  GLint outWidth, outHeight, bitPerDepth;
  OSMesaGetDepthBuffer(ctx, &outWidth, &outHeight, &bitPerDepth, (void**)&pDepthBuffer);
  plhs[1] = mxCreateNumericMatrix((int)outWidth, (int)outHeight, mxUINT32_CLASS, mxREAL);
  
  memcpy((unsigned int*) mxGetData(plhs[1]),pDepthBuffer,sizeof(unsigned int)*outWidth*outHeight);
  
  OSMesaDestroyContext(ctx);
  delete [] pbuffer;
} 