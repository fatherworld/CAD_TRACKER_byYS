// CAD_Track.cpp : 定义控制台应用程序的入口点。
//
#include "Config.h"
#include "Format.h"
#include "opencv2/opencv.hpp"
#include "DXF3GL.h"
#include "CadInterface.h"
#include "CadMatrix.h"
#include "cadStruct.h"
#include "CadDraw.h"
#include "AlvaTimer.h"
#include "AndroidHelper.h"
#include "CadCalculateM.h"
#include "CadMacro.h"
#include "CadStruct.h"
#include <string>
using namespace cv;
void giveTestFile()
{
	return;
}

Mat EulerAng2Rotate(float RotX, float RotY, float RotZ)
{
	/*float temp;
	temp = RotX;
	RotX = RotY;
	RotY = temp;*/
	/*RotX = 0;
	RotY = 0;*/

	RotX = RotX*CV_PI / 180.0;
	RotY = RotY*CV_PI / 180.0;
	RotZ = RotZ*CV_PI / 180.0;

	float Cx, Sx, Cy, Sy, Cz, Sz;
	Cx = cos(RotX); Sx = sin(RotX);
	Cy = cos(RotY); Sy = sin(RotY);
	Cz = cos(RotZ); Sz = sin(RotZ);

	Mat Rz = Mat::zeros(3, 3, CV_32FC1);
	Rz.at<float>(0, 0) = Cz;
	Rz.at<float>(0, 1) = -Sz;
	Rz.at<float>(1, 0) = Sz;
	Rz.at<float>(1, 1) = Cz;
	Rz.at<float>(2, 2) = 1;

	float Mz[9] = { 0.0f };
	Mz[0] = Cz;
	Mz[1] = -Sz;
	Mz[1 * 3] = Sz;
	Mz[1 * 3 + 1] = Cz;
	Mz[2 * 3 + 2] = 1;


	Mat Ry = Mat::zeros(3, 3, CV_32FC1);
	Ry.at<float>(0, 0) = Cy;
	Ry.at<float>(0, 2) = Sy;
	Ry.at<float>(1, 1) = 1;
	Ry.at<float>(2, 0) = -Sy;
	Ry.at<float>(2, 2) = Cy;

	//Ry.at<float>(0, 0) = Cy;
	//Ry.at<float>(0, 2) = -Sy;
	//Ry.at<float>(1, 1) = 1;
	//Ry.at<float>(2, 0) = Sy;
	//Ry.at<float>(2, 2) = Cy;

	float Mx[9] = { 0.0f };
	float My[9] = { 0.0f };
	My[0] = Cy;
	My[2] = Sy;
	My[1 * 3 + 1] = 1;
	My[2 * 3 + 0] = -Sy;
	My[2 * 3 + 2] = Cy;

	Mat Rx = Mat::zeros(3, 3, CV_32FC1);
	Rx.at<float>(0, 0) = 1;
	Rx.at<float>(1, 1) = Cx;
	Rx.at<float>(1, 2) = -Sx;
	Rx.at<float>(2, 1) = Sx;
	Rx.at<float>(2, 2) = Cx;

	Mx[0] = 1;
	Mx[1 * 3 + 1] = Cx;
	Mx[1 * 3 + 2] = -Sx;
	Mx[2 * 3 + 1] = Sx;
	Mx[2 * 3 + 2] = Cx;
	

	float outTemp[9] = { 0.0f };
	float out[9] = { 0.0f };
	


	Mat R;
	R = Rz*Ry*Rx;
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

	Mat zy;
	zy = Rz * Ry;
	//std::cout << Rz << std::endl;
	//std::cout << Ry << std::endl;
	//std::cout << zy << std::endl;
	mul_maritx(input_martix_third, input_martix_two, &output_temp);

	mul_maritx(output_temp, input_martix_one, &output_martix);

	//R = Rx*Rx*Ry;
	//std::cout << R;
	return R;
}


