#include "StdAfx.h"
#include "FT2232Module.h"
#include "windows.h"

CFT2232Module::CFT2232Module(void)
{
	m_ftHanle = NULL;
}

CFT2232Module::~CFT2232Module(void)
{
	Close();
}

BOOL CFT2232Module::Open(int nIndex , CEdit* pEdit)
{
	FT_STATUS ftStatus;

	pEdit->SetWindowText( _T("FT_Open") );
	ftStatus = FT_Open( nIndex , &m_ftHanle);
	if(ftStatus != FT_OK)
		return FALSE;

	pEdit->SetWindowText( _T("FT_SetBitMode") );
	//reset mode
//	ftStatus = FT_SetBitMode( m_ftHanle ,  0xff ,  0x00 );
//	if( FT_OK != ftStatus )
//		goto __label_failed;
		
	 //Sync FIFO mode
	ftStatus = FT_SetBitMode( m_ftHanle, 0xff, 0x40);//同步模式
if (ftStatus != FT_OK)
		goto __label_failed;

	Sleep(120);

	pEdit->SetWindowText( _T("FT_SetLatencyTimer") );
	if( FT_OK != FT_SetLatencyTimer(m_ftHanle, 10) )
		goto __label_failed;

	pEdit->SetWindowText( _T("FT_SetUSBParameters") );
	if( FT_OK != FT_SetUSBParameters(m_ftHanle,0x10000, 0x10000) )
		goto __label_failed;

/*
	if( FT_OK != FT_SetFlowControl(m_ftHanle, FT_FLOW_RTS_CTS, 0x0, 0x0) )
		goto __label_failed;*/
	/*
	pEdit->SetWindowText( _T("FT_Purge") );
	if( FT_OK != FT_Purge(m_ftHanle, FT_PURGE_RX) )
		goto __label_failed;
		*/
	return TRUE;
__label_failed:
	FT_Close( m_ftHanle );
	m_ftHanle = NULL;
	return FALSE;
}

BOOL CFT2232Module::Close()
{
	if( m_ftHanle )
		if( FT_OK == FT_Close( m_ftHanle ) )
			m_ftHanle = NULL;

	return NULL == m_ftHanle;
}

BOOL CFT2232Module::Read( LPVOID pBuffer , DWORD dwBufSize , DWORD* lpdwReadSize )
{
	FT_STATUS ftStatus;
	DWORD dwSend , dwRcv, dwEvent;
	ftStatus = FT_GetStatus( m_ftHanle , &dwRcv , &dwSend ,  &dwEvent  );
	if( FT_OK == ftStatus && dwRcv > 0  )
	{
		if(dwRcv > dwBufSize)
			dwRcv = dwBufSize;

		ftStatus = FT_Read( m_ftHanle , pBuffer , dwRcv , lpdwReadSize );
		if( FT_OK == ftStatus )
			return TRUE;

		/*错误处理*/
	}
	return FALSE;
}
