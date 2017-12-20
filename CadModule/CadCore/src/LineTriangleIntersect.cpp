#include <math.h>
#include "LineTriangleIntersect.h"
#include "CadStruct.h"

#include <opencv2/opencv.hpp>




namespace LTI {

	std::vector<std::vector<std::vector<float>>> CalcFive(void * ptr) {
		
		CadInfo * gli = (CadInfo *)ptr;
		std::vector<std::vector<float>> CutSurfaceAll;
		std::vector<std::vector<std::vector<float>>> Calcs;
		for (int ss = 0; ss < gli->NumFaceList; ss++)
		{
			TriangleFace TF;
			//Point3f FacePnt1,FacePnt2, FacePnt3;

			TF.P1.x = gli->MyFaceListPtr[ss].facePoint[0].v[0];
			TF.P1.y = gli->MyFaceListPtr[ss].facePoint[0].v[1];
			TF.P1.z = gli->MyFaceListPtr[ss].facePoint[0].v[2];

			TF.P2.x = gli->MyFaceListPtr[ss].facePoint[1].v[0];
			TF.P2.y = gli->MyFaceListPtr[ss].facePoint[1].v[1];
			TF.P2.z = gli->MyFaceListPtr[ss].facePoint[1].v[2];

			TF.P3.x = gli->MyFaceListPtr[ss].facePoint[2].v[0];
			TF.P3.y = gli->MyFaceListPtr[ss].facePoint[2].v[1];
			TF.P3.z = gli->MyFaceListPtr[ss].facePoint[2].v[2];

			std::vector<float> CutSurface;
			CutSurface = LTI::CalcCutSurface(TF);
			//CutSurfaceAll.push_back(CutSurface);
			Calcs.push_back(LTI::CalcThreeCutSurface(TF, CutSurface));
		}
		return Calcs;
	}


	//计算p1到p2的距离的平方
	float Distance(cv::Point3f &p1, cv::Point3f &p2)
	{
		float dist;
		dist = ((p2.x - p1.x)*(p2.x - p1.x)
			+ (p2.y - p1.y)*(p2.y - p1.y)
			+ (p2.z - p1.z)*(p2.z - p1.z));
		return (float)sqrt(dist);
	}

	//利用海伦公式求变成为a,b,c的三角形的面积
	float Area(float a, float b, float c)
	{
		float s = (a + b + c) / 2;
		return (float)sqrt(s*(s - a)*(s - b)*(s - c));
	}

	/*
	* @ pagrams
	* @ LinePoint1， LinePoint1                           直线的两个点坐标
	* @ TrianglePoint1, TrianglePoint2, TrianglePoint3    三角形的三个点坐标
	* @ InTriangle                                        输入1表示求线段与三角形相交的交点，输入0表示计算直线与平面的交点
	* @ result                                            线段（直线）与三角面片（平面）的交点坐标
	*/
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, cv::Point3f &TrianglePoint1, cv::Point3f
		&TrianglePoint2, cv::Point3f &TrianglePoint3, bool InTriangle, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //三角形所在平面的法向量

		cv::Point3f VP12, VP13;  //三角形的边方向向量

		cv::Point3f CrossPoint;  //直线与平面的交点

		float TriD;  //平面方程常数项

		cv::Point3f LineV;  //直线的方向向量
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;


		/*-------计算平面的法向量及常数项-------*/
		//point1->point2
		VP12.x = TrianglePoint2.x - TrianglePoint1.x;
		VP12.y = TrianglePoint2.y - TrianglePoint1.y;
		VP12.z = TrianglePoint2.z - TrianglePoint1.z;
		//point1->point3
		VP13.x = TrianglePoint3.x - TrianglePoint1.x;
		VP13.y = TrianglePoint3.y - TrianglePoint1.y;
		VP13.z = TrianglePoint3.z - TrianglePoint1.z;
		//计算平面的法向量 VP12xVP13
		TriangleV.x = VP12.y * VP13.z - VP12.z * VP13.y;
		TriangleV.y = -(VP12.x * VP13.z - VP12.z * VP13.x);
		TriangleV.z = VP12.x * VP13.y - VP12.y * VP13.x;
		//计算平面方程的常数项
		TriD = -(TriangleV.x*TrianglePoint1.x + TriangleV.y*TrianglePoint1.y + TriangleV.z*TrianglePoint1.z);

