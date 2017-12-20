#ifndef __MATRIX_Alva_H_
#define __MATRIX_Alva_H_
//矩阵的数据结构
#ifdef __cplusplus
extern "C"
{
#endif 

//矩阵结构体
typedef struct _MARTIX
{
    int rows;   //矩阵行数
    int cols;   //矩阵列数
    float* martix;

} MARTIX_Alva;


/*
    pragma one:左乘矩阵
    pragma two:右乘矩阵
    pragma three:结果矩阵
    THE FUNCTION IS JUST FOR TWO MARTIX
*/
int mul_maritx(MARTIX_Alva input_martix_one, MARTIX_Alva input_martix_two, MARTIX_Alva* output_martix);

int EulerAng2Rotate_(float RotX, float RotY, float RotZ, float R[9]);

int RoAndTranToRT(float RotX, float RotY, float RotZ, float Tx, float Ty, float Tz, float RT[16]);

int ThreeDim2SecDimPoint(float input[3], float InReference[3 * 4], float RT[4 * 4], float output[2]);

int RT2RotateAndTrant(float RT[16], float Angle[3], float Tran[3]);

int translate_martix(MARTIX_Alva input_martix, MARTIX_Alva* output_martix);

int converse_martix(MARTIX_Alva input_martix, MARTIX_Alva* output_martix);

int num_mul_matrix(MARTIX_Alva input_martix, float scale, MARTIX_Alva* output_martix);

int mySelf_add_maritx(MARTIX_Alva* output_martix, MARTIX_Alva input_martix_left);

int powAndJieChe2(MARTIX_Alva input_martix, MARTIX_Alva* output_martix);

int powAndJieChe3(MARTIX_Alva input_martix, MARTIX_Alva* output_martix);

int powAndJieChe4(MARTIX_Alva input_martix, MARTIX_Alva* output_martix);

int RTobj2RTcam(float RT_o[16], float RT_c[16]);

int RoAndTranToRT_W2C(float RotX, float RotY, float RotZ, float Px, float Py, float Pz, float RT[16]);

#ifdef __cplusplus
}
#endif

#endif