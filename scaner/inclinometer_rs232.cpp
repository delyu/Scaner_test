#include "stdafx.h"
#include "inclinometer_rs232.h"

/*********************************************************************************************************
** Function name:           Init_inclinometer_rs232
**
** Descriptions:            ���ڳ�ʼ������
**
** input parameters:        ComName		���ں�
**							                                                                                                  
** Returned value:          �ɹ������ļ������������򷵻�0��ʾ���󣬿���ͨ��errno�鿴������� 
*********************************************************************************************************/
HANDLE Init_inclinometer_rs232(const CString& ComName)
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





