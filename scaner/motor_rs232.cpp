#include "stdafx.h"
#include "motor_rs232.h"
#include "Definitions.h"

DWORD iError;				//������Ϣ
HANDLE xymotor;				//����豸�ļ�������
/*********************************************************************************************************
** Function name:           Open_motor
**
** Descriptions:            �򿪵��
**
** input parameters:        com_port			���ں�
**							                                                                                                  
** Returned value:          �ɹ������ļ������������򷵻�0��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
HANDLE Open_motor(char* com_port)
{
	xymotor = VCS_OpenDevice("EPOS2","MAXON SERIAL V2","USB",com_port,&iError);
	if(xymotor)
	{
		if(VCS_ClearFault(xymotor,1,&iError))
		{
			if(VCS_SetEnableState(xymotor,1,&iError) == 0)
				return 0;
		}
		else
			return 0;
	}

	return xymotor;
}

/*********************************************************************************************************
** Function name:           Close_motor
**
** Descriptions:            �رյ��
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Close_motor(HANDLE H_motor)												
{
	VCS_SetDisableState(H_motor,1,&iError);
	if(VCS_CloseDevice(H_motor,&iError))
		return TRUE;
	else
		return FALSE;
}

/*********************************************************************************************************
** Function name:           Check_Error
**
** Descriptions:            �鿴������Ϣ
**
** input parameters:        ��
**							                                                                                                  
** Returned value:          ��������Ĵ��� 
*********************************************************************************************************/
DWORD Check_Error(void)												
{
	return iError;
}

/*********************************************************************************************************
** Function name:           Position_mode
**
** Descriptions:            ���λ��ģʽ
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							v				�ٶ�
**							a				���ٶ�
**							d				���ٶ�
**							p				λ��
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Position_mode(HANDLE H_motor,DWORD v,DWORD a,DWORD d,long p)				
{
	if(VCS_ActivateProfilePositionMode(H_motor,1,&iError))
		if(VCS_SetPositionProfile(H_motor,1,v,a,d,&iError))
			if(VCS_MoveToPosition(H_motor,1,p,TRUE,TRUE,&iError))
				return TRUE;

	return FALSE;
}

/*********************************************************************************************************
** Function name:           Vel_mode
**
** Descriptions:            ����ٶ�ģʽ
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							v				�ٶ�
**							a				���ٶ�
**							d				���ٶ�
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Vel_mode(HANDLE H_motor,long v,DWORD a,DWORD d)											
{
	//�������ٶ�ģʽ
	BOOL bRet = VCS_ActivateProfileVelocityMode(H_motor,1,&iError);

	//���ò���
	bRet = bRet && VCS_SetVelocityProfile(H_motor,1,a,d,&iError);
	bRet = bRet && VCS_MoveWithVelocity(H_motor,1,v,&iError);
	return bRet;
}

/*********************************************************************************************************
** Function name:           Get_Position
**
** Descriptions:            ��ȡ���λ��
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							pp				���صĵ��λ��
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Get_Position(HANDLE H_motor, long* pp)								
{
	return (BOOL)VCS_GetPositionIs(H_motor,1,pp,&iError);
}

/*********************************************************************************************************
** Function name:           Get_Vel
**
** Descriptions:            ��ȡ����ٶ�
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							vv				���صĵ���ٶ�
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Get_Vel(HANDLE H_motor, long* vv)										
{
	return (BOOL)VCS_GetVelocityIs(H_motor,1,vv,&iError) ;
}

/*********************************************************************************************************
** Function name:           Stop_Vel
**
** Descriptions:            ����ٶ�ģʽֹͣ
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Stop_Vel(HANDLE H_motor)												
{
//	VCS_SetVelocityMust(H_motor,1,0,&iError);
	//���ٶ�ģʽ�²���Ҫֹͣ

	return (BOOL)VCS_HaltVelocityMovement(H_motor,1,&iError);
}

/*********************************************************************************************************
** Function name:           Stop_Pos
**
** Descriptions:            ���λ��ģʽֹͣ
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Stop_Pos(HANDLE H_motor)												
{
	// VCS_SetVelocityMust(h_Motor1,1,0,iError);
	//���ٶ�ģʽ�²���Ҫֹͣ
	return VCS_HaltPositionMovement(H_motor,1,&iError);
}

/*********************************************************************************************************
** Function name:           Homing_xmotor
**
** Descriptions:            ˮƽ�������
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							a				���ٶ�
**							v				�ٶ�
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Homing_xmotor(HANDLE H_motor,DWORD a,DWORD v)						
{
	BOOL bRet = VCS_SetMaxAcceleration(H_motor,1,500,&iError);
	bRet = bRet && VCS_ActivateHomingMode(H_motor,1,&iError);
	bRet = bRet && VCS_SetHomingParameter(H_motor,1,a,v,v,0,1500,0,&iError);
	bRet = bRet && VCS_FindHome(H_motor,1,HM_INDEX_POSITIVE_SPEED ,&iError);

	return bRet;
}


/*********************************************************************************************************
** Function name:           Homing_ymotor
**
** Descriptions:            ��ֱ�������
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							a				���ٶ�
**							v				�ٶ�
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Homing_ymotor(HANDLE H_motor,DWORD a,DWORD v)						
{
	//VCS_SetMaxAcceleration(H_motor,1,500,&iError);
	BOOL bRet = VCS_ActivateHomingMode(H_motor,1,&iError);
	bRet = bRet && VCS_SetHomingParameter(H_motor,1,a,v,v,0,1500,0,&iError);
	bRet = bRet && VCS_FindHome(H_motor,1,HM_INDEX_POSITIVE_SPEED,&iError);
	return bRet;
}


/*********************************************************************************************************
** Function name:           Homing_stop
**
** Descriptions:            �������ֹͣ
**
** input parameters:        H_motor			�����Ӧ�ļ�������
**							                                                                                                  
** Returned value:          �ɹ�����TRUE�����򷵻�FALSE��ʾ���󣬿���ͨ������Check_Error�鿴����Ĵ��� 
*********************************************************************************************************/
BOOL Homing_stop(HANDLE H_motor)											
{
	return VCS_StopHoming(H_motor,1,&iError);
}