#include "Config.h"
#include "Format.h"
#define LOG_TAG "martix"
#include "Log.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "CadMatrix.h"
#include "opencv2/opencv.hpp"



//矩阵相乘
int mul_maritx(MARTIX_Alva input_martix_left, MARTIX_Alva input_martix_right, MARTIX_Alva* output_martix)
{
    int ret = 0;
    if (input_martix_left.cols != input_martix_right.rows)
    {
        printf("不满足矩阵相乘条件");
        ret = -1;
        return ret;
    }
    if (output_martix == NULL)
    {
        printf("结果矩阵不能为空");
        ret = -2;
        return ret;
    }

    output_martix->cols = input_martix_right.cols;
    output_martix->rows = input_martix_left.rows;

    for (int i = 0; i < input_martix_left.rows; i++)
    {
        for (int j = 0;j<input_martix_right.cols;j++)
        {
            double temp = 0.0;
            for (int k = 0; k < input_martix_left.cols; k++)
            {
                float temp1 = input_martix_left.martix[i*input_martix_left.cols+k];
                float temp2 = input_martix_right.martix[k*input_martix_right.cols+j];
                temp += (double)(temp1 * temp2);
            }
            output_martix->martix[i*input_martix_right.cols+j]= (float)temp;
        }
    }

	return 0;
}

// 角度转化为旋转矩阵
int EulerAng2Rotate_(float RotX, float RotY, float RotZ, float R[9])
{
	/*float temp;
	temp = RotX;
	RotX = RotY;
	RotY = temp;*/
	/*RotX = 0;
	RotY = 0;*/
	float PI_m = 3.1415926f;
	RotX = RotX*PI_m / 180.0;
	RotY = RotY*PI_m / 180.0;
	RotZ = RotZ*PI_m / 180.0;

	float Cx, Sx, Cy, Sy, Cz, Sz;
	Cx = cos(RotX); Sx = sin(RotX);
	Cy = cos(RotY); Sy = sin(RotY);
	Cz = cos(RotZ); Sz = sin(RotZ);


	float Mz[9] = { 0.0f };
	Mz[0] = Cz;
	Mz[1] = -Sz;
	Mz[1 * 3] = Sz;
	Mz[1 * 3 + 1] = Cz;
	Mz[2 * 3 + 2] = 1;


	float Mx[9] = { 0.0f };
	float My[9] = { 0.0f };
	My[0] = Cy;
	My[2] = Sy;
	My[1 * 3 + 1] = 1;
	My[2 * 3 + 0] = -Sy;
	My[2 * 3 + 2] = Cy;

	Mx[0] = 1;
	Mx[1 * 3 + 1] = Cx;
	Mx[1 * 3 + 2] = -Sx;
	Mx[2 * 3 + 1] = Sx;
	Mx[2 * 3 + 2] = Cx;


	float outTemp[9] = { 0.0f };
	float out[9] = { 0.0f };

	MARTIX_Alva input_martix_one, input_martix_two, input_martix_third, output_martix, output_temp;

	input_martix_one.cols = 3;
	input_martix_one.rows = 3;
	input_martix_one.martix = Mx;

	input_martix_two.cols = 3;
	input_martix_two.rows = 3;
	input_martix_two.martix = My;

	input_martix_third.cols = 3;
	input_martix_third.rows = 3;
	input_martix_third.martix = Mz;

	output_temp.cols = 3;
	output_temp.rows = 3;
	output_temp.martix = outTemp;

	output_martix.cols = 3;
	output_martix.rows = 3;
	output_martix.martix = out;

	mul_maritx(input_martix_third, input_martix_two, &output_temp);

	mul_maritx(output_temp, input_martix_one, &output_martix);

	memcpy(R, output_martix.martix, sizeof(float) * 9);

	return 0;
}


int RoAndTranToRT(float RotX, float RotY, float RotZ, float Tx, float Ty, float Tz, float RT[16])
{
	int i = 0;
	int j = 0;
	memset(RT, 0, sizeof(float) * 16);
	float R[9] = { 0.0f };

	EulerAng2Rotate_(RotX, RotY, RotZ, R);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			RT[i * 4 + j] = R[i * 3 + j];
		}
	}
	RT[3]  = Tx;
	RT[7]  = Ty;
	RT[11] = -Tz;
	RT[15] = 1.0f;

	return 0;
}



