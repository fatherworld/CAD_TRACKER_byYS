#ifndef _CadCutImage_h_
#define  _CadCutImage_h_

#ifdef __cplusplus
extern "C"
{
#endif
	int cutImageResult(unsigned char * src, unsigned char * desData, int srcW, int srcH, int x0, int y0, int x1, int y1, int channel);

	int convertU1ToF1(unsigned char * src, float * des, int width, int height);

	int LinearImage(unsigned char * srcImage, unsigned char * destImage, int srcW, int srcH, int destW, int destH, int channels);

	// 插值到目标图大小
	int cutImageAndLinear(unsigned char * src, unsigned char * desData, int srcW, int srcH, int linearW, int linearH, int x0, int y0, int x1, int y1, int channels);

	int NVLinearRect(unsigned char * srcImage, int srcW, int srcH, int x0, int y0, int x1, int y1, unsigned char *destImage, int lineW, int lineH);

	int NV21ToBGR(unsigned char * srcYVU, unsigned char * destBGR, int width, int height);

	int NV21ToRGB(unsigned char * srcYVU, unsigned char * dest, int width, int height);

#ifdef __cplusplus
}
#endif
#endif
