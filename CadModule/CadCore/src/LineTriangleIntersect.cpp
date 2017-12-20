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


	//����p1��p2�ľ����ƽ��
	float Distance(cv::Point3f &p1, cv::Point3f &p2)
	{
		float dist;
		dist = ((p2.x - p1.x)*(p2.x - p1.x)
			+ (p2.y - p1.y)*(p2.y - p1.y)
			+ (p2.z - p1.z)*(p2.z - p1.z));
		return (float)sqrt(dist);
	}

	//���ú��׹�ʽ����Ϊa,b,c�������ε����
	float Area(float a, float b, float c)
	{
		float s = (a + b + c) / 2;
		return (float)sqrt(s*(s - a)*(s - b)*(s - c));
	}

	/*
	* @ pagrams
	* @ LinePoint1�� LinePoint1                           ֱ�ߵ�����������
	* @ TrianglePoint1, TrianglePoint2, TrianglePoint3    �����ε�����������
	* @ InTriangle                                        ����1��ʾ���߶����������ཻ�Ľ��㣬����0��ʾ����ֱ����ƽ��Ľ���
	* @ result                                            �߶Σ�ֱ�ߣ���������Ƭ��ƽ�棩�Ľ�������
	*/
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, cv::Point3f &TrianglePoint1, cv::Point3f
		&TrianglePoint2, cv::Point3f &TrianglePoint3, bool InTriangle, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //����������ƽ��ķ�����

		cv::Point3f VP12, VP13;  //�����εı߷�������

		cv::Point3f CrossPoint;  //ֱ����ƽ��Ľ���

		float TriD;  //ƽ�淽�̳�����

		cv::Point3f LineV;  //ֱ�ߵķ�������
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;


		/*-------����ƽ��ķ�������������-------*/
		//point1->point2
		VP12.x = TrianglePoint2.x - TrianglePoint1.x;
		VP12.y = TrianglePoint2.y - TrianglePoint1.y;
		VP12.z = TrianglePoint2.z - TrianglePoint1.z;
		//point1->point3
		VP13.x = TrianglePoint3.x - TrianglePoint1.x;
		VP13.y = TrianglePoint3.y - TrianglePoint1.y;
		VP13.z = TrianglePoint3.z - TrianglePoint1.z;
		//����ƽ��ķ����� VP12xVP13
		TriangleV.x = VP12.y * VP13.z - VP12.z * VP13.y;
		TriangleV.y = -(VP12.x * VP13.z - VP12.z * VP13.x);
		TriangleV.z = VP12.x * VP13.y - VP12.y * VP13.x;
		//����ƽ�淽�̵ĳ�����
		TriD = -(TriangleV.x*TrianglePoint1.x + TriangleV.y*TrianglePoint1.y + TriangleV.z*TrianglePoint1.z);

		/*----------------------------���ֱ����ƽ��Ľ�������-----------------------*/
		/* ˼·��
		* ���Ƚ�ֱ�߷���ת��Ϊ����������ʽ��Ȼ�����ƽ�淽�̣���ò���t��
		* ��t����ֱ�ߵĲ������̼��������������
		*/
		float tempU, tempD; //��ʱ����
		tempU = TriangleV.x*LinePoint1.x + TriangleV.y*LinePoint1.y + TriangleV.z*LinePoint1.z + TriD;
		tempD = TriangleV.x*LineV.x + TriangleV.y*LineV.y + TriangleV.z*LineV.z;  //�ж�ƽ�淨������ֱ�ߵķ��������Ƿ�ֱ

		//std::cout << "tempU = " << tempU << ", tempD = " << tempD << std::endl;

		//ֱ����ƽ��ƽ�л���ƽ����
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

		//�������t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
			(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//����ֱ���������εĽ�������
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
		/*-----------------------�жϽ����Ƿ����������ڲ�--------------------*/
		//���������������ߵĳ���
		float d12 = Distance(TrianglePoint1, TrianglePoint2);
		float d13 = Distance(TrianglePoint1, TrianglePoint3);
		float d23 = Distance(TrianglePoint2, TrianglePoint3);

		//���㽻�㵽��������ĳ���
		float c1 = Distance(CrossPoint, TrianglePoint1);
		float c2 = Distance(CrossPoint, TrianglePoint2);
		float c3 = Distance(CrossPoint, TrianglePoint3);

		//�������μ��������ε����
		float areaD = Area(d12, d13, d23); //���������
		float area1 = Area(c1, c2, d12); //��������1�����
		float area2 = Area(c1, c3, d13); //��������2�����
		float area3 = Area(c2, c3, d23); //��������3�����

		//std::cout << "���㣺 " << result << std::endl;
		//std::cout << "����" << fabs(area1 + area2 + area3 - areaD) << std::endl;
		 //��������жϵ��Ƿ����������ڲ�
		if (fabs(area1 + area2 + area3 - areaD) > 0.01 && InTriangle)
		{
			return false;
		}
		//double t2 = timeEnd();
		//timePrint("test-time = ", t1, t2);
#endif
		result = CrossPoint;  // ��������
		return true;
	}

	// @ 2017-12-07
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, std::vector<std::vector<float>> & Calcs, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //����������ƽ��ķ�����
		cv::Point3f CrossPoint;  //ֱ����ƽ��Ľ���

		float TriD;  //ƽ�淽�̳�����

		cv::Point3f LineV;  //ֱ�ߵķ�������
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;

		//LineV.x = -LineV.x;
		//LineV.y = -LineV.y;
		//LineV.z = -LineV.z;
		/*-------����ƽ��ķ�������������-------*/
		//TriangleV.x = Calcs[4][0];
		//TriangleV.y = Calcs[4][1];
		//TriangleV.z = Calcs[4][2];
		////����ƽ�淽�̵ĳ�����
		//TriD = Calcs[4][3];

		/*----------------------------���ֱ����ƽ��Ľ�������-----------------------*/
		/* ˼·��
		* ���Ƚ�ֱ�߷���ת��Ϊ����������ʽ��Ȼ�����ƽ�淽�̣���ò���t��
		* ��t����ֱ�ߵĲ������̼��������������
		*/
		float tempU, tempD; //��ʱ����
		//tempU = TriangleV.x * LinePoint1.x + TriangleV.y * LinePoint1.y + TriangleV.z * LinePoint1.z + TriD;
		//tempD = TriangleV.x * LineV.x + TriangleV.y * LineV.y + TriangleV.z * LineV.z;  //�ж�ƽ�淨������ֱ�ߵķ��������Ƿ�ֱ

		tempU = Calcs[4][0] * LinePoint1.x + Calcs[4][1] * LinePoint1.y + Calcs[4][2] * LinePoint1.z + Calcs[4][3];
		tempD = Calcs[4][0] * LineV.x + Calcs[4][1] * LineV.y + Calcs[4][2] * LineV.z;  //�ж�ƽ�淨������ֱ�ߵķ��������Ƿ�ֱ

		 //std::cout << "tempU = " << tempU << ", tempD = " << tempD << std::endl;

		 //ֱ����ƽ��ƽ�л���ƽ����
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

		//�������t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
		(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//����ֱ���������εĽ�������
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
		/*-----------------------�жϽ����Ƿ����������ڲ�--------------------*/
		//���������������ߵĳ���
		float d12 = Distance(TrianglePoint1, TrianglePoint2);
		float d13 = Distance(TrianglePoint1, TrianglePoint3);
		float d23 = Distance(TrianglePoint2, TrianglePoint3);

		//���㽻�㵽��������ĳ���
		float c1 = Distance(CrossPoint, TrianglePoint1);
		float c2 = Distance(CrossPoint, TrianglePoint2);
		float c3 = Distance(CrossPoint, TrianglePoint3);

		//�������μ��������ε����
		float areaD = Area(d12, d13, d23); //���������
		float area1 = Area(c1, c2, d12); //��������1�����
		float area2 = Area(c1, c3, d13); //��������2�����
		float area3 = Area(c2, c3, d23); //��������3�����

										 //std::cout << "���㣺 " << result << std::endl;
										 //std::cout << "����" << fabs(area1 + area2 + area3 - areaD) << std::endl;
										 //��������жϵ��Ƿ����������ڲ�
		if (fabs(area1 + area2 + area3 - areaD) > 0.01 && InTriangle)
		{
			return false;
		}
#endif
		result = CrossPoint;  // ��������
		return true;
	}





	/*
	* @ pagrams
	* @ LinePoint1�� LinePoint1    ֱ�ߵ�����������
	* @ CutSurface                 �ָ��淽�̵�ϵ��
	* @ result                     �߶Σ�ֱ�ߣ���������Ƭ��ƽ�棩�Ľ�������
	*/
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, TriangleFace TF, std::vector<float> CutSurface, cv::Point3f &result)
	{
		cv::Point3f TriangleV;  //����������ƽ��ķ�����

		cv::Point3f CrossPoint;  //ֱ����ƽ��Ľ���

		float TriD;  //ƽ�淽�̳�����

		cv::Point3f LineV;  //ֱ�ߵķ�������
		LineV.x = LinePoint2.x - LinePoint1.x, LineV.y = LinePoint2.y - LinePoint1.y, LineV.z = LinePoint2.z - LinePoint1.z;

		/*-------����ƽ��ķ�������������-------*/
		//����ƽ��ķ����� VP12xVP13
		TriangleV.x = CutSurface[0];
		TriangleV.y = CutSurface[1];
		TriangleV.z = CutSurface[2];
		//����ƽ�淽�̵ĳ�����
		TriD = CutSurface[3];

		/*----------------------------���ֱ����ƽ��Ľ�������-----------------------*/
		/* ˼·��
		* ���Ƚ�ֱ�߷���ת��Ϊ����������ʽ��Ȼ�����ƽ�淽�̣���ò���t��
		* ��t����ֱ�ߵĲ������̼��������������
		*/
		float tempU, tempD; //��ʱ����
		tempU = TriangleV.x*LinePoint1.x + TriangleV.y*LinePoint1.y + TriangleV.z*LinePoint1.z + TriD;
		tempD = TriangleV.x*LineV.x + TriangleV.y*LineV.y + TriangleV.z*LineV.z;  //�ж�ƽ�淨������ֱ�ߵķ��������Ƿ�ֱ
		float rd2 = TriangleV.x*LinePoint2.x + TriangleV.y*LinePoint2.y + TriangleV.z*LinePoint2.z + TriD;
																				  //ֱ����ƽ��ƽ�л���ƽ����
		if (tempD == 0.0 || fabs(tempU) < 0.01)
		{
			//printf("The line is parallel with the plane.\n");
			return false;
		}
		if (tempU*rd2 >= 0) {
			return false;
		}
		//�������t
		float t = -tempU / tempD;
		/*float t = ((TrianglePoint1.x - LinePoint1.x) * TriangleV.x + (TrianglePoint1.y - LinePoint1.y) * TriangleV.y +
		(TrianglePoint1.z - LinePoint1.z) * TriangleV.z) / tempD;*/

		//����ֱ���������εĽ�������
		CrossPoint.x = LineV.x*t + LinePoint1.x;
		CrossPoint.y = LineV.y*t + LinePoint1.y;
		CrossPoint.z = LineV.z*t + LinePoint1.z;

		/*-----------------------�жϽ����Ƿ����������ڲ�--------------------*/

		//���������������ߵĳ���
		float d12 = Distance(TF.P1, TF.P2);
		float d13 = Distance(TF.P1, TF.P3);
		float d23 = Distance(TF.P2, TF.P3);

		//���㽻�㵽��������ĳ���
		float c1 = Distance(CrossPoint, TF.P1);
		float c2 = Distance(CrossPoint, TF.P2);
		float c3 = Distance(CrossPoint, TF.P3);

		//�������μ��������ε����
		float areaD = Area(d12, d13, d23); //���������
		float area1 = Area(c1, c2, d12); //��������1�����
		float area2 = Area(c1, c3, d13); //��������2�����
		float area3 = Area(c2, c3, d23); //��������3�����

										 //std::cout << "���㣺 " << result << std::endl;
										 //std::cout << "����" << fabs(area1 + area2 + area3 - areaD) << std::endl;
										 //��������жϵ��Ƿ����������ڲ�
		if (fabs(area1 + area2 + area3 - areaD) > 0.01)
		{
			return false;
		}
		result = CrossPoint;  // ��������
		return true;
	}

	/* ��������Ƭ��Ϊ�ָ��棬������ƽ�淽��
	*  @ params
	*  @ TF     ������Ƭ�Ķ�������
	*/
	std::vector<float> CalcCutSurface(TriangleFace TF) {

		cv::Point3f TriangleV;  //����������ƽ��ķ�����
		cv::Point3f VP12, VP13;  //�����εı߷�������
		float TriD;  //ƽ�淽�̵ĳ�����

		/*-------����ƽ��ķ�������������-------*/
		//point1->point2
		VP12.x = TF.P2.x - TF.P1.x;
		VP12.y = TF.P2.y - TF.P1.y;
		VP12.z = TF.P2.z - TF.P1.z;
		//point1->point3
		VP13.x = TF.P3.x - TF.P1.x;
		VP13.y = TF.P3.y - TF.P1.y;
		VP13.z = TF.P3.z - TF.P1.z;
		//����ƽ��ķ����� VP12xVP13
		TriangleV.x = VP12.y*VP13.z - VP12.z*VP13.y;
		TriangleV.y = -(VP12.x*VP13.z - VP12.z*VP13.x);
		TriangleV.z = VP12.x*VP13.y - VP12.y*VP13.x;
		//����ƽ�淽�̵ĳ�����
		TriD = -(TriangleV.x*TF.P1.x + TriangleV.y*TF.P1.y + TriangleV.z*TF.P1.z);

		std::vector<float> CutSurface(4);  //ƽ�淽�̵�����ϵ����һ������
		CutSurface[0] = TriangleV.x;
		CutSurface[1] = TriangleV.y;
		CutSurface[2] = TriangleV.z;
		CutSurface[3] = TriD;
		return CutSurface;
	}

	/* ��ֱ������Ƭ������ƽ�棬������ƽ��������ߣ�������ƽ�淽��
	*  @ params
	*  @ TF     ������Ƭ�Ķ�������
	*/
	std::vector<std::vector<float>> CalcThreeCutSurface(TriangleFace TF, std::vector<float> CutSurface) {

		TriangleFace TF1, TF2, TF3;
		cv::Point3f InTriangle;
		InTriangle = ((TF.P1 + TF.P2) / 2 + TF.P3) / 2;
		std::vector<float> CutSurface1, CutSurface2, CutSurface3;
		std::vector<std::vector<float>> CutSurfaceWithSign;

		// ���������ڵĴ�ֱ��������Ƭ��ƽ��Ķ�Ӧ��������
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