int RoAndTranToRT_W2C(float RotX, float RotY, float RotZ, float Px, float Py, float Pz, float RT[16])
{
	int i = 0;
	int j = 0;
	memset(RT, 0, sizeof(float) * 16);
	float R[9] = { 0.0f };

	EulerAng2Rotate_(RotX, RotY, RotZ, R);
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			RT[i * 4 + j] = R[i * 3 + j];
		}
	}

	MARTIX_Alva RM;
	RM.cols = 3;
	RM.rows = 3;
	float RM_[9] = { 0.0f };
	RM.martix = RM_;
	memcpy(RM.martix, R, sizeof(float) * 9);

	MARTIX_Alva CM;
	float CM_[3] = { 0.0f };
	CM.rows = 3;
	CM.cols = 1;
	CM.martix = CM_;
	CM.martix[0] = -Px;
	CM.martix[1] = -Py;
	CM.martix[2] = -Pz;


	MARTIX_Alva Res;
	Res.cols = 1;
	Res.rows = 3;
	float Res_[3] = { 0.0f };
	Res.martix = Res_;

	mul_maritx(RM, CM, &Res);

	RT[3] = Res.martix[0];
	RT[7] = Res.martix[1];
	RT[11] = Res.martix[2];
	RT[15] = 1.0f;

	return 0;
}



int ThreeDim2SecDimPoint(float input[3], float InReference[3 * 4], float RT[4 * 4], float output[2])
{

	float temp[3 * 4] = { 0.0f };
	float threeDim[4 * 1] = { input[0], input[1], input[2], 1.0f };
	float secDim[3 * 1] = { 0.0f };

	MARTIX_Alva input_martix_one, input_martix_two, output_temp, in_point, out_point;

	input_martix_one.cols = 4;
	input_martix_one.rows = 3;
	input_martix_one.martix = InReference;

	input_martix_two.cols = 4;
	input_martix_two.rows = 4;
	input_martix_two.martix = RT;

	output_temp.cols = 4;
	output_temp.rows = 3;
	output_temp.martix = temp;

	in_point.cols = 1;
	in_point.rows = 4;
	in_point.martix = threeDim;

	out_point.cols = 1;
	out_point.rows = 3;
	out_point.martix = secDim;


	mul_maritx(input_martix_one, input_martix_two, &output_temp);

	mul_maritx(output_temp, in_point, &out_point);


	output[0] = out_point.martix[0] / out_point.martix[2];
	output[1] = out_point.martix[1] / out_point.martix[2];

	return 0;
}

int RT2RotateAndTrant(float RT[16], float Angle[3], float Tran[3])
{
	float r11, r21, r31, r32, r33;

	r11 = RT[0];
	r21 = RT[4];
	r31 = RT[8];
	r32 = RT[9];
	r33 = RT[10];

	float ThetaX, ThetaY, ThetaZ;
	float Rad2AngC = 180.0 / 3.1415926f;

	ThetaX = Rad2AngC*atan2(r32, r33);
	ThetaY = Rad2AngC*atan2(-r31, sqrt(r32*r32 + r33*r33));
	ThetaZ = Rad2AngC*atan2(r21, r11);

	Angle[0] = ThetaX;
	Angle[1] = ThetaY;
	Angle[2] = ThetaZ;

	Tran[0] = RT[3];
	Tran[1] = RT[7];
	Tran[2] = -RT[11];

	return 0;
}

// 4*4矩阵 从物体RT和相机RT相互转换
int RTobj2RTcam(float RT_o[16], float RT_c[16])
{

	MARTIX_Alva one;
	MARTIX_Alva two;

	one.cols = 4; one.rows = 4; one.martix = RT_o;
	two.cols = 4; two.rows = 4; two.martix = RT_c;
	
	one.martix = RT_o;
	two.martix = RT_c;

	converse_martix(one, &two);
#if 0
	cv::Mat mat0 = cv::Mat::zeros(4, 4, CV_32FC1);
	cv::Mat mat1 = cv::Mat::zeros(4, 4, CV_32FC1);
	memcpy(mat0.data, RT_o, sizeof(float) * 16);

	cv::invert(mat0, mat1);
	memcpy(RT_c, mat1.data, sizeof(float) * 16);
#endif
	return 0;
}