int main(int argc, char *argv[])
{
	char *FileName;
	FileName = "nai8.dxf";

	float rx = 0.0f;
	float ry = 0.0f;
	float rz = 0.0f;

	cv::VideoCapture capture(0); // 打开摄像头
	cv::Mat imageTemp;

	capture >> imageTemp;

	int win_w = 640;
	int win_h = 480;
	Mat ImageMake = Mat(win_h, win_w, CV_8UC1);
	myGLInit(argc, FileName, argv, win_w, win_h);

	CadInfo * cadInfoPtr = (CadInfo *)initCad(FileName, win_w, win_h);

#if 0
	for (int i = 7; i < 27; i++)
	{
		char * source = NULL;
		int fileLen = 0;

	
		char fName[300];
		sprintf(fName, "I:/Workspace/OpenCVD_CADSimpleRecognitionAndTracking3_6_1(20171125)/testData/%d_pointData_[size= 179].txt", i);
		readData(fName, (alvaft)1, &source, &fileLen);




		memcpy(cadInfoPtr->mForMartix, source, sizeof(ForMartix) * 179);

		ForMartix *ptrMartix = cadInfoPtr->mForMartix;
	
		for (int i = 0; i < 179; i++)
		{
			
			
			printf("3d= %f %f %f \n", ptrMartix[i].d3Point.v[0], ptrMartix[i].d3Point.v[1], ptrMartix[i].d3Point.v[2]);
			printf("2d= %f %f \n", ptrMartix[i].d2Point.v[0], ptrMartix[i].d2Point.v[1]);
			printf("2dFind= %f %f \n", ptrMartix[i].d2FindPoint.v[0], ptrMartix[i].d2FindPoint.v[1]);
			printf("Nor= %f %f \n", ptrMartix[i].d2Normal.v[0], ptrMartix[i].d2Normal.v[1]);
		}
		cadInfoPtr->NumForMartixNow = 179;
		free(source);

		source = NULL;
		sprintf(fName, "I:/Workspace/OpenCVD_CADSimpleRecognitionAndTracking3_6_1(20171125)/testData/%d_Befor_RT.txt", i);

		readData(fName, (alvaft)1 ,&source, &fileLen);

		char *p = source;

		char * p1 = NULL;

		float * RT = cadInfoPtr->mCameraInfo.RT;

		for (int i = 0; i < 4; i++)
		{
			sscanf(p, "%f %f %f %f ", &RT[i * 4 + 0], &RT[i * 4 + 1], &RT[i * 4 + 2], &RT[i * 4 + 3]);

			p1 = strstr(p, "\n");

			printf("%f %f %f %f \n", RT[i * 4 + 0], RT[i * 4 + 1], RT[i * 4 + 2], RT[i * 4 + 3]);
			p = p1 + 1;
		}

		CadCalculateM(cadInfoPtr);
		float *u = cadInfoPtr->mAllMartix.U;
		for (int i = 0; i < 6; i++)
		{
			printf("u= %f %f %f %f %f %f \n", u[0], u[1], u[2], u[3], u[4], u[5]);
		}

		printf("---%d \n", i);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%f ", cadInfoPtr->mAllMartix.M[i * 4 + j]);
			}
			printf("\n");
		}

		free(source);

	}
#endif


	memcpy(ImageMake.data, cadInfoPtr->bufferList[mMakeGrayBuffer],win_w * win_h);
#if 0
	for (int i = 0; i < cadInfoPtr->NumFaceList; i++)
	{
		
		MyFaceList * current = &(cadInfoPtr->MyFaceListPtr[i]);
		printf("%d ", i);
		for (int j = 0; j < 3; j++)
		{
			if (cadInfoPtr->MyFaceListPtr[i].flag[j] == 1 && cadInfoPtr->MyFaceListPtr[i].isEyeSee[j] == 1)
			{
				for (int m = 0; m < cadInfoPtr->MyFaceListPtr[i].linePointNum[j]; m++)
				{
					cv::Point px;
					px.x = current->line2DPoint[j][m].v[0];
					px.y = current->line2DPoint[j][m].v[1];

					circle(ImageMake, px, 3, Scalar(255, 0, 0), -1);
				}
			}
			
		}

	}
#endif

	//flip(ImageMake, ImageMake, 1);
	cv::imshow("make", ImageMake);
	
	cv::waitKey(0);

