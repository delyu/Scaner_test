#include "stdafx.h"
#include "battery_rs232.h"

/*********************************************************************************************************
** Function name:           Init_battery_rs232
**
** Descriptions:            串口初始化函数
**
** input parameters:        ComName		串口号
**							                                                                                                  
** Returned value:          成功返回文件描述符，否则返回0表示错误，可以通过errno查看具体错误 
*********************************************************************************************************/

HANDLE Init_battery_rs232(CString ComName)
{
	HANDLE hComx; 
	hComx = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 , NULL);	//异步方式

	if(hComx ==INVALID_HANDLE_VALUE)
		return 0;
	SetCommMask(hComx, EV_RXCHAR|EV_TXEMPTY );		//设置事件驱动类型
	SetupComm( hComx, 6144,6144);					//设置输入输出缓冲区大小

	COMMTIMEOUTS CommTimeOuts;						//定义超时结构
	CommTimeOuts.ReadIntervalTimeout = 100;				        /*  接收字符间最大时间间隔      */
	CommTimeOuts.ReadTotalTimeoutConstant = 100;		        /*  读数据总超时常量            */
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;	
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;  /*  写数据总超时常量 (写为0量表示没有超时控制)      */	
	CommTimeOuts.WriteTotalTimeoutMultiplier = 10;
	if(!SetCommTimeouts(hComx, &CommTimeOuts))			//设置读写操作所允许的超时
	{
		CloseHandle(hComx);
		return 0;
	}
	DCB mydcb;										// 定义数据控制块结构
	GetCommState(hComx, &mydcb);					//读串口原来的参数设置
	mydcb.BaudRate = 9600;							//波特率
	mydcb.fBinary = TRUE;
	//	mydcb.fBinary = FALSE;							//二进制
	mydcb.fParity = FALSE;							//无校验
	mydcb.Parity = NOPARITY;
		
	mydcb.ByteSize = 8;
	mydcb.StopBits = ONESTOPBIT;
					
	mydcb.fDtrControl  = FALSE;			                        /*  硬件流量控制设置            */	
	mydcb.fOutxDsrFlow = FALSE;		
	mydcb.fRtsControl  = FALSE;
	mydcb.fOutxCtsFlow = FALSE;  
	
	mydcb.fOutX = FALSE;						    /*  禁止软件流控制。fOutX: 收到Xoff后停止发送       */
	mydcb.fInX  = FALSE;							/*  禁止软件流控制。fInX: 缓冲区接收满后发送Xoff    */
	mydcb.fTXContinueOnXoff = FALSE;				/*  禁止软件流控制。fInX事件之后，发送是否继续运行  */
	if(!SetCommState(hComx, &mydcb))					//设置新串口参数
	{
		CloseHandle(hComx);
		return 0;
	}
	PurgeComm(hComx, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//清理缓冲区
	return hComx;
}


/*********************************************************************************************************
** Function name:           SDataCom_batteryr
**
** Descriptions:            串口发送数据函数
**
** input parameters:        HComx			Init_battery_rs232返回的句柄
**							sdcom			待发送数据
**							dwBytesWrite	数据长度
**                                                                                                   
** Returned value:          成功返回实际发送数据长度，否则返回-1表示错误，可以通过errno查看具体错误 
*********************************************************************************************************/
DWORD SDataCom_batteryr(HANDLE HComx, const UCHAR* sdcom, DWORD dwBytesWrite)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED Write_overlapped;
	memset(&Write_overlapped, 0, sizeof(OVERLAPPED));
	Write_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ClearCommError(HComx, &dwErrorFlags, &ComStat);
	BOOL bWriteStat;
	bWriteStat = WriteFile(HComx, sdcom, dwBytesWrite, &dwBytesWrite, &Write_overlapped);
	if(!bWriteStat)
	{		
		if(GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(Write_overlapped.hEvent,2000);			//异步等待2秒
			CloseHandle(Write_overlapped.hEvent);
			return dwBytesWrite;
		}
		else
		{
			CloseHandle(Write_overlapped.hEvent);
			PurgeComm(HComx, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
			return -1;
		}
	}
	else
	{
		CloseHandle(Write_overlapped.hEvent);
		return dwBytesWrite;
	}
}



/*********************************************************************************************************
** Function name:           RDataCom_batteryr
**
** Descriptions:            串口接收数据函数
**
** input parameters:        HComx			Init_battery_rs232返回的句柄
**							rdcom			接收数据保存地址
**							dwBytesRead		数据长度
**                                                                                                   
** Returned value:          成功返回实际接收数据长度，否则返回-1表示错误，可以通过errno查看具体错误
*********************************************************************************************************/
DWORD RDataCom_batteryr(HANDLE HComx, UCHAR* rdcom, DWORD dwBytesRead)
{
	DWORD dwRead =0;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED Read_overlapped;	
	memset(&Read_overlapped, 0, sizeof(OVERLAPPED));	
	ClearCommError(HComx, &dwErrorFlags, &ComStat );
	dwRead = ComStat.cbInQue;						//输入缓冲区有多少数据														
	if (dwRead > 0)
	{
		BOOL fReadStat;
		Read_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			
		fReadStat = ReadFile(HComx, rdcom, dwBytesRead, &dwBytesRead, &Read_overlapped);
		if (!fReadStat)
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
				WaitForSingleObject(Read_overlapped.hEvent,2000);			//异步等待2秒
				CloseHandle(Read_overlapped.hEvent);
				return dwBytesRead;
			}
			else
			{
				CloseHandle(Read_overlapped.hEvent);
				PurgeComm(HComx, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
				return -1;
			}									
		}
		else
		{
			CloseHandle(Read_overlapped.hEvent);
			return dwBytesRead;
		}
	}
	else
		return 0;
}


/*********************************************************************************************************
** Function name:           Get_dump_energy
**
** Descriptions:            获取电池剩余电量
**
** input parameters:        HComx			Init_battery_rs232返回的句柄							
**                                                                                                   
** Returned value:          成功返回true，否则返回false。
*********************************************************************************************************/
bool Get_dump_energy(HANDLE HComx)
{
	UCHAR scomdata[7]={0x00,0xaa,0x4f,0xaa,0x01,0x02,0x0a};
	if(SDataCom_batteryr(HComx,scomdata,7) == -1)
		return false;
	return true;
}


/*********************************************************************************************************
** Function name:           Get_remaining_time
**
** Descriptions:            获取电池可用时间
**
** input parameters:        HComx			Init_battery_rs232返回的句柄							
**                                                                                                   
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
bool Get_remaining_time(HANDLE HComx)
{
	UCHAR scomdata[7]={0x00,0xaa,0xff,0x16,0x01,0x11,0x0a};
	if(SDataCom_batteryr(HComx,scomdata,7) == -1)
		return false;	
	return true;
}


/*********************************************************************************************************
** Function name:           Send_read_order
**
** Descriptions:            发送读取指令
**
** input parameters:        HComx			Init_battery_rs232返回的句柄							
**                                                                                                   
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
bool Send_read_order(HANDLE HComx)
{
	static UCHAR scomdata[7]={0x00,0xa5,0x4f,0xaa,0x00,0x02,0x0a};
	if(SDataCom_batteryr(HComx,scomdata,7) == -1)
		return false;
	return true;
}