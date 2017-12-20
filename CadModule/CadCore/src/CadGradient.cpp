#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadGradient"
#include "Log.h"
#include <stdio.h>
#include <math.h>
#include "CadMatrix.h"
#include "CadGradient.h"
#include "CadErodeDilate.h"
#include "CadMacro.h"


//梯度计算结果
int  CadGradient(float* input_data, int width, int height, float* gradientResult)
{
	int ret = 0;
	if (!input_data || !gradientResult)
	{
		ret = -1;
		printf("输入或者输出不能为空");
		return ret;
	}

	MARTIX_Alva  Convolve3x;
	float temp[9] = { 0.0f };

	//构造一个3*3索贝尔卷积
	Convolve3x.cols = 3;
	Convolve3x.rows = 3;
	Convolve3x.martix = temp;
	Convolve3x.martix[0] = -1.0;
	Convolve3x.martix[1] = 0.0;
	Convolve3x.martix[2] = 1.0;
	Convolve3x.martix[3] = -2.0;
	Convolve3x.martix[4] = 0.0;
	Convolve3x.martix[5] = 2.0;
	Convolve3x.martix[6] = -1.0;
	Convolve3x.martix[7] = 0.0;
	Convolve3x.martix[8] = 1.0;
	int noZeroNum = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int prei = i - 1;
			int prej = j - 1;
			int nxti = i + 1;
			int nxtj = j + 1;

			if (i == 0)
			{
				prei = i + 1;
				nxti = i + 2;
			}
			if (i == height - 1)
			{
				prei = i - 2;
				nxti = i - 1;
			}
			if (j == 0)
			{
				prej = j + 1;
				nxtj = j + 2;
			}
			if (j == width - 1)
			{
				prej = j - 2;
				nxtj = j - 1;
			}


			gradientResult[i*width * 4 + j * 4 + 0] = (Convolve3x.martix[0] * input_data[prei*width + prej] + Convolve3x.martix[1] * input_data[prei*width + j] +
				Convolve3x.martix[2] * input_data[prei*width + nxtj] + Convolve3x.martix[3] * input_data[i*width + prej] + Convolve3x.martix[4] * input_data[i*width + j]
				+ Convolve3x.martix[5] * input_data[i*width + nxtj] + Convolve3x.martix[6] * input_data[nxti*width + prej] + Convolve3x.martix[7] * input_data[nxti*width + j]
				+ Convolve3x.martix[8] * input_data[nxti*width + nxtj]);

			//计算dy
			gradientResult[i*width * 4 + j * 4 + 1] = (float)(Convolve3x.martix[0] * input_data[prei*width + prej] + Convolve3x.martix[3] * input_data[prei*width + j] +
				Convolve3x.martix[6] * input_data[prei*width + nxtj] + Convolve3x.martix[1] * input_data[i*width + prej] + Convolve3x.martix[4] * input_data[i*width + j]
				+ Convolve3x.martix[7] * input_data[i*width + nxtj] + Convolve3x.martix[2] * input_data[nxti*width + prej] + Convolve3x.martix[5] * input_data[nxti*width + j]
				+ Convolve3x.martix[8] * input_data[nxti*width + nxtj]);

			//cout << gradientResult[i*width * 4 + j * 4 + 1] << endl;

			//计算正切角
			gradientResult[i*width * 4 + j * 4 + 2] = (float)(atan2(gradientResult[i*width * 4 + j * 4 + 0], gradientResult[i*width * 4 + j * 4 + 1]));


			//计算模长
			gradientResult[i*width * 4 + j * 4 + 3] = (float)sqrt(gradientResult[i*width * 4 + j * 4 + 0] * gradientResult[i*width * 4 + j * 4 + 0] + gradientResult[i*width * 4 + j * 4 + 1] * gradientResult[i*width * 4 + j * 4 + 1]);

			if (gradientResult[i*width * 4 + j * 4 + 3] < 0.15f)
			{
				gradientResult[i*width * 4 + j * 4 + 0] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 1] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 2] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 3] = 0.0f;
			}
			else
			{
				noZeroNum++;
			}

			//cout << gradientResult[i*width * 4 + j * 4 + 3] << endl;
			
		}
	}

	return noZeroNum;
}

