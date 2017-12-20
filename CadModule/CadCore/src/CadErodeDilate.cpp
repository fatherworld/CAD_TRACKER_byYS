#include "Config.h"
#include "Format.h"
#include "CadStruct.h"
#include "CadErodeDilate.h"

#if 1
int CadErodeDilate(float* src, int dataWidth, int dataHeight, int eroDilWH, float * tempBuffer)
{

	int erodeWidth = eroDilWH;
	int erodeHeight = eroDilWH;

	int dilateWidth = eroDilWH;
	int dilateHeight = eroDilWH;

	int widthTemp;
	int heightTemp;
	int i, j;
	int m, n;
	float dataTemp = 0;

	int count = 0;


	float minValue = 0;
	float maxValue = 0;

	if (erodeWidth % 2 == 0) {
		widthTemp = erodeWidth / 2 - 1;
	}
	else {
		widthTemp = erodeWidth / 2;
	}
	if (erodeHeight % 2 == 0) {
		heightTemp = erodeHeight / 2 - 1;
	}
	else {
		heightTemp = erodeHeight / 2;
	}
	// 在模板范围内找最小值
	for (i = 0; i < dataHeight; i++)
	{
		for (j = 0; j < dataWidth; j++)
		{
			dataTemp = 0;
			count = 0;
			for (m = -heightTemp; m < erodeHeight - heightTemp; m++)
			{
				for (n = -widthTemp; n < erodeWidth - widthTemp; n++)
				{
					if (i + m >= 0 && i + m < dataHeight && j + n >= 0 && j + n < dataWidth)
					{
						dataTemp = src[((i + m) * dataWidth + (j + n))];
						if (dataTemp == 0)
						{
							minValue = 0;
							goto nextPoint0;
						}
						if (count == 0)
						{
							minValue = dataTemp;
						}
						else
						{
							if (dataTemp < minValue)
							{
								minValue = dataTemp;
							}
						}
						count++;
					}
				}
			}
		nextPoint0:
			tempBuffer[i * dataWidth + j] = minValue;
		}
	}


	// 在模板范围内找最大值
	for (i = 0; i < dataHeight; i++)
	{
		for (j = 0; j < dataWidth; j++)
		{

			dataTemp = 0;
			count = 0;
			for (m = -heightTemp; m < dilateHeight - heightTemp; m++)
			{
				for (n = -widthTemp; n < dilateWidth - widthTemp; n++)
				{
					if (i + m >= 0 && i + m < dataHeight && j + n >= 0 && j + n < dataWidth)
					{
						dataTemp = tempBuffer[((i + m) * dataWidth + (j + n))];
						if (dataTemp == 1.0f)
						{
							maxValue = 1.0f;
							goto nextPoint1;
						}
						if (count == 0)
						{
							maxValue = dataTemp;
						}
						else
						{
							if (dataTemp > maxValue)
							{
								maxValue = dataTemp;
							}
						}
						count++;
					}
				}
			}
		nextPoint1:
			src[(i * dataWidth + j)] = maxValue;
		}
	}


	return 0;
}
#endif