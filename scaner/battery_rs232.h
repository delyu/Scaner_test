//ͷ�ļ�//
#pragma once

HANDLE Init_battery_rs232(CString ComName);											//��ʼ��
DWORD SDataCom_batteryr(HANDLE HComx, const UCHAR* sdcom, DWORD dwBytesWrite);		//���ڷ���
DWORD RDataCom_batteryr(HANDLE HComx, UCHAR* rdcom, DWORD dwBytesRead);				//���ڽ���
bool Get_dump_energy(HANDLE HComx);													//��ȡ���ʣ�����
bool Get_remaining_time(HANDLE HComx);												//��ȡ��ؿ���ʱ��
bool Send_read_order(HANDLE HComx);													//���Ͷ�ȡָ��