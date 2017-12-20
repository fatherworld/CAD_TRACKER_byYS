#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadSimilarity"
#include "Log.h"
#include "CadMacro.h"
#include "CadSimilarity.h"
// 0Ã»ÓÐÆ¥Åä£¬ 1Æ¥ÅäÉÏ
int findMaxSimilar(float* source_data, int source_width, int source_height,
	float* module_data, int module_width, int module_height, int offset_x,
	int offset_y, int step, int rangeNum, int noZeroNum, float thresh, int outOffset[2])
{
	// È·¶¨ËÑË÷·¶Î§
	int xStart = CLAMP(offset_x - rangeNum, 0, source_width - 1);
	int xEnd = CLAMP(offset_x + rangeNum, 0, source_width - 1);

	int yStart = CLAMP(offset_y - rangeNum, 0, source_height - 1);
	int yEnd = CLAMP(offset_y + rangeNum, 0, source_height - 1);

	int i, j, m, n;
	
	int tempH = yEnd - yStart + 1;
	int tempW = xEnd - xStart + 1;

	int ret = 0;
	float t_thresh = thresh;
	
	for (i = yStart; i <= yEnd; i += step)
	{
		for (j = xStart; j <= xEnd; j += step)
		{
			float sum = 0.0f;
			int tempNum = 0;
			for (n = 0; n < module_height; n++)
			{
				
				for (m = 0; m < module_width; m++)
				{
					float dx = module_data[(n * module_width + m) * 4 + 0];
					float dy = module_data[(n * module_width + m) * 4 + 1];
					float dL = module_data[(n * module_width + m) * 4 + 3];

					if (dL > 0.0f)
					{
						tempNum++;
					}

					float dx1 = 0.0f;
					float dy1 = 0.0f;
					float d1L = 0.0f;

					int ii = i + n;
					int jj = j + m;

					if (ii < source_height && ii >= 0 && jj >= 0 && jj < source_width)
					{
						dx1 = source_data[(ii * source_width + jj) * 4 + 0];
						dy1 = source_data[(ii * source_width + jj) * 4 + 1];
						d1L = source_data[(ii * source_width + jj) * 4 + 3];

					}
					if (dL * d1L > 0.0f)
					{
						float temp = (dx * dx1 + dy * dy1) / (dL * d1L);
						temp = temp > 0 ? temp : -temp;
						sum += temp;

					}
				}
			}


			//sum = sum > 0 ? sum : -sum;
			sum = sum / noZeroNum;

			//printf("sum = %f \n", sum);
			if (sum > t_thresh)
			{
				ret = 1;
				t_thresh = sum;
				outOffset[0] = j;
				outOffset[1] = i;
			}
		}
	}

	return ret;
}


int findMaxSimilar_optimiza(float* source_data, int source_width, int source_height,
	float* module_data, int * module_index, int offset_x,
	int offset_y, int step, int rangeNum, int noZeroNum, float thresh, int outOffset[2])
{
	// È·¶¨ËÑË÷·¶Î§
	int xStart = CLAMP(offset_x - rangeNum, 0, source_width - 1);
	int xEnd = CLAMP(offset_x + rangeNum, 0, source_width - 1);

	int yStart = CLAMP(offset_y - rangeNum, 0, source_height - 1);
	int yEnd = CLAMP(offset_y + rangeNum, 0, source_height - 1);

	int i, j, m, n, k;

	int tempH = yEnd - yStart + 1;
	int tempW = xEnd - xStart + 1;

	int ret = 0;
	float t_thresh = thresh;

	for (i = yStart; i <= yEnd; i += step)
	{
		for (j = xStart; j <= xEnd; j += step)
		{
			float sum = 0.0f;
			int tempNum = 0;
			for (k = 0; k < noZeroNum; k++)
			{
				int mod_x = module_index[k * 2 + 0];
				int mod_y = module_index[k * 2 + 1];

				float dx = module_data[k * 4 + 0];
				float dy = module_data[k * 4 + 1];
				float dL = module_data[k * 4 + 3];


				float dx1 = 0.0f;
				float dy1 = 0.0f;
				float d1L = 0.0f;

				int ii = i + mod_y;
				int jj = j + mod_x;

				if (ii < source_height && ii >= 0 && jj >= 0 && jj < source_width)
				{
					dx1 = source_data[(ii * source_width + jj) * 4 + 0];
					dy1 = source_data[(ii * source_width + jj) * 4 + 1];
					d1L = source_data[(ii * source_width + jj) * 4 + 3];

				}

				if (dL * d1L > 0.0f)
				{
					float temp = (dx * dx1 + dy * dy1) / (dL * d1L);
					temp = temp > 0 ? temp : -temp;
					sum += temp;
				}
			}

			sum = sum / noZeroNum;

			//printf("sum = %f \n", sum);
			if (sum > t_thresh)
			{
				ret = 1;
				t_thresh = sum;
				outOffset[0] = j;
				outOffset[1] = i;
			}

		}
	}

	return ret;
}