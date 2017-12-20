#pragma once
#ifndef LineTriangleIntersect_h
#define LineTriangleIntersect_h
#define FINDINTRIANGLE(a,b,c) ((a+b)/2+c)/2
#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif 
    /*
    pragma one:ֱ�������������
    pragma two:ֱ�������������
    pragma three:����Ƭ�����������
    pragma four:����Ƭ��ķ�������Ϣ
    pragma five:��������
    */

    //��ά�����
    typedef struct _3Dimension
    {
        float real_x;
        float real_y;
        float real_z;
    } threespace;

    typedef struct __CALCFIVE {
        float  CutSurfaces[16] = { 0.0f };
        float sign[3] = { 0.0f };
    } CalcFives;


    int CalcFive(void* ptr, CalcFives* Calcs);

    bool ValidPoint_byArea(threespace LinePoint1, threespace LinePoint2, threespace TF[3], float CutSurface[4], threespace* result);

    bool ValidPointf_byThree(threespace LinePoint1, threespace LinePoint2, CalcFives TrianglePoint, threespace* result);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // !LineTriangleIntersect_h
