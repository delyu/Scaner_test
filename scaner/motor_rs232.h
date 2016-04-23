//头文件//
#pragma once

HANDLE Open_motor(char* com_port);													//打开电机
BOOL Close_motor(HANDLE H_motor);													//关闭电机
BOOL Position_mode(HANDLE H_motor,DWORD v,DWORD a,DWORD d,long p);					//电机位置模式
BOOL Vel_mode(HANDLE H_motor,long v,DWORD a,DWORD d);								//电机速度模式
BOOL Get_Position(HANDLE H_motor, long* pp);										//获取电机位置
BOOL Get_Vel(HANDLE H_motor, long* vv);												//获取电机速度
BOOL Stop_Vel(HANDLE H_motor);														//电机速度模式停止
BOOL Stop_Pos(HANDLE H_motor);														//电机位置模式停止
BOOL Homing_xmotor(HANDLE H_motor,DWORD a,DWORD v);									//水平电机归零
BOOL Homing_ymotor(HANDLE H_motor,DWORD a,DWORD v);									//水平电机归零
BOOL Homing_stop(HANDLE H_motor);													//电机归零停止

DWORD Check_Error(void);															//查看错误信息