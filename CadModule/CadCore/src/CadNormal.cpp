#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadNormal"
#include "log.h"
#include "CadNormal.h"



int CadNormal(unsigned char * src, int width, int height, int format, float * outData)
{
	int i = 0;
	int j = 0;
	unsigned char R, G, B;
	if (format == Alva_FMT_BGR_888)
	{
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				unsigned char B = src[(i * width + j) * 3 + 0];
				unsigned char G = src[(i * width + j) * 3 + 1];
				unsigned char R = src[(i * width + j) * 3 + 2];

				outData[i * width + j] = (R * 299 + G * 587 + B * 114 + 500) / 255000.0f;
			}
		}

	}
	else if (format == Alva_FMT_RGB_888)
	{
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				unsigned char R = src[(i * width + j) * 3 + 0];
				unsigned char G = src[(i * width + j) * 3 + 1];
				unsigned char B = src[(i * width + j) * 3 + 2];

				outData[i * width + j] = (R * 299 + G * 587 + B * 114 + 500) / 255000.0f;
			}
		}
	}
	else if (format == Alva_FMT_NV21)
	{
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				unsigned char R = src[(i * width + j)];

				outData[i * width + j] = R / 255.0f;
			}
		}
	}
	else
	{
		LOGE("No support please input RGB or BGR")
	}
	return 0;
}
