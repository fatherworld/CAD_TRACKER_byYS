#ifndef __CADINTERFACE_H_
#define __CADINTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

	// ����ģ���ļ������ٿռ�, 
	void * initCad(char * modelPath, int width, int height);

	int CadDetect(void * ptrInfo, unsigned char * data, int format);

	int CadDetect_Track(void * ptrInfo, unsigned char * data, int format);

	int uintCad(void * ptrInfo);


#ifdef __cplusplus
}
#endif

#endif