//矩阵相加
int add_maritx(MARTIX_Alva input_martix_left, MARTIX_Alva input_martix_right, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (input_martix_left.cols != input_martix_right.cols || input_martix_left.rows != input_martix_right.rows)
	{
		printf("不满足矩阵相加条件");
		ret = -1;
		return ret;
	}
	if (!output_martix)
	{
		printf("结果矩阵不能为空");
		ret = -2;
		return ret;
	}

	output_martix->cols = input_martix_right.cols;
	output_martix->rows = input_martix_right.rows;

	for (int i = 0; i < input_martix_left.rows; i++)
	{
		for (int j = 0; j < input_martix_left.cols; j++)
		{
			int temp = input_martix_left.martix[i*input_martix_left.cols + j] + input_martix_right.martix[i*input_martix_right.cols + j];
			output_martix->martix[i*input_martix_left.cols + j] = temp;
		}
	}
	return ret;
}


//矩阵自加
int mySelf_add_maritx(MARTIX_Alva* output_martix, MARTIX_Alva input_martix_left)
{
	int ret = 0;
	if (input_martix_left.cols != output_martix->cols || input_martix_left.rows != output_martix->rows)
	{
		printf("不满足矩阵相加条件");
		ret = -1;
		return ret;
	}
	if (!output_martix)
	{
		printf("结果矩阵不能为空");
		ret = -2;
		return ret;
	}


	for (int i = 0; i < input_martix_left.rows; i++)
	{
		for (int j = 0; j < input_martix_left.cols; j++)
		{
			float temp = input_martix_left.martix[i*input_martix_left.cols + j] + output_martix->martix[i*output_martix->cols + j];

			output_martix->martix[i*input_martix_left.cols + j] = temp;
		}
	}
	return ret;
}



//矩阵相减
int sub_maritx(MARTIX_Alva input_martix_left, MARTIX_Alva input_martix_right, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (input_martix_left.cols != input_martix_right.cols || input_martix_left.rows != input_martix_right.rows)
	{
		printf("不满足矩阵相加条件");
		ret = -1;
		return ret;
	}
	if (!output_martix)
	{
		printf("结果矩阵不能为空");
		ret = -2;
		return ret;
	}

	output_martix->cols = input_martix_right.cols;
	output_martix->rows = input_martix_right.rows;

	for (int i = 0; i < input_martix_left.rows; i++)
	{
		for (int j = 0; j < input_martix_left.cols; j++)
		{
			int temp = input_martix_left.martix[i*input_martix_left.cols + j] - input_martix_right.martix[i*input_martix_right.cols + j];
			output_martix->martix[i*input_martix_left.cols + j] = temp;
		}
	}
	return ret;
}
//按照第一行展开计算|A| 方阵A的行列式
float determinals_martix(MARTIX_Alva input_martix, int n)
{
	if (n == 1)
	{
		return input_martix.martix[0];
	}
	MARTIX_Alva temp_martix;
	temp_martix.cols = input_martix.cols - 1;
	temp_martix.rows = input_martix.rows - 1;
	temp_martix.martix = (float*)malloc(sizeof(float)*(n - 1)*(n - 1));
	memset(temp_martix.martix, 0, (n - 1)*(n - 1) * sizeof(float));
	float result = 0.0f;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - 1; j++)
		{
			for (int k = 0; k < n - 1; k++)
			{
				int tempk = (k >= i ? k + 1 : k);
				temp_martix.martix[j*(n - 1) + k] = input_martix.martix[(j + 1)*n + tempk];
				//    printf("%f=", temp_martix.martix[j*(n - 1) + k]);
			}
		}
		float temp = determinals_martix(temp_martix, n - 1);//此时求的是余子式

															//此时求代数余子式
		if (i % 2 == 0)
		{
			result += input_martix.martix[i] * temp;
		}
		else {
			result -= input_martix.martix[i] * temp;
		}
	}
	if (temp_martix.martix)
	{
		free(temp_martix.martix);
		temp_martix.martix = NULL;
	}
	return result;
}


