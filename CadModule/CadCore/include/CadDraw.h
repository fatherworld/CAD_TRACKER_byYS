#ifndef __CadDraw_H_
#define __CadDraw_H_

#ifdef __cplusplus
extern "C"
{
#endif

	int CadDrawLine(unsigned char * src, int width, int height, int x0, int y0, int x1, int y1, int shickness);

	int CadDrawAllLine(void * ptrCadInfo);

#ifdef __cplusplus
}
#endif


#endif
