//ͷ�ļ�//
#pragma once

HANDLE Open_motor(char* com_port);													//�򿪵��
BOOL Close_motor(HANDLE H_motor);													//�رյ��
BOOL Position_mode(HANDLE H_motor,DWORD v,DWORD a,DWORD d,long p);					//���λ��ģʽ
BOOL Vel_mode(HANDLE H_motor,long v,DWORD a,DWORD d);								//����ٶ�ģʽ
BOOL Get_Position(HANDLE H_motor, long* pp);										//��ȡ���λ��
BOOL Get_Vel(HANDLE H_motor, long* vv);												//��ȡ����ٶ�
BOOL Stop_Vel(HANDLE H_motor);														//����ٶ�ģʽֹͣ
BOOL Stop_Pos(HANDLE H_motor);														//���λ��ģʽֹͣ
BOOL Homing_xmotor(HANDLE H_motor,DWORD a,DWORD v);									//ˮƽ�������
BOOL Homing_ymotor(HANDLE H_motor,DWORD a,DWORD v);									//ˮƽ�������
BOOL Homing_stop(HANDLE H_motor);													//�������ֹͣ

DWORD Check_Error(void);															//�鿴������Ϣ