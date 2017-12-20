#include "CadMatrix.h"
#include "CadStruct.h"
#include "opencv2/opencv.hpp"
#include <iostream>


extern "C" int CadCalculateM(void * ptrCadInfo_)
{
#if 0
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	fopen_s(&fp, "nei.txt", "w");
	fopen_s(&fp1, "wai.txt", "w");
#endif
	CadInfo * ptrCadInfo = (CadInfo *)ptrCadInfo_;

	if (ptrCadInfo->NumForMartixNow == 0)
	{
		return -1;
	}
	

	MARTIX_Alva Jn6; 
	Jn6.rows = ptrCadInfo->NumForMartixNow;
	Jn6.cols = 6;
	Jn6.martix = (float *)calloc(sizeof(float), Jn6.cols * Jn6.rows);

	
	MARTIX_Alva Wnn;
	Wnn.cols = ptrCadInfo->NumForMartixNow;
	Wnn.rows = ptrCadInfo->NumForMartixNow;
	Wnn.martix = (float *)calloc(sizeof(float), Wnn.cols * Wnn.rows);

	MARTIX_Alva ep;
	ep.rows = ptrCadInfo->NumForMartixNow;
	ep.cols = 1;
	ep.martix = (float *)calloc(sizeof(float), ptrCadInfo->NumForMartixNow);

	MARTIX_Alva Et;
	Et.cols = 4; Et.rows = 4;
	Et.martix = ptrCadInfo->mCameraInfo.RT; // 目前是物体的RT 可能需要相机的RT-----------------------------

	MARTIX_Alva Jk; // 内参
	Jk.rows = 2;
	Jk.cols = 2;
	
	float H[4] = { 0.0f};
	H[0] = ptrCadInfo->mCameraInfo.InRef[0];
	H[1] = ptrCadInfo->mCameraInfo.InRef[1];
	H[2] = ptrCadInfo->mCameraInfo.InRef[4];
	H[3] = ptrCadInfo->mCameraInfo.InRef[5];
	Jk.martix = H;
#if 0
	fprintf(fp, "%f %f \n %f %f \n", H[0], H[1], H[2], H[3]);

	fclose(fp);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			fprintf(fp1, "%f ", ptrCadInfo->mCameraInfo.RT[i * 4 + j]);
		}
		fprintf(fp1, "\n");
	}
	fclose(fp1);
#endif
	

	int i = 0; int j = 0;
	for (i = 0; i < ptrCadInfo->NumForMartixNow; i++)
	{
		ForMartix * ptrForMartix = &(ptrCadInfo->mForMartix[i]);
		MARTIX_Alva Ni; // 法向量
		float Nii[2] = {0.0f};
		Ni.martix = Nii;
		Ni.rows = 1; Ni.cols = 2;
		Ni.martix[0] = ptrForMartix->d2Normal.v[0];
		Ni.martix[1] = ptrForMartix->d2Normal.v[1];

		MARTIX_Alva obj; // 物体坐标系下的坐标
		obj.rows = 4;  obj.cols = 1;
		float objj[4] = { 0.0f };
		objj[0] = ptrForMartix->d3Point.v[0]; objj[1] = ptrForMartix->d3Point.v[1]; 
		objj[2] = ptrForMartix->d3Point.v[2]; objj[3] = 1.0f;
		obj.martix = objj;

		MARTIX_Alva obj_cam; // 相机坐标系下的坐标
		obj_cam.rows = 4; obj_cam.cols = 1;
		float obj_camm[4] = { 0.0f };
		obj_cam.martix = obj_camm;

		mul_maritx(Et, obj, &obj_cam);
		
		MARTIX_Alva Jp;
		
		Jp.rows = 2;
		Jp.cols = 4;
		float Jpp[8] = { 0.0f };
		
		Jpp[0 * 4 + 0] = 1.0f / obj_cam.martix[2];
		Jpp[0 * 4 + 1] = 0.0f;
		Jpp[0 * 4 + 2] = -obj_cam.martix[0] / (obj_cam.martix[2] * obj_cam.martix[2]);
		Jpp[0 * 4 + 3] = 0.0f;

		Jpp[1 * 4 + 0] = 0.0f;
		Jpp[1 * 4 + 1] = 1.0f / obj_cam.martix[2];
		Jpp[1 * 4 + 2] = -obj_cam.martix[1] / (obj_cam.martix[2] * obj_cam.martix[2]);
		Jpp[1 * 4 + 3] = 0.0f;

		Jp.martix = Jpp;
		float c = 0.0001f;
		float distant = sqrt(powf(ptrForMartix->d2FindPoint.v[0] - ptrForMartix->d2Point.v[0], 2.0f) + powf(ptrForMartix->d2FindPoint.v[1] - ptrForMartix->d2Point.v[1], 2.0f));
		
		Wnn.martix[i * Wnn.cols + i] = 1.0f / (c + distant);
		ep.martix[i] = distant;

		for (j = 0; j < 6; j++)
		{
			MARTIX_Alva Gj;
			Gj.rows = 4; Gj.cols = 4;
			Gj.martix = ptrCadInfo->mAllMartix.G[j];

			MARTIX_Alva temp;
			float temp_[16] = { 0.0f };
			temp.martix = temp_;

			MARTIX_Alva temp1;
			float temp_1[16] = { 0.0f };
			temp1.martix = temp_1;

			mul_maritx(Ni, Jk, &temp); // 12 * 22 = 12

			mul_maritx(temp, Jp, &temp1); // 12 * 24 = 14

			mul_maritx(temp1, Et, &temp); // 14 * 44 = 14

			mul_maritx(temp, Gj, &temp1); // 14 * 44 = 14

			mul_maritx(temp1, obj, &temp); // 14 * 41 = 11

			Jn6.martix[i * 6 + j] = temp.martix[0];

		}
	}


	MARTIX_Alva J6n;
	J6n.rows = 6;
	J6n.cols = ptrCadInfo->NumForMartixNow;
	J6n.martix = (float *)calloc(sizeof(float), J6n.cols * J6n.rows);

	translate_martix(Jn6, &J6n);
	MARTIX_Alva temp;
	temp.martix = (float *)calloc(sizeof(float), ptrCadInfo->NumForMartixNow * ptrCadInfo->NumForMartixNow);

	MARTIX_Alva temp1;
	temp1.martix = (float *)calloc(sizeof(float), ptrCadInfo->NumForMartixNow * ptrCadInfo->NumForMartixNow);


	mul_maritx(J6n, Wnn, &temp); //6n * nn = 6n

	mul_maritx(temp, Jn6, &temp1); // 6n * n6 = 66

	temp.rows = 6; temp.cols = 6;
	converse_martix(temp1, &temp); // 66 = 66
