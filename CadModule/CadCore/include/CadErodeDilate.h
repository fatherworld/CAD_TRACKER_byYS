#ifndef __CadErodeDilate_H_
#define __CadErodeDilate_H_

#ifdef __cplusplus
extern "C"
{
#endif
	// 腐蚀膨胀 会对输入的src处理，处理后的结果也在src， tempBuffer只是临时的buffer和src大小相同
	int CadErodeDilate(float* src, int dataWidth, int dataHeight, int eroDilWH, float * tempBuffer);

#ifdef __cplusplus
}
#endif


#endif
