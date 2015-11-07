#include <cmath>
#include <cstdio>
#include <iostream>
#include <Eigen/Dense>
#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "glog/logging.h"

using namespace std;

double* objectHalfSize;
double* objectWeight;
double* focalLen;
double fx, fy, px, py, w3Dv2D;

struct AlignmentError3D {
  AlignmentError3D(double* observed_in): observed(observed_in) {}

  template <typename T>
  bool operator()(const T* const camera_extrinsic,
                  const T* const point,
                  T* residuals) const {
                  
    // camera_extrinsic[0,1,2] are the angle-axis rotation.
    T p[3];
    
    ceres::AngleAxisRotatePoint(camera_extrinsic, point, p);

    // camera_extrinsic[3,4,5] are the translation.
    p[0] += camera_extrinsic[3];
    p[1] += camera_extrinsic[4];
    p[2] += camera_extrinsic[5];
    
    // The error is the difference between the predicted and observed position.
    residuals[0] = T(observed[5])*(p[0] - T(observed[2]));
    residuals[1] = T(observed[5])*(p[1] - T(observed[3]));
    residuals[2] = T(observed[5])*(p[2] - T(observed[4]));

    return true;
  }
  double* observed;
};

void printRt(double *Rt, int len) {
  for (int i = 0; i < len; i++) {
    cout << Rt[i] << " ";
  }
  cout << endl;
}

struct PoseGraphRotationError {
  PoseGraphRotationError(double *m_R_ij, double m_weight): m_R_ij(m_R_ij), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const R_i,
                  const T* const R_j,
                  T* residuals) const {

    // TODO: compare results to optimizing angle axis directly rather
    // than converting in residual block

    // Get T-ified weight
    T weight = T(m_weight);

    // Initialize a vector so we can compare rotations; we choose the
    // view direction, but this choice is arbitrary
    T z[3] = {T(0), T(0), T(1)};

    // Create a T-ified R_ij for computation. Note that this rotation
    // is camera-to-world
    T R_ij[3] = {T(m_R_ij[0]), T(m_R_ij[1]), T(m_R_ij[2])};

    // Create a view direction vector between i and j
    T v_ij_observed[3];
    T v_ij_predicted[3];

    // Compute observed view direction vector from world to camera by
    // relative rotation between i and j
    ceres::AngleAxisRotatePoint(R_ij, z, v_ij_observed);

    // Compute predicted view direction vector
    // Get inverse rotation of R_i
    T IR_i[3] = {-R_i[0], -R_i[1], -R_i[2]};

    // Rotate from according to j's rotation
    ceres::AngleAxisRotatePoint(R_j, z, v_ij_predicted);

    // Rotate back from according to i's rotation
    ceres::AngleAxisRotatePoint(IR_i, v_ij_predicted, v_ij_predicted);

    residuals[0] = weight * (v_ij_predicted[0] - v_ij_observed[0]);
    residuals[1] = weight * (v_ij_predicted[1] - v_ij_observed[1]);
    residuals[2] = weight * (v_ij_predicted[2] - v_ij_observed[2]);

    return true;
  }

  double *m_R_ij;
  double m_weight;
};

struct PoseGraphTranslationError {
  PoseGraphTranslationError(double *m_t_ij, double m_weight): m_t_ij(m_t_ij), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const t_i,
                  const T* const t_j,
                  T* residuals) const {
    // Create T-ified weight
    T weight = T(m_weight);

    // Create observed translation vector
    T t_ij_observed[3] = {T(m_t_ij[0]), T(m_t_ij[1]), T(m_t_ij[2])};

    // Create predicted translation vector. Note this should be t_j -
    // t_i, but because t_j and t_i are world-to-camera and t_ij is
    // camera-to-world
    T t_ij_predicted[3] = {t_j[0] - t_i[0], t_j[1] - t_i[1], t_j[2] - t_i[2]};

    // We use the cross product between t_ij_observed and
    // t_ij_predicted as the distance
    residuals[0] = weight * (t_ij_predicted[1] * t_ij_observed[2] - t_ij_predicted[2] * t_ij_observed[1]);
    residuals[1] = weight * (t_ij_predicted[2] * t_ij_observed[0] - t_ij_predicted[0] * t_ij_observed[2]);
    residuals[2] = weight * (t_ij_predicted[0] * t_ij_observed[1] - t_ij_predicted[1] * t_ij_observed[0]);

    return true;
  }

  double *m_t_ij;
  double m_weight;
};

