#include "Pair.h"

Pair::Pair(vector<char> *color_buffer, vector<char> *depth_buffer, Parameters *parameters) {
  color = cv::imdecode(*color_buffer, cv::IMREAD_COLOR);
  gray = cv::imdecode(*color_buffer, cv::IMREAD_GRAYSCALE);
  depth = cv::imdecode(*depth_buffer, cv::IMREAD_ANYDEPTH);

  initPair(parameters);
}

Pair::Pair(string color_path, string depth_path, Parameters *parameters) {
  color = cv::imread(color_path, cv::IMREAD_COLOR);
  gray = cv::imread(color_path, cv::IMREAD_GRAYSCALE); // Set flag to convert any image to grayscale
  depth = cv::imread(depth_path, cv::IMREAD_ANYDEPTH);

  initPair(parameters);
}

Pair::~Pair() {
  FreeSiftData(siftData);
  pointCloud_camera.release();
  pointCloud_world.release();
  color.release();
  gray.release();
  depth.release();
}

void Pair::initPair(Parameters *parameters) {
  bitShiftDepth();

  // // Align color and depth in software
  // createPointCloud(parameters->depth_camera);
  // transformPointCloud(parameters->projection_d2c);
  // projectPointCloud(parameters->color_camera);

  pointCloud_camera = createPointCloud(parameters->color_camera);

  computeSift();
}

