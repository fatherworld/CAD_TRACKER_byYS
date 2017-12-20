#ifndef __CadPoints_H_
#define __CadPoints_H_

#ifdef __cplusplus
extern "C"
{
#endif


	int Updata2DPoint(void * ptrCadInfo_);

	int FindLinePoint(void * ptrCadInfo_);

	int LookForNew2DPoint(void *ptrCadInfo_, float * GradientLen, int width, int height);

	int LookForNew2DPoint_1(void *ptrCadInfo_, float * GradientLen, int width, int height);

	int LookForNew2DPoint_cv(void *ptrCadInfo_, float * GradientLen, float * GradientAng, int width, int height);

#ifdef __cplusplus
}
#endif
#endif