#if 0
	cv::Mat one = cv::Mat::zeros(6, 6, CV_32FC1);
	cv::Mat two = cv::Mat::zeros(6, 6, CV_32FC1);

	memcpy(one.data, temp1.martix, 6 * 6 * sizeof(float));

	cv::invert(one, two);

	//std::cout << "one" << std::endl << one;
	//std::cout << "two" << std::endl << two;

	memcpy(temp.martix, two.data, sizeof(float) * two.cols * two.rows);
#endif
	mul_maritx(temp, J6n, &temp1); // 66 * 6n = 6n

	mul_maritx(temp1, Wnn, &temp); // 6n * nn = 6n

	mul_maritx(temp, ep, &temp1); // 6n * n1 = 61

	MARTIX_Alva u;
	u.rows = 6; u.cols = 1;

	u.martix = temp1.martix;

	// 记录 U[6]
	memcpy(ptrCadInfo->mAllMartix.U, u.martix, sizeof(float) * 6);

	MARTIX_Alva u_sum;
	u_sum.cols = 4; u_sum.rows = 4;
	float s_sum_[16] = { 0.0f };
	u_sum.martix = s_sum_;

	MARTIX_Alva u_sum1;
	u_sum1.cols = 4; u_sum1.rows = 4;
	float s_sum_1[16];
	u_sum1.martix = s_sum_1;

	for (j = 0; j < 6; j++)
	{
		MARTIX_Alva Gj;
		Gj.rows = 4; Gj.cols = 4;
		Gj.martix = ptrCadInfo->mAllMartix.G[j];

		num_mul_matrix(Gj, u.martix[j], &u_sum1);

		mySelf_add_maritx(&u_sum, u_sum1);

	}
	MARTIX_Alva M_result;
	M_result.cols = 4; M_result.rows = 4;
	float result[16] = { 0.0f };
	result[0 * 4 + 0] = 1.0f;
	result[1 * 4 + 1] = 1.0f;
	result[2 * 4 + 2] = 1.0f;
	result[3 * 4 + 3] = 1.0f;
	M_result.martix = result;

	mySelf_add_maritx(&M_result, u_sum);

	powAndJieChe2(u_sum, &u_sum1);
	mySelf_add_maritx(&M_result, u_sum1);

	powAndJieChe3(u_sum,  &u_sum1);
	mySelf_add_maritx(&M_result, u_sum1);

	powAndJieChe4(u_sum, &u_sum1);
	mySelf_add_maritx(&M_result, u_sum1);

#if 0
	printf(" M \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf(" %f ", M_result.martix[i * 4 + j]);
		}
		printf("\n");
	}
#endif
	memcpy(ptrCadInfo->mAllMartix.M, M_result.martix, sizeof(float) * 16);



	free(Jn6.martix);
	free(J6n.martix);
	free(ep.martix);
	free(Wnn.martix);
	free(temp.martix);
	free(temp1.martix);

	return 0;

}