static int NormalStretch(float * src, int width, int height, float LenMinMax[2])
{
	float scale = 1.0f / (LenMinMax[1] - LenMinMax[0]);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			float temp = src[i * width + j];
			src[i * width + j] = (temp - LenMinMax[0]) * scale;
		}
	}
	return 0;
}
#if 1
//梯度计算结果
int  CadGradient1(float* input_data, int width, int height, float* gradientResult, float * gradientLen, float LenMinMax[2])
{
	int ret = 0;
	if (!input_data || !gradientResult)
	{
		ret = -1;
		printf("输入或者输出不能为空");
		return ret;
	}

	MARTIX_Alva  Convolve3x;
	float temp[9] = { 0.0f };
	float MinMax[2] = { 0.0f, 0.0f };

	//构造一个3*3索贝尔卷积
	Convolve3x.cols = 3;
	Convolve3x.rows = 3;
	Convolve3x.martix = temp;
	Convolve3x.martix[0] = -1.0;
	Convolve3x.martix[1] = 0.0;
	Convolve3x.martix[2] = 1.0;
	Convolve3x.martix[3] = -2.0;
	Convolve3x.martix[4] = 0.0;
	Convolve3x.martix[5] = 2.0;
	Convolve3x.martix[6] = -1.0;
	Convolve3x.martix[7] = 0.0;
	Convolve3x.martix[8] = 1.0;
	int noZeroNum = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int prei = i - 1;
			int prej = j - 1;
			int nxti = i + 1;
			int nxtj = j + 1;

			if (i == 0)
			{
				prei = i;
				nxti = i + 1;
			}
			if (i == height - 1)
			{
				prei = i - 1;
				nxti = i;
			}
			if (j == 0)
			{
				prej = j;
				nxtj = j + 1;
			}
			if (j == width - 1)
			{
				prej = j - 1;
				nxtj = j ;
			}


			gradientResult[i*width * 4 + j * 4 + 0] = (Convolve3x.martix[0] * input_data[prei*width + prej] + Convolve3x.martix[1] * input_data[prei*width + j] +
				Convolve3x.martix[2] * input_data[prei*width + nxtj] + Convolve3x.martix[3] * input_data[i*width + prej] + Convolve3x.martix[4] * input_data[i*width + j]
				+ Convolve3x.martix[5] * input_data[i*width + nxtj] + Convolve3x.martix[6] * input_data[nxti*width + prej] + Convolve3x.martix[7] * input_data[nxti*width + j]
				+ Convolve3x.martix[8] * input_data[nxti*width + nxtj]);

			//计算dy
			gradientResult[i*width * 4 + j * 4 + 1] = (float)(Convolve3x.martix[0] * input_data[prei*width + prej] + Convolve3x.martix[3] * input_data[prei*width + j] +
				Convolve3x.martix[6] * input_data[prei*width + nxtj] + Convolve3x.martix[1] * input_data[i*width + prej] + Convolve3x.martix[4] * input_data[i*width + j]
				+ Convolve3x.martix[7] * input_data[i*width + nxtj] + Convolve3x.martix[2] * input_data[nxti*width + prej] + Convolve3x.martix[5] * input_data[nxti*width + j]
				+ Convolve3x.martix[8] * input_data[nxti*width + nxtj]);

			//cout << gradientResult[i*width * 4 + j * 4 + 1] << endl;

			//计算正切角
			gradientResult[i*width * 4 + j * 4 + 2] = (float)(atan2(gradientResult[i*width * 4 + j * 4 + 1], gradientResult[i*width * 4 + j * 4 + 0]));


			//计算模长
			
			float t_temp = (float)sqrt(gradientResult[i*width * 4 + j * 4 + 0] * gradientResult[i*width * 4 + j * 4 + 0] + gradientResult[i*width * 4 + j * 4 + 1] * gradientResult[i*width * 4 + j * 4 + 1]);


			gradientResult[i*width * 4 + j * 4 + 3] = t_temp;
			if (t_temp < 0.3f)
			{
				gradientResult[i*width * 4 + j * 4 + 0] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 1] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 2] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 3] = 0.0f;
			}
			else
			{
				noZeroNum++;
			}

			gradientLen[i * width + j] = gradientResult[i*width * 4 + j * 4 + 3];
			if (i == 0 && j == 0)
			{
				MinMax[0] = gradientLen[i * width + j];
				MinMax[1] = gradientLen[i * width + j];
			}
			else
			{
				if (gradientLen[i * width + j] < MinMax[0])
				{
					MinMax[0] = gradientLen[i * width + j];
				}
				if (gradientLen[i * width + j] > MinMax[1])
				{
					MinMax[1] = gradientLen[i * width + j];
				}
			}
			//cout << gradientResult[i*width * 4 + j * 4 + 3] << endl;

		}
	}
	LenMinMax[0] = MinMax[0];
	LenMinMax[1] = MinMax[1];

	float * tempBuffer = (float *)malloc(sizeof(float) * width * height);
	CadErodeDilate(gradientLen, width, height, 3, tempBuffer);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//gradientResult[i*width * 4 + j * 4 + 3] = gradientLen[i * width + j];
		}
	}
	free(tempBuffer);
	NormalStretch(gradientLen, width, height, LenMinMax);
	return noZeroNum;
}
#else
//梯度计算结果
int  CadGradient1(float* input_data, int width, int height, float* gradientResult, float * gradientLen, float LenMinMax[2])
{
	int ret = 0;
	if (!input_data || !gradientResult)
	{
		ret = -1;
		printf("输入或者输出不能为空");
		return ret;
	}

	MARTIX_Alva  Convolve3x;
	float temp[9] = { 0.0f };
	float MinMax[2] = { 0.0f, 0.0f };

	//构造一个3*3索贝尔卷积


	float * sobelX = (float *)malloc(sizeof(float) * width * height);
	float * sobelY = (float *)malloc(sizeof(float) * width * height);

	SobelFilter(input_data, width, height, sobelX, sobelY);

	cv::Mat solbelXmat = cv::Mat(height, width, CV_8UC1);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			solbelXmat.data[i * width + j] = (unsigned char)(sobelX[i * width + j] * 255.0f);

		}

	}

	cv::imshow("solx", solbelXmat);


	int noZeroNum = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{



			gradientResult[i*width * 4 + j * 4 + 0] = sobelX[i * width + j];

			//计算dy
			gradientResult[i*width * 4 + j * 4 + 1] = sobelY[i * width + j];


			//计算正切角
			gradientResult[i*width * 4 + j * 4 + 2] = (float)(atan2(gradientResult[i*width * 4 + j * 4 + 1], gradientResult[i*width * 4 + j * 4 + 0]));


			//计算模长
			//gradientResult[i*width * 4 + j * 4 + 3] = gradientResult[i*width * 4 + j * 4 + 0] * 0.5f + gradientResult[i*width * 4 + j * 4 + 1] * 0.5f;
			gradientResult[i*width * 4 + j * 4 + 3] = 0.0f;
			float t_temp = (float)sqrt(gradientResult[i*width * 4 + j * 4 + 0] * gradientResult[i*width * 4 + j * 4 + 0] + gradientResult[i*width * 4 + j * 4 + 1] * gradientResult[i*width * 4 + j * 4 + 1]);

			if (t_temp < 0.3f)
			{
				gradientResult[i*width * 4 + j * 4 + 0] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 1] = 0.0f;
				gradientResult[i*width * 4 + j * 4 + 2] = 0.0f;
				//gradientResult[i*width * 4 + j * 4 + 3] = 0.0f;
			}
			else
			{
				noZeroNum++;
			}

			gradientLen[i * width + j] = gradientResult[i*width * 4 + j * 4 + 3];
			if (i == 0 && j == 0)
			{
				MinMax[0] = gradientLen[i * width + j];
				MinMax[1] = gradientLen[i * width + j];
			}
			else
			{
				if (gradientLen[i * width + j] < MinMax[0])
				{
					MinMax[0] = gradientLen[i * width + j];
				}
				if (gradientLen[i * width + j] > MinMax[1])
				{
					MinMax[1] = gradientLen[i * width + j];
				}
			}
			//cout << gradientResult[i*width * 4 + j * 4 + 3] << endl;

		}
	}
	LenMinMax[0] = MinMax[0];
	LenMinMax[1] = MinMax[1];

	float * tempBuffer = (float *)malloc(sizeof(float) * width * height);
	CadErodeDilate(gradientLen, width, height, 3, tempBuffer);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//gradientResult[i*width * 4 + j * 4 + 3] = gradientLen[i * width + j];
		}
	}
	free(tempBuffer);
	NormalStretch(gradientLen, width, height, LenMinMax);
	return noZeroNum;
}

