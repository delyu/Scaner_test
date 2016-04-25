#include "stdafx.h"
#include "laser_rs232.h"

/*********************************************************************************************************
** Function name:           Init_laser_rs232
**
** Descriptions:            串口初始化函数
**
** input parameters:        ComName		串口号
**							                                                                                                  
** Returned value:          成功返回文件描述符，否则返回0表示错误，可以通过errno查看具体错误 
*********************************************************************************************************/
HANDLE Init_laser_rs232(const CString& ComName)
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
	mydcb.BaudRate = 9600;							//激光器监控串口波特率
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
** Function name:           Get_Laser_Tem
**
** Descriptions:            获取激光器温度
**
** input parameters:        HComx			Init_laser_rs232返回的句柄
**							                                                                                                  
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
BOOL Get_Laser_Tem(HANDLE HComx)
{
	const char* scomdata = "IT\r\n";
	DWORD dwWrite = 0;
	return WriteFile( HComx , scomdata , 4 , &dwWrite , NULL );
}


/*********************************************************************************************************
** Function name:           Get_Laser_I
**
** Descriptions:            获取激光器电流
**
** input parameters:        HComx			Init_laser_rs232返回的句柄
**							                                                                                                  
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
BOOL Get_Laser_I(HANDLE HComx)
{
	const char* scomdata = "acc 2\r\n";
	DWORD dwWrite = 0;
	return WriteFile( HComx , scomdata , 7 , &dwWrite , NULL );
}


/*********************************************************************************************************
** Function name:           Get_Laser_F
**
** Descriptions:            获取激光器频率
**
** input parameters:        HComx			Init_laser_rs232返回的句柄
**							                                                                                                  
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
BOOL Get_Laser_F(HANDLE HComx)
{
	const char* scomdata = "FM\r\n";
	DWORD dwWrite = 0;
	return WriteFile( HComx , scomdata , 4 , &dwWrite , NULL );
}


/*********************************************************************************************************
** Function name:           Set_Laser_Currents
**
** Descriptions:            设置激光器电流
**
** input parameters:        HComx			Init_laser_rs232返回的句柄
**							Cdata			电流值，传入100即100mA		
**
** Returned value:          成功返回true，否则返回false。 
*********************************************************************************************************/
BOOL Set_Laser_Currents(HANDLE HComx,int Cdata)
{
	//char scomdata[16] = {0x61,0x63,0x63,0x20,0x32,0x20};
	//if(Cdata >= 1000)
	//{
	//	scomdata[6] = Cdata / 1000 + 0x30;
	//	scomdata[7] = (Cdata % 1000)/100 + 0x30;
	//	scomdata[8] = (Cdata % 100)/10 + 0x30;
	//	scomdata[9] = Cdata % 10 + 0x30;
	//	scomdata[10]= 0x0d;
	//	scomdata[11]= 0x0a;
	//}
	//else//??????????????????????找施蕾确认
	//{
	//	scomdata[6] = Cdata / 100 + 0x30;
	//	scomdata[7] = (Cdata % 100)/10 + 0x30;
	//	scomdata[8] = Cdata % 10 + 0x30;
	//	scomdata[9]= 0x0d;
	//	scomdata[10]= 0x0a;
	//}
	//DWORD  dwLenth = (DWORD)strlen(scomdata);

	//DWORD dwWrite = 0;
	//return WriteFile( HComx , scomdata , dwLenth , &dwWrite , NULL );
	 //////////////////////////////////////////////////////////////////////////
	int nRetryCount = 0;
	int nDataLen = 0;
	char chDataBuffer[16] = { 0x61,0x63,0x63,0x20,0x32,0x20 };
	char chRecvBuffer[32];

	itoa(Cdata, chDataBuffer + 6, 10);
	nDataLen = 6 + strlen(chDataBuffer + 6);

	chDataBuffer[nDataLen++] = 0x0d;
	chDataBuffer[nDataLen++] = 0x0a;

	DWORD dwSize;

	char* pAccTag;

	while (nRetryCount++ < 6)
	{

		if (!WriteFile(HComx, chDataBuffer, nDataLen, &dwSize, NULL))
			goto SLEEP_AND_RETRY;

		Sleep(200);

		if (dwSize != nDataLen)
			goto SLEEP_AND_RETRY;

		if (!ReadFile(HComx, chRecvBuffer, sizeof(chRecvBuffer) - 1, &dwSize, NULL))
			goto SLEEP_AND_RETRY;

		if (dwSize < nDataLen)
			goto SLEEP_AND_RETRY;

		chRecvBuffer[dwSize] = 0;

		pAccTag = strstr(chRecvBuffer, "ACC");
		if (!pAccTag)
			goto SLEEP_AND_RETRY;

		return TRUE;

	SLEEP_AND_RETRY:
		Sleep(100);
	}
	return FALSE;

}