		/*----------------------------求解直线与平面的交点坐标-----------------------*/
		/* 思路：
		* 首先将直线方程转换为参数方程形式，然后代入平面方程，求得参数t，
		* 将t代入直线的参数方程即可求出交点坐标
		*/
		float tempU, tempD; //临时变量
		tempU = TriangleV.x*LinePoint1.x + TriangleV.y*LinePoint1.y + TriangleV.z*LinePoint1.z + TriD;
		tempD = TriangleV.x*LineV.x + TriangleV.y*LineV.y + TriangleV.z*LineV.z;  //判断平面法向量和直线的方向向量是否垂直

		//std::cout << "tempU = " << tempU << ", tempD = " << tempD << std::endl;

		//直线与平面平行或在平面上
		if (tempD == 0.0 || fabs(tempU) < 0.01)
		{
			//printf("The line is parallel with the plane.\n");
			return false;
		}

		float rd1 = tempU;
		float rd2 = TriangleV.x*LinePoint2.x + TriangleV.y*LinePoint2.y + TriangleV.z*LinePoint2.z + TriD;
		//std::cout << "rd1 = " << rd1 << ",  rd2 = " << rd2 << std::endl;
		if (rd1*rd2 >= 0) {
			return false;
		}

		//计算参数t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
			(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//计算直线与三角形的交点坐标
		CrossPoint.x = LineV.x*t + LinePoint1.x;
		CrossPoint.y = LineV.y*t + LinePoint1.y;
		CrossPoint.z = LineV.z*t + LinePoint1.z;

#if 0
		std::vector<float> CutSurface(4);
		CutSurface[0] = TriangleV.x, CutSurface[1] = TriangleV.y, CutSurface[2] = TriangleV.z, CutSurface[3] = TriD;
		std::vector<std::vector<float>>  Calcs;
		TriangleFace TF;
		TF.P1 = TrianglePoint1, TF.P2 = TrianglePoint2, TF.P3 = TrianglePoint3;
		Calcs = CalcThreeCutSurface(TF, CutSurface);

		float a = CrossPoint.x * Calcs[0][0] + CrossPoint.y * Calcs[0][1] + CrossPoint.z * Calcs[0][2] + Calcs[0][3];
		float b = CrossPoint.x * Calcs[1][0] + CrossPoint.y * Calcs[1][1] + CrossPoint.z * Calcs[1][2] + Calcs[1][3];
		float c = CrossPoint.x * Calcs[2][0] + CrossPoint.y * Calcs[2][1] + CrossPoint.z * Calcs[2][2] + Calcs[2][3];
		if (a*Calcs[3][0] < 0 || b*Calcs[3][1] < 0 || c*Calcs[3][2] < 0) {
			return false;
		}
#endif

#if 1
		//double t1 = timeBegin();
		/*-----------------------判断交点是否在三角形内部--------------------*/
		//计算三角形三条边的长度
		float d12 = Distance(TrianglePoint1, TrianglePoint2);
		float d13 = Distance(TrianglePoint1, TrianglePoint3);
		float d23 = Distance(TrianglePoint2, TrianglePoint3);

		//计算交点到三个顶点的长度
		float c1 = Distance(CrossPoint, TrianglePoint1);
		float c2 = Distance(CrossPoint, TrianglePoint2);
		float c3 = Distance(CrossPoint, TrianglePoint3);

		//求三角形及子三角形的面积
		float areaD = Area(d12, d13, d23); //三角形面积
		float area1 = Area(c1, c2, d12); //子三角形1的面积
		float area2 = Area(c1, c3, d13); //子三角形2的面积
		float area3 = Area(c2, c3, d23); //子三角形3的面积

		//std::cout << "交点： " << result << std::endl;
		//std::cout << "面积差：" << fabs(area1 + area2 + area3 - areaD) << std::endl;
		 //根据面积判断点是否在三角形内部
		if (fabs(area1 + area2 + area3 - areaD) > 0.01 && InTriangle)
		{
			return false;
		}
		//double t2 = timeEnd();
		//timePrint("test-time = ", t1, t2);
#endif
		result = CrossPoint;  // 交点坐标
		return true;
	}

	// @ 2017-12-07
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, std::vector<std::vector<float>> & Calcs, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //三角形所在平面的法向量
		cv::Point3f CrossPoint;  //直线与平面的交点

