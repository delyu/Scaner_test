#include "stdafx.h"
#include "fpga_rs232.h"




#ifndef _NEW_PROTOCOL

#define RETURN_COMMUNICATE_RESULT( handle , data , size  ) \
	DWORD dwSize;\
	return WriteFile( handle , data , size , &dwSize , NULL );

#else

#define RETURN_COMMUNICATE_RESULT( handle , data , size ) \
	unsigned char chRecvBuffer[96];\
	DWORD dwSize;\
	if( !WriteFile(handle , data , size , &dwSize , NULL ) )\
		return FALSE;\
	if( !ReadFile(handle,chRecvBuffer,sizeof(chRecvBuffer),&dwSize,NULL) )\
		return FALSE;\
	if( dwSize < (size) )\
		return FALSE;\
	return \
		(0==memcmp( data , chRecvBuffer + dwSize - (size) , size ) );//���ܻ�����������������

#endif







long set_Brate = 0;//URAT BRATE

/*********************************************************************************************************
** Function name:           Init_fpga_rs232
**
** Descriptions:            ���ڳ�ʼ������
**
** input parameters:        ComName		���ں�
**							                                                                                                   
** Returned value:          �ɹ������ļ������������򷵻�0��ʾ���󣬿���ͨ��errno�鿴������� 
*********************************************************************************************************/
HANDLE Init_fpga_rs232(const CString& ComName)
{
	HANDLE hComx; 
	hComx = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL  , NULL);	
//	ASSERT(hComx != INVALID_HANDLE_VALUE);
	if(hComx ==INVALID_HANDLE_VALUE)
		return 0;
	SetCommMask(hComx, EV_RXCHAR|EV_TXEMPTY );		//�����¼���������
	SetupComm( hComx, 8192,8192);					//�������������������С

	COMMTIMEOUTS CommTimeOuts;						//���峬ʱ�ṹ
	CommTimeOuts.ReadIntervalTimeout = 200;				        /*  �����ַ������ʱ����      */
	CommTimeOuts.ReadTotalTimeoutConstant = 500;		        /*  �������ܳ�ʱ����            */
	CommTimeOuts.ReadTotalTimeoutMultiplier = 200;	
	CommTimeOuts.WriteTotalTimeoutConstant = 500;  /*  д�����ܳ�ʱ���� (дΪ0����ʾû�г�ʱ����)      */	
	CommTimeOuts.WriteTotalTimeoutMultiplier = 200;
	if(!SetCommTimeouts(hComx, &CommTimeOuts))			//���ö�д����������ĳ�ʱ
	{
		CloseHandle(hComx);
		return 0;
	}
	DCB mydcb;										// �������ݿ��ƿ�ṹ
	GetCommState(hComx, &mydcb);					//������ԭ���Ĳ�������
	mydcb.BaudRate = set_Brate;							//FPGA���ڲ�����
	mydcb.fBinary = TRUE;
	//	mydcb.fBinary = FALSE;							//������
	mydcb.fParity = FALSE;							//��У��λ
	mydcb.Parity = NOPARITY;

	mydcb.ByteSize = 8;
	mydcb.StopBits = ONESTOPBIT;
					
	mydcb.fDtrControl  = FALSE;			          /*  Ӳ��������������            */	
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


void Set_fpga_brate(long BrateNUM)												//���ò�����
{
	if(set_Brate != BrateNUM)
		set_Brate = BrateNUM;
}


