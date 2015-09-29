
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

extern void loadTrajectories(FILE* file, double* trajectories, int rows, int cols, int maxline, int dim, int* counts){
	char* line = (char*)malloc(sizeof(char) * maxline);
	char* temp = (char*)malloc(sizeof(char) * maxline);
	temp = "This is unlikely to be the same as what's stored in 'line'.";
	int maxcol = cols*dim*rows;
	for(int j=0; j<rows; j++){
		int colOffset = j*cols*dim;
		int colIndex = colOffset;
		int rowcount=0;
		if((fgets(line, maxline, file) != NULL)){
			while(colIndex<maxcol && !(line[0]==32 && line[1]==10)){
				trajectories[colIndex] = strtod(line, &temp);
				_ASSERTE( _CrtCheckMemory( ) );
				memcpy(line, temp, maxline);
				colIndex++;
				if(colIndex%2==0)rowcount++;
			}
		}
		counts[j]=rowcount;
	}
	return;
}