#if 0
	
	
	CamerInfo mCamerInfo;


	for (int i = 0; i < 50000; i++)
	{

		mCamerInfo.rotation[0] = i * 10.0f;
		mCamerInfo.rotation[1] = i * 10.0f;
		mCamerInfo.rotation[2] = i * 2.0f;

		mCamerInfo.translate[0] = .0f;
		mCamerInfo.translate[1] = .0f;
		mCamerInfo.translate[2] = 25.0f;

		mCamerInfo.fovy = 60.0f;
		mCamerInfo.zNear = 1.0f;
		mCamerInfo.zFar = 500.0f;

		mCamerInfo.Sigma = 30.0f;

		float t_RT[16];
		RoAndTranToRT(mCamerInfo.rotation[0], mCamerInfo.rotation[1], mCamerInfo.rotation[2], 
			mCamerInfo.translate[0], mCamerInfo.translate[1], mCamerInfo.translate[2], t_RT);

		memcpy(cadInfoPtr->mCameraInfo.RT, t_RT, sizeof(float) * 16);
		float input[3];
		float output[2];

		GLInfo * g_glInfo = myGLDeal(&mCamerInfo);
		//printf("sie = %d \n", g_glInfo->numPtrLines);

		char fName[200];
		sprintf_s(fName, "Image/xxx_%d.bmp", i);
		//WriteBMP(fName, &mImage, BIT24);    
		memset(g_glInfo->data, 0, sizeof(char) * 3);


		cv::Mat image = cv::Mat::zeros(win_h, win_w, CV_8UC3);

		cv::Mat imageTest = cv::Mat::zeros(win_h, win_w, CV_8UC3);
#if 0

		for (int i = 0; i < g_glInfo->numPtrLines; i++)
		{
			if (g_glInfo->ptrLines[i].isVisal)   ///g_LineInfo.mVectorPoint[i].isVisible
			{

				cv::Mat t_p0 = cv::Mat::zeros(4, 1, CV_32F);

				t_p0.at<float>(0, 0) = g_glInfo->ptrLines[i].m_Point[0].v[0];
				t_p0.at<float>(1, 0) = g_glInfo->ptrLines[i].m_Point[0].v[1];
				t_p0.at<float>(2, 0) = g_glInfo->ptrLines[i].m_Point[0].v[2];
				t_p0.at<float>(3, 0) = 1.0f;

				cv::Mat t_p1 = cv::Mat::zeros(4, 1, CV_32F);

				t_p1.at<float>(0, 0) = g_glInfo->ptrLines[i].m_Point[1].v[0];
				t_p1.at<float>(1, 0) = g_glInfo->ptrLines[i].m_Point[1].v[1];
				t_p1.at<float>(2, 0) = g_glInfo->ptrLines[i].m_Point[1].v[2];
				t_p1.at<float>(3, 0) = 1.0f;


				cv::Point p00, p11;

				cv::Mat m1 = cv::Mat::zeros(3, 4, CV_32F);

				m1.at<float>(0, 0) = g_glInfo->ax;
				m1.at<float>(1, 1) = g_glInfo->ay;

				m1.at<float>(0, 2) = g_glInfo->u0;
				m1.at<float>(1, 2) = g_glInfo->v0;
				m1.at<float>(2, 2) = 1.0f;

				cv::Mat R_T = cv::Mat::zeros(4, 4, CV_32F);


				cv::Mat p0 = cv::Mat::zeros(3, 1, CV_32F);
				cv::Mat p1 = cv::Mat::zeros(3, 1, CV_32F);

				Mat R_ = EulerAng2Rotate(mCamerInfo.rotation[0], mCamerInfo.rotation[1], mCamerInfo.rotation[2]);
				float r9[9];
				EulerAng2Rotate_(mCamerInfo.rotation[0], mCamerInfo.rotation[1], mCamerInfo.rotation[2], r9);

				R_T.at<float>(0, 0) = R_.at<float>(0, 0);
				R_T.at<float>(0, 1) = R_.at<float>(0, 1);
				R_T.at<float>(0, 2) = R_.at<float>(0, 2);
				R_T.at<float>(1, 0) = R_.at<float>(1, 0);
				R_T.at<float>(1, 1) = R_.at<float>(1, 1);
				R_T.at<float>(1, 2) = R_.at<float>(1, 2);
				R_T.at<float>(2, 0) = R_.at<float>(2, 0);
				R_T.at<float>(2, 1) = R_.at<float>(2, 1);
				R_T.at<float>(2, 2) = R_.at<float>(2, 2);
				//std::cout << "R_" << std::endl << R_ << std::endl;
				R_T.at<float>(0, 3) = mCamerInfo.translate[0];

				R_T.at<float>(1, 3) = mCamerInfo.translate[1];

				R_T.at<float>(2, 3) = -mCamerInfo.translate[2];

				R_T.at<float>(3, 3) = 1.0f;


				p0 = m1 * R_T*t_p0;
				p1 = m1 * R_T*t_p1;



				input[0] = g_glInfo->ptrLines[i].m_Point[0].v[0];
				input[1] = g_glInfo->ptrLines[i].m_Point[0].v[1];
				input[2] = g_glInfo->ptrLines[i].m_Point[0].v[2];

				ThreeDim2SecDimPoint( input, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output);

				//std::cout << "m1" << std::endl << m1 << std::endl;
				//std::cout << "R_T" << std::endl << R_T << std::endl;
				//std::cout << "t_p0" << std::endl << t_p0 << std::endl;
				//std::cout << "t_p1" << std::endl << t_p1 << std::endl;
				//std::cout << "p0" << std::endl << p0 << std::endl;
				//std::cout << "p1" << std::endl << p1 << std::endl;

				p00.x = p0.at<float>(0, 0) / p0.at<float>(2, 0);
				p00.y = p0.at<float>(1, 0) / p0.at<float>(2, 0);

				ThreeDim2SecDimPoint(input, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output);

				p11.x = p1.at<float>(0, 0) / p1.at<float>(2, 0);
				p11.y = p1.at<float>(1, 0) / p1.at<float>(2, 0);


				cv::line(image, p00, p11, cv::Scalar(255, 255, 0), 1);

				//printf("line %d %d %d %d\n", p00.x, p00.y, p11.x, p11.y);


				
			
			}


		}

#else
		Mat newImage = Mat(win_h, win_w, CV_8UC3);

		for (int i = 0; i < g_glInfo->NumFaceList; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int k = j + 1;
				if (k == 3)
				{
					k = 0;
				}
				float input0[3];
				float input1[3];

				float output0[2];
				float output1[2];
				input0[0] = g_glInfo->MyFaceListPtr[i].facePoint[j].v[0];
				input0[1] = g_glInfo->MyFaceListPtr[i].facePoint[j].v[1];
				input0[2] = g_glInfo->MyFaceListPtr[i].facePoint[j].v[2];

				input1[0] = g_glInfo->MyFaceListPtr[i].facePoint[k].v[0];
				input1[1] = g_glInfo->MyFaceListPtr[i].facePoint[k].v[1];
				input1[2] = g_glInfo->MyFaceListPtr[i].facePoint[k].v[2];

				ThreeDim2SecDimPoint(input0, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output0);
				ThreeDim2SecDimPoint(input1, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output1);


				cv::Point p00, p11;

				p00.x = output0[0];
				p00.y = output0[1];

				p11.x = output1[0];
				p11.y = output1[1];

				MyFaceList * current = &(cadInfoPtr->MyFaceListPtr[i]);
				if (g_glInfo->MyFaceListPtr[i].flag[j] == 1 && g_glInfo->MyFaceListPtr[i].isEyeSee[j] == 1)
				{
					cv::line(newImage, p00, p11, cv::Scalar(255, 255, 0), 1);

					
					for (int m = 0; m < cadInfoPtr->MyFaceListPtr[i].linePointNum[j]; m++)
					{
						cv::Point px;
						px.x = current->line2DPoint[j][m].v[0];
						px.y = current->line2DPoint[j][m].v[1];
					
						circle(newImage, px, 3, Scalar(255, 0, 0), -1);
					}
				}
			}
		}
		GLInfo * ptr = myEyeDeal(mCamerInfo.rotation[0], mCamerInfo.rotation[1], mCamerInfo.rotation[2],
			mCamerInfo.translate[0], mCamerInfo.translate[1], mCamerInfo.translate[2]);

		for (int i = 0; i < ptr->NumFaceList; i++)
		{
			memcpy(cadInfoPtr->MyFaceListPtr[i].isEyeSee, ptr->MyFaceListPtr[i].isEyeSee, sizeof(int) * 3);
		}
		CadDrawAllLine(cadInfoPtr);
		//flip(image, image, 1);
		//flip(newImage, newImage, 1);

		//cv::imshow("xxx", image);
		cv::imshow("newImage", newImage);

		Mat mynewImage = Mat(win_h, win_w, CV_8UC1, cadInfoPtr->bufferList[mMakeGrayBuffer]);
		
		//flip(mynewImage, mynewImage, 1);
		cv::imshow("mynewImage", mynewImage);
		cv::waitKey(30);
