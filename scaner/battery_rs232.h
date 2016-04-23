//头文件//
#pragma once

HANDLE Init_battery_rs232(CString ComName);											//初始化
DWORD SDataCom_batteryr(HANDLE HComx, const UCHAR* sdcom, DWORD dwBytesWrite);		//串口发送
DWORD RDataCom_batteryr(HANDLE HComx, UCHAR* rdcom, DWORD dwBytesRead);				//串口接收
bool Get_dump_energy(HANDLE HComx);													//获取电池剩余电量
bool Get_remaining_time(HANDLE HComx);												//获取电池可用时间
bool Send_read_order(HANDLE HComx);													//发送读取指令