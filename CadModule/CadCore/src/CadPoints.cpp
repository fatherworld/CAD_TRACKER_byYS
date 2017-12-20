#include "CadMatrix.h"
#include "CadStruct.h"
#include "CadPoints.h"
#include "CadMacro.h"


int Updata2DPoint(void * ptrCadInfo_)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;
	int i = 0;
	for (i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);
		int j = 0; int k = 0;

		for (j = 0; j < 3; j++)
		{
			//printf("[j=%d] %f %f %f ", j, dd.v[0], dd.v[1], dd.v[2]);
			for (k = 0; k < current->linePointNum[j]; k++)
			{

				float input0[3] = { 0.0f };
				float output0[2] = { 0.0f };

				input0[0] = current->linePoint[j][k].v[0];
				input0[1] = current->linePoint[j][k].v[1];
				input0[2] = current->linePoint[j][k].v[2];

				ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
				current->line2DPoint[j][k].v[0] = output0[0];
				current->line2DPoint[j][k].v[1] = output0[1];

				//printf("%f %f", output0[0], output0[1]);
			}

		}
	}

	return 0;
}


int FindLinePoint(void * ptrCadInfo_)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;
	//ptrCadInfo->GetPointStep = 1;
	int i = 0;
	for (i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);
		int j = 0; int k = 0;

		// 计算三条线的模长
		vecf3 p[3];
		p[0] = current->facePoint[0];
		p[1] = current->facePoint[1];
		p[2] = current->facePoint[2];

		float input0[3] = { p[0].v[0], p[0].v[1], p[0].v[2] };
		float input1[3] = { p[1].v[0], p[1].v[1], p[1].v[2] };
		float input2[3] = { p[2].v[0], p[2].v[1], p[2].v[2] };

		float output0[2];
		float output1[2];
		float output2[2];

		ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
		ThreeDim2SecDimPoint(input1, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output1);
		ThreeDim2SecDimPoint(input2, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output2);

		current->face2DimPoint[0].v[0] = output0[0]; current->face2DimPoint[0].v[1] = output0[1];
		current->face2DimPoint[1].v[0] = output1[0]; current->face2DimPoint[1].v[1] = output1[1];
		current->face2DimPoint[2].v[0] = output2[0]; current->face2DimPoint[2].v[1] = output2[1];

		//printf("[%d] = {%f %f %f %f %f %f} \n", i, current->face2DimPoint[0].v[0], current->face2DimPoint[0].v[1], current->face2DimPoint[1].v[0], current->face2DimPoint[1].v[1],
		//	current->face2DimPoint[2].v[0], current->face2DimPoint[2].v[1]);

		current->lineLen[0] = sqrtf((p[1].v[0] - p[0].v[0]) * (p[1].v[0] - p[0].v[0]) + (p[1].v[1] - p[0].v[1]) * (p[1].v[1] - p[0].v[1]) + (p[1].v[2] - p[0].v[2]) * (p[1].v[2] - p[0].v[2]));
		current->lineLen[1] = sqrtf((p[1].v[0] - p[2].v[0]) * (p[1].v[0] - p[2].v[0]) + (p[1].v[1] - p[2].v[1]) * (p[1].v[1] - p[2].v[1]) + (p[1].v[2] - p[2].v[2]) * (p[1].v[2] - p[2].v[2]));
		current->lineLen[2] = sqrtf((p[2].v[0] - p[0].v[0]) * (p[2].v[0] - p[0].v[0]) + (p[2].v[1] - p[0].v[1]) * (p[2].v[1] - p[0].v[1]) + (p[2].v[2] - p[0].v[2]) * (p[2].v[2] - p[0].v[2]));

		current->linePointNum[0] = ((int)(current->lineLen[0]) + 1) / ptrCadInfo->GetPointStep;
		current->linePointNum[1] = ((int)(current->lineLen[1]) + 1) / ptrCadInfo->GetPointStep;
		current->linePointNum[2] = ((int)(current->lineLen[2]) + 1) / ptrCadInfo->GetPointStep;

		current->linePoint[0] = (vecf3 *)calloc(sizeof(vecf3), current->linePointNum[0]);
		current->linePoint[1] = (vecf3 *)calloc(sizeof(vecf3), current->linePointNum[1]);
		current->linePoint[2] = (vecf3 *)calloc(sizeof(vecf3), current->linePointNum[2]);

		current->line2DPoint[0] = (vecf2 *)calloc(sizeof(vecf2), current->linePointNum[0]);
		current->line2DPoint[1] = (vecf2 *)calloc(sizeof(vecf2), current->linePointNum[1]);
		current->line2DPoint[2] = (vecf2 *)calloc(sizeof(vecf2), current->linePointNum[2]);

		current->flagEveryPoint[0] = (int*)calloc(sizeof(int), current->linePointNum[0]);
		current->flagEveryPoint[1] = (int*)calloc(sizeof(int), current->linePointNum[1]);
		current->flagEveryPoint[2] = (int*)calloc(sizeof(int), current->linePointNum[2]);



		//LOGE("lineNum =[%d] %d %d %d \n", i, current->linePointNum[0], current->linePointNum[1], current->linePointNum[2]);

		for (j = 0; j < 3; j++)
		{
			int jj = j + 1;
			if (jj == 3)
			{
				jj = 0;
			}
			vecf3 d;
			vecf2 dd;
			d.v[0] = p[jj].v[0] - p[j].v[0]; d.v[1] = p[jj].v[1] - p[j].v[1]; d.v[2] = p[jj].v[2] - p[j].v[2];

			dd.v[0] = d.v[0] / current->lineLen[j];
			dd.v[1] = d.v[1] / current->lineLen[j];
			dd.v[2] = d.v[2] / current->lineLen[j];
			//printf("[j=%d] %f %f %f ", j, dd.v[0], dd.v[1], dd.v[2]);
			for (k = 0; k < current->linePointNum[j]; k++)
			{
				int t = k * ptrCadInfo->GetPointStep;

				current->linePoint[j][k].v[0] = p[j].v[0] + t * dd.v[0];
				current->linePoint[j][k].v[1] = p[j].v[1] + t * dd.v[1];
				current->linePoint[j][k].v[2] = p[j].v[2] + t * dd.v[2];


				float input0[3] = { 0.0f };
				float output0[2] = { 0.0f };

				input0[0] = current->linePoint[j][k].v[0];
				input0[1] = current->linePoint[j][k].v[1];
				input0[2] = current->linePoint[j][k].v[2];

				ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
				current->line2DPoint[j][k].v[0] = output0[0];
				current->line2DPoint[j][k].v[1] = output0[1];

				//printf("%f %f", output0[0], output0[1]);
			}
			printf("\n");
		}

	}

	return 0;
}


