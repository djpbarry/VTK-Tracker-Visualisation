#ifndef _READ_TRAJS_
#define _READ_TRAJS_

#define MAX_LINE 4000
#define MAX_FILS 100
#define FRAMES "FRAMES"
//#define TRAJS "TRAJECTORIES"
#define WIDTH "WIDTH"
#define HEIGHT "HEIGHT"
//#define DIM "DIM"
#define _trajfilename_ "C:/Users/barry05/Desktop/Sim_Actin_Tails/Tue Sep 29 11-01-21 BST 2015_Output/55.0_Output/anim_data.txt"

extern void loadTrajectories(FILE* file, double* trajectories, int rows, int cols, int maxline, int dim, int* counts);
extern bool loadFloatParam(const char *names, int numParams, void *params, char* format, int maxline, char numType, FILE *inputFile);
void removeSpace(char *str);

#endif