void Pair::bitShiftDepth() {
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

void Pair::linearizeDepth() {
  // Depth from depth buffer comes out in non-linear units to give
  // close points more depth precision than far points. We must
  // convert into linear units so we can use

  // Assume that far plane is far relative to near such that f / (f -
  //  n) approximately equals 1

  // Currently implemented at the end projectPointCloud
}

// TODO: move to GPU
cv::Mat Pair::createPointCloud(Camera *camera) {

  // Initialize 3 dimensions for each pixel in depth image
  cv::Mat result(depth.rows * depth.cols, 3, cv::DataType<float>::type);

  // TODO: should reinvestigate calibrated cx, cy
  float half_cols = depth.cols / 2;
  float half_rows = depth.rows / 2;

  for (int r = 0; r < depth.rows; r++) {
    for (int c = 0; c < depth.cols; c++) {

      float iz = depth.at<float>(r, c);
      // float ix = iz * (c - camera->cx) / camera->fx;
      // float iy = iz * (r - camera->cy) / camera->fy;
      float ix = iz * (c + 1 - half_cols) / camera->fx;
      float iy = iz * (r + 1 - half_rows) / camera->fy;

      result.at<float>(r * depth.cols + c, 0) = ix;
      result.at<float>(r * depth.cols + c, 1) = iy;
      result.at<float>(r * depth.cols + c, 2) = iz;
    }
  }

  return result;
}

// TODO: move to GPU
cv::Mat Pair::transformPointCloud(cv::Mat pointCloud, float T[12]) {
  cv::Mat result(pointCloud.size().height, 3, cv::DataType<float>::type);
  for (int v = 0; v < pointCloud.size().height; ++v) {
    float ix = pointCloud.at<float>(v,0);
    float iy = pointCloud.at<float>(v,1);
    float iz = pointCloud.at<float>(v,2);

    result.at<float>(v,0) = T[0] * ix + T[1] * iy + T[2] * iz + T[3];
    result.at<float>(v,1) = T[4] * ix + T[5] * iy + T[6] * iz + T[7];
    result.at<float>(v,2) = T[8] * ix + T[9] * iy + T[10] * iz + T[11];
  }

  return result;
}

// TODO: move to GPU
void Pair::projectPointCloud(Camera *camera) {
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
  double final_matrix[16] = {0};

  // Set projection parameters
  float m_near = 0.3; // TODO
  float m_far  = 1e8;  // TODO

  // new way: faster way by reuse computation and symbolic derive. See
  // sym_derive.m to check the math.
  double inv_width_scale  = 1.0/(num_cols*scale);
  double inv_height_scale = 1.0/(num_rows*scale);
  double inv_width_scale_1 = inv_width_scale - 1.0;
  double inv_height_scale_1_s = -(inv_height_scale - 1.0);
  double inv_width_scale_2 = inv_width_scale*2.0;
  double inv_height_scale_2_s = -inv_height_scale*2.0;
  double m_far_a_m_near = m_far + m_near;
  double m_far_s_m_near = m_far - m_near;
  double m_far_d_m_near = m_far_a_m_near/m_far_s_m_near;

  final_matrix[ 0]= camera->fx * inv_width_scale_2;
  final_matrix[ 5]= camera->fy * inv_height_scale_2_s;
  final_matrix[ 8]= inv_width_scale_1 + camera->cx * inv_width_scale_2;
  final_matrix[ 9]= inv_height_scale_1_s + camera->cy * inv_height_scale_2_s;
  final_matrix[10]= m_far_d_m_near;
  final_matrix[11]= 1;
  final_matrix[14]= -(2*m_far*m_near)/m_far_s_m_near;

  // matrix is ready. use it
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(final_matrix);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // --------------------------------------------------------------------------
  // Step 4: render the mesh with depth as color
  // --------------------------------------------------------------------------

  double zThreshold = 0.1;
  int numPixels = 0;


  for (unsigned int r = 0; r < num_rows - 1; r++) {
    for (unsigned int c = 0; c < num_cols - 1; c++) {
      float x00 = -pointCloud_camera.at<float>(c + r * num_cols, 0);
      float x01 = -pointCloud_camera.at<float>(c + r * num_cols + 1, 0);
      float x10 = -pointCloud_camera.at<float>(c + (r + 1) * num_cols, 0);
      float x11 = -pointCloud_camera.at<float>(c + (r + 1) * num_cols + 1, 0);

      float y00 = pointCloud_camera.at<float>(c + r * num_cols, 1);
      float y01 = pointCloud_camera.at<float>(c + r * num_cols + 1, 1);
      float y10 = pointCloud_camera.at<float>(c + (r + 1) * num_cols, 1);
      float y11 = pointCloud_camera.at<float>(c + (r + 1) * num_cols + 1, 1);

      float z00 = pointCloud_camera.at<float>(c + r * num_cols, 2);
      float z01 = pointCloud_camera.at<float>(c + r * num_cols + 1, 2);
      float z10 = pointCloud_camera.at<float>(c + (r + 1) * num_cols, 2);
      float z11 = pointCloud_camera.at<float>(c + (r + 1) * num_cols + 1, 2);

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
          numPixels++;
        }
      }

      // Else if data is missing at 11
      else {
        if (z11 == 0.0){
          if (z01 != 0.0 && z10 != 0.0 && z00 != 0.0 &&
              fabs(z00 - z01) < zThreshold && fabs(z01 - z10) < zThreshold && fabs(z10 - z00) < zThreshold) {
            glBegin(GL_TRIANGLES);
            glVertex3d(x00,y00,z00);
            glVertex3d(x01,y01,z01);
            glVertex3d(x10,y10,z10);
            glEnd();
            numPixels++;
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
            numPixels++;
          }
          if (z10 != 0.0 && fabs(z00 - z11) < zThreshold && fabs(z11 - z10) < zThreshold && fabs(z10 - z00) < zThreshold) {
            glBegin(GL_TRIANGLES);
            glVertex3d(x00,y00,z00);
            glVertex3d(x11,y11,z11);
            glVertex3d(x10,y10,z10);
            glEnd();
            numPixels++;
          }
        }
      }
    }
  }

  unsigned int* pDepthBuffer;
  GLint outWidth, outHeight, bitPerDepth;
  OSMesaGetDepthBuffer(ctx, &outWidth, &outHeight, &bitPerDepth, (void**)&pDepthBuffer);

  unsigned int shift = -1;

  // TODO: figure out memcpy and cast
  // Linearize depth map
  for (unsigned int r = 0; r < num_rows; r++) {
    for (unsigned int c = 0; c < num_cols; c++) {
      unsigned int r_flip = num_rows - r - 1;
      unsigned int c_flip = num_cols - c - 1;

      depth.at<float>(r_flip, c_flip) = m_near / (1 - ((float)pDepthBuffer[c + r * num_cols]) / shift);

      // // Ignore data that is closer than near plane or further than 15 meters
      if (depth.at<float>(r_flip, c_flip) > 15 || depth.at<float>(r_flip, c_flip) < m_near) {
        depth.at<float>(r_flip, c_flip) = 0;
      }

      depth.at<float>(r_flip, c_flip) *= 50;
      // fprintf(stderr, "%0.2f ", depth.at<float>(r,c));
    }
  }

  fprintf(stderr, "\n%u\n", shift);
  OSMesaDestroyContext(ctx);
  delete [] pbuffer;
}

void Pair::computeSift() {

  // Convert grayscale image to 32-bit float with 1 channel
  gray.convertTo(gray, CV_32FC1);

  // Get width and height of image
  unsigned int w = gray.cols;
  unsigned int h = gray.rows;
  
  // Blur image
  cv::GaussianBlur(gray, gray, cv::Size(5, 5), 1.0);

  // Initializing image data onto GPU
  InitCuda();
  CudaImage cudaImage;
  cudaImage.Allocate(w, h, iAlignUp(w, 128), false, NULL, (float*) gray.data);
  cudaImage.Download();

  // Initialize SIFT data on both host and device
  float initBlur = 0.0f;
  float thresh = 3.0f;
  InitSiftData(siftData, 2048, true, true);

  // Extract sift data
  ExtractSift(siftData, cudaImage, 5, initBlur, thresh, 0.0f);
  SiftPoint *siftPoints = siftData.h_data;

  for (int i = 0; i < siftData.numPts; i++) {
    fprintf(stderr, "Siftpoint (x, y): (%d, %d)\n", (int)siftPoints[i].xpos, (int)siftPoints[i].ypos);
  }

  fprintf(stderr, "Number of original features: %d\n", siftData.numPts);
}