int LookForNew2DPoint(void *ptrCadInfo_, float * GradientLen, int width, int height)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;
	int i = 0; int j = 0; int k = 0;
	ptrCadInfo->NumForMartixNow = 0;
	int numIter = 30;
	float thresh_gray = 0.4f;
	for (i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);
		for (j = 0; j < 3; j++)
		{
			if (current->flag[j] == 1 && current->isEyeSee[j] == 1)
			{
				
				// 先求法向量
				int jj = j + 1;
				if (jj == 3) { jj = 0; }
				float input0[3]; float input1[3]; float output0[2]; float output1[2];
				input0[0] = current->facePoint[j].v[0];
				input0[1] = current->facePoint[j].v[1];
				input0[2] = current->facePoint[j].v[2];

				input1[0] = current->facePoint[jj].v[0];
				input1[1] = current->facePoint[jj].v[1];
				input1[2] = current->facePoint[jj].v[2];

				ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
				ThreeDim2SecDimPoint(input1, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output1);

				float a = output1[0] - output0[0];
				float b = output1[1] - output0[1];

				float len = sqrt(a * a + b * b);
				float tempA = a;
				a = -b / len;
				b = tempA / len;



				for (k = 1; k < current->linePointNum[j] - 1; k++)
				{
					ForMartix * mForMartix = &(ptrCadInfo->mForMartix[ptrCadInfo->NumForMartixNow]);

					input0[0] = current->linePoint[j][k].v[0];
					input0[1] = current->linePoint[j][k].v[1];
					input0[2] = current->linePoint[j][k].v[2];

					ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);

					current->line2DPoint[j][k].v[0] = output0[0];
					current->line2DPoint[j][k].v[1] = output0[1];

					int index[2] = { 0, 0 };
					int count = 0;
					int value[4];
					float valueF[4];
					float valueIndex[2];
					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] + m * a;
						output1[1] = output0[1] + m * b;

						
						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						if (GradientLen[index[1] * width + index[0]] > thresh_gray)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];
							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}

					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] - m * a;
						output1[1] = output0[1] - m * b;
					
						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						if (GradientLen[index[1] * width + index[0]] > thresh_gray)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];

							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}
					if (1 == count)
					{
						float aa = (value[0] + valueF[0]) / 2;
						float bb = (value[1] + valueF[1]) / 2;

						mForMartix->d2FindPoint.v[0] = aa;
						mForMartix->d2FindPoint.v[1] = bb;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;



						ptrCadInfo->NumForMartixNow++;

						if (ptrCadInfo->NumForMartixNow >= ptrCadInfo->NumForMartixCap)
						{

						}

					}
					else if (2 == count) // 找到一个比较近的值
					{
						int suitIndex = 0;
						suitIndex = valueIndex[0] < valueIndex[1] ? 0 : 1;


						mForMartix->d2FindPoint.v[0] = (value[suitIndex * 2 + 0] + valueF[suitIndex * 2 + 0]) / 2.0f;
						mForMartix->d2FindPoint.v[1] = (value[suitIndex * 2 + 1] + valueF[suitIndex * 2 + 1]) / 2.0f;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;

						ptrCadInfo->NumForMartixNow++;

					}

				}
			}
		}

	}

	return 0;
}