		float TriD;  //平面方程常数项

		cv::Point3f LineV;  //直线的方向向量
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;

		//LineV.x = -LineV.x;
		//LineV.y = -LineV.y;
		//LineV.z = -LineV.z;
		/*-------计算平面的法向量及常数项-------*/
		//TriangleV.x = Calcs[4][0];
		//TriangleV.y = Calcs[4][1];
		//TriangleV.z = Calcs[4][2];
		////计算平面方程的常数项
		//TriD = Calcs[4][3];

		/*----------------------------求解直线与平面的交点坐标-----------------------*/
		/* 思路：
		* 首先将直线方程转换为参数方程形式，然后代入平面方程，求得参数t，
		* 将t代入直线的参数方程即可求出交点坐标
		*/
		float tempU, tempD; //临时变量
		//tempU = TriangleV.x * LinePoint1.x + TriangleV.y * LinePoint1.y + TriangleV.z * LinePoint1.z + TriD;
		//tempD = TriangleV.x * LineV.x + TriangleV.y * LineV.y + TriangleV.z * LineV.z;  //判断平面法向量和直线的方向向量是否垂直

		tempU = Calcs[4][0] * LinePoint1.x + Calcs[4][1] * LinePoint1.y + Calcs[4][2] * LinePoint1.z + Calcs[4][3];
		tempD = Calcs[4][0] * LineV.x + Calcs[4][1] * LineV.y + Calcs[4][2] * LineV.z;  //判断平面法向量和直线的方向向量是否垂直

		 //std::cout << "tempU = " << tempU << ", tempD = " << tempD << std::endl;

		 //直线与平面平行或在平面上
		if (tempD == 0.0 || fabs(tempU) < 0.01)
		{
			//printf("The line is parallel with the plane.\n");
			return false;
		}

		float rd1 = tempU;
		float rd2 = Calcs[4][0] * LinePoint2.x + Calcs[4][1] * LinePoint2.y + Calcs[4][2] * LinePoint2.z + Calcs[4][3];
		//std::cout << "rd1 = " << rd1 << ",  rd2 = " << rd2 << std::endl;
		if (rd1*rd2 >= 0) {
			return false;
		}

		//计算参数t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
		(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//计算直线与三角形的交点坐标
		CrossPoint.x = LineV.x*t + LinePoint1.x;
		CrossPoint.y = LineV.y*t + LinePoint1.y;
		CrossPoint.z = LineV.z*t + LinePoint1.z;

#if 1
		//std::vector<float> CutSurface(4);
		//CutSurface[0] = TriangleV.x, CutSurface[1] = TriangleV.y, CutSurface[2] = TriangleV.z, CutSurface[3] = TriD;
		//std::vector<std::vector<float>>  Calcs;
		//TriangleFace TF;
		//TF.P1 = TrianglePoint1, TF.P2 = TrianglePoint2, TF.P3 = TrianglePoint3;
		//Calcs = CalcThreeCutSurface(TF, CutSurface);

		//double t1 = timeBegin();
		float a = CrossPoint.x * Calcs[0][0] + CrossPoint.y * Calcs[0][1] + CrossPoint.z * Calcs[0][2] + Calcs[0][3];
		float b = CrossPoint.x * Calcs[1][0] + CrossPoint.y * Calcs[1][1] + CrossPoint.z * Calcs[1][2] + Calcs[1][3];
		float c = CrossPoint.x * Calcs[2][0] + CrossPoint.y * Calcs[2][1] + CrossPoint.z * Calcs[2][2] + Calcs[2][3];
		//if (a*Calcs[3][0] < 0 || b*Calcs[3][1] < 0 || c*Calcs[3][2] < 0) {
		//	return false;
		//}
		if ( (a > 0 && Calcs[3][0] < 0) || (a < 0 && Calcs[3][0] > 0) || (b > 0 && Calcs[3][1] < 0) || (b < 0 && Calcs[3][1] > 0) || 
			(c > 0 && Calcs[3][2] < 0) || (c < 0 && Calcs[3][2] > 0)) {
			return false;
		}
		//double t2 = timeEnd();
		//timePrint("test-time = ", t1, t2);
#endif

#if 0
		/*-----------------------判断交点是否在三角形内部--------------------*/
		//计算三角形三条边的长度
		float d12 = Distance(TrianglePoint1, TrianglePoint2);
		float d13 = Distance(TrianglePoint1, TrianglePoint3);
		float d23 = Distance(TrianglePoint2, TrianglePoint3);

		//计算交点到三个顶点的长度
		float c1 = Distance(CrossPoint, TrianglePoint1);
		float c2 = Distance(CrossPoint, TrianglePoint2);
		float c3 = Distance(CrossPoint, TrianglePoint3);

		//求三角形及子三角形的面积
		float areaD = Area(d12, d13, d23); //三角形面积
		float area1 = Area(c1, c2, d12); //子三角形1的面积
		float area2 = Area(c1, c3, d13); //子三角形2的面积
		float area3 = Area(c2, c3, d23); //子三角形3的面积

										 //std::cout << "交点： " << result << std::endl;
										 //std::cout << "面积差：" << fabs(area1 + area2 + area3 - areaD) << std::endl;
										 //根据面积判断点是否在三角形内部
		if (fabs(area1 + area2 + area3 - areaD) > 0.01 && InTriangle)
		{
			return false;
		}
#endif
		result = CrossPoint;  // 交点坐标
		return true;
	}