#endif
	}

#endif
	



	std::string ImFolder = "I:\\Workspace\\OpenCVD_CADSimpleRecognitionAndTracking3_6_1(20171125)\\ImFile";
	char ImName[200];
	std::string ImPath;



	printf("Image size: [%d %d]\n", imageTemp.cols, imageTemp.rows);
	int PictureNum = 80000;
	for (int FrameCount = 1; FrameCount < PictureNum; FrameCount++)
	{
		//sprintf_s(ImName, 200, "Track%d.jpg", FrameCount);
		//ImPath = ImFolder + "\\" + ImName;
		//imageTemp = imread(ImPath);

		capture >> imageTemp;
		Mat gray;
		//cvtColor(imageTemp, gray, CV_BGR2GRAY);
		Timer mTimer;
		startTime(&mTimer);
		int ret = CadDetect_Track(cadInfoPtr, imageTemp.data, 5);
		stopTime(&mTimer);
		printf("time = %d \n", diffTime(mTimer));
		if (1 == ret)
		{
#if 0
			printf("detect... .......%d \n", cadInfoPtr->NumForMartixNow);
			for (int i = 0; i < cadInfoPtr->NumForMartixNow; i++)
			{
				cv::Point px;
				px.x = cadInfoPtr->mForMartix[i].d2Point.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2Point.v[1];
			
			
				CLAMP(px.x, 0, imageTemp.cols - 1);
				CLAMP(px.y, 0, imageTemp.rows - 1);
			
				circle(imageTemp, px, 3, Scalar(255, 0, 0), -1);
			
				px.x =  cadInfoPtr->mForMartix[i].d2FindPoint.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2FindPoint.v[1];
			
			
				CLAMP(px.x, 0, imageTemp.cols - 1);
				CLAMP(px.y, 0, imageTemp.rows - 1);
				circle(imageTemp, px, 3, Scalar(0, 255, 0), -1);
			
			}
#endif

			float offset[2];
			offset[0] = cadInfoPtr->DetecResult.x - cadInfoPtr->edgeMin.x;
			offset[1] = cadInfoPtr->DetecResult.y - cadInfoPtr->edgeMin.y;
			for (int i = 0; i < cadInfoPtr->NumFaceList; i++)
			{
				MyFaceList * current = &(cadInfoPtr->MyFaceListPtr[i]);
				for (int j = 0; j < 3; j++)
				{
					if (current->flag[j] == 1 && current->isEyeSee[j] == 1)
					{
						int jj = j + 1;
						if (jj == 3) { jj = 0; }
						float input0[3]; float input1[3]; float output0[2]; float output1[2];
						input0[0] = current->facePoint[j].v[0];
						input0[1] = current->facePoint[j].v[1];
						input0[2] = current->facePoint[j].v[2];

						input1[0] = current->facePoint[jj].v[0];
						input1[1] = current->facePoint[jj].v[1];
						input1[2] = current->facePoint[jj].v[2];

						ThreeDim2SecDimPoint(input0, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output0);
						ThreeDim2SecDimPoint(input1, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output1);

						cv::Point p1;
						p1.x = output0[0] + offset[0];
						p1.y = output0[1] + offset[1];

						cv::Point p2;
						p2.x = output1[0] + offset[0];
						p2.y = output1[1] + offset[1];

						line(imageTemp, p1, p2, cv::Scalar(255, 0, 255), 2);

						circle(imageTemp, p1, 2, cv::Scalar(255, 0, 255), 2);
						circle(imageTemp, p2, 2, cv::Scalar(255, 0, 255), 2);

			}
		}

	}
			//cv::flip(imageTemp, imageTemp, 1);
			cv::imshow("show", imageTemp);
			cv::waitKey(30);
 			//printf("detect... .......end \n");
#if 0
			FILE *fp;
			fopen_s(&fp, "model.txt", "wb");

			fwrite(cadInfoPtr->mForMartix, cadInfoPtr->NumForMartixNow, sizeof(ForMartix), fp);

			fclose(fp);

			printf("modelnum = %d \n", cadInfoPtr->NumForMartixNow);
#endif

		}
#if 1
		else if (2 == ret)
		{
			float offset[2];
			offset[0] = cadInfoPtr->DetecResult.x - cadInfoPtr->edgeMin.x;
			offset[1] = cadInfoPtr->DetecResult.y - cadInfoPtr->edgeMin.y;
			//printf("tranking .......\n");
#if 0
			for (int i = 0; i < cadInfoPtr->NumForMartixNow; i++)
			{
				cv::Point px;
				//px.x = cadInfoPtr->bufferSize[mGrayBuffer].width - 1 - cadInfoPtr->mForMartix[i].d2Point.v[0];
				px.x = cadInfoPtr->mForMartix[i].d2Point.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2Point.v[1];
		
				CLAMP(px.x, 0, imageTemp.cols - 1);
				CLAMP(px.y, 0, imageTemp.rows - 1);

				circle(imageTemp, px, 3, Scalar(255, 0, 0), -1);

				//px.x = cadInfoPtr->bufferSize[mGrayBuffer].width - 1  - cadInfoPtr->mForMartix[i].d2FindPoint.v[0];
				px.x = cadInfoPtr->mForMartix[i].d2FindPoint.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2FindPoint.v[1];
			
				CLAMP(px.x, 0, imageTemp.cols - 1);
				CLAMP(px.y, 0, imageTemp.rows - 1);
				circle(imageTemp, px, 3, Scalar(0, 255, 0), -1);

			}
#endif

			for (int i = 0; i < cadInfoPtr->NumFaceList; i++)
			{
				MyFaceList * current = &(cadInfoPtr->MyFaceListPtr[i]);
				for (int j = 0; j < 3; j++)
				{
					if (current->flag[j] == 1 && current->isEyeSee[j] == 1)
					{
						int jj = j + 1;
						if (jj == 3) { jj = 0; }
						float input0[3]; float input1[3]; float output0[2]; float output1[2];
						input0[0] = current->facePoint[j].v[0];
						input0[1] = current->facePoint[j].v[1];
						input0[2] = current->facePoint[j].v[2];

						input1[0] = current->facePoint[jj].v[0];
						input1[1] = current->facePoint[jj].v[1];
						input1[2] = current->facePoint[jj].v[2];

						ThreeDim2SecDimPoint(input0, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output0);
						ThreeDim2SecDimPoint(input1, cadInfoPtr->mCameraInfo.InRef, cadInfoPtr->mCameraInfo.RT, output1);

						cv::Point p1;
						p1.x = output0[0];
						p1.y = output0[1];

						cv::Point p2;
						p2.x = output1[0];
						p2.y = output1[1];

						line(imageTemp, p1, p2, cv::Scalar(255, 0, 255), 2);

						circle(imageTemp, p1, 2, cv::Scalar(255, 0, 255), 2);
						circle(imageTemp, p2, 2, cv::Scalar(255, 0, 255), 2);

					}
				}

			}
			cv::imshow("show", imageTemp);
			cv::waitKey(2);
		}
#endif
		else
		{
#if 1
			//printf("w=%d  h=%d \n", cadInfoPtr->bufferSize[mMakeGraySmallBuffer].width, cadInfoPtr->bufferSize[mMakeGraySmallBuffer].height);
			for (int i = 0; i < cadInfoPtr->bufferSize[mMakeGraySmallBuffer].height; i++)
			{
				for (int j = 0; j < cadInfoPtr->bufferSize[mMakeGraySmallBuffer].width; j++)
				{
					unsigned char *temp = (unsigned char *)cadInfoPtr->bufferList[mMakeGraySmallBuffer];
					unsigned char value = temp[i * cadInfoPtr->bufferSize[mMakeGraySmallBuffer].width + j];
					int ii = i + cadInfoPtr->edgeMin.y;
					int jj = j + cadInfoPtr->edgeMin.x;
					CLAMP(ii, 0, imageTemp.rows - 1);
					CLAMP(jj, 0, imageTemp.cols - 1);
					if (value != 0)
					{
						imageTemp.data[(ii * imageTemp.cols + jj) * 3 + 0] = 255;
						imageTemp.data[(ii * imageTemp.cols + jj) * 3 + 1] = 0;
						imageTemp.data[(ii * imageTemp.cols + jj) * 3 + 2] = 0;
					}

				}
			}
#else
			for (int i = 0; i < cadInfoPtr->NumForMartixNow; i++)
			{
				cv::Point px;
				px.x = cadInfoPtr->mForMartix[i].d2Point.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2Point.v[1];

				circle(imageTemp, px, 3, Scalar(255, 0, 0), -1);

				px.x = cadInfoPtr->mForMartix[i].d2FindPoint.v[0];
				px.y = cadInfoPtr->mForMartix[i].d2FindPoint.v[1];

				circle(imageTemp, px, 3, Scalar(0, 255, 0), -1);

			}
#endif

			cv::imshow("show", imageTemp);
			cv::waitKey(30);
		}

		
	}



	return 0;
}