// 四通道的梯度图

int LookForNew2DPoint_1(void *ptrCadInfo_, float * GradientLen, int width, int height)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;
	int i = 0; int j = 0; int k = 0;
	ptrCadInfo->NumForMartixNow = 0;
	int numIter = ptrCadInfo->numIter;
	float thresh_gray = ptrCadInfo->mGrayTheshold;
	float AngTol = ptrCadInfo->mAngleTol;
	for (i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);
		for (j = 0; j < 3; j++)
		{
			if (current->flag[j] == 1 && current->isEyeSee[j] == 1)
			{

				// 先求法向量
				int jj = j + 1;
				if (jj == 3) { jj = 0; }
				float input0[3]; float input1[3]; float output0[2]; float output1[2];
				input0[0] = current->facePoint[j].v[0];
				input0[1] = current->facePoint[j].v[1];
				input0[2] = current->facePoint[j].v[2];

				input1[0] = current->facePoint[jj].v[0];
				input1[1] = current->facePoint[jj].v[1];
				input1[2] = current->facePoint[jj].v[2];

				ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
				ThreeDim2SecDimPoint(input1, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output1);

				float a = output1[0] - output0[0];
				float b = output1[1] - output0[1];

				float len = sqrt(a * a + b * b);
				float tempA = a;
				a = -b / len;
				b = tempA / len;

				float t_angle = atan2(b, a);

				for (k = 1; k < current->linePointNum[j] - 1; k++)
				{
					ForMartix * mForMartix = &(ptrCadInfo->mForMartix[ptrCadInfo->NumForMartixNow]);

					input0[0] = current->linePoint[j][k].v[0];
					input0[1] = current->linePoint[j][k].v[1];
					input0[2] = current->linePoint[j][k].v[2];

					ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);

					current->line2DPoint[j][k].v[0] = output0[0];
					current->line2DPoint[j][k].v[1] = output0[1];

					int currentX = (int)(output0[0] + 0.5f);
					int currentY = (int)(output0[1] + 0.5f);

					int index[2] = { 0, 0 };
					int count = 0;
					int value[4];
					float valueF[4];
					float valueIndex[2];
					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] + m * a;
						output1[1] = output0[1] + m * b;


						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						float dAngle = fabs(GradientLen[(index[1] * width + index[0]) * 4 + 2] - t_angle);
						if (GradientLen[(index[1] * width + index[0]) * 4 + 3] > thresh_gray && dAngle < AngTol)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];
							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}

					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] - m * a;
						output1[1] = output0[1] - m * b;

						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						float dAngle = fabs(GradientLen[(index[1] * width + index[0]) * 4 + 2] - t_angle);
						if (GradientLen[(index[1] * width + index[0]) * 4 + 3] > thresh_gray && dAngle < AngTol)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];

							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}
					if (1 == count)
					{
						float aa = (value[0] + valueF[0]) / 2;
						float bb = (value[1] + valueF[1]) / 2;

						mForMartix->d2FindPoint.v[0] = aa;
						mForMartix->d2FindPoint.v[1] = bb;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;



						ptrCadInfo->NumForMartixNow++;

						if (ptrCadInfo->NumForMartixNow >= ptrCadInfo->NumForMartixCap)
						{

						}

					}
					else if (2 == count) // 找到一个比较近的值
					{
						int suitIndex = 0;
						suitIndex = valueIndex[0] < valueIndex[1] ? 0 : 1;


						mForMartix->d2FindPoint.v[0] = (value[suitIndex * 2 + 0] + valueF[suitIndex * 2 + 0]) / 2.0f;
						mForMartix->d2FindPoint.v[1] = (value[suitIndex * 2 + 1] + valueF[suitIndex * 2 + 1]) / 2.0f;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;

						ptrCadInfo->NumForMartixNow++;

					}

				}
			}
		}

	}

	return 0;
}