	/*
	* @ pagrams
	* @ LinePoint1， LinePoint1    直线的两个点坐标
	* @ CutSurface                 分割面方程的系数
	* @ result                     线段（直线）与三角面片（平面）的交点坐标
	*/
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, TriangleFace TF, std::vector<float> CutSurface, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //三角形所在平面的法向量

		cv::Point3f CrossPoint;  //直线与平面的交点

		float TriD;  //平面方程常数项

		cv::Point3f LineV;  //直线的方向向量
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;

		/*-------计算平面的法向量及常数项-------*/
		//计算平面的法向量 VP12xVP13
		TriangleV.x = CutSurface[0];
		TriangleV.y = CutSurface[1];
		TriangleV.z = CutSurface[2];
		//计算平面方程的常数项
		TriD = CutSurface[3];

		/*----------------------------求解直线与平面的交点坐标-----------------------*/
		/* 思路：
		* 首先将直线方程转换为参数方程形式，然后代入平面方程，求得参数t，
		* 将t代入直线的参数方程即可求出交点坐标
		*/
		float tempU, tempD; //临时变量
		tempU = TriangleV.x*LinePoint1.x + TriangleV.y*LinePoint1.y + TriangleV.z*LinePoint1.z + TriD;
		tempD = TriangleV.x*LineV.x + TriangleV.y*LineV.y + TriangleV.z*LineV.z;  //判断平面法向量和直线的方向向量是否垂直
		float rd2 = TriangleV.x*LinePoint2.x + TriangleV.y*LinePoint2.y + TriangleV.z*LinePoint2.z + TriD;
																				  //直线与平面平行或在平面上
		if (tempD == 0.0 || fabs(tempU) < 0.01)
		{
			//printf("The line is parallel with the plane.\n");
			return false;
		}
		if (tempU*rd2 >= 0) {
			return false;
		}
		//计算参数t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
		(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//计算直线与三角形的交点坐标
		CrossPoint.x = LineV.x*t + LinePoint1.x;
		CrossPoint.y = LineV.y*t + LinePoint1.y;
		CrossPoint.z = LineV.z*t + LinePoint1.z;

		/*-----------------------判断交点是否在三角形内部--------------------*/

		//计算三角形三条边的长度
		float d12 = Distance(TF.P1, TF.P2);
		float d13 = Distance(TF.P1, TF.P3);
		float d23 = Distance(TF.P2, TF.P3);

		//计算交点到三个顶点的长度
		float c1 = Distance(CrossPoint, TF.P1);
		float c2 = Distance(CrossPoint, TF.P2);
		float c3 = Distance(CrossPoint, TF.P3);

		//求三角形及子三角形的面积
		float areaD = Area(d12, d13, d23); //三角形面积
		float area1 = Area(c1, c2, d12); //子三角形1的面积
		float area2 = Area(c1, c3, d13); //子三角形2的面积
		float area3 = Area(c2, c3, d23); //子三角形3的面积

										 //std::cout << "交点： " << result << std::endl;
										 //std::cout << "面积差：" << fabs(area1 + area2 + area3 - areaD) << std::endl;
										 //根据面积判断点是否在三角形内部
		if (fabs(area1 + area2 + area3 - areaD) > 0.01)
		{
			return false;
		}
		result = CrossPoint;  // 交点坐标
		return true;
	}

