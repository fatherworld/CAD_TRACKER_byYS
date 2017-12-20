#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadGaosi"
#include "Log.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "CadMacro.h"
#include "CadGaosi.h"



#define ENumber 2.71828183f
#define PI 3.1415926f

int makeGaussianData(float ** data, int width, int height, float filterRatio){
    int i, j;
    float sum = 0.0f;
    *data = (float*)malloc(sizeof(float) * width * height);

    for(i = 0; i < height; i ++){
        for(j = 0; j < width; j ++){
            float temp = 0.0f;
            temp = -1.0f * ((j - width / 2) * (j - width / 2) + (i - height / 2) * (i - height / 2)) / (2.0f * filterRatio * filterRatio);
            temp = powf(ENumber, temp);
            (*data)[i * width + j] = (1.0f / (2.0f * PI * filterRatio * filterRatio)) * temp;
            sum += (*data)[i * width + j];
        }
    }
    //printf("sum = %f\n", sum);
    for(i = 0; i < height; i ++){
        for(j = 0; j < width; j ++){

            (*data)[i * width + j] = (*data)[i * width + j] / sum;
        }
    }
    return 0;
}
int cpuConvolution(unsigned char * src, unsigned char * des, float * model, int srcW, int srcH, int modW, int modH, int channel)
{


    int i, j, k;
    int m, n;

	int desW = srcW;
	int desH = srcH;

	int halfModW = modW / 2;
	int halfModH = modH / 2;

	
    for( i = 0; i < desH; i ++)
    {
        for(j = 0; j < desW; j ++)
        {

			for(k = 0; k < channel; k ++)
			{
				float temp = 0.0f;
				for (m = -halfModH; m <= halfModH;  m++)
				{
					int srcY = i + m;
					for (n = -halfModW; n <= halfModW; n++)
					{
						int srcX = j + n;
						unsigned char tempSrc = 0;
						int modX = halfModW + n;
						int modY = halfModH + m;
						if (srcX < 0 || srcX > srcW - 1 || srcY < 0 || srcY > srcH - 1)
						{
							tempSrc = 0;
						}
						else
						{
							tempSrc = src[(srcY * srcW + srcX) * channel + k];
						}

						temp += tempSrc * model[(modY * modW + modX)];

					}
				}

				des[(i * desW + j) * channel + k] = (unsigned char)CLAMP( temp, 0.0f, 255.0f);

			}

        }
    }

    return 0;
}
int Gaussian(unsigned char * src, int width, int height , unsigned char * dest, float * data,  int Size ,int channel)
{
	cpuConvolution(src, dest, data, width, height, Size , Size, channel);
	return 0;
}

int GaussianCon(float * src, int srcW, int srcH, float * GaussianKernal, int kernelSize, int channel, float * des)
{
	int i, j, k;
	int m, n;

	int desW = srcW;
	int desH = srcH;

	int halfModW = kernelSize / 2;
	int halfModH = kernelSize / 2;


	for (i = 0; i < desH; i++)
	{
		for (j = 0; j < desW; j++)
		{

			for (k = 0; k < channel; k++)
			{
				float temp = 0.0f;
				for (m = -halfModH; m <= halfModH; m++)
				{
					int srcY = i + m;
					for (n = -halfModW; n <= halfModW; n++)
					{
						int srcX = j + n;
						float tempSrc = 0;
						int modX = halfModW + n;
						int modY = halfModH + m;
						if (srcX < 0 || srcX > srcW - 1 || srcY < 0 || srcY > srcH - 1)
						{
							tempSrc = 0;
						}
						else
						{
							tempSrc = src[(srcY * srcW + srcX) * channel + k];
						}

						temp += tempSrc * GaussianKernal[(modY * kernelSize + modX)];

					}
				}

				des[(i * desW + j) * channel + k] = CLAMP(temp, 0.0f, 1.0f);
			}

		}
	}

	return 0;

}
