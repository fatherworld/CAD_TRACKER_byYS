
#include  <vector>
#include "LineTriangleIntersect1.h"
#include "CadMatrix.h"
#include "CadStruct.h"
#include "CadIsSee.h"

//std::vector<std::vector<std::vector<float>>> Calcs;

CalcFives* Calcs;
extern "C"
{

	int CadIsSeeInit(void * ptr)
	{
        CadInfo * ptrCadInfo = (CadInfo *)ptr;
        Calcs = (CalcFives*)malloc(sizeof(CalcFives)*ptrCadInfo->NumFaceList);
		CalcFive(ptr, Calcs);
		return 0;
	}


	 int CadIsSeeDeal(void * ptr)
	{

		CadInfo * ptrCadInfo = (CadInfo *)ptr;
		for (int i = 0; i < ptrCadInfo->NumFaceList; i++)
		{
			MyFaceList * current = &(ptrCadInfo->MyFaceListPtr[i]);

			for (int j = 0; j < 3; j++)
			{
				if (current->flag[j] == 0) continue;
				int numIsSee = 0;
				for (int k = 0; k < current->linePointNum[j]; k++)
				{
					bool view = false;
					threespace SamplePnt;


                    SamplePnt.real_x = current->linePoint[j][k].v[0];
                    SamplePnt.real_y = current->linePoint[j][k].v[1];
                    SamplePnt.real_z = current->linePoint[j][k].v[2];
                                  
					threespace look_dot;
					MARTIX_Alva Ro;
					float R_o[9] = { 0.0f };
					Ro.cols = 3;
					Ro.rows = 3;
					Ro.martix = R_o;

					MARTIX_Alva To;
					float T_o[3] = { 0.0f };
					To.cols = 1;
					To.rows = 3;
					To.martix = T_o;

					MARTIX_Alva TT;
					float TT_o[3] = { 0.0f };
					TT.cols = 1;
					TT.rows = 3;
					TT.martix = TT_o;

					//EulerAng2Rotate_(ptrCadInfo->mCameraInfo.angle[0], ptrCadInfo->mCameraInfo.angle[1], ptrCadInfo->mCameraInfo.angle[2], Ro.martix);

					for (int mm = 0; mm < 3; mm++)
					{
						for (int nn = 0; nn < 3; nn++)
						{
							Ro.martix[mm * 3 + nn] = ptrCadInfo->mCameraInfo.RT[mm * 4 + nn];
						}

					}

					MARTIX_Alva Ro1;
					float R_o1[9] = { 0.0f };
					Ro1.cols = 3;
					Ro1.rows = 3;
					Ro1.martix = R_o1;

					converse_martix(Ro, &Ro1);


					To.martix[0] = -ptrCadInfo->mCameraInfo.RT[3];
					To.martix[1] = -ptrCadInfo->mCameraInfo.RT[7];
					To.martix[2] = -ptrCadInfo->mCameraInfo.RT[11];

					mul_maritx(Ro1, To, &TT);

					int dd = 1;


					//look_dot.x = -ptrCadInfo->mCameraInfo.transform[0];
					//look_dot.y = -ptrCadInfo->mCameraInfo.transform[1];
					//look_dot.z = -ptrCadInfo->mCameraInfo.transform[2];

                    look_dot.real_x = TT.martix[0];
                    look_dot.real_y = TT.martix[1];
                    look_dot.real_z = TT.martix[2];

					for (int ss = 0; ss < ptrCadInfo->NumFaceList; ss++)
					{
						threespace result;
						view = ValidPointf_byThree(SamplePnt, look_dot, Calcs[ss], &result);
						if (view)
						{
							break;
						}
					}
					if (!view) {

						numIsSee++;
						current->flagEveryPoint[j][k] = 1;
					}
					else
					{
						current->flagEveryPoint[j][k] = 0;
					}

				}
				if (numIsSee > (int)(current->linePointNum[j] * 0.5f))
					//if(1)
				{
					current->isEyeSee[j] = 1;
				}
				else
				{
					current->isEyeSee[j] = 0;
				}
			}

		}
		return 0;
	}
}