//求方阵的伴随矩阵
int follow_martix(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (!output_martix)
	{
		ret = -1;
		printf("输出矩阵不能为空\n");
		return ret;
	}
	for (int i = 0; i < input_martix.rows; i++)
	{
		for (int j = 0; j < input_martix.cols; j++)
		{
			MARTIX_Alva temp_martix;
			temp_martix.cols = input_martix.cols - 1;
			temp_martix.rows = input_martix.rows - 1;
			temp_martix.martix = (float*)malloc(sizeof(float)*temp_martix.cols*temp_martix.rows);

			for (int p = 0; p < input_martix.rows - 1; p++)
			{
				int tempp = (p >= i ? p + 1 : p);
				for (int k = 0; k < input_martix.cols - 1; k++)
				{
					int tempk = (k >= j ? k + 1 : k);
					temp_martix.martix[p*(input_martix.cols - 1) + k] = input_martix.martix[tempp*input_martix.cols + tempk];
				}
			}
			//伴随矩阵第i行第j列的值为原矩阵第j行第i列的对应的代数余子式
			float temp = determinals_martix(temp_martix, temp_martix.cols);
			if ((i + j) % 2 == 0)
			{
				output_martix->martix[j*input_martix.cols + i] = temp;
			}
			else
			{
				output_martix->martix[j*input_martix.cols + i] = -temp;
			}
			if (temp_martix.martix)
			{
				free(temp_martix.martix);
				temp_martix.martix = NULL;
			}
		}
	}
	return ret;
}

