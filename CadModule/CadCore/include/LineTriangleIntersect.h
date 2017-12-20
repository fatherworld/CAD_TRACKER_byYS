#ifndef LineTriangleIntersect_h
#define LineTriangleIntersect_h

#include <opencv2\opencv.hpp>

struct TriangleFace{
	cv::Point3f P1;
	cv::Point3f P2;
	cv::Point3f P3;
};

namespace LTI {
	std::vector<std::vector<std::vector<float>>> CalcFive(void * ptr);
	float Distance(cv::Point3f &p1, cv::Point3f &p2);
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, cv::Point3f &TrianglePoint1, cv::Point3f
		&TrianglePoint2, cv::Point3f &TrianglePoint3, bool InTriangle, cv::Point3f &result);
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, std::vector<std::vector<float>> & Calcs, cv::Point3f &result);
	bool ValidPoint(cv::Point3f &LinePoint1, cv::Point3f &LinePoint2, TriangleFace TF, std::vector<float> CutSurface, cv::Point3f &result);
	std::vector<float> CalcCutSurface(TriangleFace TF);
	std::vector<std::vector<float>> CalcThreeCutSurface(TriangleFace TF, std::vector<float> CutSurface);
};


#endif // !LineTriangleIntersect_h
