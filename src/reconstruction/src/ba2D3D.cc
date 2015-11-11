#include <cmath>
#include <cstdio>
#include <iostream>
#include <Eigen/Dense>
#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "glog/logging.h"

using namespace std;

int NUM_BA_POINTS;

struct BundleAdjustmentError {
  BundleAdjustmentError(double* m_point_observed, double m_weight):
    m_point_observed(m_point_observed), m_weight(m_weight) {}

  template <typename T>
  bool operator()(const T* const Rt,
                  const T* const m_point_predicted,
                  T* residuals) const {

    // T-ify the weight and points
    T weight = T(m_weight);
    T point_observed[3] = {T(m_point_observed[0]), T(m_point_observed[1]), T(m_point_observed[2])};
    T point_predicted[3] = {m_point_predicted[0], m_point_predicted[1], m_point_predicted[2]};

    // Rt[0,1,2] are the angle-axis rotation.
    ceres::AngleAxisRotatePoint(Rt, point_observed, point_observed);

    // Rt[3,4,5] are the translation.
    point_observed[0] += Rt[3];
    point_observed[1] += Rt[4];
    point_observed[2] += Rt[5];

    // The error is the difference between the predicted and observed position.
    residuals[0] = weight * (point_predicted[0] - point_observed[0]);
    residuals[1] = weight * (point_predicted[1] - point_observed[1]);
    residuals[2] = weight * (point_predicted[2] - point_observed[2]);

    return true;
  }

  double *m_point_observed;
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
    // Get inverse rotation of R_i and translation of t_i
    T IR_i[3] = {-Rt_i[0], -Rt_i[1], -Rt_i[2]};
    T It_i[3] = {-Rt_i[3], -Rt_i[4], -Rt_i[5]};
    ceres::AngleAxisRotatePoint(IR_i, It_i, It_i);

    // Z and y vectors to constrain rotation
    T points[6] = {T(0), T(0), T(1), T(0), T(1), T(0)};

    // Loop through z and y vectors
    for (int i = 0; i < 2; i++) {
      T p_ij_observed[3];
      T p_ij_predicted[3];

      // Compute observed view direction vector from world to camera by
      // relative rotation between i and j
      ceres::AngleAxisRotatePoint(R_ij, points + 3*i, p_ij_observed);

      // Rotate from according to j's rotation
      ceres::AngleAxisRotatePoint(Rt_j, points + 3*i, p_ij_predicted);

      // Rotate back from according to i's rotation
      ceres::AngleAxisRotatePoint(IR_i, p_ij_predicted, p_ij_predicted);

      residuals[3 + 3 * i] = weight * (p_ij_predicted[0] - p_ij_observed[0]);
      residuals[4 + 3 * i] = weight * (p_ij_predicted[1] - p_ij_observed[1]);
      residuals[5 + 3 * i] = weight * (p_ij_predicted[2] - p_ij_observed[2]);
    }

