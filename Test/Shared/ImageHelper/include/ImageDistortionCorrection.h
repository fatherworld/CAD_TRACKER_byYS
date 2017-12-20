/*
 * ͼƬ�������
 */
#ifndef _ImageDistortionCorrection_h_
#define _ImageDistortionCorrection_h_

typedef enum _DistortionCorrectionType_ {
    DC_Start = -1,
    DC_H,
}DCType;

#ifdef __cplusplus
extern "C"
{
#endif
    /*
     * ImageDistortionCorrection ��uChar�Ҷ�ͼ�Ļ������
     * @inGray : ����ĻҶ�ͼ
     * @inWidth: ����Ҷ�ͼ�Ŀ�
     * @inHeight: ����Ҷ�ͼ�ĸ�
     * @outGray: ����ĻҶ�ͼ
     * @outWidth: ����Ҷ�ͼ�Ŀ�
     * @outHeight: ����Ҷ�ͼ�ĸ�
     * @DCData : ���ν�����Ҫ�õ��Ĳ���
     * @DCType: ���ν�����ģʽ
     * return: null
     * ע: ���ͼ���ڴ�ռ���Ҫʹ���߿���  �ͷ�
     */
    void ImageDistortionCorrection(unsigned char* inGray, int inWidth, int inHeight, unsigned char* outGray, int outWidth, int outHeight, float* DCData, DCType dcType);

#ifdef __cplusplus
}
#endif

#endif
