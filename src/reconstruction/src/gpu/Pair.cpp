#include "Pair.h"

Pair::Pair(vector<char> *color_buffer, vector<char> *depth_buffer, Camera camera) {
  color = cv::imdecode(*color_buffer, cv::IMREAD_GRAYSCALE);
  depth = cv::imdecode(*depth_buffer, cv::IMREAD_ANYDEPTH);

  initPair(camera);
}

Pair::Pair(string color_path, string depth_path, Camera camera) {
  color = cv::imread(color_path, cv::IMREAD_GRAYSCALE); // Set flag to convert any image to grayscale
  depth = cv::imread(depth_path, cv::IMREAD_ANYDEPTH);

  initPair(camera);
}

Pair::~Pair() {
  fprintf(stderr, "Destroying pair\n");
  FreeSiftData(siftData);
  pointCloud.release();
  color.release();
  depth.release();
}

void Pair::initPair(Camera camera) {
  processDepth();
  createPointCloud(camera);

  // Need to figure out a better way to do this
  // reprojectPointCloud(P);
  // createPointCloud(camera);
  computeSift();
}

void Pair::processDepth() {
  cv::Mat result(depth.rows, depth.cols, cv::DataType<float>::type);

  for (int i = 0; i < depth.rows; i++) {
    for (int j = 0; j < depth.cols; j++) {
      unsigned short s = depth.at<ushort>(i, j);

      // In order to visually see depth, we bit shift during
      // capture. Now we must shift back.
      s = (s << 13 | s >> 3);
      float f = (float)s / 1000.0;
      result.at<float>(i, j) = f;
    }
  }

  depth.release();
  depth = result;
}

// TODO: move to GPU
void Pair::createPointCloud(Camera camera) {
  // Initialize 3 dimensions for each pixel in depth image
  cv::Mat result(depth.size().height * depth.size().width, 3, cv::DataType<float>::type);

  for (int v = 0; v < depth.size().height; v++) {
    for (int u = 0; u < depth.size().width; u++) {

      float iz = depth.at<float>(v,u);
      float ix = iz * (u - camera.cx) / camera.fx;
      float iy = iz * (v - camera.cy) / camera.fy;

      result.at<float>(v * depth.size().width + u, 0) = ix;
      result.at<float>(v * depth.size().width + u, 1) = iy;
      result.at<float>(v * depth.size().width + u, 2) = iz;
    }
  }

  pointCloud.release();
  pointCloud = result;
}

// TODO: move to GPU
void Pair::transformPointCloud(float T[12]) {
  cv::Mat result(pointCloud.size().height, 3, cv::DataType<float>::type);
  for (int v = 0; v < pointCloud.size().height; ++v) {
    float ix = pointCloud.at<float>(v,0);
    float iy = pointCloud.at<float>(v,1);
    float iz = pointCloud.at<float>(v,2);

    result.at<float>(v,0) = T[0] * ix + T[1] * iy + T[2] * iz + T[3];
    result.at<float>(v,1) = T[4] * ix + T[5] * iy + T[6] * iz + T[7];
    result.at<float>(v,2) = T[8] * ix + T[9] * iy + T[10] * iz + T[11];
  }

  pointCloud.release();
  pointCloud = result;
}