/*********************************************************************************************************
** Function name:           Set_Prober_Gain
**
** Descriptions:            ����̽��������(������)
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							Gdata			̽��������ֵ
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Set_Prober_Gain(HANDLE HComx,int Gdata)
{
	UCHAR scomdata[5]= { 0xa5,0xa5,0x06,0x00,0x00 };

	scomdata[3] = Gdata /256;	
	scomdata[4] = Gdata % 256;
	
	RETURN_COMMUNICATE_RESULT( HComx,  scomdata , 5 )
}


/*********************************************************************************************************
** Function name:           Get_Prober_Tem
**
** Descriptions:            �´���ȡ̽�����¶ȵ�ָ��ڴ��ڽ����߳����жϽ��յ����ݵİ�ͷ
**							��Ϊ5a a5 00��Ϊ̽�����¶�ֵ
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Get_Prober_Tem(HANDLE HComx,char chRecvBuffer[])
{
	/*static UCHAR scomdata[3]={0xa5,0xa5,0x04};
	DWORD dwWrite = 0;
	return WriteFile( HComx , scomdata , 3 , &dwWrite , NULL );*/
	//////////////////////////////////////////////////////////////////////////
	int nRetryCount = 0;
	int nDataLen = 0;
	char chDataBuffer[3] = { 0xa5,0xa5,0x04 };
	char readDataBuffer[72];
	//char TestRecvBuffer[72] = { 0xA5,0xA5,0x01,0x08,0xA5,0xA5,0x02,0xA5,0xA5,0x03,0xA5,0xA5,0x06,0xa5,0x87,0x67 };

	//itoa(Cdata, chDataBuffer + 6, 10);
	nDataLen =  strlen(chDataBuffer );
	DWORD dwSize;	
	//char* pAccTag;

	while (nRetryCount++ < 6)
	{

	if (!WriteFile(HComx, chDataBuffer, nDataLen, &dwSize, NULL))
			goto SLEEP_AND_RETRY;

		Sleep(200);

		if (dwSize != nDataLen)
			goto SLEEP_AND_RETRY;

		if (!ReadFile(HComx, readDataBuffer, 72, &dwSize, NULL))
			goto SLEEP_AND_RETRY;
		for(int i = 0; i < sizeof(readDataBuffer); i++)
		{
			chRecvBuffer[i] = readDataBuffer[i];
		}
		//delete readDataBuffer;

	/*	if (dwSize < nDataLen)
			goto SLEEP_AND_RETRY;

		chRecvBuffer[dwSize] = 0;*/

// 		pAccTag = strstr(chRecvBuffer, "ACC");
// 		if (!pAccTag)
// 			goto SLEEP_AND_RETRY;

		return TRUE;

	SLEEP_AND_RETRY:
		Sleep(100);
	}
	return FALSE;
}


/*********************************************************************************************************
** Function name:           Laser_ON
**
** Descriptions:            ������
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Laser_ON(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xa5,0xa5,0x02};//�޸�ΪA5A502
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           Laser_OFF
**
** Descriptions:            �ؼ���
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Laser_OFF(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xa5,0xa5,0x03};//�޸�Ϊa5a503
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           Set_Vth
**
** Descriptions:            ������ֵ��ѹ
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Set_Vth(HANDLE HComx,int Vdata)
{
	static UCHAR scomdata[5]={0xa5,0xa5,0x00,0x00,0x00};
	scomdata[3] = Vdata / 256;
	scomdata[4] = Vdata % 256;
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 5 )
}

/*********************************************************************************************************
** Function name:           Set_PulseWidth
**
** Descriptions:            �������������ѹ
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**
** Returned value:          �ɹ�����true�����򷵻�false��
*********************************************************************************************************/
BOOL Set_PulseWidth(HANDLE HComx, int Pdata)
{
	static UCHAR scomdata[5] = { 0xa5,0xa5,0x05,0x00,0x00 };
	scomdata[3] = Pdata / 256;
	scomdata[4] = Pdata % 256;

	RETURN_COMMUNICATE_RESULT(HComx, scomdata, 5)
}

/*********************************************************************************************************
** Function name:           Set_PulseVth1
**
** Descriptions:            ����������ֵ��ѹ1
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**
** Returned value:          �ɹ�����true�����򷵻�false��
*********************************************************************************************************/
BOOL Set_PulseVth1(HANDLE HComx, int Pdata)
{
	static UCHAR scomdata[5] = { 0xa5,0xa5,0x07,0x00,0x00 };
	scomdata[3] = Pdata / 256;
	scomdata[4] = Pdata % 256;

	RETURN_COMMUNICATE_RESULT(HComx, scomdata, 5)
}

