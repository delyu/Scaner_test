//ͷ�ļ�//
#pragma once

HANDLE Init_fpga_rs232(const CString& ComName);										//FPGA���ڳ�ʼ��
VOID Set_fpga_brate(long BrateNUM);												//���ò�����
BOOL Set_Prober_Gain(HANDLE HComx,int Gdata);									//����̽��������,��APD����
BOOL Get_Prober_Tem(HANDLE HComx);												//��ȡ̽�����¶�
BOOL Laser_ON(HANDLE HComx);													//������
BOOL Laser_OFF(HANDLE HComx);													//�ؼ���
BOOL Set_Vth(HANDLE HComx,int Vdata);											//������ֵ��ѹ
BOOL Set_Tem_M(HANDLE HComx,int Mdata);											//�����²�����
BOOL Set_Tem_N(HANDLE HComx,int Ndata);											//�����²�ϵ��
BOOL Warm_ON(HANDLE HComx);														//���²�����ģʽ
BOOL Warm_OFF(HANDLE HComx);													//�ر��²�����ģʽ
BOOL Set_AD_ns(HANDLE HComx,int xns);											//����AD�ɼ��屣��ʱʱ��
BOOL JD_Switch_ON(HANDLE HComx);												//�򿪽ڵ翪��
BOOL JD_Switch_OFF(HANDLE HComx);												//�رսڵ翪��
BOOL _GNSS(HANDLE HComx);														//���ػ�����GNSS����
BOOL Set_PulseWidth(HANDLE, int Pdata);                                         //��������
BOOL Set_PulseVth1(HANDLE, int Pdata);											//������ֵ1
BOOL Set_PulseVth2(HANDLE, int Pdata);											//������ֵ2

BOOL StartGNSS(HANDLE hCom);
BOOL StopGNSS(HANDLE hCom);

BOOL GetTempAndSener( HANDLE hCom , int* pTemp , int* pSen );

//���������
BOOL Get_Laser_Tem(HANDLE HComx);												//��ȡ�������¶�
BOOL Get_Laser_I(HANDLE HComx);													//��ȡ����������
BOOL Get_Laser_F(HANDLE HComx);													//��ȡ������Ƶ��
BOOL Set_Laser_Currents(HANDLE HComx,int Cdata);								//���ü���������
