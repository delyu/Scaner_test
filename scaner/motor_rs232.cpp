#include "stdafx.h"
#include "motor_rs232.h"
#include "Definitions.h"

DWORD iError;				//错误信息
HANDLE xymotor;				//电机设备文件描述符
/*********************************************************************************************************
** Function name:           Open_motor
**
** Descriptions:            打开电机
**
** input parameters:        com_port			串口号
**							                                                                                                  
** Returned value:          成功返回文件描述符，否则返回0表示错误，可以通过函数Check_Error查看最近的错误 
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
** Descriptions:            关闭电机
**
** input parameters:        H_motor			电机对应文件描述符
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
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
** Descriptions:            查看错误信息
**
** input parameters:        无
**							                                                                                                  
** Returned value:          返回最近的错误 
*********************************************************************************************************/
DWORD Check_Error(void)												
{
	return iError;
}

/*********************************************************************************************************
** Function name:           Position_mode
**
** Descriptions:            电机位置模式
**
** input parameters:        H_motor			电机对应文件描述符
**							v				速度
**							a				加速度
**							d				减速度
**							p				位置
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
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
** Descriptions:            电机速度模式
**
** input parameters:        H_motor			电机对应文件描述符
**							v				速度
**							a				加速度
**							d				减速度
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Vel_mode(HANDLE H_motor,long v,DWORD a,DWORD d)											
{
	//参数化速度模式
	BOOL bRet = VCS_ActivateProfileVelocityMode(H_motor,1,&iError);

	//设置参数
	bRet = bRet && VCS_SetVelocityProfile(H_motor,1,a,d,&iError);
	bRet = bRet && VCS_MoveWithVelocity(H_motor,1,v,&iError);
	return bRet;
}

/*********************************************************************************************************
** Function name:           Get_Position
**
** Descriptions:            获取电机位置
**
** input parameters:        H_motor			电机对应文件描述符
**							pp				返回的电机位置
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Get_Position(HANDLE H_motor, long* pp)								
{
	return (BOOL)VCS_GetPositionIs(H_motor,1,pp,&iError);
}

/*********************************************************************************************************
** Function name:           Get_Vel
**
** Descriptions:            获取电机速度
**
** input parameters:        H_motor			电机对应文件描述符
**							vv				返回的电机速度
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Get_Vel(HANDLE H_motor, long* vv)										
{
	return (BOOL)VCS_GetVelocityIs(H_motor,1,vv,&iError) ;
}

/*********************************************************************************************************
** Function name:           Stop_Vel
**
** Descriptions:            电机速度模式停止
**
** input parameters:        H_motor			电机对应文件描述符
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Stop_Vel(HANDLE H_motor)												
{
//	VCS_SetVelocityMust(H_motor,1,0,&iError);
	//在速度模式下才需要停止

	return (BOOL)VCS_HaltVelocityMovement(H_motor,1,&iError);
}

/*********************************************************************************************************
** Function name:           Stop_Pos
**
** Descriptions:            电机位置模式停止
**
** input parameters:        H_motor			电机对应文件描述符
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Stop_Pos(HANDLE H_motor)												
{
	// VCS_SetVelocityMust(h_Motor1,1,0,iError);
	//在速度模式下才需要停止
	return VCS_HaltPositionMovement(H_motor,1,&iError);
}

/*********************************************************************************************************
** Function name:           Homing_xmotor
**
** Descriptions:            水平电机归零
**
** input parameters:        H_motor			电机对应文件描述符
**							a				加速度
**							v				速度
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
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
** Descriptions:            垂直电机归零
**
** input parameters:        H_motor			电机对应文件描述符
**							a				加速度
**							v				速度
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
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
** Descriptions:            电机归零停止
**
** input parameters:        H_motor			电机对应文件描述符
**							                                                                                                  
** Returned value:          成功返回TRUE，否则返回FALSE表示错误，可以通过函数Check_Error查看最近的错误 
*********************************************************************************************************/
BOOL Homing_stop(HANDLE H_motor)											
{
	return VCS_StopHoming(H_motor,1,&iError);
}