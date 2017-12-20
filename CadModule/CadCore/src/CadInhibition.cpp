#include "Config.h"
#include "Format.h"
#define LOG_TAG "CadInhibition"
#include "Log.h"
#include "CadInhibition.h"
#include "CadStruct.h"
#include "CadInhibition.h"
// 输入三个点face[3], 返回法向量normal
int getNormal(vecf3 face[3], vecf3 normal[])
{
	vecf3 vect_t1, vect_t2;

	vect_t1.v[0] = face[0].v[0] - face[1].v[0];
	vect_t1.v[1] = face[0].v[1] - face[1].v[1];
	vect_t1.v[2] = face[0].v[2] - face[1].v[2];

	vect_t2.v[0] = face[2].v[0] - face[1].v[0];
	vect_t2.v[1] = face[2].v[1] - face[1].v[1];
	vect_t2.v[2] = face[2].v[2] - face[1].v[2];

	float r1 = vect_t1.v[1] * vect_t2.v[2] - vect_t1.v[2] * vect_t2.v[1];
	float g1 = vect_t1.v[2] * vect_t2.v[0] - vect_t1.v[0] * vect_t2.v[2];
	float b1 = vect_t1.v[0] * vect_t2.v[1] - vect_t1.v[1] * vect_t2.v[0];

	float sss = 0.0f;
	r1 = r1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);
	g1 = g1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);
	b1 = b1 / (pow((r1 * r1 + g1 * g1 + b1 * b1), 0.5) + sss);

	normal[0].v[0] = r1;
	normal[0].v[1] = g1;
	normal[0].v[2] = b1;

	return 0;
}
// 判断是否为同一条线,是返回1，否返回0
int IsTheSameLine(vecf3 m0, vecf3 m1, vecf3 n0, vecf3 n1)
{
	if ((m0.v[0] == n0.v[0] && m0.v[1] == n0.v[1] && m0.v[2] == n0.v[2]) && (m1.v[0] == n1.v[0] && m1.v[1] == n1.v[1] && m1.v[2] == n1.v[2])
		|| (m0.v[0] == n1.v[0] && m0.v[1] == n1.v[1] && m0.v[2] == n1.v[2]) && (m1.v[0] == n0.v[0] && m1.v[1] == n0.v[1] && m1.v[2] == n0.v[2]))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}


// 判断两个面是否有共线的, 以及当前哪条边共线

int IsHaveSameLineWithTwoFace(MyFaceList * current, MyFaceList * other, int * WhichLine, int * WhichOtherLine)
{
	vecf3 m_vect[3];
	vecf3 m_vect1[3];
	int index[2] = { 9, 9 };
	int indexOther[2] = { 9, 9 };

	m_vect[0] = current->facePoint[0];
	m_vect[1] = current->facePoint[1];
	m_vect[2] = current->facePoint[2];

	m_vect1[0] = other->facePoint[0];
	m_vect1[1] = other->facePoint[1];
	m_vect1[2] = other->facePoint[2];

	int count = 0;

	int i = 0; int j = 0;
	int coincideIndex = -1;
	int coincideOtherIndex = -1;


	// 当前面的三个点和其他三个点比较，入过有两个点重合，那么说明有一条边共线
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (m_vect[i].v[0] == m_vect1[j].v[0] && m_vect[i].v[1] == m_vect1[j].v[1] && m_vect[i].v[2] == m_vect1[j].v[2])
			{
				if (count == 0)
				{

					index[count] = i;
					indexOther[count] = j;
				}
				else if (count == 1)
				{

					index[count] = i;
					indexOther[count] = j;
				}

				count++;
				break;

			}

		}
	}
	if (count == 2)
	{
		if ((index[0] == 0 && index[1] == 1) || (index[0] == 1 && index[1] == 0))
		{
			coincideIndex = 0;
		}
		else if ((index[0] == 1 && index[1] == 2) || (index[0] == 2 && index[1] == 1))
		{
			coincideIndex = 1;
		}
		else if ((index[0] == 0 && index[1] == 2) || (index[0] == 2 && index[1] == 0))
		{
			coincideIndex = 2;
		}

		if ((indexOther[0] == 0 && indexOther[1] == 1) || (indexOther[0] == 1 && indexOther[1] == 0))
		{
			coincideOtherIndex = 0;
		}
		else if ((indexOther[0] == 1 && indexOther[1] == 2) || (indexOther[0] == 2 && indexOther[1] == 1))
		{
			coincideOtherIndex = 1;
		}
		else if ((indexOther[0] == 0 && indexOther[1] == 2) || (indexOther[0] == 2 && indexOther[1] == 0))
		{
			coincideOtherIndex = 2;
		}

		*WhichLine = coincideIndex;
		*WhichOtherLine = coincideOtherIndex;
		return 1;
	}

	return 0;
}
// current 的三条边是否抑制
int IsInhibition(MyFaceList * current, MyFaceList * other, int n, int m)
{
	int whitchLine = -1;
	int whitchOtherLine = -1;
	if ((n == 0 && m == 5) || (n == 5 && m == 0))
	{
		//system("pause");
	}
	int ret = IsHaveSameLineWithTwoFace(current, other, &whitchLine, &whitchOtherLine);
	// 有相交线
	if (ret == 1)
	{

		float A = pow(current->normal.v[0] - other->normal.v[0], 2) + pow(current->normal.v[1] - other->normal.v[1], 2) +
			pow(current->normal.v[2] - other->normal.v[2], 2);
		A = pow(A, 0.5f);
		float sigma = 30.0f;
		float t_threshold = 2 * sinf(sigma * 3.1415926f / 360);

		//printf("n = %d m = %d A=%f whitchLine=%d otherLine = %d \n", n, m, A, whitchLine, whitchOtherLine);

		if (A <= t_threshold)
		{

			current->flag[whitchLine] = 0;
		}
		else
		{
			current->flag[whitchLine] = 1;
			other->flag[whitchOtherLine] = 0;

		}

	}
	return 0;
}
