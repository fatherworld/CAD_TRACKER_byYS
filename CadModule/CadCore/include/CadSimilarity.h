#ifndef CADSIMILARITY_H_
#define CADSIMILARITY_H_
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#ifdef __cplusplus
extern "C"
{
#endif

	int findMaxSimilar(float* source_data, int source_width, int source_height,
		float* module_data, int module_width, int module_height, int offset_x,
		int offset_y, int step, int rangeNum, int noZeroNum, float thresh, int outOffset[2]);
 
	int findMaxSimilar_optimiza(float* source_data, int source_width, int source_height,
		float* module_data, int * module_index, int offset_x,
		int offset_y, int step, int rangeNum, int noZeroNum, float thresh, int outOffset[2]);

#ifdef __cplusplus
}
#endif

#endif

