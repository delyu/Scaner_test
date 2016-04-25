#include "stdafx.h"
#include "laser_rs232.h"

/*********************************************************************************************************
** Function name:           Init_laser_rs232
**
** Descriptions:            ���ڳ�ʼ������
**
** input parameters:        ComName		���ں�
**							                                                                                                  
** Returned value:          �ɹ������ļ������������򷵻�0��ʾ���󣬿���ͨ��errno�鿴������� 
*********************************************************************************************************/
HANDLE Init_laser_rs232(const CString& ComName)
{
	HANDLE hComx; 
	hComx = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL , NULL);	//�첽��ʽ
//	ASSERT(hComx != INVALID_HANDLE_VALUE);
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
	mydcb.BaudRate = 9600;							//��������ش��ڲ�����
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
** Function name:           Get_Laser_Tem
**
** Descriptions:            ��ȡ�������¶�
**
** input parameters:        HComx			Init_laser_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
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
** Descriptions:            ��ȡ����������
**
** input parameters:        HComx			Init_laser_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
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
** Descriptions:            ��ȡ������Ƶ��
**
** input parameters:        HComx			Init_laser_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
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
** Descriptions:            ���ü���������
**
** input parameters:        HComx			Init_laser_rs232���صľ��
**							Cdata			����ֵ������100��100mA		
**
** Returned value:          �ɹ�����true�����򷵻�false�� 
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
	//else//??????????????????????��ʩ��ȷ��
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