struct PoseGraphError {
  PoseGraphError(double *m_Rt_ij, double m_weight): m_Rt_ij(m_Rt_ij), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const Rt_i,
                  const T* const Rt_j,
                  T* residuals) const {

    // TODO: compare results to optimizing angle axis directly rather
    // than converting in residual block

    // Get T-ified weight
    T weight = T(m_weight);

    // Initialize a vector so we can compare rotations; we choose the
    // view direction, but this choice is arbitrary
    T z[3] = {T(0), T(0), T(1)};

    // Create a T-ified R_ij for computation. Note that this rotation
    // is camera-to-world
    T R_ij[3] = {T(m_Rt_ij[0]), T(m_Rt_ij[1]), T(m_Rt_ij[2])};

    // Create a view direction vector between i and j
    T v_ij_observed[3];
    T v_ij_predicted[3];

    // Compute observed view direction vector from world to camera by
    // relative rotation between i and j
    ceres::AngleAxisRotatePoint(R_ij, z, v_ij_observed);

    // Compute predicted view direction vector
    // Get inverse rotation of R_i
    T IR_i[3] = {-Rt_i[0], -Rt_i[1], -Rt_i[2]};

    // Rotate from according to j's rotation
    ceres::AngleAxisRotatePoint(Rt_j, z, v_ij_predicted);

    // Rotate back from according to i's rotation
    ceres::AngleAxisRotatePoint(IR_i, v_ij_predicted, v_ij_predicted);

    // Create observed translation vector
    T t_ij_observed[3] = {T(m_Rt_ij[3]), T(m_Rt_ij[4]), T(m_Rt_ij[5])};
    ceres::AngleAxisRotatePoint(Rt_i, t_ij_observed, t_ij_observed);

    // Create predicted translation vector. Note this should be t_j -
    // t_i, but because Rt_j and Rt_i are world-to-camera and Rt_ij is
    // camera-to-world
    T t_ij_predicted[3] = {Rt_j[3] - Rt_i[3], Rt_j[4] - Rt_i[4], Rt_j[5] - Rt_i[5]};

    /* 3 residual
    residuals[0] = weight * (v_ij_predicted[0] + t_ij_predicted[0] - v_ij_observed[0] - t_ij_observed[0]);
    residuals[1] = weight * (v_ij_predicted[1] + t_ij_predicted[1] - v_ij_observed[1] - t_ij_observed[1]);
    residuals[2] = weight * (v_ij_predicted[2] + t_ij_predicted[2] - v_ij_observed[2] - t_ij_observed[2]);
    */

    residuals[0] = weight * (v_ij_predicted[0] - v_ij_observed[0]);
    residuals[1] = weight * (v_ij_predicted[1] - v_ij_observed[1]);
    residuals[2] = weight * (v_ij_predicted[2] - v_ij_observed[2]);
    residuals[3] = weight * T(2) * (t_ij_predicted[0] - t_ij_observed[0]);
    residuals[4] = weight * T(2) * (t_ij_predicted[1] - t_ij_observed[1]);
    residuals[5] = weight * T(2) * (t_ij_predicted[2] - t_ij_observed[2]);

    /* Cross product difference
    residuals[3] = t_ij_predicted[1] * t_ij_observed[2] - t_ij_predicted[2] * t_ij_observed[1];
    residuals[4] = t_ij_predicted[2] * t_ij_observed[0] - t_ij_predicted[0] * t_ij_observed[2];
    residuals[5] = t_ij_predicted[0] * t_ij_observed[1] - t_ij_predicted[1] * t_ij_observed[0];
    */

    return true;
  }

  double *m_Rt_ij;
  double m_weight;
};