/*********************************************************************************************************
** Function name:           Set_PulseVth2
**
** Descriptions:            ����������ֵ��ѹ2
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**
** Returned value:          �ɹ�����true�����򷵻�false��
*********************************************************************************************************/
BOOL Set_PulseVth2(HANDLE HComx, int Pdata)
{
	static UCHAR scomdata[5] = { 0xa5,0xa5,0x17,0x00,0x00 };
	scomdata[3] = Pdata / 256;
	scomdata[4] = Pdata % 256;

	RETURN_COMMUNICATE_RESULT(HComx, scomdata, 5)
}

/*********************************************************************************************************
** Function name:           Set_Tem_M
**
** Descriptions:            �����²�����
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Set_Tem_M(HANDLE HComx,int Mdata)
{
	static UCHAR scomdata[5]={0xa5,0xa5,0x08,0x00,0x00};
	scomdata[3] = Mdata / 256;	
	scomdata[4] = Mdata % 256;
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 5 )
}


/*********************************************************************************************************
** Function name:           Set_Tem_N
**
** Descriptions:            �����²�ϵ��
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Set_Tem_N(HANDLE HComx,int Ndata)
{
	UCHAR scomdata[4]={0xa5,0xa5,0x09,0x00};
	scomdata[3] = Ndata;
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 4 )
}


/*********************************************************************************************************
** Function name:           Warm_ON
**
** Descriptions:            ���²�����ģʽ
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Warm_ON(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xa5,0xa5,0x0A};
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           Warm_OFF
**
** Descriptions:            �ر��²�����ģʽ
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Warm_OFF(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xa5,0xa5,0x0B};
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           Set_AD_ns
**
** Descriptions:            ����AD�ɼ��屣��ʱʱ��
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							xns				ʱ��ֵns
**
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL Set_AD_ns(HANDLE HComx,int xns)
{
	UCHAR scomdata[4]={0x55,0x00,0xff,0x00};
	scomdata[3] = xns;
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 4 )
}


/*********************************************************************************************************
** Function name:           JD_Switch_ON
**
** Descriptions:            �򿪽ڵ翪��
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL JD_Switch_ON(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xaa,0xff,0x55};
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           JD_Switch_OFF
**
** Descriptions:            �رսڵ翪��
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL JD_Switch_OFF(HANDLE HComx)
{
	static UCHAR scomdata[3]={0xaa,0x55,0xff};
	
	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}


/*********************************************************************************************************
** Function name:           _GNSS
**
** Descriptions:            ���ػ�����GNSS����
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**                                                                                                   
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
BOOL _GNSS(HANDLE HComx)														
{
	static UCHAR scomdata[3]={0xff,0x5a,0x5a};

	RETURN_COMMUNICATE_RESULT( HComx , scomdata , 3 )
}



/*********************************************************************************************************
** Function name:           Get_Laser_Tem
**
** Descriptions:            ��ȡ�������¶�
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
// BOOL Get_Laser_Tem(HANDLE HComx)
// {
// 	char pro_data[3]={0xff,0xa5,0xa5};
// 	if(SDataCom_fpga(HComx,(UCHAR*)pro_data,3) == -1)
// 		return false;
// 	Sleep(1);
// 	char* scomdata = "IT\r\n";
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	Sleep(1);
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	return true;
// }


/*********************************************************************************************************
** Function name:           Get_Laser_I
**
** Descriptions:            ��ȡ����������
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
// BOOL Get_Laser_I(HANDLE HComx)
// {
// 	char pro_data[3]={0xff,0xa5,0xa5};
// 	if(SDataCom_fpga(HComx,(UCHAR*)pro_data,3) == -1)
// 		return false;
// 	Sleep(10);
// 	char* scomdata = "acc 2\r\n";
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	Sleep(10);
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	return true;
// }


/*********************************************************************************************************
** Function name:           Get_Laser_F
**
** Descriptions:            ��ȡ������Ƶ��
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							                                                                                                  
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
// BOOL Get_Laser_F(HANDLE HComx)
// {
// 	char pro_data[3]={0xff,0xa5,0xa5};
// 	if(SDataCom_fpga(HComx,(UCHAR*)pro_data,3) == -1)
// 		return false;
// 	Sleep(1);
// 	char* scomdata = "FM\r\n";
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	Sleep(1);
// 	if(SDataCom_fpga(HComx,(UCHAR*)scomdata,strlen(scomdata)) == -1)
// 		return false;
// 	return true;
// }


/*********************************************************************************************************
** Function name:           Set_Laser_Currents
**
** Descriptions:            ���ü���������
**
** input parameters:        HComx			Init_fpga_rs232���صľ��
**							Cdata			����ֵ������100��100mA		
**
** Returned value:          �ɹ�����true�����򷵻�false�� 
*********************************************************************************************************/
// BOOL Set_Laser_Currents(HANDLE HComx,int Cdata)
// {
// 	UCHAR uRcvBuf[16];
// 	char scomdata[16] = {0x41,0x43,0x43,0x20,0x32,0x20};
// 	int nDataLen;
// 	if(Cdata >= 1000)
// 	{
// 		scomdata[6] = Cdata / 1000 + 0x30;
// 		scomdata[7] = (Cdata % 1000)/100 + 0x30;
// 		scomdata[8] = (Cdata % 100)/10 + 0x30;
// 		scomdata[9] = Cdata % 10 + 0x30;
// 		scomdata[10]= 0x0d;
// 		scomdata[11]= 0x0a;
// 		nDataLen = 12;
// 	}
// 	else
// 	{
// 		scomdata[6] = Cdata / 100 + 0x30;
// 		scomdata[7] = (Cdata % 100)/10 + 0x30;
// 		scomdata[8] = Cdata % 10 + 0x30;
// 		scomdata[9]= 0x0d;
// 		scomdata[10]= 0x0a;
// 		nDataLen = 11;
// 	}
// 	
// 	int nRetryCount = 0;
// __label_send_data:
// 	if(SDataCom_fpga( HComx,(UCHAR*)scomdata, nDataLen ) == (DWORD)-1)
// 		return false;
// 	Sleep( 20 );
// 	if(RDataCom_fpga( HComx , uRcvBuf , sizeof(uRcvBuf) ) == (DWORD)-1 )
// 		return false;
// 
// 	if( 0x41 != uRcvBuf[0] )/*���ܵ��ĵ�һ���ֽڲ���0x41˵����������*/
// 	{
// 		if( nRetryCount++ > 3 )/*�ظ����Գ�����ξͷ��ش���*/
// 			return false;
// 
// 		Sleep(10);
// 		goto __label_send_data;
// 	}
// 		
// 	return true;
// }

