#ifndef __CadGradient_H_
#define __CadGradient_H_

#ifdef __cplusplus
extern "C"
{
#endif

	int CadGradient(float* input_data, int width, int height, float* gradientResult);

	void SobelFilter(float* grayImg, int width, int height, float* sobelX, float* sobelY);
	int  CadGradient1(float* input_data, int width, int height, float* gradientResult, float * gradientLen, float LenMinMax[2]);

#ifdef __cplusplus
}
#endif


#endif