struct PoseGraphUpError {
  PoseGraphUpError(double *m_Rt_ij, double m_weight): m_Rt_ij(m_Rt_ij), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const Rt_i,
                  const T* const Rt_j,
                  T* residuals) const {

    // TODO: compare results to optimizing angle axis directly rather
    // than converting in residual block

    // Get T-ified weight
    T weight = T(m_weight);

    // Initialize a vector so we can compare rotations; we choose the
    // view direction, but this choice is arbitrary
    T z[3] = {T(0), T(0), T(1)};
    T y[3] = {T(0), T(1), T(0)};

    // Create a T-ified R_ij for computation. Note that this rotation
    // is camera-to-world
    T R_ij[3] = {T(m_Rt_ij[0]), T(m_Rt_ij[1]), T(m_Rt_ij[2])};

    // Create a view direction vector between i and j
    T v_ij_z_observed[3];
    T v_ij_z_predicted[3];
    T v_ij_y_observed[3];
    T v_ij_y_predicted[3];

    // Compute observed view direction vector from world to camera by
    // relative rotation between i and j
    ceres::AngleAxisRotatePoint(R_ij, z, v_ij_z_observed);
    ceres::AngleAxisRotatePoint(R_ij, y, v_ij_y_observed);

    // Compute predicted view direction vector
    // Get inverse rotation of R_i
    T IR_i[3] = {-Rt_i[0], -Rt_i[1], -Rt_i[2]};

    // Rotate from according to j's rotation
    ceres::AngleAxisRotatePoint(Rt_j, z, v_ij_z_predicted);
    ceres::AngleAxisRotatePoint(Rt_j, y, v_ij_y_predicted);

    // Rotate back from according to i's rotation
    ceres::AngleAxisRotatePoint(IR_i, v_ij_z_predicted, v_ij_z_predicted);
    ceres::AngleAxisRotatePoint(IR_i, v_ij_y_predicted, v_ij_y_predicted);

    // Create observed translation vector
    T t_ij_observed[3] = {T(m_Rt_ij[3]), T(m_Rt_ij[4]), T(m_Rt_ij[5])};
    ceres::AngleAxisRotatePoint(Rt_i, t_ij_observed, t_ij_observed);

    // Create predicted translation vector. Note this should be t_j -
    // t_i, but because Rt_j and Rt_i are world-to-camera and Rt_ij is
    // camera-to-world
    T t_ij_predicted[3] = {Rt_j[3] - Rt_i[3], Rt_j[4] - Rt_i[4], Rt_j[5] - Rt_i[5]};

    /* 3 residual
    residuals[0] = weight * (v_ij_z_predicted[0] + t_ij_predicted[0] - v_ij_z_observed[0] - t_ij_observed[0]);
    residuals[1] = weight * (v_ij_z_predicted[1] + t_ij_predicted[1] - v_ij_z_observed[1] - t_ij_observed[1]);
    residuals[2] = weight * (v_ij_z_predicted[2] + t_ij_predicted[2] - v_ij_z_observed[2] - t_ij_observed[2]);
    */

    residuals[0] = weight * (v_ij_z_predicted[0] - v_ij_z_observed[0]);
    residuals[1] = weight * (v_ij_z_predicted[1] - v_ij_z_observed[1]);
    residuals[2] = weight * (v_ij_z_predicted[2] - v_ij_z_observed[2]);
    residuals[3] = weight * T(2) * (t_ij_predicted[0] - t_ij_observed[0]);
    residuals[4] = weight * T(2) * (t_ij_predicted[1] - t_ij_observed[1]);
    residuals[5] = weight * T(2) * (t_ij_predicted[2] - t_ij_observed[2]);
    residuals[6] = weight * (v_ij_y_predicted[0] - v_ij_y_observed[0]);
    residuals[7] = weight * (v_ij_y_predicted[1] - v_ij_y_observed[1]);
    residuals[8] = weight * (v_ij_y_predicted[2] - v_ij_y_observed[2]);

    /* Cross product difference
    residuals[3] = t_ij_predicted[1] * t_ij_observed[2] - t_ij_predicted[2] * t_ij_observed[1];
    residuals[4] = t_ij_predicted[2] * t_ij_observed[0] - t_ij_predicted[0] * t_ij_observed[2];
    residuals[5] = t_ij_predicted[0] * t_ij_observed[1] - t_ij_predicted[1] * t_ij_observed[0];
    */

    return true;
  }

  double *m_Rt_ij;
  double m_weight;
};

