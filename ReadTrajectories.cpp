
#include "stdafx.h"
#include <stdlib.h>
#include <cstring>
#include "read_trajs.h"
#include <boost/math/special_functions/round.hpp>

using namespace boost;

extern bool loadFloatParam(const char *names, int numParams, void *params, char* format, int maxline, char numType, FILE *inputFile) {
	char *line = (char*) malloc(maxline * sizeof(char));
	char *paramName = (char*) malloc(maxline * sizeof(char));
	float temp;
	if(fgets(line, maxline, inputFile) != NULL){
		sscanf_s(line, format, paramName, maxline, &temp);
		for(int i = 0; i < numParams; i++){
			if(strcmp(paramName, &names[i * maxline]) == 0){
				switch(numType){
					case 'i':
						((int*)params)[i] = (int)boost::math::round<double>(temp);
						break;
					case 'l':
						((long*)params)[i] = (long)boost::math::round<double>(temp);
						break;
					case 'f':
						((float*)params)[i] = temp;
						break;
					default:
						((double*)params)[i] = temp;
				}
				return true;
			}
		}
	}
	return false;
}

extern bool loadTrajectories(FILE* file, double* trajectories, int rows, int cols, int maxline, int N){
	char* line = (char*)malloc(sizeof(char) * maxline);
	char* temp = (char*)malloc(sizeof(char) * maxline);
	temp = "This is unlikely to be the same as what's stored in 'line'.";
	int colcheck = 0, rowcheck = 0;
	//int collimit = N * cols;
	for(int j=0; j<rows; j++){
		if((fgets(line, maxline, file) != NULL)){
			rowcheck++;
			//for(int i=0; i<collimit; i++){
			while(!(line[0]==32 && line[1]==10)){
				trajectories[colcheck] = strtod(line, &temp);
				_ASSERTE( _CrtCheckMemory( ) );
				//[DEBUG]
				//printf("%d, %d\n", rowcheck, colcheck);
				//[/DEBUG]
				memcpy(line, temp, maxline);
				colcheck++;
			}
		}
	}
	if(rowcheck != rows || (colcheck / (rowcheck * N)) != cols){
		return false;
	} else {
		return true;
	}
}
