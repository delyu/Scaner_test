
// scanerDlg.h : 头文件
//

#pragma once


#include "FT2232Module.h"

// CscanerDlg 对话框
class CscanerDlg : public CDialogEx
{
// 构造
public:
	CscanerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCANER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:	
	char laser_montor;									//用以监控激光器
	char battery_montor;								//电池监控


	HANDLE fpga_H;
	HANDLE pThread_fpga;
	HANDLE sensor_H;
	HANDLE pThread_sensor;
	HANDLE battery_H;
	HANDLE pThread_battery;
	HANDLE xmotor_H;
	HANDLE ymotor_H;

	//HANDLE m_hLaser;/*激光器句柄*/

	HANDLE m_hThread;
	DWORD m_dwThreadID;

	HANDLE m_hThreadGPS;

	volatile BOOL m_bGPSThreadRun;

	CFT2232Module m_2232;


	BOOL m_bTest;

	CRITICAL_SECTION m_GpsCS;

	BOOL GetGPSThreadRun()
	{
		BOOL bRet;
		EnterCriticalSection( &m_GpsCS  );
		bRet = m_bGPSThreadRun;
		LeaveCriticalSection( &m_GpsCS );
		return bRet;
	}


VOID SetGPSThreadRun( BOOL bRun )
{
	EnterCriticalSection( &m_GpsCS  );
	m_bGPSThreadRun = bRun ;
	LeaveCriticalSection( &m_GpsCS );
}


	BOOL m_bAskForLaserInfo;

	void QuitReadThread();
	void StartUsbRead(LPCTSTR lpszFileName );
	void EndUsbRead();
	enum{BufferSize = 4096*16,
	GnssBufferSize = 4096 };/*缓冲区大小*/


	BOOL _GetMultByteStringFromCtrl(char* pBuffer , DWORD dwBufSize , UINT uID );

// 实现
protected:
	HICON m_hIcon;

	void _ReportError( LPCTSTR lpszErrorMsg  );

	void _CalculateWarmFactor();


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton12();
	virtual void OnCancel();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton18();

	afx_msg void OnBnClickedButton13();

	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();

	
	volatile BOOL m_bRecvGNSS;
	enum{
		MsgStartRead = WM_USER + 1,
		MsgQuitThread,
		MsgStopRead,

		MsgGpsDataArrive,/*GPS数据到达*/
	};

	static unsigned __stdcall ReadUsbThread(  void* pArgs );
	static unsigned __stdcall ReadGNSSThread( void* pArgs );
	static unsigned __stdcall ReadGPSThread( void* pArgs );

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton31();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedButton28();
//	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton38();
//	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton40();
	afx_msg void OnBnClickedButton41();
	afx_msg void OnBnClickedButton34();
	afx_msg void OnBnClickedButton35();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton37();
	afx_msg void OnBnClickedButtonGetInfo();

	afx_msg void OnBnClickedButtonPosy();
	afx_msg void OnBnClickedButtonBeginGps();
	afx_msg void OnBnClickedButtonClearText();
	afx_msg void OnBnClickedButtonStopGps();
	afx_msg void OnBnClickedButtonEdttTest();
//	afx_msg void OnBnClickedCheckWarm();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnEnChangeEditWarmN();
	afx_msg void OnEnChangeEditWarmV2ad();
	afx_msg void OnBnClickedButtonOpenLaser();
	afx_msg void OnBnClickedButtonCloseLaser();
	afx_msg void OnBnClickedButton42();
	afx_msg void OnBnClickedButton43();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton30();
};
