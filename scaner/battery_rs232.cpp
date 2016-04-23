#include "stdafx.h"
#include "battery_rs232.h"

/*********************************************************************************************************
** Function name:           Init_battery_rs232
**
** Descriptions:            ���ڳ�ʼ������
**
** input parameters:        ComName		���ں�
**							                                                                                                  
** Returned value:          �ɹ������ļ������������򷵻�0��ʾ���󣬿���ͨ��errno�鿴������� 
*********************************************************************************************************/

HANDLE Init_battery_rs232(CString ComName)
{
	HANDLE hComx; 
	hComx = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 , NULL);	//�첽��ʽ

	if(hComx ==INVALID_HANDLE_VALUE)
		return 0;
	SetCommMask(hComx, EV_RXCHAR|EV_TXEMPTY );		//�����¼���������
	SetupComm( hComx, 6144,6144);					//�������������������С

	COMMTIMEOUTS CommTimeOuts;						//���峬ʱ�ṹ
	CommTimeOuts.ReadIntervalTimeout = 100;				        /*  �����ַ������ʱ����      */
	CommTimeOuts.ReadTotalTimeoutConstant = 100;		        /*  �������ܳ�ʱ����            */
	CommTimeOuts.ReadTotalTimeoutMultiplier = 1;	
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;  /*  д�����ܳ�ʱ���� (дΪ0����ʾû�г�ʱ����)      */	
	CommTimeOuts.WriteTotalTimeoutMultiplier = 10;
	if(!SetCommTimeouts(hComx, &CommTimeOuts))			//���ö�д����������ĳ�ʱ
	{
		CloseHandle(hComx);
		return 0;
	}
	DCB mydcb;										// �������ݿ��ƿ�ṹ
	GetCommState(hComx, &mydcb);					//������ԭ���Ĳ�������
	mydcb.BaudRate = 9600;							//������
	mydcb.fBinary = TRUE;
	//	mydcb.fBinary = FALSE;							//������
	mydcb.fParity = FALSE;							//��У��
	mydcb.Parity = NOPARITY;
		
	mydcb.ByteSize = 8;
	mydcb.StopBits = ONESTOPBIT;
					
	mydcb.fDtrControl  = FALSE;			                        /*  Ӳ��������������            */	
	mydcb.fOutxDsrFlow = FALSE;		
	mydcb.fRtsControl  = FALSE;
	mydcb.fOutxCtsFlow = FALSE;  
	
	mydcb.fOutX = FALSE;						    /*  ��ֹ��������ơ�fOutX: �յ�Xoff��ֹͣ����       */
	mydcb.fInX  = FALSE;							/*  ��ֹ��������ơ�fInX: ����������������Xoff    */
	mydcb.fTXContinueOnXoff = FALSE;				/*  ��ֹ��������ơ�fInX�¼�֮�󣬷����Ƿ��������  */
	if(!SetCommState(hComx, &mydcb))					//�����´��ڲ���
	{
		CloseHandle(hComx);
		return 0;
	}
	PurgeComm(hComx, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//��������
	return hComx;
}


/*********************************************************************************************************
** Function name:           SDataCom_batteryr
**
** Descriptions:            ���ڷ������ݺ���
**
** input parameters:        HComx			Init_battery_rs232���صľ��
**							sdcom			����������
**							dwBytesWrite	���ݳ���
**                                                                                                   
** Returned value:          �ɹ�����ʵ�ʷ������ݳ��ȣ����򷵻�-1��ʾ���󣬿���ͨ��errno�鿴������� 
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
			WaitForSingleObject(Write_overlapped.hEvent,2000);			//�첽�ȴ�2��
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
** Descriptions:            ���ڽ������ݺ���
**
** input parameters:        HComx			Init_battery_rs232���صľ��
**							rdcom			�������ݱ����ַ
**							dwBytesRead		���ݳ���
**                                                                                                   
** Returned value:          �ɹ�����ʵ�ʽ������ݳ��ȣ����򷵻�-1��ʾ���󣬿���ͨ��errno�鿴�������
*********************************************************************************************************/
DWORD RDataCom_batteryr(HANDLE HComx, UCHAR* rdcom, DWORD dwBytesRead)
{
	DWORD dwRead =0;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED Read_overlapped;	
	memset(&Read_overlapped, 0, sizeof(OVERLAPPED));	
	ClearCommError(HComx, &dwErrorFlags, &ComStat );
	dwRead = ComStat.cbInQue;						//���뻺�����ж�������														
	if (dwRead > 0)
	{
		BOOL fReadStat;
		Read_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			
		fReadStat = ReadFile(HComx, rdcom, dwBytesRead, &dwBytesRead, &Read_overlapped);
		if (!fReadStat)
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
				WaitForSingleObject(Read_overlapped.hEvent,2000);			//�첽�ȴ�2��
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
** Descriptions:            ��ȡ���ʣ�����
**
** input parameters:        HComx			Init_battery_rs232���صľ��							
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false��
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
** Descriptions:            ��ȡ��ؿ���ʱ��
**
** input parameters:        HComx			Init_battery_rs232���صľ��							
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
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
** Descriptions:            ���Ͷ�ȡָ��
**
** input parameters:        HComx			Init_battery_rs232���صľ��							
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
bool Send_read_order(HANDLE HComx)
{
	static UCHAR scomdata[7]={0x00,0xa5,0x4f,0xaa,0x00,0x02,0x0a};
	if(SDataCom_batteryr(HComx,scomdata,7) == -1)
		return false;
	return true;
}