#define NUM_BA_POINTS 0

struct PoseGraphBAError {
  PoseGraphBAError(double *m_Rt_ij, double *points, double m_weight): m_Rt_ij(m_Rt_ij), points(points), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const Rt_i,
                  const T* const Rt_j,
                  T* residuals) const {

    // TODO: compare results to optimizing angle axis directly rather
    // than converting in residual block

    // Get T-ified weight
    T weight = T(m_weight);

    // Create observed translation vector
    T t_ij_observed[3] = {T(m_Rt_ij[3]), T(m_Rt_ij[4]), T(m_Rt_ij[5])};
    ceres::AngleAxisRotatePoint(Rt_i, t_ij_observed, t_ij_observed);

    // Create predicted translation vector. Note this should be t_j -
    // t_i, but because Rt_j and Rt_i are world-to-camera and Rt_ij is
    // camera-to-world
    T t_ij_predicted[3] = {Rt_j[3] - Rt_i[3], Rt_j[4] - Rt_i[4], Rt_j[5] - Rt_i[5]};

    residuals[0] = weight * T(2) * (t_ij_predicted[0] - t_ij_observed[0]);
    residuals[1] = weight * T(2) * (t_ij_predicted[1] - t_ij_observed[1]);
    residuals[2] = weight * T(2) * (t_ij_predicted[2] - t_ij_observed[2]);

    // Create a T-ified R_ij for computation. Note that this rotation
    // is camera-to-world
    T R_ij[3] = {T(m_Rt_ij[0]), T(m_Rt_ij[1]), T(m_Rt_ij[2])};

    // Compute predicted view direction vector
    // Get inverse rotation of R_i
    T IR_i[3] = {-Rt_i[0], -Rt_i[1], -Rt_i[2]};

    // Loop through the bundle adjustment points (SIFT features) plus
    // y and z vectors
    for (int i = 0; i < NUM_BA_POINTS + 2; i++) {
      T p_ij_observed[3];
      T p_ij_predicted[3];

      // Get T-ified point
      double *point = points + 3 * i;
      T p[3] = {T(point[0]), T(point[1]), T(point[2])};

      // Compute observed view direction vector from world to camera by
      // relative rotation between i and j
      ceres::AngleAxisRotatePoint(R_ij, p, p_ij_observed);

      // Rotate from according to j's rotation
      ceres::AngleAxisRotatePoint(Rt_j, p, p_ij_predicted);

      // Rotate back from according to i's rotation
      ceres::AngleAxisRotatePoint(IR_i, p_ij_predicted, p_ij_predicted);

      residuals[3 + 3 * i] = weight * (p_ij_predicted[0] - p_ij_observed[0]);
      residuals[4 + 3 * i] = weight * (p_ij_predicted[1] - p_ij_observed[1]);
      residuals[5 + 3 * i] = weight * (p_ij_predicted[2] - p_ij_observed[2]);
    }

    return true;
  }

  double *m_Rt_ij;
  double *points;
  double m_weight;
};