// TODO: move to GPU
void Pair::reprojectPointCloud(float P[12]) {
  /**
   * Step 1: setup off-screen binding. See header file for more
   * information:
   *
   * https://github.com/freedreno/mesa/blob/master/include/GL/osmesa.h
   */

  int num_cols = depth.cols;
  int num_rows = depth.rows;

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
  unsigned char * pbuffer = new unsigned char [3 * depth.cols * depth.rows];

  // Bind the buffer to the context and make it current
  if (!OSMesaMakeCurrent(
                         ctx,                // ctx: Context to bind
                         (void*)pbuffer,     // buffer: Buffer to bind to
                         GL_UNSIGNED_BYTE,   // type: Data typefor pixel components
                         num_cols,       // width: Width of buffer in pixels
                         num_rows        // height: Height of buffer in pixels
                         )) {
    fprintf(stderr, "OSMesaMakeCurrent failed!");
  }

  // Y coordinates increase downwardfind_package(OSMesa REQUIRED)
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

  final_matrix[ 0]= P[2+0*3]*inv_width_scale_1 + P[0+0*3]*inv_width_scale_2;
  final_matrix[ 1]= P[2+0*3]*inv_height_scale_1_s + P[1+0*3]*inv_height_scale_2_s;
  final_matrix[ 2]= P[2+0*3]*m_far_d_m_near;
  final_matrix[ 3]= P[2+0*3];
  final_matrix[ 4]= P[2+1*3]*inv_width_scale_1 + P[0+1*3]*inv_width_scale_2;
  final_matrix[ 5]= P[2+1*3]*inv_height_scale_1_s + P[1+1*3]*inv_height_scale_2_s;
  final_matrix[ 6]= P[2+1*3]*m_far_d_m_near;
  final_matrix[ 7]= P[2+1*3];
  final_matrix[ 8]= P[2+2*3]*inv_width_scale_1 + P[0+2*3]*inv_width_scale_2;
  final_matrix[ 9]= P[2+2*3]*inv_height_scale_1_s + P[1+2*3]*inv_height_scale_2_s;
  final_matrix[10]= P[2+2*3]*m_far_d_m_near;
  final_matrix[11]= P[2+2*3];
  final_matrix[12]= P[2+3*3]*inv_width_scale_1 + P[0+3*3]*inv_width_scale_2;
  final_matrix[13]= P[2+3*3]*inv_height_scale_1_s + P[1+3*3]*inv_height_scale_2_s;
  final_matrix[14]= P[2+3*3]*m_far_d_m_near - (2*m_far*m_near)/m_far_s_m_near;
  final_matrix[15]= P[2+3*3];

  // matrix is ready. use it
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(final_matrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // --------------------------------------------------------------------------
  // Step 4: render the mesh with depth as color
  // --------------------------------------------------------------------------
  unsigned int layerSize = pointCloud.rows;

  double zThreshold = 0.1;

  for (unsigned int r = 0; r < num_rows - 1; r++) {
    for (unsigned int c = 0; c < num_cols - 1; c++) {
      float x00 = pointCloud.at<float>(c + r * num_cols, 0);
      float x01 = pointCloud.at<float>(c + r * num_cols + 1, 0);
      float x10 = pointCloud.at<float>(c + r * (num_cols + 1), 0);
      float x11 = pointCloud.at<float>(c + r * (num_cols + 1) + 1, 0);

      float y00 = pointCloud.at<float>(c + r * num_cols, 1);
      float y01 = pointCloud.at<float>(c + r * num_cols + 1, 1);
      float y10 = pointCloud.at<float>(c + r * (num_cols + 1), 1);
      float y11 = pointCloud.at<float>(c + r * (num_cols + 1) + 1, 1);

      float z00 = pointCloud.at<float>(c + r * num_cols, 2);
      float z01 = pointCloud.at<float>(c + r * num_cols + 1, 2);
      float z10 = pointCloud.at<float>(c + r * (num_cols + 1), 2);
      float z11 = pointCloud.at<float>(c + r * (num_cols + 1) + 1, 2);

      // If depth data at 00 is missing (indicated by z00 = 0)
      if (z00 == 0.0) {

        // Make sure we can create a triangle from the other three pixels
        if (z01 != 0.0 && z10 != 0.0 && z11 != 0.0 &&
            fabs(z01 - z10) < zThreshold && fabs(z11 - z10) < zThreshold && fabs(z01 - z11) < zThreshold) {
          glBegin(GL_TRIANGLES);
          glVertex3d(x11,y11,z11);
          glVertex3d(x10,y10,z10);
          glVertex3d(x01,y01,z01);
          glEnd();
        }
      }

      // Else if data is missing at 11
      else if (z11 == 0.0){
        if (z01 != 0.0 && z10 != 0.0 && z00 != 0.0 &&
            fabs(z00 - z01) < zThreshold && fabs(z01 - z10) < zThreshold && fabs(z10 - z00) < zThreshold) {
          glBegin(GL_TRIANGLES);
          glVertex3d(x00,y00,z00);
          glVertex3d(x01,y01,z01);
          glVertex3d(x10,y10,z10);
          glEnd();
        }
      }

      // If data is available at both 00 and 11, then check to see if we can form a triangle with 01 or 10
      else {
        if (z01 != 0.0 && fabs(z00 - z01) < zThreshold && fabs(z01 - z11) < zThreshold && fabs(z11 - z00) < zThreshold) {
          glBegin(GL_TRIANGLES);
          glVertex3d(x00,y00,z00);
          glVertex3d(x01,y01,z01);
          glVertex3d(x11,y11,z11);
          glEnd();
        }
        if (z10 != 0.0 && fabs(z00 - z11) < zThreshold && fabs(z11 - z10) < zThreshold && fabs(z10 - z00) < zThreshold) {
          glBegin(GL_TRIANGLES);
          glVertex3d(x00,y00,z00);
          glVertex3d(x11,y11,z11);
          glVertex3d(x10,y10,z10);
          glEnd();
        }
      }
    }
  }

  unsigned int* pDepthBuffer;
  GLint outWidth, outHeight, bitPerDepth;
  OSMesaGetDepthBuffer(ctx, &outWidth, &outHeight, &bitPerDepth, (void**)&pDepthBuffer);

  // TODO: figure out memcpy and cast
  for (unsigned int r; r < num_rows; r++) {
    for (unsigned int c; c < num_cols; c++) {
      depth.at<float>(r, c) = (float)pDepthBuffer[c + r * num_cols];
    }
  }

  OSMesaDestroyContext(ctx);
  delete [] pbuffer;
  free(pDepthBuffer);
}

void Pair::computeSift() {
  color.convertTo(color, CV_32FC1);
  unsigned int w = color.cols;
  unsigned int h = color.rows;
  cout << "Image size = (" << w << "," << h << ")" <<endl;
  cv::GaussianBlur(color, color, cv::Size(5, 5), 1.0);

  cout << "Initializing data..." << endl;
  InitCuda();
  CudaImage cudaImage;
  cudaImage.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*) color.data);
  cudaImage.Download();

  float initBlur = 0.0f;
  float thresh = 3.0f;
  InitSiftData(siftData, 2048, true, true);
  double timesift1 = ExtractSift(siftData, cudaImage, 5, initBlur, thresh, 0.0f);
  cout << "Extract sift time: " <<  timesift1 << endl;
  cout << "Number of original features: " << siftData.numPts << endl;
}

int Pair::getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch) {
  MatchSiftData(siftData, other->siftData);

  float minScore = 0.75f;
  float maxAmbiguity = 0.95f;
  int numToMatchedSift = 0;
  int imgw = color.cols;

  SiftPoint *siftPoint = siftData.h_data;
  for(int i = 0; i < siftData.numPts; i++) {
    int index_self = ((int)siftPoint[i].xpos + (int)siftPoint[i].ypos * imgw);
    int index_other = ((int)siftPoint[i].match_xpos + (int)siftPoint[i].match_ypos * imgw);

    if (siftPoint[i].ambiguity < maxAmbiguity &&
        siftPoint[i].score > minScore &&
        pointCloud.at<float>(index_self, 2) > 0 &&
        other->pointCloud.at<float>(index_other, 2) > 0) {
      lmatch.push_back(pointCloud.row(index_self));
      rmatch.push_back(other->pointCloud.row(index_other));

      numToMatchedSift++;
      siftPoint[i].valid = 1;
    }
  }

  return numToMatchedSift;
}

void Pair::convert(int type) {
  color.convertTo(color, type);
}
