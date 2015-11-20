#include "utilities.h"

char *substr(char *arr, int begin, int len) {
  char *res = (char *)malloc(sizeof(char) * len);
  for (int i = 0; i < len; i++)
    res[i] = *(arr + begin + i);
  res[len] = 0;

  return res;
}

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir) {

  // Find the string length of the directory path
  int len = 0;
  while (*(dir + len) != 0) len++; len++;

  char *p = NULL;

  // Remove any trailing /
  if (dir[len - 1] == '/') {
    dir[len - 1] = 0;
  }

  struct stat st = {0};

  // Loop through each character in the directory path
  for (p = dir + 1; *p; p++) {

    // If the character is /, temporarily replace with \0 to terminate
    // string and create directory at the parent path
    if (*p == '/') {

      *p = 0;

      if (stat(dir, &st) == -1) {
        mkdir(dir, mode);
      }

      // Change \0 back to /
      *p = '/';
    }
  }

  // Create the last directory in the hierarchy
  if (stat(dir, &st) == -1 && is_dir) {
    mkdir(dir, mode);
  }
}

void print_pathname_from_file_pointer(FILE *fp) {  
  // TODO: Make this work on Linux
  // int fno = fileno(fp);;

  // char filePath[PATH_MAX];

  // if (fp != NULL && fcntl(fno, F_GETPATH, filePath) != -1) {
  //   fprintf(stderr, "Outfile exists at %s, (%d) %p\n", filePath, fno, (void *) fp);
  // } else {
  //   fprintf(stderr, "Outfile doesn't exist!\n");
  // }
}

// void write_plys(const char *plyfile, const vector<Frames *>frames) {
//   FILE *fp = fopen(plyfile,"w");
//   int pointCount = 0;
//   for (int i = 0; i < )
// }

// void write_plys(const char* plyfile, const cv::Mat pointCloud, const cv::Mat color){
//   FILE *fp = fopen(plyfile,"w");
//   int pointCount =0;
//   for (int v = 0; v < pointCloud.size().height; ++v) {
//     if (pointCloud.at<float>(v,2)>0.0001){
//       pointCount++;
//     }
//   }

//   cout << "Write PLY" << endl;

//   fprintf(fp, "ply\n");
//   fprintf(fp, "format binary_little_endian 1.0\n");
//   fprintf(fp, "element vertex %d\n", pointCount);
//   fprintf(fp, "property float x\n");
//   fprintf(fp, "property float y\n");
//   fprintf(fp, "property float z\n");
//   fprintf(fp, "property uchar red\n");
//   fprintf(fp, "property uchar green\n");
//   fprintf(fp, "property uchar blue\n");
//   fprintf(fp, "end_header\n");

//   for (int v = 0; v < pointCloud.size().height; ++v) {
//     if (pointCloud.at<float>(v,2)>0.0001){
//       fwrite(&pointCloud.at<float>(v,0), sizeof(float), 1, fp);
//       fwrite(&pointCloud.at<float>(v,1), sizeof(float), 1, fp);
//       fwrite(&pointCloud.at<float>(v,2), sizeof(float), 1, fp);
//       int i= (int)v/color.size().width;
//       fwrite(&color.at<cv::Vec3b>(i,j)[2], sizeof(uchar), 1, fp);
//       int j= (int)v%color.size().width;
//       fwrite(&color.at<cv::Vec3b>(i,j)[1], sizeof(uchar), 1, fp);
//       fwrite(&color.at<cv::Vec3b>(i,j)[0], sizeof(uchar), 1, fp);
//     }
//   }
//   fclose(fp);
// }