	/* 把三角面片作为分割面，计算其平面方程
	*  @ params
	*  @ TF     三角面片的顶点坐标
	*/
	std::vector<float> CalcCutSurface(TriangleFace TF) {

		cv::Point3f TriangleV;  //三角形所在平面的法向量
		cv::Point3f VP12, VP13;  //三角形的边方向向量
		float TriD;  //平面方程的常数项

		/*-------计算平面的法向量及常数项-------*/
		//point1->point2
		VP12.x = TF.P2.x - TF.P1.x;
		VP12.y = TF.P2.y - TF.P1.y;
		VP12.z = TF.P2.z - TF.P1.z;
		//point1->point3
		VP13.x = TF.P3.x - TF.P1.x;
		VP13.y = TF.P3.y - TF.P1.y;
		VP13.z = TF.P3.z - TF.P1.z;
		//计算平面的法向量 VP12xVP13
		TriangleV.x = VP12.y*VP13.z - VP12.z*VP13.y;
		TriangleV.y = -(VP12.x*VP13.z - VP12.z*VP13.x);
		TriangleV.z = VP12.x*VP13.y - VP12.y*VP13.x;
		//计算平面方程的常数项
		TriD = -(TriangleV.x*TF.P1.x + TriangleV.y*TF.P1.y + TriangleV.z*TF.P1.z);

		std::vector<float> CutSurface(4);  //平面方程的三个系数和一个常数
		CutSurface[0] = TriangleV.x;
		CutSurface[1] = TriangleV.y;
		CutSurface[2] = TriangleV.z;
		CutSurface[3] = TriD;
		return CutSurface;
	}

	/* 求垂直三角面片的三个平面，且三个平面过三条边，计算其平面方程
	*  @ params
	*  @ TF     三角面片的顶点坐标
	*/
	std::vector<std::vector<float>> CalcThreeCutSurface(TriangleFace TF, std::vector<float> CutSurface) {

		TriangleFace TF1, TF2, TF3;
		cv::Point3f InTriangle;
		InTriangle = ((TF.P1 + TF.P2) / 2 + TF.P3) / 2;
		std::vector<float> CutSurface1, CutSurface2, CutSurface3;
		std::vector<std::vector<float>> CutSurfaceWithSign;

		// 三条边所在的垂直于三角面片的平面的对应的三个点
		TF1.P1 = TF.P1, TF1.P2 = TF.P2, TF1.P3 = cv::Point3f(TF.P1.x + 2 * CutSurface[0], TF.P1.y + 2 * CutSurface[1], TF.P1.z + 2 * CutSurface[2]);
		TF2.P1 = TF.P1, TF2.P2 = TF.P3, TF2.P3 = cv::Point3f(TF.P1.x + 2 * CutSurface[0], TF.P1.y + 2 * CutSurface[1], TF.P1.z + 2 * CutSurface[2]);
		TF3.P1 = TF.P3, TF3.P2 = TF.P2, TF3.P3 = cv::Point3f(TF.P3.x + 2 * CutSurface[0], TF.P3.y + 2 * CutSurface[1], TF.P3.z + 2 * CutSurface[2]);

		CutSurface1 = CalcCutSurface(TF1);
		CutSurface2 = CalcCutSurface(TF2);
		CutSurface3 = CalcCutSurface(TF3);
		CutSurfaceWithSign.push_back(CutSurface1);
		CutSurfaceWithSign.push_back(CutSurface2);
		CutSurfaceWithSign.push_back(CutSurface3);

		float a = InTriangle.x*CutSurface1[0] + InTriangle.y * CutSurface1[1] + InTriangle.z * CutSurface1[2] + CutSurface1[3];
		float b = InTriangle.x*CutSurface2[0] + InTriangle.y * CutSurface2[1] + InTriangle.z * CutSurface2[2] + CutSurface2[3];
		float c = InTriangle.x*CutSurface3[0] + InTriangle.y * CutSurface3[1] + InTriangle.z * CutSurface3[2] + CutSurface3[3];
		std::vector<float> sign(3);
		sign[0] = a;
		sign[1] = b;
		sign[2] = c;
		CutSurfaceWithSign.push_back(sign);
		CutSurfaceWithSign.push_back(CutSurface);
		return CutSurfaceWithSign;
	}
	
};

