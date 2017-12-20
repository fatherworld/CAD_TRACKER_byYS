
#include "CadStruct.h"
#include "CadMatrix.h"
#include "CadIsSee.h"
#include "CadUpDataRT.h"
extern "C"
{

	int isUpdataOrNot(float U[6], float U_pre[6])
	{
		float TxyzDiffThresh = 0.15f;
		float AngxyzDiffThresh = 0.015f;
		int MeetGrade1 = 0;
		int MeetGrade2 = 0;

		int ChangeLittleOrNot = 0;

		for (int i = 0; i < 3; i++)
		{
			float Delat_u_i = abs(U[i] - U_pre[i]);
			if (Delat_u_i <= TxyzDiffThresh)
			{
				MeetGrade1++;
			}
		}

		for (int i = 3; i < 6; i++)
		{
			float Delat_u_i = abs(U[i] - U_pre[i]);
			if (Delat_u_i <= AngxyzDiffThresh)
			{
				MeetGrade2++;
			}

		}

		if ((MeetGrade1 == 3) && (MeetGrade2 == 3))
		{
			ChangeLittleOrNot = 1;
		}

		return ChangeLittleOrNot;

	}


	int CadUpDataRT(void * ptr)
	{
		
		int ret = 0;
		CadInfo * ptrCadInfo = (CadInfo *)ptr;

		ret = isUpdataOrNot(ptrCadInfo->mAllMartix.U, ptrCadInfo->mAllMartix.U_pre);
		// 不更新数据用前一帧的数据
		//if (1 == ret)
		{

		}
	//	else
		{
			memcpy(ptrCadInfo->mAllMartix.U_pre, ptrCadInfo->mAllMartix.U, sizeof(float) * 6);

			MARTIX_Alva RT;
			MARTIX_Alva NewRT;
			MARTIX_Alva M;
			RT.cols = 4; RT.rows = 4;
			M.cols = 4; M.rows = 4;
			NewRT.cols = 4; NewRT.rows = 4;

			float tempS[16 * 3];
			float * temp = &tempS[0];
			RT.martix = temp + 0;
			NewRT.martix = temp + 16;
			M.martix = temp + 16 * 2;

			memcpy(RT.martix, ptrCadInfo->mCameraInfo.RT, sizeof(float) * 16);
			memcpy(M.martix, ptrCadInfo->mAllMartix.M, sizeof(float) * 16);

			mul_maritx(RT, M, &NewRT);
			// 更新了 RT
			memcpy(ptrCadInfo->mCameraInfo.RT, NewRT.martix, sizeof(float) * 16);

			CadIsSeeDeal(ptrCadInfo);

			float Angle[3] = { 0.0f };
			float Transform[3] = { 0.0f };

			//RT2RotateAndTrant(ptrCadInfo->mCameraInfo.RT, Angle, Transform);
		}

		return 0;
	}


}