//方阵的逆
int converse_martix(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{

	int ret = 0;
	if (!output_martix)
	{
		printf("结果矩阵不能为空");
		ret = -1;
		return ret;
	}
	if (input_martix.cols == 1)
	{
		output_martix->cols = 1;
		output_martix->rows = output_martix->cols;
		output_martix->martix[0] = 1;
		return ret;
	}

	MARTIX_Alva* follow_martixs;
	follow_martixs = (MARTIX_Alva*)malloc(sizeof(MARTIX_Alva));
	follow_martixs->cols = input_martix.cols;
	follow_martixs->rows = input_martix.rows;

	follow_martixs->martix = (float*)malloc(sizeof(float)*follow_martixs->cols*follow_martixs->rows);


	//求该矩阵的行列式
	float determinals = determinals_martix(input_martix, input_martix.cols);
	//    printf("%f     ]", determinals);

	//行列式的倒数
	float converse_determinals = 1.0 / determinals;
	//    printf("%f >>>", converse_determinals);

	ret = follow_martix(input_martix, follow_martixs);
	if (ret)
	{
		printf("求伴随矩阵出错\n");
	}
	ret = num_mul_matrix(*follow_martixs, converse_determinals, output_martix);//最终的结果
	if (follow_martixs)
	{
		if (follow_martixs->martix)
		{
			free(follow_martixs->martix);
			follow_martixs->martix = NULL;
		}
		follow_martixs = NULL;
	}
	return ret;
}


//数乘矩阵运算
int num_mul_matrix(MARTIX_Alva input_martix, float scale, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (ret < 0)
	{
		ret = -1;
		printf("输入的乘数不能为负");
		return ret;
	}
	for (int i = 0; i < input_martix.rows; i++)
	{
		for (int j = 0; j < input_martix.cols; j++)
		{
			output_martix->martix[i*output_martix->cols + j] = input_martix.martix[i*input_martix.cols + j] * scale;
		}
	}
	return ret;
}

//矩阵的转置
int translate_martix(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (!output_martix)
	{
		ret = -1;
		printf("输出矩阵不能为空");
		return ret;
	}
	output_martix->cols = input_martix.rows;
	output_martix->rows = input_martix.cols;

	for (int i = 0; i < output_martix->rows; i++)
	{
		for (int j = 0; j < output_martix->cols; j++)
		{
			output_martix->martix[i*output_martix->cols + j] = input_martix.martix[j*input_martix.cols + i];
		}
	}
	return ret;
}
// 矩阵平方然后除以阶乘
int powAndJieChe2(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (!output_martix)
	{
		ret = -1;
		printf("输出矩阵不能为空");
		return ret;
	}
	output_martix->cols = input_martix.rows;
	output_martix->rows = input_martix.cols;

	MARTIX_Alva temp;
	temp.cols = input_martix.cols; temp.rows = input_martix.rows;

	temp.martix = (float *)calloc(sizeof(float), temp.cols * temp.rows);

	mul_maritx(input_martix, input_martix, &temp);

	float scale = 1 / 2.0f;

	num_mul_matrix(temp, scale, output_martix);

	free(temp.martix);

	return ret;
}
// 矩阵平方然后除以阶乘
int powAndJieChe3(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (!output_martix)
	{
		ret = -1;
		printf("输出矩阵不能为空");
		return ret;
	}
	output_martix->cols = input_martix.rows;
	output_martix->rows = input_martix.cols;

	MARTIX_Alva temp;
	temp.cols = input_martix.cols; temp.rows = input_martix.rows;
	temp.martix = (float *)calloc(sizeof(float), temp.cols * temp.rows);

	MARTIX_Alva temp1;
	temp1.cols = input_martix.cols; temp1.rows = input_martix.rows;
	temp1.martix = (float *)calloc(sizeof(float), temp1.cols * temp1.rows);

	mul_maritx(input_martix, input_martix, &temp);

	mul_maritx(temp, input_martix, &temp1);

	float scale = 1 / (3.0f * 2.0f);

	num_mul_matrix(temp1, scale, output_martix);

	free(temp.martix);
	free(temp1.martix);
	return ret;
}

int powAndJieChe4(MARTIX_Alva input_martix, MARTIX_Alva* output_martix)
{
	int ret = 0;
	if (!output_martix)
	{
		ret = -1;
		printf("输出矩阵不能为空");
		return ret;
	}
	output_martix->cols = input_martix.rows;
	output_martix->rows = input_martix.cols;

	MARTIX_Alva temp;
	temp.cols = input_martix.cols; temp.rows = input_martix.rows;
	temp.martix = (float *)calloc(sizeof(float), temp.cols * temp.rows);

	MARTIX_Alva temp1;
	temp1.cols = input_martix.cols; temp1.rows = input_martix.rows;
	temp1.martix = (float *)calloc(sizeof(float), temp1.cols * temp1.rows);

	mul_maritx(input_martix, input_martix, &temp);

	mul_maritx(temp, input_martix, &temp1);

	mul_maritx(temp1, input_martix, &temp);


	float scale = 1 / (4.0f * 3.0f * 2.0f);

	num_mul_matrix(temp, scale, output_martix);

	free(temp.martix);
	free(temp1.martix);

	return ret;
}




#if 0
float *Rotate2EulerAng(Mat E_t)
{
	Mat RotM = E_t.colRange(0, 3);
	float r11, r21, r31, r32, r33;
	r11 = RotM.at

		<float>(0, 0);
	r21 = RotM.at

		<float>(1, 0);
	r31 = RotM.at

		<float>(2, 0);
	r32 = RotM.at

		<float>(2, 1);
	r33 = RotM.at

		<float>(2, 2);

	float ThetaX, ThetaY, ThetaZ;
	float Rad2AngC = 180.0 / CV_PI;
	ThetaX = Rad2AngC*atan2(r32, r33);
	ThetaY = Rad2AngC*atan2(-r31, sqrt(r32*r32 + r33*r33));
	ThetaZ = Rad2AngC*atan2(r21, r11);

	float *Rxyz = (float*)malloc(3 * sizeof(float));
	Rxyz[0] = ThetaX;
	Rxyz[1] = ThetaY;
	Rxyz[2] = ThetaZ;

	return Rxyz;
}

//从变换矩阵中得到平移向量
float *TranslationV(Mat E_t)
{
	float Tx, Ty, Tz;
	Tx = E_t.at<float>(0, 3);
	Ty = E_t.at<float>(1, 3);
	Tz = E_t.at<float>(2, 3);

	float *Txyz = (float*)malloc(3 * sizeof(float));
	Txyz[0] = Tx;
	Txyz[1] = Ty;
	Txyz[2] = Tz;

	return Txyz;
}



#endif