#include "stdafx.h"
#include "inclinometer_rs232.h"

/*********************************************************************************************************
** Function name:           Init_inclinometer_rs232
**
** Descriptions:            串口初始化函数
**
** input parameters:        ComName		串口号
**							                                                                                                  
** Returned value:          成功返回文件描述符，否则返回0表示错误，可以通过errno查看具体错误 
*********************************************************************************************************/
HANDLE Init_inclinometer_rs232(const CString& ComName)
{
	HANDLE hComx; 
	hComx = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL , NULL);	//异步方式
//	ASSERT(hComx != INVALID_HANDLE_VALUE);
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





