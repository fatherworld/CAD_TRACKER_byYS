#ifndef CadInhibition_H_
#define CadInhibition_H_
#include "CadStruct.h"
#ifdef __cplusplus
extern "C"
{
#endif

	int getNormal(vecf3 face[3], vecf3 normal[]);

	int IsTheSameLine(vecf3 m0, vecf3 m1, vecf3 n0, vecf3 n1);

	int IsHaveSameLineWithTwoFace(MyFaceList * current, MyFaceList * other, int * WhichLine, int * WhichOtherLine);

	int IsTheSameLine(vecf3 m0, vecf3 m1, vecf3 n0, vecf3 n1);

	int IsInhibition(MyFaceList * current, MyFaceList * other, int n, int m);

#ifdef __cplusplus
}
#endif

#endif