int LookForNew2DPoint_cv(void *ptrCadInfo_, float * GradientLen, float * GradientAng, int width, int height)
{
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;
	int i = 0; int j = 0; int k = 0;
	ptrCadInfo->NumForMartixNow = 0;
	int numIter = ptrCadInfo->numIter;
	float thresh_gray = 70.0f; //ptrCadInfo->mGrayTheshold;
	float AngTol = 5.01f; // ptrCadInfo->mAngleTol;
	for (i = 0; i < ptrCadInfo->NumFaceList; i++)
	{
		MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);
		for (j = 0; j < 3; j++)
		{
			if (current->flag[j] == 1 && current->isEyeSee[j] == 1)
			{

				// 先求法向量
				int jj = j + 1;
				if (jj == 3) { jj = 0; }
				float input0[3]; float input1[3]; float output0[2]; float output1[2];
				input0[0] = current->facePoint[j].v[0];
				input0[1] = current->facePoint[j].v[1];
				input0[2] = current->facePoint[j].v[2];

				input1[0] = current->facePoint[jj].v[0];
				input1[1] = current->facePoint[jj].v[1];
				input1[2] = current->facePoint[jj].v[2];

				ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);
				ThreeDim2SecDimPoint(input1, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output1);

				float a = output1[0] - output0[0];
				float b = output1[1] - output0[1];

				float len = sqrt(a * a + b * b);
				float tempA = a;
				a = -b / len;
				b = tempA / len;

				float t_angle = atan2(b, a);

				for (k = 1; k < current->linePointNum[j] - 1; k++)
				{
					ForMartix * mForMartix = &(ptrCadInfo->mForMartix[ptrCadInfo->NumForMartixNow]);

					input0[0] = current->linePoint[j][k].v[0];
					input0[1] = current->linePoint[j][k].v[1];
					input0[2] = current->linePoint[j][k].v[2];

					ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);

					current->line2DPoint[j][k].v[0] = output0[0];
					current->line2DPoint[j][k].v[1] = output0[1];

					int currentX = (int)(output0[0] + 0.5f);
					int currentY = (int)(output0[1] + 0.5f);

					int index[2] = { 0, 0 };
					int count = 0;
					int value[4];
					float valueF[4];
					float valueIndex[2];
					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] + m * a;
						output1[1] = output0[1] + m * b;


						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						float dAngle = fabs(GradientAng[index[1] * width + index[0]] - -t_angle);//fabs(GradientLen[(index[1] * width + index[0]) * 4 + 2] - t_angle);
						if (GradientLen[index[1] * width + index[0]] > thresh_gray && dAngle < AngTol)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];
							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}

					for (int m = 0; m < numIter; m++)
					{
						output1[0] = output0[0] - m * a;
						output1[1] = output0[1] - m * b;

						index[0] = (int)(output1[0] + 0.5f);
						index[1] = (int)(output1[1] + 0.5f);

						index[0] = CLAMP(index[0], 0, width - 1);
						index[1] = CLAMP(index[1], 0, height - 1);

						float dAngle = fabs(GradientAng[index[1] * width + index[0]] - t_angle);// fabs(GradientLen[(index[1] * width + index[0]) * 4 + 2] - t_angle);
						if (GradientLen[index[1] * width + index[0]] > thresh_gray && dAngle < AngTol)
						{
							value[count * 2 + 0] = index[0];
							value[count * 2 + 1] = index[1];

							valueF[count * 2 + 0] = output1[0];
							valueF[count * 2 + 1] = output1[1];
							valueIndex[count] = m;
							count++;
							break;
						}

					}
					if (1 == count)
					{
						float aa = (value[0] + valueF[0]) / 2;
						float bb = (value[1] + valueF[1]) / 2;

						mForMartix->d2FindPoint.v[0] = aa;
						mForMartix->d2FindPoint.v[1] = bb;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;



						ptrCadInfo->NumForMartixNow++;

						if (ptrCadInfo->NumForMartixNow >= ptrCadInfo->NumForMartixCap)
						{

						}

					}
					else if (2 == count) // 找到一个比较近的值
					{
						int suitIndex = 0;
						suitIndex = valueIndex[0] < valueIndex[1] ? 0 : 1;


						mForMartix->d2FindPoint.v[0] = (value[suitIndex * 2 + 0] + valueF[suitIndex * 2 + 0]) / 2.0f;
						mForMartix->d2FindPoint.v[1] = (value[suitIndex * 2 + 1] + valueF[suitIndex * 2 + 1]) / 2.0f;

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb / len;

						ptrCadInfo->NumForMartixNow++;

					}

				}
			}
		}

	}

	return 0;
}