    return true;
  }

  double *m_Rt_ij;
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
  NUM_BA_POINTS = atof(argv[2]);

  // Open input file
  FILE* fp = fopen(argv[3],"rb");
  if (fp == NULL) {
    cout << "fail to open file" << endl;
    return false;
  }

  // Get number of camera poses (i.e., frames; assumes that each pose
  // uses the same camera)
  unsigned int nCam;  fread((void*)(&nCam), sizeof(unsigned int), 1, fp);

  // Get number of matched pairs
  unsigned int nPairs; fread((void*)(&nPairs), sizeof(unsigned int), 1, fp);

  // Get number of matched key points
  unsigned int nMatchedPoints; fread((void*)(&nMatchedPoints), sizeof(unsigned int), 1, fp);

  // Read all of the extrinsic matrices for the nCam camera
  // poses. Converts camera coordinates into world coordinates
  double* cameraRtC2W = new double [12*nCam];
  cout << "Reading cameraRtC2W" << endl;
  fread((void*)(cameraRtC2W), sizeof(double), 12*nCam, fp);

  // Read all relative poses for each matched pair
  double *cameraRt_ij = new double[12*nPairs];
  cout << "Reading cameraRt_ij" << endl;
  fread((void*)(cameraRt_ij), sizeof(double), 12*nPairs, fp);

  //Read all camera pose indices for matched pairs
  unsigned int *cameraRt_ij_indices = new unsigned int [2*nPairs];
  cout << "Reading cameraRt_ij_indices" << endl;
  fread((void*)(cameraRt_ij_indices), sizeof(unsigned int), 2*nPairs, fp);

  // Read all of the matched key points for a given pair (3D
  // coordinates in i's coordinates)
  double *cameraRt_ij_points_observed_i = new double[3*nMatchedPoints];
  cout << "Reading cameraRt_ij_points_observed_i" << endl;
  fread((void*)(cameraRt_ij_points_observed_i), sizeof(double), 3*nMatchedPoints, fp);

  // Read all of the matched key points for a given pair (3D
  // coordinates in i's coordinates)
  double *cameraRt_ij_points_observed_j = new double[3*nMatchedPoints];
  cout << "Reading cameraRt_ij_points_observed_j" << endl;
  fread((void*)(cameraRt_ij_points_observed_j), sizeof(double), 3*nMatchedPoints, fp);

  // Read all of the matched key points for a given pair (3D
  // coordinates in world frame)
  double *cameraRt_ij_points_predicted = new double[3*nMatchedPoints];
  cout << "Reading cameraRt_ij_points_predicted" << endl;
  fread((void*)(cameraRt_ij_points_predicted), sizeof(double), 3*nMatchedPoints, fp);

  // Read the count of matched key points per pair so we can index
  // correctly
  unsigned int *cameraRt_ij_points_count = new unsigned int [nPairs];
  cout << "Reading cameraRt_ij_points_count" << endl;
  fread((void*)(cameraRt_ij_points_count), sizeof(unsigned int), nPairs, fp);

  // finish reading
  fclose(fp);

  // Construct camera parameters from camera matrix
  double* cameraParameter = new double [6*nCam];

  cout << "Converting camera parameters" << endl;
  // Loop through all poses
  for(int cameraID = 0; cameraID < nCam; cameraID++) {
    double* cameraPtr = cameraParameter + 6 * cameraID;
    double* cameraMat = cameraRtC2W + 12 * cameraID;

    if (!(isnan(*cameraPtr))) {
      // Converting column-major order cameraMat into first three
      // elements of cameraPtr
      ceres::RotationMatrixToAngleAxis<double>(cameraMat, cameraPtr);

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
  ceres::LossFunction* loss_function_PoseGraph = new ceres::TrivialLoss();
  ceres::LossFunction* loss_function_BundleAdjustment = new ceres::TrivialLoss();

  // Create residuals for each observation in the bundle adjustment problem. The
  // parameters for cameras and points are added automatically.
  ceres::Problem problem_PoseGraph;
  ceres::Problem problem_BundleAdjustment;

  // Which matched point are we on?
  unsigned int points_count = 0;
  unsigned int cameraRt_ij_points_total = 0;
  double *points_observed_i = new double[3 * NUM_BA_POINTS * nPairs];
  double *points_observed_j = new double[3 * NUM_BA_POINTS * nPairs];
  double *points_predicted = new double[3 * NUM_BA_POINTS * nPairs];

  cout << "Building problem" << endl;

  // TODO: compute inverse outside
  for (int i = 0; i < nPairs; i++) {
    double *cameraMat = cameraRt_ij + 12 * i;

    // Get relative pose between matched frames
    double Rt_ij[6];

    // Converting column-major order cameraMat into first three
    // elements of cameraPtr
    ceres::RotationMatrixToAngleAxis<double>(cameraMat, Rt_ij);

    // Grabbing translation
    Rt_ij[3] = cameraMat[9];
    Rt_ij[4] = cameraMat[10];
    Rt_ij[5] = cameraMat[11];

    // Get initial values for Rt_i and Rt_j. Note that we subtract one
    // from our offset because MATLAB is 1-indexed. Note that these
    // transforms are camera to world coordinates
    double *Rt_i = cameraParameter + 6 * (cameraRt_ij_indices[2 * i] - 1);
    double *Rt_j = cameraParameter + 6 * (cameraRt_ij_indices[2 * i + 1] - 1);

    double weight = cameraRt_ij_indices[2 * i + 1] - cameraRt_ij_indices[2 * i] == 1 ? -50.0 : -1.0;
    double point_weight = cameraRt_ij_indices[2 * i + 1] - cameraRt_ij_indices[2 * i] == 1 ? 0.1 : 1;

    // We want to add some number of points to constrain rotations
    int num_points = min(NUM_BA_POINTS, (int)cameraRt_ij_points_count[i]);

    for (int j = 0; j < num_points; j++) {
      int index = (points_count + j) * 3;
      int t_index = (cameraRt_ij_points_total + j) * 3;

      // Fill up observed points
      points_observed_i[index] = cameraRt_ij_points_observed_i[t_index];
      points_observed_i[index + 1] = cameraRt_ij_points_observed_i[t_index + 1];
      points_observed_i[index + 2] = cameraRt_ij_points_observed_i[t_index + 2];

      points_observed_j[index] = cameraRt_ij_points_observed_j[t_index];
      points_observed_j[index + 1] = cameraRt_ij_points_observed_j[t_index + 1];
      points_observed_j[index + 2] = cameraRt_ij_points_observed_j[t_index + 2];

      // Fill up predicted points
      points_predicted[index] = cameraRt_ij_points_predicted[t_index];
      points_predicted[index + 1] = cameraRt_ij_points_predicted[t_index + 1];
      points_predicted[index + 2] = cameraRt_ij_points_predicted[t_index + 2];

      double *point_observed_i = points_observed_i + index;
      double *point_observed_j = points_observed_j + index;
      double *point_predicted = points_predicted + index;

      ceres::CostFunction *point_cost_function_i = new ceres::AutoDiffCostFunction<BundleAdjustmentError, 3, 6, 3>(new BundleAdjustmentError(point_observed_i, point_weight));
      problem_BundleAdjustment.AddResidualBlock(point_cost_function_i, loss_function_BundleAdjustment, Rt_i, point_predicted);

      ceres::CostFunction *point_cost_function_j = new ceres::AutoDiffCostFunction<BundleAdjustmentError, 3, 6, 3>(new BundleAdjustmentError(point_observed_j, point_weight));
      problem_BundleAdjustment.AddResidualBlock(point_cost_function_j, loss_function_BundleAdjustment, Rt_j, point_predicted);
    }

    // ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<PoseGraphError, 9, 6, 6>(new PoseGraphError(Rt_ij, weight));
    // problem_PoseGraph.AddResidualBlock(cost_function, loss_function_PoseGraph, Rt_i, Rt_j);

    points_count += num_points;
    cameraRt_ij_points_total += cameraRt_ij_points_count[i];
  }

  //----------------------------------------------------------------

  //----------------------------------------------------------------
  // Make Ceres automatically detect the bundle structure. Note that the
  // standard solver, SPARSE_NORMAL_CHOLESKY, also works fine but it is slower
  // for standard bundle adjustment problems.
  ceres::Solver::Summary summary_BundleAdjustment;
  ceres::Solver::Summary summary_PoseGraph;
  ceres::Solver::Summary summary_PoseGraphBA;
  ceres::Solver::Summary Rsummary;
  ceres::Solver::Summary tsummary;


  cout << "Starting full pose graph solver" << endl;
  // ceres::Solve(options, &problem_PoseGraph, &summary_PoseGraph);
  cout << summary_PoseGraph.BriefReport() << endl;

  cout << "Starting pose graph BA solver" << endl;
  ceres::Solve(options, &problem_BundleAdjustment, &summary_PoseGraphBA);
  cout << summary_PoseGraphBA.BriefReport() << endl;

  // obtain camera matrix from parameters
  for(int cameraID=0; cameraID<nCam; cameraID++){
    double* cameraPtr = cameraParameter + 6 * cameraID;
    double* cameraMat = cameraRtC2W + 12 * cameraID;
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
  fwrite((void*)(cameraRtC2W), sizeof(double), 12*nCam, fpout);

  fclose (fpout);

  // clean up
  delete [] cameraRtC2W;
  delete [] cameraRt_ij;
  delete [] cameraRt_ij_indices;
  delete [] cameraRt_ij_points_observed_i;
  delete [] cameraRt_ij_points_observed_j;
  delete [] cameraRt_ij_points_predicted;
  delete [] cameraRt_ij_points_count;
  delete [] cameraParameter;
  delete [] points_observed_i;
  delete [] points_observed_j;
  delete [] points_predicted;

  return 0;
  }
