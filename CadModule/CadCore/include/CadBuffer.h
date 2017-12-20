#ifndef __CadBuffer_H_
#define  __CadBuffer_H_

#ifdef __cplusplus
extern "C"
{
#endif

	int initBuffer(void * ptrCadInfo, int width, int height);

	int memcpyNoZeroGradient(void * ptrCadInfo);

	int SaveDetectBeforAfter(void * ptrInfo, int offsetd[2]);

	int FirstTrack(void * ptrInfo, int offsetd[2], float * GradientLen, int width, int height);

	int FirstTrack_1(void * ptrInfo, int offsetd[2], float * GradientLen, int width, int height);

#ifdef __cplusplus
}
#endif
#endif

