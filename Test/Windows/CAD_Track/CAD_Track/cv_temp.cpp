
#include <opencv2/opencv.hpp>
#include <vector>


using namespace cv;
using namespace std;

#include "cv_temp.h"
//图像滤波
Mat ImFilter(Mat Im)
{
	int ModelW = 3;
	Size Ksize(ModelW, ModelW);
	Mat NewIm;
	double SigmX, SigmY;
	SigmX = 1.5; SigmY = 1.5;
	GaussianBlur(Im, NewIm, Ksize, SigmX, SigmY);

	return NewIm;
}

vector<Mat> GetImGrad(Mat Im)
{
	int ChannelNum = Im.channels();
	if (ChannelNum > 1)
	{
		cvtColor(Im, Im, CV_BGR2GRAY);
	}
	Im.convertTo(Im, CV_32FC1);
	Im = Im / 255;
	Mat GradX, GradY;
	Sobel(Im, GradX, CV_32FC1, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(Im, GradY, CV_32FC1, 0, 1, 3, 1, 0, BORDER_DEFAULT);

	vector<Mat> ImGrad;
	ImGrad.push_back(GradX);
	ImGrad.push_back(GradY);

	return ImGrad;
}


vector<Mat> SuppressMinGradMag(vector<Mat> ImGrad, float MinGradMag)
{
	Mat ImGradX = ImGrad[0];
	Mat ImGradY = ImGrad[1];
	int m = ImGradX.rows;
	int n = ImGradX.cols;
	float *ImGxRow, *ImGyRow;
	for (int i = 0; i < m; i++)
	{
		ImGxRow = ImGradX.ptr<float>(i);
		ImGyRow = ImGradY.ptr<float>(i);
		for (int j = 0; j < n; j++)
		{
			float iGx = ImGxRow[j];
			float iGy = ImGyRow[j];
			//梯度幅度
			float iGradMag = sqrt(iGx*iGx + iGy*iGy);
			if (iGradMag < MinGradMag)
			{
				ImGxRow[j] = 0;
				ImGyRow[j] = 0;
			}
		}
	}
	vector<Mat> NewImGrad;
	NewImGrad.push_back(ImGradX);
	NewImGrad.push_back(ImGradY);

	return NewImGrad;
}
//图像开运算
Mat ImOpenOrCloseCal(Mat GradMag)
{
	int ModelW = 3;
	Size Ksize(ModelW, ModelW);
	Mat  StrElem;
	StrElem = getStructuringElement(MORPH_RECT, Ksize);
	Mat NewGradMag;
	//morphologyEx(GradMag, NewGradMag, MORPH_OPEN, StrElem);
	morphologyEx(GradMag, NewGradMag, MORPH_CLOSE, StrElem);

	return NewGradMag;
}
Mat MatAtan2(Mat GradX, Mat GradY)
{
	int m, n;
	m = GradX.rows;
	n = GradX.cols;
	Mat GradAng = Mat::zeros(m, n, CV_32FC1);

	for (int i = 0; i < m; i++)
	{
		float *GradXRow = GradX.ptr<float>(i);
		float *GradYRow = GradY.ptr<float>(i);
		float *GradAngRow = GradAng.ptr<float>(i);
		for (int j = 0; j < n; j++)
		{
			GradAngRow[j] = atan2(GradYRow[j], GradXRow[j]);//初创就有
															//GradAngRow[j] = atan(GradYRow[j]/GradXRow[j]);//2017/11/23/14:25新添
		}
	}

	return GradAng;
}
void ShowImEdge(vector<Mat> ImGrad, char *WinName)
{
	Mat GradX, GradY;
	GradX = ImGrad[0];
	GradY = ImGrad[1];

	Mat AbsGradX, AbsGradY, ModelGradMag, Model;
	convertScaleAbs(255 * GradX, AbsGradX);
	convertScaleAbs(255 * GradY, AbsGradY);
	addWeighted(AbsGradX, 0.5, AbsGradY, 0.5, 0, ModelGradMag);

	imshow(WinName, ModelGradMag);
}


vector<Mat> GetImEdge(Mat Im)
{
	Im = ImFilter(Im);

	vector<Mat> ImGrad0 = GetImGrad(Im);

	float MinGradMag = 0.15;
	vector<Mat> ImGrad = SuppressMinGradMag(ImGrad0, MinGradMag);
	ShowImEdge(ImGrad, "ImEdge");

	Mat GradX, GradY;
	GradX = ImGrad[0];
	GradY = ImGrad[1];
	Mat AbsGradX, AbsGradY, GradMag, Model;
	convertScaleAbs(255 * GradX, AbsGradX);
	convertScaleAbs(255 * GradY, AbsGradY);
	addWeighted(AbsGradX, 0.5, AbsGradY, 0.5, 0, GradMag);

	GradMag = ImOpenOrCloseCal(GradMag);

	//Mat GradAng = GradY / GradX;
	Mat GradAng = MatAtan2(GradX, GradY);

	vector<Mat> ImEdge;
	ImEdge.push_back(GradMag);
	ImEdge.push_back(GradAng);

	return ImEdge;
}