#endif

void SobelFilter(float* grayImg, int width, int height, float* sobelX, float* sobelY, float * angle, float * lenth) {

	float MinGradMag = 0.15f;
	float MinMax[2] = { 0.0f, 0.0f };
	int noZeroNum = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int pre_i = CLAMP(i - 1, 0, height - 1);
			int nex_i = CLAMP(i + 1, 0, height - 1);

			int pre_j = CLAMP(j - 1, 0, height - 1);
			int nex_j = CLAMP(j + 1, 0, height - 1);

			float left_up = grayImg[pre_i * width + pre_j];
			float left = grayImg[i * width + pre_j];
			float left_down = grayImg[nex_i * width + pre_j];

			float right_up = grayImg[pre_i * width + nex_j];
			float right = grayImg[i * width + nex_j];
			float right_down = grayImg[nex_i * width + nex_j];

			float up = grayImg[pre_i * width + j];
			float down = grayImg[nex_i * width + j];

			float dx = left_up * (-1.0f) + left * (-2.0f) + left_down * (-1.0f) + right_up * (1.0f) + right * (2.0f) + right_down * 1.0f;
			float dy = left_up * (-1.0f) + up * (-2.0f) + right_up * (-1.0f) + left_down * (1.0f) + down * (2.0f) + right_down * 1.0f;

			float iGradMag = sqrt(dx*dx + dy*dy);

			if (iGradMag < MinGradMag)
			{
				dx = 0;
				dy = 0;
			}

			sobelX[i * width + j] = dx * 255;
			sobelY[i * width + j] = dy * 255;

			angle[i * width + j] = atan2(dy, dx);
			lenth[i * width + j] = 0.5f * dx + 0.5f * dy;



			if (i == 0 && j == 0)
			{
				MinMax[0] = lenth[i * width + j];
				MinMax[1] = lenth[i * width + j];
			}
			else
			{
				if (lenth[i * width + j] < MinMax[0])
				{
					MinMax[0] = lenth[i * width + j];
				}
				if (lenth[i * width + j] > MinMax[1])
				{
					MinMax[1] = lenth[i * width + j];
				}
			}

		}
	}
	
}