int Pair::getMatched3DPoints(Pair *other, cv::Mat &lmatch, cv::Mat &rmatch) {
  MatchSiftData(siftData, other->siftData);
  fprintf(stderr, "Num matched sift points %d\n", siftData.numPts);

  float minScore = 0.75f;
  float maxAmbiguity = 0.95f;
  int numToMatchedSift = 0;
  int imgw = gray.cols;
  int imgh = gray.rows;

  SiftPoint *siftPoints = siftData.h_data;
  for(int i = 0; i < siftData.numPts; i++) {
    int index_self = ((int)siftPoints[i].xpos + (int)siftPoints[i].ypos * imgw);
    int index_other = ((int)siftPoints[i].match_xpos + (int)siftPoints[i].match_ypos * imgw);

    if (siftPoints[i].ambiguity < maxAmbiguity &&
        siftPoints[i].score > minScore &&
        pointCloud_camera.at<float>(index_self, 2) > 0 &&
        other->pointCloud_camera.at<float>(index_other, 2) > 0 &&
        (int)siftPoints[i].xpos < imgw &&
        (int)siftPoints[i].ypos < imgh &&
        (int)siftPoints[i].match_xpos < imgw &&
        (int)siftPoints[i].match_ypos < imgh
        ) {
      // fprintf(stderr, "Imgw, h %d, %d\n", imgw, gray.rows);
      // fprintf(stderr, "Curr point (x, y): (%d, %d)\n", (int)siftPoints[i].xpos, (int)siftPoints[i].ypos);
      // fprintf(stderr, "Curr height and index %d %d\n", pointCloud_camera.size().height, index_self);
      // fprintf(stderr, "Other point (x, y): (%d, %d)\n", (int)siftPoints[i].match_xpos, (int)siftPoints[i].match_ypos);
      // fprintf(stderr, "Other height and index %d %d\n", other->pointCloud_camera.size().height, index_other);
      lmatch.push_back(pointCloud_camera.row(index_self));
      rmatch.push_back(other->pointCloud_camera.row(index_other));

      numToMatchedSift++;
      siftPoints[i].valid = 1;
    } else {
      // fprintf(stderr, "Amb: %f, Score: %f, p_z: %f, o_z: %f\n", siftPoints[i].ambiguity, siftPoints[i].score, pointCloud.at<float>(index_self, 2), other->pointCloud.at<float>(index_other, 2));
    }
  }

  return numToMatchedSift;
}

void Pair::convert(int type) {
  gray.convertTo(gray, type);
}

void Pair::writePLY(const char *plyfile) {
  FILE *fp = fopen(plyfile,"w");
  int pointCount = 0;
  int skip = 2;

  for (int v = 0; v < pointCloud_world.size().height; ++v) {
    if (pointCloud_world.at<float>(v,2)>0.0001 && v % skip == 0) {
      pointCount++;
    }
  }

  cout << "Write PLY" << endl;

  fprintf(fp, "ply\n");
  fprintf(fp, "format binary_little_endian 1.0\n");
  fprintf(fp, "element vertex %d\n", pointCount);
  fprintf(fp, "property float x\n");
  fprintf(fp, "property float y\n");
  fprintf(fp, "property float z\n");
  fprintf(fp, "property uchar red\n");
  fprintf(fp, "property uchar green\n");
  fprintf(fp, "property uchar blue\n");
  fprintf(fp, "end_header\n");

  for (int v = 0; v < pointCloud_world.size().height; ++v) {
    if (pointCloud_world.at<float>(v,2)>0.0001 && v % skip == 0){
      fwrite(&pointCloud_world.at<float>(v,0), sizeof(float), 1, fp);
      fwrite(&pointCloud_world.at<float>(v,1), sizeof(float), 1, fp);
      fwrite(&pointCloud_world.at<float>(v,2), sizeof(float), 1, fp);
      int i= (int)v/color.size().width;
      int j= (int)v%color.size().width;
      fwrite(&color.at<cv::Vec3b>(i,j)[2], sizeof(uchar), 1, fp);
      fwrite(&color.at<cv::Vec3b>(i,j)[1], sizeof(uchar), 1, fp);
      fwrite(&color.at<cv::Vec3b>(i,j)[0], sizeof(uchar), 1, fp);
    }
  }
  fclose(fp);
}

void Pair::readSIFT(const char *siftfile) {
  InitSiftData(siftData, 2048, true, true);
}