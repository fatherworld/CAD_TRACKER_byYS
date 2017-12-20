#include "CadStruct.h"
#include "CadGaosi.h"
#include "CadMatrix.h"
#include "CadMacro.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int initBuffer(void * ptrCadInfo, int width, int height)
	{
		CadInfo * t_ptrCadInfo = (CadInfo *)ptrCadInfo;

		t_ptrCadInfo->bufferList[mBgrBuffer] = (unsigned char *)calloc(1, sizeof(char) * width * height * 3);
		t_ptrCadInfo->bufferSize[mBgrBuffer].width = width;
		t_ptrCadInfo->bufferSize[mBgrBuffer].height = height;

		t_ptrCadInfo->bufferList[mGrayBuffer] = (unsigned char *)calloc(1, sizeof(char) * width * height);
		t_ptrCadInfo->bufferSize[mGrayBuffer].width = width;
		t_ptrCadInfo->bufferSize[mGrayBuffer].height = height;

		t_ptrCadInfo->bufferList[mNormalizedBuffer] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mNormalizedBuffer].width = width;
		t_ptrCadInfo->bufferSize[mNormalizedBuffer].height = height;

		t_ptrCadInfo->bufferSize[mGaussianKernal].width = 3;
		t_ptrCadInfo->bufferSize[mGaussianKernal].height = 3;
		t_ptrCadInfo->bufferList[mGaussianKernal] = NULL;

		makeGaussianData((float **)(&(t_ptrCadInfo->bufferList[mGaussianKernal])), t_ptrCadInfo->bufferSize[mGaussianKernal].width,
			t_ptrCadInfo->bufferSize[mGaussianKernal].height, 1.5f);

		t_ptrCadInfo->bufferList[mGaussianBuffer] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mGaussianBuffer].width = width;
		t_ptrCadInfo->bufferSize[mGaussianBuffer].height = height;

		t_ptrCadInfo->bufferList[mGradientBuffer] = (float *)calloc(1, sizeof(float) * width * height * 4);
		t_ptrCadInfo->bufferSize[mGradientBuffer].width = width;
		t_ptrCadInfo->bufferSize[mGradientBuffer].height = height;

		t_ptrCadInfo->bufferList[mGradientLenBuffer] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mGradientLenBuffer].width = width;
		t_ptrCadInfo->bufferSize[mGradientLenBuffer].height = height;

		t_ptrCadInfo->bufferList[mGradientAng] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mGradientAng].width = width;
		t_ptrCadInfo->bufferSize[mGradientAng].height = height;

		t_ptrCadInfo->bufferList[mGradientEdge] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mGradientEdge].width = width;
		t_ptrCadInfo->bufferSize[mGradientEdge].height = height;

		t_ptrCadInfo->bufferList[mMakeGrayBuffer] = (unsigned char *)calloc(1, sizeof(char) * width * height);
		t_ptrCadInfo->bufferSize[mMakeGrayBuffer].width = width;
		t_ptrCadInfo->bufferSize[mMakeGrayBuffer].height = height;

		t_ptrCadInfo->bufferList[mMakeGraySmallBuffer] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mMakeGraySmallBuffer].width = width;
		t_ptrCadInfo->bufferSize[mMakeGraySmallBuffer].height = height;

		t_ptrCadInfo->bufferList[mMakeSmallNormalizeBuffer] = (float *)calloc(1, sizeof(float) * width * height);
		t_ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].width = width;
		t_ptrCadInfo->bufferSize[mMakeSmallNormalizeBuffer].height = height;



		t_ptrCadInfo->bufferList[mMakeSmallGradientBuffer] = (float *)calloc(1, sizeof(float) * width * height * 4);
		t_ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].width = width;
		t_ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].height = height;

		memset(t_ptrCadInfo->mAllMartix.U_pre, 0, sizeof(float) * 6);

		memset(t_ptrCadInfo->mAllMartix.U, 0, sizeof(float) * 6);

		return 0;
	}


	int memcpyNoZeroGradient(void * t_ptrCadInfo)
	{
		CadInfo * ptrCadInfo = (CadInfo *)t_ptrCadInfo;
		int ret = 0;
		int width = ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].width;
		int height = ptrCadInfo->bufferSize[mMakeSmallGradientBuffer].height;
		float * ptr = (float *)(ptrCadInfo->bufferList[mMakeSmallGradientBuffer]);

		int * ptrIndex = (int *)(ptrCadInfo->bufferList[mMakeNoZeroIndexBuffer]);
		float * ptrGridient = (float *)(ptrCadInfo->bufferList[mMakeNoZeroGradientBuffer]);
		int i = 0;
		int j = 0;
		int k = 0;
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if (ptr[(i * width + j) * 4 + 3] > 0.0f)
				{

					ptrIndex[k * 2 + 0] = j;
					ptrIndex[k * 2 + 1] = i;

					ptrGridient[k * 4 + 0] = ptr[(i * width + j) * 4 + 0];
					ptrGridient[k * 4 + 1] = ptr[(i * width + j) * 4 + 1];
					ptrGridient[k * 4 + 2] = ptr[(i * width + j) * 4 + 2];
					ptrGridient[k * 4 + 3] = ptr[(i * width + j) * 4 + 3];

					k++;
				}
			}
		}
		if (k == ptrCadInfo->noZeroNum)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		return ret;
	}

	// 找到最大相似的位置，记录相似前后的位置
	int SaveDetectBeforAfter(void * ptrInfo, int offsetd[2])
	{
		CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;

		int i, j, k;
		ptrCadInfo->NumForMartixNow = 0;
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

					output0[0] = ptrCadInfo->bufferSize[mBgrBuffer].width - 1 - output0[0];
					
					output1[0] = ptrCadInfo->bufferSize[mBgrBuffer].width - 1 - output1[0];

					if (ptrCadInfo->mCameraInfo.transform[2] < 0)
					{
						output0[1] = ptrCadInfo->bufferSize[mBgrBuffer].height - 1 - output0[1];
						output1[1] = ptrCadInfo->bufferSize[mBgrBuffer].height - 1 - output1[1];
					}
					

					float a = output1[0] - output0[0];
					float b = output1[1] - output0[1];

					float len = sqrt(a * a + b * b);

					float tempa = a;
					a = -b / len;
					b = tempa / len;

					for (k = 1; k < current->linePointNum[j] - 1; k++)
					{
						ForMartix * mForMartix = &(ptrCadInfo->mForMartix[ptrCadInfo->NumForMartixNow]);

						input0[0] = current->linePoint[j][k].v[0];
						input0[1] = current->linePoint[j][k].v[1];
						input0[2] = current->linePoint[j][k].v[2];

						ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);

						output0[0] = ptrCadInfo->bufferSize[mBgrBuffer].width - 1 - output0[0];
						if (ptrCadInfo->mCameraInfo.transform[2] < 0)
						{
							output0[1] = ptrCadInfo->bufferSize[mBgrBuffer].height - 1 - output0[1];
						}

						current->line2DPoint[j][k].v[0] = output0[0];
						current->line2DPoint[j][k].v[1] = output0[1];


						mForMartix->d2FindPoint.v[0] = output0[0] + offsetd[0];
						mForMartix->d2FindPoint.v[1] = output0[1] + offsetd[1];

						mForMartix->d3Point.v[0] = input0[0];
						mForMartix->d3Point.v[1] = input0[1];
						mForMartix->d3Point.v[2] = input0[2];

						mForMartix->d2Point.v[0] = output0[0];
						mForMartix->d2Point.v[1] = output0[1];

						float ka = mForMartix->d2FindPoint.v[0] - mForMartix->d2Point.v[0];
						float kb = mForMartix->d2FindPoint.v[1] - mForMartix->d2Point.v[1];

						float len = sqrt(ka * ka + kb * kb);

						mForMartix->d2Normal.v[0] = ka / len;
						mForMartix->d2Normal.v[1] = kb /len;

						ptrCadInfo->NumForMartixNow++;

					}
				}
			}
		}


		return 0;
	}


	// 第一帧跟踪时候，把二维点都
	int FirstTrack(void * ptrInfo, int offsetd[2], float * GradientLen, int width, int height)
	{
		CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;

		int i, j, k;
		ptrCadInfo->NumForMartixNow = 0;
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

					float tempa = a;
					a = -b / len;
					b = tempa / len;

					for (k = 1; k < current->linePointNum[j] - 1; k++)
					{
						ForMartix * mForMartix = &(ptrCadInfo->mForMartix[ptrCadInfo->NumForMartixNow]);

						input0[0] = current->linePoint[j][k].v[0];
						input0[1] = current->linePoint[j][k].v[1];
						input0[2] = current->linePoint[j][k].v[2];

						ThreeDim2SecDimPoint(input0, ptrCadInfo->mCameraInfo.InRef, ptrCadInfo->mCameraInfo.RT, output0);

						current->line2DPoint[j][k].v[0] = output0[0];
						current->line2DPoint[j][k].v[1] = output0[1];


						output0[0] = output0[0] + offsetd[0];
						output0[1] = output0[1] + offsetd[1];


						int index[2] = { 0, 0 };
						int count = 0;
						int value[4];
						float valueF[4];
						float valueIndex[2];
						int numIter = 50;
						for (int m = 0; m < numIter; m++)
						{
							output1[0] = output0[0] + m * a;
							output1[1] = output0[1] + m * b;


							index[0] = (int)(output1[0] + 0.5f);
							index[1] = (int)(output1[1] + 0.5f);

							index[0] = CLAMP(index[0], 0, width - 1);
							index[1] = CLAMP(index[1], 0, height - 1);

							if (GradientLen[index[1] * width + index[0]] > 0.3f)
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

							if (GradientLen[index[1] * width + index[0]] > 0.3f)
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




	// 第一帧跟踪时候，把二维点都 // 穿入四通道梯度图
	int FirstTrack_1(void * ptrInfo, int offsetd[2], float * GradientLen, int width, int height)
	{
		CadInfo * ptrCadInfo = (CadInfo *)ptrInfo;
		float thresh_gray = ptrCadInfo->mGrayTheshold;
		float AngTol = ptrCadInfo->mAngleTol;
		int i, j, k;
		ptrCadInfo->NumForMartixNow = 0;
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

					float tempa = a;
					a = -b / len;
					b = tempa / len;

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


						output0[0] = output0[0] + offsetd[0];
						output0[1] = output0[1] + offsetd[1];

						int currentX = (int)(output0[0] + 0.5f);
						int currentY = (int)(output0[1] + 0.5f);

						int index[2] = { 0, 0 };
						int count = 0;
						int value[4];
						float valueF[4];
						float valueIndex[2];
						int numIter = ptrCadInfo->numIter;
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

							if (GradientLen[(index[1] * width + index[0]) * 4 + 3] > thresh_gray  && dAngle < AngTol)
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

#ifdef __cplusplus
}
#endif


