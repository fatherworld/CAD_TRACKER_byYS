#ifndef __CadGAOSI_H_
#define __CadGAOSI_H_

#ifdef __cplusplus
extern "C"
{
#endif

	int makeGaussianData(float ** data, int width, int height, float filterRatio);

	int Gaussian(unsigned char * src, int width, int height, unsigned char * dest, float * data, int Size, int channel);

	int GaussianCon(float * src, int width, int height, float * GaussianKernal, int kernelSize, int channel, float * out);

#ifdef __cplusplus
}
#endif


#endif