BOOL StartGNSS( HANDLE hCom )
{
	static UCHAR data[3]={0xff,0x5a,0x5a};
	RETURN_COMMUNICATE_RESULT( hCom , data , 3 )
}

BOOL StopGNSS( HANDLE hCom )
{
	static UCHAR data[3]={0xff,0x00,0xaa};
	RETURN_COMMUNICATE_RESULT( hCom , data , 3 )
}

BOOL GetTempAndSener( HANDLE hCom , int* pTemp , int* pSen )
{
	static unsigned char chSendData[3] = {0x5a,0xa5,0x00};

	unsigned char chRecvBuffer[16];

	DWORD dwSize = 0;
	if( !WriteFile( hCom , chSendData , sizeof(chSendData) , &dwSize , NULL  ) )
		return FALSE;
	
	if( !ReadFile( hCom , chRecvBuffer , sizeof(chRecvBuffer) , &dwSize ,NULL ) )
		return FALSE;

	if( 0x5a != chRecvBuffer[0])
		return FALSE;

	//�������¶�
	*pTemp = (((chRecvBuffer[2])&0xf) << 8 )| chRecvBuffer[3];
	//������
	*pSen = (((chRecvBuffer[9])&0xf) << 8 )| chRecvBuffer[10];

	return TRUE;
}
