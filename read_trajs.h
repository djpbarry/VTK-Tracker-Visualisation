#ifndef _READ_TRAJS_
#define _READ_TRAJS_

#define MAX_LINE 4000
#define FRAMES "FRAMES"
#define TRAJS "TRAJECTORIES"
#define WIDTH "WIDTH"
#define HEIGHT "HEIGHT"
//#define DIM "DIM"
#define _trajfilename_ "C:/Users/barry05/Desktop/Test_Data_Sets/Tracking_Test_Sequences/TiffSim13/CudaOutput/tracker_data.txt"

extern bool loadTrajectories(FILE* file, double* trajectories, int rows, int cols, int maxline, int N);
extern bool loadFloatParam(const char *names, int numParams, void *params, char* format, int maxline, char numType, FILE *inputFile);
void removeSpace(char *str);

#endif