int main(int argc, char** argv)
{
  google::InitGoogleLogging(argv[0]);

  // NOTE: Data from MATLAB is in column-major order

  cout << "Ba2D3D bundle adjuster in 2D and 3D. Writen by Jianxiong Xiao." << endl;
  cout << "Usage: EXE mode(1,2,3,5) w3Dv2D input_file_name output_file_name" << endl;

  // Get bundle adjustment mode
  int mode = atoi(argv[1]);

  // TODO: figure out
  w3Dv2D = atof(argv[2]);

  // Open input file
  FILE* fp = fopen(argv[3],"rb");
  if (fp == NULL) {
    cout << "fail to open file" << endl;
    return false;
  }

  // Get number of camera poses (i.e., frames; assumes that each pose
  // uses the same camera)
  unsigned int nCam;  fread((void*)(&nCam), sizeof(unsigned int), 1, fp);

  // Get number of points in the point cloud
  unsigned int nPts;  fread((void*)(&nPts), sizeof(unsigned int), 1, fp);

  // Get number of SIFT features
  unsigned int nObs;  fread((void*)(&nObs), sizeof(unsigned int), 1, fp);

  // Get number of matched pairs
  unsigned int nPairs; fread((void*)(&nPairs), sizeof(unsigned int), 1, fp);

  // Get number of matched key points
  unsigned int nMatchedPoints; fread((void*)(&nMatchedPoints), sizeof(unsigned int), 1, fp);

  // Read camera intrinsic
  fread((void*)(&fx), sizeof(double), 1, fp);
  fread((void*)(&fy), sizeof(double), 1, fp);
  fread((void*)(&px), sizeof(double), 1, fp);
  fread((void*)(&py), sizeof(double), 1, fp);

  focalLen = new double [2];
  focalLen[0] = fx;
  focalLen[1] = fy;

  // Read all of the extrinsic matrices for the nCam camera
  // poses. Converts camera coordinates into world coordinates
  double* cameraRtC2W_PoseGraph = new double [12*nCam];
  cout << "Reading cameraRtC2W_PoseGraph" << endl;
  fread((void*)(cameraRtC2W_PoseGraph), sizeof(double), 12*nCam, fp);

  // Read all of the extrinsic matrices for the nCam camera
  // poses. Converts world coordinates into camera coordinates
  double *cameraRtW2C_BundleAdjustment = new double[12*nCam];
  cout << "Reading cameraRtW2C_BundleAdjustment" << endl;
  fread((void*)(cameraRtW2C_BundleAdjustment), sizeof(double), 12*nCam, fp);

  // Read initial 3D point position (i.e., position when rotated /
  // translated according to time-based alignmetn)
  double* pointCloud = new double [3*nPts];
  cout << "Reading pointCloud" << endl;
  fread((void*)(pointCloud), sizeof(double), 3*nPts, fp);

  // Read all relative poses for each matched pair
  double *cameraRt_ij = new double[12*nPairs];
  cout << "Reading cameraRt_ij" << endl;
  fread((void*)(cameraRt_ij), sizeof(double), 12*nPairs, fp);

  //Read all camera pose indices for matched pairs
  unsigned int *cameraRt_ij_indices = new unsigned int [2*nPairs];
  cout << "Reading cameraRt_ij_indices" << endl;
  fread((void*)(cameraRt_ij_indices), sizeof(unsigned int), 2*nPairs, fp);

  // Read all of the matched key points for a given pair (3D
  // coordinates in j's coordinates)
  double *cameraRt_ij_points = new double[3*nMatchedPoints];
  cout << "Reading cameraRt_ij_points" << endl;
  fread((void*)(cameraRt_ij_points), sizeof(double), 3*nMatchedPoints, fp);

  // Read the count of matched key points per pair so we can index
  // correctly
  unsigned int *cameraRt_ij_points_count = new unsigned int [nPairs];
  cout << "Reading cameraRt_ij_points_count" << endl;
  fread((void*)(cameraRt_ij_points_count), sizeof(unsigned int), nPairs, fp);

  // Read 3D point position as observed relative to previous frame
  unsigned int* pointObservedIndex = new unsigned int [2*nObs];
  double* pointObservedValue = new double [6*nObs];
  cout << "Reading pointObserved" << endl;
  fread((void*)(pointObservedIndex), sizeof(unsigned int), 2*nObs, fp);
  fread((void*)(pointObservedValue), sizeof(double), 6*nObs, fp);

  // finish reading
  fclose(fp);

  // output info
  cout << "Parameters: ";
  cout << "mode=" << mode << " ";
  cout << "w3Dv2D=" << w3Dv2D << endl;

  cout << "Meta Info: ";
  cout << "nCam=" << nCam << " ";
  cout << "nPts=" << nPts << " ";
  cout << "nObs=" << nObs << " ";
  cout << "nPairs=" << nPairs << endl;

  cout << "Camera Intrinsic: ";
  cout << "fx=" << fx << " ";
  cout << "fy=" << fy << " ";
  cout << "px=" << px << " ";
  cout << "py=" << py << endl;

  // Construct camera parameters from camera matrix
  double* cameraParameter_PoseGraph = new double [6*nCam];
  double* cameraParameter_BundleAdjustment = new double [6*nCam];

  cout << "Converting camera parameters" << endl;
  // Loop through all poses
  for(int cameraID = 0; cameraID < nCam; ++cameraID) {
    double* cameraPtr_PoseGraph = cameraParameter_PoseGraph + 6*cameraID;
    double* cameraMat_PoseGraph = cameraRtC2W_PoseGraph + 12*cameraID;
    double* cameraPtr_BundleAdjustment = cameraParameter_BundleAdjustment + 6*cameraID;
    double* cameraMat_BundleAdjustment = cameraRtW2C_BundleAdjustment + 12*cameraID;

    if (!(isnan(*cameraPtr_PoseGraph))) {
      // Converting column-major order cameraMat_PoseGraph into first three
      // elements of cameraPtr_PoseGraph
      ceres::RotationMatrixToAngleAxis<double>(cameraMat_PoseGraph, cameraPtr_PoseGraph);

      // Grabbing translation
      cameraPtr_PoseGraph[3] = cameraMat_PoseGraph[9];
      cameraPtr_PoseGraph[4] = cameraMat_PoseGraph[10];
      cameraPtr_PoseGraph[5] = cameraMat_PoseGraph[11];
    }

    if (!(isnan(*cameraPtr_BundleAdjustment))) {
      ceres::RotationMatrixToAngleAxis<double>(cameraMat_BundleAdjustment, cameraPtr_BundleAdjustment);

      cameraPtr_BundleAdjustment[3] = cameraMat_BundleAdjustment[9];
      cameraPtr_BundleAdjustment[4] = cameraMat_BundleAdjustment[10];
      cameraPtr_BundleAdjustment[5] = cameraMat_PoseGraph[11];
    }
  }

  // Initialize a vector we can rotate
  double viewVec[3] = {1, 0, 0};

  // Construct camera parameters from camera matrix for relative poses
  double *cameraParameter_ij = new double[6 * nPairs];

  for (int i = 0; i < nPairs; i++) {
    double *cameraPtr = cameraParameter_ij + 6 * i;
    double *cameraMat = cameraRt_ij + 12 * i;

    if (!(isnan(*cameraPtr))) {
      // Converting column-major order cameraMat into first three
      // elements of cameraPtr
      ceres::RotationMatrixToAngleAxis<double>(cameraMat, cameraPtr);

      // TODO: We actually want the viewing direction vector so we can make
      // comparisons.
      // ceres::AngleAxisRotatePoint(cameraPtr, viewVec, cameraPtr);

      // Grabbing translation
      cameraPtr[3] = cameraMat[9];
      cameraPtr[4] = cameraMat[10];
      cameraPtr[5] = cameraMat[11];
    }
  }

  cout << "Setting options" << endl;
  ceres::Solver::Options options;
  options.max_num_iterations = 2000;
  options.minimizer_progress_to_stdout = true;
  options.linear_solver_type = ceres::SPARSE_NORMAL_CHOLESKY;
  options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT;

  cout << "Setting loss function" << endl;
  ceres::LossFunction* loss_function_BundleAdjustment = new ceres::TrivialLoss();
  ceres::LossFunction* loss_function_PoseGraph = new ceres::TrivialLoss();
  ceres::LossFunction* Rloss_function = new ceres::TrivialLoss();
  ceres::LossFunction* tloss_function = new ceres::TrivialLoss();

  // Create residuals for each observation in the bundle adjustment problem. The
  // parameters for cameras and points are added automatically.
  ceres::Problem problem_BundleAdjustment;
  ceres::Problem problem_PoseGraph;
  ceres::Problem Rproblem; // rotation only
  ceres::Problem tproblem; // translation only

  //----------------------------------------------------------------

  cout << "Building problem" << endl;

  for (int i = 0; i < nObs; i++) {
    double *cameraPtr = cameraParameter_BundleAdjustment + pointObservedIndex[2*i] * 6;
    double *observePtr = pointObservedValue + 6*i;
    double *pointPtr = pointCloud + pointObservedIndex[2*i + 1] * 3;

    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<AlignmentError3D, 3, 6, 3>(new AlignmentError3D(observePtr));
    problem_BundleAdjustment.AddResidualBlock(cost_function, loss_function_BundleAdjustment, cameraPtr, pointPtr);
  }

  // Loop over all matched pairs
  for (int i = 0; i < nPairs; i++) {

    // cout << cameraRt_ij_indices[2 * i] << " " << cameraRt_ij_indices[2 * i + 1] << endl;
    // Get relative pose between matched frames
    double *Rt_ij = cameraParameter_ij + 6 * i;

    // Get initial values for Rt_i and Rt_j. Note that we subtract one
    // from our offset because MATLAB is 1-indexed. Note that these
    // transforms are camera to world coordinates
    double *Rt_i = cameraParameter_PoseGraph + 6 * (cameraRt_ij_indices[2 * i] - 1);
    // printRt(Rt_i, 6);
    // printRt(cameraRt + 12 * (cameraRt_ij_indices[2 * i] - 1), 12);

    double *Rt_j = cameraParameter_PoseGraph + 6 * (cameraRt_ij_indices[2 * i + 1] - 1);
    // printRt(Rt_j, 6);
    // printRt(cameraRt + 12 * (cameraRt_ij_indices[2 * i + 1] - 1), 12);

    double z[3] = {0, 0, 1};
    double v_ij_observed[3];
    double v_ij_predicted[3];

    ceres::AngleAxisRotatePoint(Rt_ij, z, v_ij_observed);
    cout << "observed: " << v_ij_observed[0] << " " << v_ij_observed[1] << " " << v_ij_observed[2] << endl;

    double IR_i[3] = {-Rt_i[0], -Rt_i[1], -Rt_i[2]};

    ceres::AngleAxisRotatePoint(Rt_j, z, v_ij_predicted);
    ceres::AngleAxisRotatePoint(IR_i, v_ij_predicted, v_ij_predicted);
    cout << "predicted: " << v_ij_predicted[0] << " " << v_ij_predicted[1] << " " << v_ij_predicted[2] << endl;

     // Create observed translation vector
    double t_ij_observed[3] = {Rt_ij[3], Rt_ij[4], Rt_ij[5]};
    ceres::AngleAxisRotatePoint(Rt_i, t_ij_observed, t_ij_observed);
    cout << "observed: " << t_ij_observed[0] << " " << t_ij_observed[1] << " " << t_ij_observed[2] << endl;

    // Create predicted translation vector. Note this should be t_j -
    // t_i, but because Rt_j and Rt_i are world-to-camera and Rt_ij is
    // camera-to-world``
    double t_ij_predicted[3] = {Rt_j[3] - Rt_i[3], Rt_j[4] - Rt_i[4], Rt_j[5] - Rt_i[5]};
    // ceres::AngleAxisRotatePoint(IR_i, t_ij_predicted, t_ij_predicted);
    cout << "predicted: " << t_ij_predicted[0] << " " << t_ij_predicted[1] << " " << t_ij_predicted[2] << endl;

    cout << endl;

    double weight = cameraRt_ij_indices[2 * i + 1] - cameraRt_ij_indices[2 * i] == 1 ? -50.0 : -1.0;

    double points[3*(NUM_BA_POINTS + 2)];

    // z
    points[0] = 0;
    points[1] = 0;
    points[2] = 1;

    // y
    points[3] = 0;
    points[4] = 1;
    points[5] = 0;

    // ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<PoseGraphError, 6, 6, 6>(new PoseGraphError(Rt_ij, weight));
    // problem_PoseGraph.AddResidualBlock(cost_function, loss_function_PoseGraph, Rt_i, Rt_j);

    // ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<PoseGraphUpError, 9, 6, 6>(new PoseGraphUpError(Rt_ij, weight));
    // problem_PoseGraph.AddResidualBlock(cost_function, loss_function_PoseGraph, Rt_i, Rt_j);

    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<PoseGraphBAError, 3 + 3 * (NUM_BA_POINTS + 2), 6, 6>(new PoseGraphBAError(Rt_ij, points, weight));
    problem_PoseGraph.AddResidualBlock(cost_function, loss_function_PoseGraph, Rt_i, Rt_j);

    // ceres::CostFunction *Rcost_function = new ceres::AutoDiffCostFunction<PoseGraphRotationError, 3, 3, 3>(new PoseGraphRotationError(Rt_ij, weight));
    // Rproblem.AddResidualBlock(Rcost_function, Rloss_function, Rt_i, Rt_j);

    // ceres::CostFunction *tcost_function = new ceres::AutoDiffCostFunction<PoseGraphTranslationError, 3, 3, 3>(new PoseGraphTranslationError(Rt_ij + 3, weight));
    // tproblem.AddResidualBlock(tcost_function, tloss_function, Rt_i + 3, Rt_j + 3);
  }

  //----------------------------------------------------------------
  // Make Ceres automatically detect the bundle structure. Note that the
  // standard solver, SPARSE_NORMAL_CHOLESKY, also works fine but it is slower
  // for standard bundle adjustment problems.
  ceres::Solver::Summary summary_BundleAdjustment;
  ceres::Solver::Summary summary_PoseGraph;
  ceres::Solver::Summary Rsummary;
  ceres::Solver::Summary tsummary;

  cout << "Starting rotation solver" << endl;
  // ceres::Solve(options, &Rproblem, &Rsummary);
  cout << Rsummary.BriefReport() << endl;

  cout << "Starting translation solver" << endl;
  // ceres::Solve(options, &tproblem, &tsummary);
  cout << tsummary.BriefReport() << endl;

  cout << "Starting full pose graph solver" << endl;
  ceres::Solve(options, &problem_PoseGraph, &summary_PoseGraph);
  cout << summary_PoseGraph.BriefReport() << endl;

  cout << "Starting full bundle adjustment solver" << endl;
  // ceres::Solve(options, &problem_BundleAdjustment, &summary_BundleAdjustment);
  cout << summary_BundleAdjustment.BriefReport() << endl;

  cout<<" fx: "<<focalLen[0]<<" fy: "<<focalLen[1]<<endl;

  // obtain camera matrix from parameters
  for(int cameraID=0; cameraID<nCam; ++cameraID){
    double* cameraPtr = cameraParameter_PoseGraph+6*cameraID;
    double* cameraMat = cameraRtC2W_PoseGraph+12*cameraID;
    if (!(isnan(*cameraPtr))){
      ceres::AngleAxisToRotationMatrix<double>(cameraPtr, cameraMat);
      cameraMat[9]  = cameraPtr[3];
      cameraMat[10] = cameraPtr[4];
      cameraMat[11] = cameraPtr[5];
    }
  }

  for(int cameraID=0; cameraID<nCam; ++cameraID){
    double* cameraPtr = cameraParameter_BundleAdjustment+6*cameraID;
    double* cameraMat = cameraRtW2C_BundleAdjustment+12*cameraID;
    if (!(isnan(*cameraPtr))){
      ceres::AngleAxisToRotationMatrix<double>(cameraPtr, cameraMat);
      cameraMat[9]  = cameraPtr[3];
      cameraMat[10] = cameraPtr[4];
      cameraMat[11] = cameraPtr[5];
    }
  }

  // write back result files
  FILE* fpout = fopen(argv[4],"wb");
  fwrite((void*)(&nCam), sizeof(unsigned int), 1, fpout);
  fwrite((void*)(&nPts), sizeof(unsigned int), 1, fpout);
  fwrite((void*)(cameraRtC2W_PoseGraph), sizeof(double), 12*nCam, fpout);
  fwrite((void*)(cameraRtW2C_BundleAdjustment), sizeof(double), 12*nCam, fpout);
  fwrite((void*)(pointCloud), sizeof(double), 3*nPts, fpout);
  fwrite((void*)(focalLen), sizeof(double), 2, fpout);

  fclose (fpout);

  // clean up
  delete [] cameraRtC2W_PoseGraph;
  delete [] cameraRtW2C_BundleAdjustment;
  delete [] pointCloud;
  delete [] cameraRt_ij;
  delete [] cameraRt_ij_indices;
  delete [] cameraRt_ij_points;
  delete [] cameraRt_ij_points_count;
  delete [] pointObservedIndex;
  delete [] pointObservedValue;
  delete [] cameraParameter_PoseGraph;
  delete [] cameraParameter_BundleAdjustment;
  delete [] cameraParameter_ij;

  return 0;
}
