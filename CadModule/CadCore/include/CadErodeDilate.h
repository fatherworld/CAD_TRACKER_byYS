#ifndef __CadErodeDilate_H_
#define __CadErodeDilate_H_

#ifdef __cplusplus
extern "C"
{
#endif
	// ��ʴ���� ��������src���������Ľ��Ҳ��src�� tempBufferֻ����ʱ��buffer��src��С��ͬ
	int CadErodeDilate(float* src, int dataWidth, int dataHeight, int eroDilWH, float * tempBuffer);

#ifdef __cplusplus
}
#endif


#endif
