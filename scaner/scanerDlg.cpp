
// scanerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "scaner.h"
#include "scanerDlg.h"
#include "afxdialogex.h"


#include "fpga_rs232.h"
#include "laser_rs232.h"
#include "inclinometer_rs232.h"
#include "battery_rs232.h"
#include "motor_rs232.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  TIMER_laser 6001
#define  TIMER_battery 6002
#define  TIMER_xmotor 6003
#define  TIMER_ymotor 6004

//全局
BOOL stop_thread_fpga;							//用以控制fpga线程结束
BOOL stop_thread_battery;						//用以控制电池线程结束
BOOL stop_thread_sensor;						//用以控制倾角仪线程结束
UINT fpga_ComRcvTread(LPVOID pParm);			//fpga串口接收线程
UINT laser_ComRcvTread(LPVOID pParm);			//laser串口线程
UINT sensor_ComRcvTread(LPVOID pParm);			//sensor串口线程
UINT battery_ComRcvTread(LPVOID pParm);			//battery串口线程

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CscanerDlg 对话框




CscanerDlg::CscanerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CscanerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	laser_montor = 0;
	battery_montor = 0x0d;
	//全局
	stop_thread_fpga = false;
	stop_thread_battery = false;
	stop_thread_sensor = false;
	xmotor_H = NULL;

	m_hThread = NULL;

	fpga_H = NULL;
	pThread_fpga = NULL;
	sensor_H = NULL;
	pThread_sensor = NULL;
	battery_H = NULL;
	pThread_battery = NULL;
	ymotor_H = NULL;
	m_bAskForLaserInfo = FALSE;

//	m_hLaser = NULL;


	m_hThreadGPS = NULL;
	m_bGPSThreadRun = FALSE;

	m_bTest = TRUE;
}

void CscanerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CscanerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON12, &CscanerDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON7, &CscanerDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CscanerDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CscanerDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CscanerDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON18, &CscanerDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON13, &CscanerDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON19, &CscanerDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CscanerDlg::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON11, &CscanerDlg::OnBnClickedButton11)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CscanerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CscanerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CscanerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CscanerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CscanerDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CscanerDlg::OnBnClickedButton6)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON21, &CscanerDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON22, &CscanerDlg::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CscanerDlg::OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON23, &CscanerDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON31, &CscanerDlg::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CscanerDlg::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON25, &CscanerDlg::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CscanerDlg::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON27, &CscanerDlg::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON28, &CscanerDlg::OnBnClickedButton28)
//	ON_BN_CLICKED(IDC_BUTTON33, &CscanerDlg::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON38, &CscanerDlg::OnBnClickedButton38)
//	ON_BN_CLICKED(IDC_BUTTON39, &CscanerDlg::OnBnClickedButton39)
ON_BN_CLICKED(IDC_BUTTON39, &CscanerDlg::OnBnClickedButton39)
ON_BN_CLICKED(IDC_BUTTON40, &CscanerDlg::OnBnClickedButton40)
ON_BN_CLICKED(IDC_BUTTON41, &CscanerDlg::OnBnClickedButton41)
ON_BN_CLICKED(IDC_BUTTON34, &CscanerDlg::OnBnClickedButton34)
ON_BN_CLICKED(IDC_BUTTON35, &CscanerDlg::OnBnClickedButton35)
ON_BN_CLICKED(IDC_BUTTON36, &CscanerDlg::OnBnClickedButton36)
ON_BN_CLICKED(IDC_BUTTON37, &CscanerDlg::OnBnClickedButton37)
ON_BN_CLICKED(IDC_BUTTON_GET_INFO, &CscanerDlg::OnBnClickedButtonGetInfo)
ON_BN_CLICKED(IDC_BUTTON_POSY, &CscanerDlg::OnBnClickedButtonPosy)
ON_BN_CLICKED(IDC_BUTTON_BEGIN_GPS, &CscanerDlg::OnBnClickedButtonBeginGps)
ON_BN_CLICKED(IDC_BUTTON_CLEAR_TEXT, &CscanerDlg::OnBnClickedButtonClearText)
ON_BN_CLICKED(IDC_BUTTON_STOP_GPS, &CscanerDlg::OnBnClickedButtonStopGps)
ON_BN_CLICKED(IDC_BUTTON_EDTT_TEST, &CscanerDlg::OnBnClickedButtonEdttTest)
ON_BN_CLICKED(IDC_CHECK_WARM, &CscanerDlg::OnBnClickedCheckWarm)
ON_EN_CHANGE(IDC_EDIT7, &CscanerDlg::OnEnChangeEdit7)
ON_EN_CHANGE(IDC_EDIT_WARM_N, &CscanerDlg::OnEnChangeEditWarmN)
ON_EN_CHANGE(IDC_EDIT_WARM_V2AD, &CscanerDlg::OnEnChangeEditWarmV2ad)
//ON_BN_CLICKED(IDC_BUTTON_OPEN_LASER, &CscanerDlg::OnBnClickedButtonOpenLaser)
//ON_BN_CLICKED(IDC_BUTTON_CLOSE_LASER, &CscanerDlg::OnBnClickedButtonCloseLaser)
ON_BN_CLICKED(IDC_BUTTON42, &CscanerDlg::OnBnClickedButton42)
ON_BN_CLICKED(IDC_BUTTON43, &CscanerDlg::OnBnClickedButton43)
//ON_BN_CLICKED(IDC_BUTTON29, &CscanerDlg::OnBnClickedButton29)
ON_BN_CLICKED(IDC_BUTTON30, &CscanerDlg::OnBnClickedButton30)
END_MESSAGE_MAP()


// CscanerDlg 消息处理程序

BOOL CscanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetDlgItemInt(IDC_EDIT1,1500);	//x电机速度
	SetDlgItemInt(IDC_EDIT14,1500);	//y电机速度
	SetDlgItemInt(IDC_EDIT2,500);
	SetDlgItemInt(IDC_EDIT3,500);
	SetDlgItemInt(IDC_EDIT4,2000);	//回零速度
	SetDlgItemInt(IDC_EDIT5,500);	//回零加速度
	SetDlgItemInt(IDC_EDIT7,2600);			//APD增益
	SetDlgItemInt(IDC_EDIT20,5350);			//温补常量
	SetDlgItemInt(IDC_EDIT21,22);			//温补系数
	//SetDlgItemInt(IDC_EDIT22,20);			//AD延时
	SetDlgItemInt(IDC_EDIT30,115200);			//FPGA波特率										
	
	SetDlgItemInt(IDC_EDIT35, 4000);//整形 脉宽电压
	SetDlgItemInt(IDC_EDIT36, 149);//整形阈值电压1
	SetDlgItemInt(IDC_EDIT37, 240);//整形阈值电压2
	//串口号
	SetDlgItemText(IDC_EDIT24,_T("COM1"));
	SetDlgItemText(IDC_EDIT25,_T("USB0"));
	SetDlgItemText(IDC_EDIT26,_T("USB1"));
	SetDlgItemText(IDC_EDIT27,_T("COM4"));
	SetDlgItemText(IDC_EDIT28,_T("COM1"));
	




	SetDlgItemText( IDC_EDIT_WARM_N , _T("22") );
	SetDlgItemText( IDC_EDIT_WARM_V2AD , _T("2000") );


	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_GPS );
	pEdit->FmtLines( TRUE );


	unsigned int uiAddr;
	m_hThread = (HANDLE)_beginthreadex(NULL,0,ReadUsbThread,this, NULL,&uiAddr );
	if(  !m_hThread )
	{
		AfxMessageBox( _T("usb 读取线程开打失败") );
		return FALSE;
	}
	m_dwThreadID = (DWORD)uiAddr;



	InitializeCriticalSection( &m_GpsCS );
		

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CscanerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CscanerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CscanerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//测试
void CscanerDlg::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
/*	UINT u = GetDlgItemInt(IDC_EDIT35);
	Set_PulseWidth(nullptr, u);*/
}

void CscanerDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	stop_thread_fpga =true;
	stop_thread_battery = true;
	stop_thread_sensor = true;
	Sleep(100);
	if( fpga_H )
		KillTimer(TIMER_laser);
	if(battery_H)
		KillTimer(TIMER_battery);
	if(xmotor_H)
		KillTimer(TIMER_xmotor);
	if(ymotor_H)
		KillTimer(TIMER_ymotor);
	
	if( fpga_H )
		CloseHandle(fpga_H);
	//if( m_hLaser )
	//	CloseHandle( m_hLaser );

	if(pThread_fpga)
		CloseHandle(pThread_fpga);
	if(sensor_H)
		CloseHandle(sensor_H);
	if(pThread_sensor)
		CloseHandle(pThread_sensor);
	if(battery_H)
		CloseHandle(battery_H);
	if(pThread_battery)
		CloseHandle(pThread_battery);
	if(xmotor_H)
		Close_motor(xmotor_H);
	if(ymotor_H)
		Close_motor(ymotor_H);

	DeleteCriticalSection( &m_GpsCS );

	CDialogEx::OnCancel();
}

//fpga_com接收线程
UINT fpga_ComRcvTread(LPVOID pParm)
{
	CscanerDlg* dlg = (CscanerDlg*)pParm;
	UCHAR rBuffer[32] = {0x00};
	DWORD rnum = 0;
	while(!stop_thread_fpga)
	{
		Sleep(1);

		if( !ReadFile( dlg->fpga_H , rBuffer , sizeof(rBuffer) , &rnum , NULL  ) )
			continue;

		if(rBuffer[0] == 0x5a && rBuffer[1] == 0xa5)	//探测器温度
		{
			UINT Tem = (rBuffer[2] & 0x0f) * 256 + rBuffer[3];
			dlg->SetDlgItemInt(IDC_EDIT19,Tem);
			memset(rBuffer,0x00,27);
		}
		if(rBuffer[5] == 0x54)			//激光器温度信息
		{
			CString ctem;
			CString cs;
			int i;
			for(i=5;i<17;i++)
			{
				cs.Format(_T("%c"), rBuffer[i]);
				ctem += cs;
			}
			dlg->SetDlgItemText(IDC_EDIT10,ctem);
			memset(rBuffer,0x00,27);
		}
		if(rBuffer[5] == 0x4c)			//激光器电流信息
		{
			CString cmA;
			CString cs;
			int i;
			for(i=5;i<22;i++)
			{
				cs.Format(_T("%c"), rBuffer[i]);
				cmA += cs;
			}
			dlg->SetDlgItemText(IDC_EDIT11,cmA);
			memset(rBuffer,0x00,27);
		}
		if(rBuffer[5] == 0x46)			//激光器频率信息
		{
			CString cHz;
			CString cs;
			int i;
			for(i=5;i<23;i++)
			{
				cs.Format(_T("%c"), rBuffer[i]);
				cHz += cs;
			}
			dlg->SetDlgItemText(IDC_EDIT12,cHz);
			memset(rBuffer,0x00,27);
		}
		memset(rBuffer,0x00,27);
	}

	return 0;
}

void BCD2String( UCHAR* pBuffer , LPTSTR pString  )
{
#define LOW_4Bit(b)		((b)&0xf)
#define HIGH_4Bit(b)	LOW_4Bit(b>>4)

	LPTSTR p = pString;

	if(  0x10 == pBuffer[0]  )
		*p++ = _T('-');

	/*整数部分*/
	*p++ = HIGH_4Bit(pBuffer[1]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[1]) + _T('0');
	*p++ = _T('.');

	/*小数部分*/
	*p++ = HIGH_4Bit(pBuffer[2]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[2]) + _T('0');
	*p++ = HIGH_4Bit(pBuffer[3]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[3]) + _T('0');

	/*字符串结束*/
	*p = _T('\0');

#undef LOW_4Bit
#undef HIGH_4Bit
}

//sensor_com接收线程
UINT sensor_ComRcvTread(LPVOID pParm)
{
	CscanerDlg* dlg = (CscanerDlg*)pParm;
	UCHAR rBuffer[18] = {0x00};
	DWORD rnum = 0;

	/*字符串缓冲区*/
	TCHAR chBuffer[16];

	while( !stop_thread_sensor )
	{
		Sleep(100);/*休眠100毫秒*/

		if( !ReadFile(dlg->sensor_H, rBuffer, 17 , &rnum , NULL ) )
			continue;

		if(0x68 == rBuffer[0]  && 0x0c == rBuffer[1])						//接收到有效数据
		{
			BCD2String( rBuffer + 4 , chBuffer );
			dlg->SetDlgItemText(IDC_EDIT17,chBuffer);

			BCD2String( rBuffer + 8 , chBuffer );
			dlg->SetDlgItemText(IDC_EDIT18,chBuffer);
		}
	}

	return 1;
}

//battery_com接收线程
UINT battery_ComRcvTread(LPVOID pParm)
{
	CscanerDlg* dlg = (CscanerDlg*)pParm;
	UCHAR rBuffer[4] = {0x00};
	int rnum = 0;
	while(1)
	{
		Sleep(1);
		if(stop_thread_battery)					//判断是否要结束进程
			return 1;
		else
			rnum = RDataCom_batteryr(dlg->battery_H, rBuffer, 4);
		if(rBuffer[0] == 0x01 && rBuffer[1] == 0x0a)		//指令发送成功
		{
			Send_read_order(dlg->battery_H);				//发送读取指令
			memset(rBuffer,0x00,4);
		}
/*		if(rBuffer[1] == 0x0a && rBuffer[0] != 0x01)		//指令未发送成功，继续发送
		{
			Get_dump_energy(dlg->battery_H);
			memset(rBuffer,0x00,4);
		}*/

		if(rBuffer[2] == 0x01 && rBuffer[3] == 0x0a)								//读取到数据
		{
			UINT num = rBuffer[1]*256+rBuffer[0];
			if(num >= 100)
				num = 100;
			dlg->SetDlgItemInt(IDC_EDIT16,num);
			memset(rBuffer,0x00,4);
		}
	}
}

//开激光
void CscanerDlg::OnBnClickedButton7()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}
	if( !Laser_ON(fpga_H))
		_ReportError( _T("laser on error!!!") );
}

//关激光
void CscanerDlg::OnBnClickedButton8()
{
	
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}
	if( !Laser_OFF(fpga_H))
		_ReportError( _T("Laser off error!!") );
}

//设置灵敏度
void CscanerDlg::OnBnClickedButton9()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}


	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_WARM );

	if( BST_CHECKED == pButton->GetCheck() )
	{
		Set_Tem_M( fpga_H ,  GetDlgItemInt( IDC_EDIT20 ) );
		Sleep( 10 );
		Set_Tem_N( fpga_H ,  GetDlgItemInt( IDC_EDIT21 ) );
		Sleep( 10 );
	}
	Set_Prober_Gain(fpga_H,GetDlgItemInt(IDC_EDIT7));									//设置探测器增益
}

//设置阈值电压
void CscanerDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}

	UINT u = GetDlgItemInt(IDC_EDIT8);
	if( !Set_Vth(fpga_H, u) )
		_ReportError( _T("set vth error!") );

}

//AD延时时间
void CscanerDlg::OnBnClickedButton18()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}
	
	UINT u =GetDlgItemInt(IDC_EDIT22);
	if( !Set_AD_ns(fpga_H,u) )
		_ReportError( _T("AD延时设置失败") );
}

//获取探测器温度
void CscanerDlg::OnBnClickedButton13()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}
	unsigned char chRecvBuffer[72];
	if (!Get_Prober_Tem(fpga_H, chRecvBuffer))												//获取探测器温度
		_ReportError(_T("获取温度补偿失败"));

	
	CString str, tem,stem1;
	//stem1 = "/r/n";
	/*if (fpga_H)
	{
	}*/
	SetDlgItemText(IDC_EDIT19, str);
	int j = 0;
	for (int i = 0; i < sizeof(chRecvBuffer); i++)
	{

		if (chRecvBuffer[i]==0xa5&&chRecvBuffer[i+1]==0xa5&& chRecvBuffer[i+34] == 0x0d && chRecvBuffer[i + 35] == 0x0a)
		{
			i += 2;
			j = i;
		}
		
		//////////////////////////////////////////////////////////////////////////
		int k = i - j;
		if (k< 34)
		{  
			int termn = chRecvBuffer[i] * 256 + chRecvBuffer[i + 1];
			tem.Format(_T("%d"), termn );
			switch (k/2)
			{
			case 0	:
				str += "APD温度: ";
					str += tem;
					str += "\r\n";
					break;
			case 1:
				str += "高压分压: ";
				str += tem;
				str += "\r\n"; 		
				break;
			case 2:
				str += "电池电压: ";
				str += tem;
				str += "\r\n";
				break;
			case 3:
				str += "电源切换电压: ";
				str += tem;
				str += "\r\n";
				break;
			case 4:
				str += "激光重频: ";
				str += tem;
				str += "\r\n";
				break;
			case 5:
				str += "激光开关: ";
				if (tem == "2")
					str += "开";
				else
				{
					str += "关";
				}
				str += "\r\n";
				break;
			case 6:
				str += "整形脉宽: ";
				str += tem;
				str += "\r\n";
				break;
			case 7:
				str += "灵敏度: ";
				str += tem;
				str += "\r\n";
				break;
			case 8:
				str += "整形阈值1: ";
				str += tem;
				str += "\r\n";
				break;
			case 9:
				str += "整形阈值2: ";
				str += tem;
				str += "\r\n";
				break;
			case 10:
				str += "温补参数M: ";
				str += tem;
				str += "\r\n";
				break;
			case 11:
				str += "温补参数N: ";
				str += tem;
				str += "\r\n";
				break;
			case 12:
				str += "温补开关: ";
				if (tem == "10")
					str += "开";
				else
				{
					str += "关";
				}
				str += "\r\n";
				break;
			case 13:
				str += "TDC延时: ";
				str += tem;
				str += "\r\n";
				break;
			case 14:
				str += "GPS开关: ";
				if (tem == "13")
					str += "开";
				else
				{
					str += "关";
				}
				str += "\r\n";
				break;
			case 15:
				str += "倾角传感器开关: ";
				if(tem=="16")
				str += "开";
				else
				{
					str += "关";
				}
				str += "\r\n";
				break;
			default:
				break;
			}				
			i++;
		}
		//////////////////////////////////////////////////////////////////////////
		/*		 tem.Format(_T("% 02x"), chRecvBuffer[i] & 0xff);
		if (tem=="a5"&&stem1!="a5")
		{
			stem1 = tem; 			
		}
		else if (tem == "a5"&& stem1 == "a5")
		{  		
			str += "\r\n";
			str += tem;	
			str += tem;	
			str += " ";
			stem1 = "";
		}
		else
		{
			if (stem1 == "a5")
			{
				str += stem1;
				str += " ";
			}
			str += tem;
			str += " ";
			stem1 = tem;
		}*/
				
		
	}	   	
	
	SetDlgItemText(IDC_EDIT19, str);
}


//打开节电开关
void CscanerDlg::OnBnClickedButton19()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}

	if( !JD_Switch_ON(fpga_H))												//打开节电开关
		_ReportError( _T("节电开关打开失败") );
}

//关闭节电开关
void CscanerDlg::OnBnClickedButton20()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpga没有打开") );
		return;
	}


	if( !JD_Switch_OFF(fpga_H) )												//关闭节电开关
		_ReportError( _T("节电开关关闭失败") );
}

//设置激光器电流
void CscanerDlg::OnBnClickedButton11()
{

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpga没有打开"));
		return;
	}

	UINT uCurrent = GetDlgItemInt(IDC_EDIT9);
	if( !Set_Laser_Currents( fpga_H , uCurrent ) )/*20141103*/
		_ReportError( _T("电流设置失败") );
		
}

//状态获取
void CscanerDlg::OnTimer(UINT_PTR nIDEvent)
{
	BOOL bNeedRead = TRUE;

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (TIMER_laser == nIDEvent)
	{/*20141103*/
		switch (laser_montor)
		{
		case 0:
			Get_Laser_Tem(fpga_H);													//获取激光器温度
			laser_montor++;
			break;
		case 1:
			Get_Laser_I(fpga_H);													//获取激光器电流
			laser_montor++;
			break;
		case 2:
			Get_Laser_F(fpga_H);													//获取激光器频率
			laser_montor++;
			break;
		default:
			laser_montor = 0;
			bNeedRead = FALSE;
			break;
		}

		unsigned char rBuffer[32];
		DWORD rnum = 0;

		if (bNeedRead && ReadFile(fpga_H, rBuffer, sizeof(rBuffer), &rnum, NULL)) {

			if (rBuffer[0] == 0x5a && rBuffer[1] == 0xa5)	//探测器温度
			{
				UINT Tem = (rBuffer[2] & 0x0f) * 256 + rBuffer[3];
				SetDlgItemInt(IDC_EDIT19, Tem);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x54)			//激光器温度信息
			{
				CString cs;
				int i;
				for (i = 5; i < 17; i++)
					cs.Format(_T("%c"), rBuffer[i]);

				SetDlgItemText(IDC_EDIT10, cs);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x4c)			//激光器电流信息
			{
				CString cs;
				int i;
				for (i = 5; i < 22; i++)
					cs.Format(_T("%c"), rBuffer[i]);

				SetDlgItemText(IDC_EDIT11, cs);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x46)			//激光器频率信息
			{
				CString cs;
				int i;
				for (i = 5; i < 23; i++)
					cs.Format(_T("%c"), rBuffer[i]);

				SetDlgItemText(IDC_EDIT12, cs);
				memset(rBuffer, 0x00, 27);
			}

		}


	}

		

	if (TIMER_battery == nIDEvent)
	{
		Get_dump_energy(battery_H);
	}
	if (TIMER_xmotor == nIDEvent)
	{
		long xp = 0;
		Get_Position(xmotor_H, &xp);								//获取电机位置
		SetDlgItemInt(IDC_EDIT6, xp);
	}
	if (TIMER_ymotor == nIDEvent)
	{
		long yv = 0;
		Get_Vel(ymotor_H, &yv);								//获取电机位置
		SetDlgItemInt(IDC_EDIT29, yv);
	}

	CDialogEx::OnTimer(nIDEvent);
	

}

//电机旋转
void CscanerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	Vel_mode(xmotor_H,GetDlgItemInt(IDC_EDIT1),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3));
}

//停止旋转
void CscanerDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	Stop_Vel(xmotor_H);
}

//电机定位
void CscanerDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	Position_mode(xmotor_H,GetDlgItemInt(IDC_EDIT1),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3),GetDlgItemInt(IDC_EDIT23));
}

//停止定位
void CscanerDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	Stop_Pos(xmotor_H);
}

//电机归零
void CscanerDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSpeed = GetDlgItemInt( IDC_EDIT4 );
	int nAcc = GetDlgItemInt( IDC_EDIT5 );

	BOOL bSuccess = Homing_xmotor(xmotor_H, nAcc , nSpeed );
	if( !bSuccess )
	{
		AfxMessageBox(  _T("x home failed...") );
	}
}

//停止归零
void CscanerDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	Homing_stop(xmotor_H);
}


void _CalculateSpeed( WORD dwTcicks , DWORD dwSumBytes , CString& sResult )
{
	DWORD dwKBytesPerMs =  dwSumBytes / dwTcicks;
	//sResult.Format( _T("%d") , (int)dwKBytesPerMs );
	sResult.Format( _T("%d.%03d mb/s") , (int)(dwKBytesPerMs / 1000) , (int)(dwKBytesPerMs % 1000) );
}
// USb数据接收
unsigned __stdcall CscanerDlg::ReadUsbThread( void* pArgs )
{
	MSG msg;
	BOOL bQuit = FALSE;
	char* pBuffer = new char[BufferSize];
	CFT2232Module* pModule = &(((CscanerDlg*)pArgs)->m_2232);
	CEdit* pEdit = (CEdit*)( ((CscanerDlg*)pArgs)->GetDlgItem(IDC_EDIT_RECV) );

	pEdit->SetWindowText( _T("线程开始运行") );

	while ( !bQuit&&GetMessage( &msg , NULL,0,0 ) )
	{
		if( MsgQuitThread == msg.message )
			break;
		if( MsgStartRead != msg.message  )
			continue;

		pEdit->SetWindowText( _T("开始任务") );
		CString *pStr = (CString*)msg.wParam;

		pEdit->SetWindowText( _T("正在打开文件。。。。") );
		/*打开文件*/
		FILE* p = _tfopen( (LPCTSTR)(*pStr) , _T("w+b") );
		delete pStr;
		if( !p )
		{
			pEdit->SetWindowText( _T("文件打开失败！")  );
			continue;
		}

		pEdit->SetWindowText( _T("正在打开ft2232.。。。") );
		if( !pModule->Open(0 , pEdit ) )
		{
			pEdit->SetWindowText(  _T("usb open error！") );
			fclose( p );
			continue;
		}
		pEdit->SetWindowText( _T("打开ft2232成功") );

		DWORD dwSum = 0;
		DWORD dwTick = GetTickCount() + 500;

		while(1)
		{
			if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
			{
				if( MsgQuitThread == msg.message ){bQuit = TRUE;break;}
				if( MsgStopRead == msg.message ){break;}
				if( MsgStartRead == msg.message ){CString* pstr=(CString*)msg.wParam;delete pstr;}
			}

			/*写入文件*/
			DWORD readSize;
			size_t writeSize;
			if( pModule->Read(  pBuffer , BufferSize , &readSize  ) )
			{
				writeSize = fwrite( pBuffer , 1 , readSize , p );
				dwSum += readSize;
			}
			
			DWORD dwTicksNow = GetTickCount();
			if( dwTicksNow > dwTick )/*到达时间点*/
			{
				CString str;
				_CalculateSpeed( dwTicksNow - dwTick + 500/*将500毫秒的时间差加上去*/, dwSum , str );
				pEdit->SetWindowText( (LPCTSTR)str );
				/*500毫秒之后重新计算*/
				dwTick = dwTicksNow + 500;
				dwSum = 0;
			}
		}

		pEdit->SetWindowText( _T("任务结束！") );
		pModule->Close();
		/*关闭文件*/
		fclose( p );	
	}

	delete[] pBuffer;
	return 0;
}

void CscanerDlg::QuitReadThread()
{
	PostThreadMessage( m_dwThreadID , MsgQuitThread , NULL , NULL );
	WaitForSingleObject( m_hThread , INFINITE );
	CloseHandle( m_hThread );
}

void CscanerDlg::StartUsbRead( LPCTSTR lpszFileName )
{
	BOOL b;
	CString *pStr = new CString;
	*pStr = lpszFileName;

	b = PostThreadMessage( m_dwThreadID , MsgStartRead ,  (WPARAM)pStr , NULL );
	if( !b )
	{
		CString str;
		str.Format( _T("error code:%d") , GetLastError() );
		AfxMessageBox( str );
		delete pStr;
	}
}

void CscanerDlg::EndUsbRead()
{
	BOOL b = PostThreadMessage( m_dwThreadID , MsgStopRead , NULL , NULL );
	if( !b )
	{
		CString str;
		str.Format( _T("error code:%d") , GetLastError() );
			AfxMessageBox( str );
	}
}

void CscanerDlg::OnDestroy()
{
	OnBnClickedButtonStopGps();

	CDialogEx::OnDestroy();
	QuitReadThread();
}

void CscanerDlg::OnBnClickedButton21()
{
	CString sName;
	GetDlgItemText( IDC_EDIT_FILE_PATH , sName );
	
	if( sName.IsEmpty() )
	{
		AfxMessageBox( _T("文件名为空") );
		return;
	}

	StartUsbRead( (LPCTSTR)sName );
}

void CscanerDlg::OnBnClickedButton22()
{
	EndUsbRead();
}

void CscanerDlg::OnBnClickedButtonFile()
{
	CFileDialog dlg(FALSE,
		_T("bin3d"),
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Source Data Files (*.bin3d)|*.bin3d||"));
	if( IDOK == dlg.DoModal() )
	{
		CString strName = dlg.GetPathName();
		SetDlgItemText( IDC_EDIT_FILE_PATH , strName );
	}
}

//发送接收GNSS数据的指令
void CscanerDlg::OnBnClickedButton23()
{
	// TODO: 在此添加控件通知处理程序代码
	_GNSS(fpga_H);
}

//FPGA串口打开
void CscanerDlg::OnBnClickedButton31()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !fpga_H )
	{
		CString comstr;
		Set_fpga_brate(GetDlgItemInt(IDC_EDIT30));
		GetDlgItemText(IDC_EDIT24,comstr);//fpga
		fpga_H = Init_fpga_rs232(comstr);
		if(fpga_H == 0)
		{
			AfxMessageBox(_T("FPGA COM口打开失败！"));
			return;
		}

		
		/*DWORD dwThreadID_fpga;
		pThread_fpga = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)fpga_ComRcvTread, this, 0, &dwThreadID_fpga);
		if(!pThread_fpga)
		{
			AfxMessageBox(_T("FPGA接收线程创建失败！"));
			CloseHandle( fpga_H );
			//CloseHandle( m_hLaser );
			//m_hLaser = NULL;
			fpga_H = NULL;
			return;
		}	*/
			
		GetDlgItem(IDC_BUTTON31)->EnableWindow(FALSE);
	}
}

void CscanerDlg::OnBnClickedButton32()
{
	// TODO: 在此添加控件通知处理程序代码
	if( fpga_H )
	{
		stop_thread_fpga =true;
		Sleep(100);
		
		KillTimer(TIMER_laser);
		OnBnClickedButton8();		//关激光
		CloseHandle(fpga_H);
		GetDlgItem(IDC_BUTTON31)->EnableWindow(TRUE);
		pThread_fpga = NULL;
		fpga_H = NULL;
		m_bAskForLaserInfo = FALSE;
		GetDlgItem(IDC_BUTTON_GET_INFO)->SetWindowText( _T("获取激光器信息") );
	}
		
}

void CscanerDlg::OnBnClickedButton25()
{
	// TODO: 在此添加控件通知处理程序代码
	Vel_mode(ymotor_H,GetDlgItemInt(IDC_EDIT14),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3));
}

void CscanerDlg::OnBnClickedButton26()
{
	// TODO: 在此添加控件通知处理程序代码
	Stop_Vel(ymotor_H);
}

void CscanerDlg::OnBnClickedButton27()
{
	// TODO: 在此添加控件通知处理程序代码
	Homing_ymotor(ymotor_H,GetDlgItemInt(IDC_EDIT5),GetDlgItemInt(IDC_EDIT4));
}

void CscanerDlg::OnBnClickedButton28()
{
	// TODO: 在此添加控件通知处理程序代码
	Homing_stop(ymotor_H);
}


//void CscanerDlg::OnBnClickedButton33()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	char pro_data[3]={0xff,0xa5,0xa5};
//	if(SDataCom_fpga(fpga_H,(UCHAR*)pro_data,3) == -1);
//	
//}


void CscanerDlg::OnBnClickedButton38()
{
	// TODO: 在此添加控件通知处理程序代码
	CString comstr;
	
	GetDlgItemText(IDC_EDIT27,comstr);//倾角仪
	sensor_H = Init_inclinometer_rs232(comstr);
	if(sensor_H == 0)
		AfxMessageBox(_T("倾角仪 COM口打开失败！"));
	DWORD dwThreadID_sensor;
	pThread_sensor = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)sensor_ComRcvTread, this, 0, &dwThreadID_sensor);
	if(!pThread_sensor)
		AfxMessageBox(_T("倾角仪接收线程创建失败！"));
	GetDlgItem(IDC_BUTTON38)->EnableWindow(FALSE);	
}


void CscanerDlg::OnBnClickedButton39()
{
	// TODO: 在此添加控件通知处理程序代码
	stop_thread_sensor =true;
	Sleep(100);
	if(sensor_H)
		CloseHandle(sensor_H);
	if(pThread_sensor)
		CloseHandle(pThread_sensor);
	GetDlgItem(IDC_BUTTON38)->EnableWindow(TRUE);	
}


void CscanerDlg::OnBnClickedButton40()
{
	// TODO: 在此添加控件通知处理程序代码
	CString comstr;
	GetDlgItemText(IDC_EDIT28,comstr);//电池
	battery_H = Init_battery_rs232(comstr);
	if(battery_H == 0)
		AfxMessageBox(_T("电池 COM口打开失败！"));
	DWORD dwThreadID_battery;
	pThread_battery = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)battery_ComRcvTread, this, 0, &dwThreadID_battery);
	if(!pThread_battery)
		AfxMessageBox(_T("电池接收线程创建失败！"));
	SetTimer( TIMER_battery, 6000, NULL);		//6s
	GetDlgItem(IDC_BUTTON40)->EnableWindow(FALSE);
}


void CscanerDlg::OnBnClickedButton41()
{
	// TODO: 在此添加控件通知处理程序代码
	stop_thread_battery =true;
	Sleep(100);
	if(battery_H)
		KillTimer(TIMER_battery);
	if(battery_H)
		CloseHandle(battery_H);
	if(pThread_battery)
		CloseHandle(pThread_battery);
	GetDlgItem(IDC_BUTTON40)->EnableWindow(TRUE);
}


void CscanerDlg::OnBnClickedButton34()	//x电机待修改
{
	// TODO: 在此添加控件通知处理程序代码
	if(!xmotor_H)
	{
		char buf[32];
		if( !_GetMultByteStringFromCtrl(buf,32,IDC_EDIT25 ) )
		{
			AfxMessageBox( _T("连接断口为空！") );
			return;
		}
		

		xmotor_H = Open_motor( buf );
		if(!xmotor_H)
		{
			AfxMessageBox(_T("x电机打开失败！"));
		}
		else
		{
			SetTimer( TIMER_xmotor, 1000, NULL);		//1s
			GetDlgItem(IDC_BUTTON34)->EnableWindow(FALSE);	
		}
			
	}
	
}


void CscanerDlg::OnBnClickedButton35()
{
	if( xmotor_H )
	{
		if( Close_motor(xmotor_H) )
		{
			xmotor_H = NULL;
			KillTimer(TIMER_xmotor);
			GetDlgItem(IDC_BUTTON34)->EnableWindow(TRUE);	
		}
	}
}


void CscanerDlg::OnBnClickedButton36()//y电机待修改
{
	if(!ymotor_H)
	{	
		char chBuffer[32];
		if( !_GetMultByteStringFromCtrl(chBuffer,32,IDC_EDIT26) )
		{
			AfxMessageBox(_T("连接端口号为空"));
			return;
		}
		
		
		ymotor_H = Open_motor( chBuffer );
		if(!ymotor_H)
			AfxMessageBox(_T("y电机打开失败！"));
		else
		{
			SetTimer( TIMER_ymotor, 1000, NULL);
			GetDlgItem(IDC_BUTTON36)->EnableWindow(FALSE);
		}
				//1s
	}
		
}

void CscanerDlg::OnBnClickedButton37()
{
	if( ymotor_H )
	{
		if( Close_motor(ymotor_H))
		{
			ymotor_H = NULL;
			KillTimer(TIMER_ymotor);
			GetDlgItem(IDC_BUTTON36)->EnableWindow(TRUE);
		}
	}
}

BOOL CscanerDlg::_GetMultByteStringFromCtrl( char* pBuffer , DWORD dwBufSize , UINT uID )
{

#ifdef UNICODE
	TCHAR tBuffer[16] = {_T('\0')};
	if( !GetDlgItemText( uID , tBuffer , 16 ) )
		return FALSE;
	return 
		0 != WideCharToMultiByte(CP_ACP,0,tBuffer,-1,pBuffer,dwBufSize,NULL,NULL) ;
#else

	return !GetDlgItemText( uID , pBuffer , dwBufSize );
#endif
}




void CscanerDlg::OnBnClickedButtonGetInfo()
{
	
	if (!fpga_H)
	{
		AfxMessageBox(_T("fpga没有打开"));
		return;
	}
	
	/*CWnd* pWnd = GetDlgItem( IDC_BUTTON_GET_INFO );
	if( !m_bAskForLaserInfo )
	{
		SetTimer( TIMER_laser , 1000 , NULL );
		pWnd->SetWindowText( _T("停止获取") );
	}
	else
	{
		pWnd->SetWindowText( _T("获取激光器信息") );
		KillTimer( TIMER_laser );
	}

	m_bAskForLaserInfo = !m_bAskForLaserInfo;*/
}


void CscanerDlg::OnBnClickedButtonPosy()
{
	if( ymotor_H )
	{
		UINT uSpeed = GetDlgItemInt(  IDC_EDIT14 );
		UINT uAcc = GetDlgItemInt( IDC_EDIT2 );
		UINT uDec = GetDlgItemInt( IDC_EDIT3 );
		if( !uSpeed || !uAcc || !uDec  )/*must not zero!!!*/
			return;

		CString sPos;
		GetDlgItemText( IDC_EDIT_YPOS , sPos );
		if( sPos.IsEmpty() )
			return;

		long lPos = _ttol( (LPCTSTR)sPos  );
		Position_mode( ymotor_H , (DWORD)uSpeed , (DWORD)uAcc , (DWORD)uDec , lPos );
	}
}


int nLineLimit = 0;
void CscanerDlg::OnBnClickedButtonBeginGps()
{

	if( (NULL == fpga_H) || (INVALID_HANDLE_VALUE == fpga_H) || NULL != m_hThreadGPS )
		return;

	if(  StartGNSS(fpga_H) )
	{
		SetGPSThreadRun( TRUE );
		m_hThreadGPS = (HANDLE)_beginthreadex( NULL,0, ReadGPSThread , this , 0 , NULL );

		if( NULL == m_hThreadGPS )
		{
			SetGPSThreadRun( FALSE );
			StopGNSS( fpga_H );
		}

	}
}


void CscanerDlg::OnBnClickedButtonClearText()
{
	 SetDlgItemText( IDC_EDIT_GPS , _T("") );
}


int LowChar( char c )
{
	static int table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	return table[ c & 0x0f ];
}

int HighChar( char c )
{
	return LowChar( c >> 4 );
}

unsigned __stdcall CscanerDlg::ReadGPSThread( void* pArgs )
{
	CscanerDlg* pDlg = (CscanerDlg*)pArgs;
	HWND hEdit = (HWND)( pDlg->GetDlgItem( IDC_EDIT_GPS )->GetSafeHwnd() );
	CEdit* pEdit = (CEdit*)pDlg->GetDlgItem( IDC_EDIT_GPS );

	char chBuffer[32];
	TCHAR tchBuffer[256];
	DWORD dwReadSize;

	FILE* pFile = _tfopen( _T("test.gps") , _T("w+b") ); 

	if( !pFile )return 0;

	while( 1 )
	{
		if( !ReadFile( pDlg->fpga_H , chBuffer , sizeof(chBuffer) , &dwReadSize , NULL ) )
			continue;

		if( 0 == dwReadSize )
			continue;

		LPTSTR p = tchBuffer;
		for( DWORD i = 0 ; i < dwReadSize  ; i++ )
		{
			*p++ = (TCHAR)HighChar(chBuffer[i]);
			*p++ = (TCHAR)LowChar(chBuffer[i]);
			*p++ = _T(' ');
		}
		*p = _T('\0'); 


		fwrite( chBuffer , 1 , dwReadSize , pFile );


		if( !pDlg->GetGPSThreadRun() )
			break;

		int nTextLenth = pEdit->GetWindowTextLength();

		if( nTextLenth > 8192 )
		{
			pEdit->SetWindowText( _T("") );
			nTextLenth = 0;
		}

		pEdit->SetSel(  nTextLenth ,  nTextLenth );
		pEdit->ReplaceSel( tchBuffer );

	}

	fclose( pFile );

	return 0;
}

void CscanerDlg::OnBnClickedButtonStopGps()
{
	if( (NULL == fpga_H) || (INVALID_HANDLE_VALUE == fpga_H) || NULL == m_hThreadGPS )
		return;

	SetGPSThreadRun( FALSE );

	WaitForSingleObject(m_hThreadGPS, 20000  );

	StopGNSS( fpga_H );

	CloseHandle( m_hThreadGPS );

	m_hThreadGPS = NULL;
}

void CscanerDlg::OnBnClickedButtonEdttTest()
{
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_GPS );

	int nTextLenth = pEdit->GetWindowTextLength();

	pEdit->SetSel(  nTextLenth ,  nTextLenth );
	pEdit->SetFocus();
}

void CscanerDlg::OnBnClickedCheckWarm()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_WARM);
	if (NULL == fpga_H)
	{
		if (BST_CHECKED == pButton->GetCheck())
			pButton->SetCheck(BST_UNCHECKED);
		return;
	}

	switch (pButton->GetCheck())
	{
	case BST_CHECKED:
		Warm_ON(fpga_H);
		break;

	case BST_UNCHECKED:
		Warm_OFF(fpga_H);
		break;
	}
	OnBnClickedButton9();

}

void CscanerDlg::OnEnChangeEdit7()
{
	_CalculateWarmFactor();
}

void CscanerDlg::_CalculateWarmFactor()
{
	UINT uVODA = GetDlgItemInt( IDC_EDIT7 );/*灵敏度*/
	UINT uV2AD = GetDlgItemInt( IDC_EDIT_WARM_V2AD );
	UINT uN = GetDlgItemInt( IDC_EDIT_WARM_N );

	UINT uConstant = 0;/*温补常量*/
	UINT uFactor = 0;/*温补系数*/

	/*计算过程begin*/
	UINT M = uN * uV2AD / 16 + uVODA;
	/*end*/

	uFactor = uN;
	uConstant = M;

	SetDlgItemInt( IDC_EDIT20 , uConstant );
	SetDlgItemInt( IDC_EDIT21 , uFactor );
}

void CscanerDlg::OnEnChangeEditWarmN()
{
	_CalculateWarmFactor();
}

void CscanerDlg::OnEnChangeEditWarmV2ad()
{
	_CalculateWarmFactor();
}

void CscanerDlg::OnBnClickedButtonOpenLaser()
{
// 
// 	if( m_hLaser )
// 	{
// 		AfxMessageBox( _T("激光器已经打开") );
// 		return;
// 	}
// 
// 	CString sCom;
// 	GetDlgItemText( IDC_EDIT_LASER_PORT , sCom );
// 
// 
// 	m_hLaser = Init_laser_rs232( sCom );
// 	if( !m_hLaser )
// 	{
// 		CloseHandle( fpga_H );
// 		fpga_H = NULL;
// 		AfxMessageBox( _T("Laser 激光器串口打开失败") );
// 		return; 
// 	}
// 
// 	GetDlgItem( IDC_BUTTON_CLOSE_LASER )->EnableWindow( TRUE );
// 	GetDlgItem( IDC_BUTTON_OPEN_LASER )->EnableWindow( FALSE );
}

void CscanerDlg::OnBnClickedButtonCloseLaser()
{
// 	if( !m_hLaser )
// 	{
// 		AfxMessageBox( _T("激光器没有打开") );
// 		return;
// 	}

//	CloseHandle( m_hLaser );
//	m_hLaser = NULL;
	
	//GetDlgItem(IDC_BUTTON_OPEN_LASER)->EnableWindow( TRUE );
	//GetDlgItem(IDC_BUTTON_CLOSE_LASER)->EnableWindow( FALSE );
}

void CscanerDlg::_ReportError( LPCTSTR lpszErrorMsg )
{
	TCHAR chBuffer[MAX_PATH];
	_stprintf( chBuffer , _T("%s :Error Code:%u") , lpszErrorMsg , GetLastError() );
	AfxMessageBox( chBuffer );
}

//整形阈值1
void CscanerDlg::OnBnClickedButton42()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpga没有打开"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT36);
	if (!Set_PulseVth1(fpga_H, u))
		_ReportError(_T("整形阈值1设置失败"));	
}


void CscanerDlg::OnBnClickedButton43()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpga没有打开"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT37);
	if (!Set_PulseVth2(fpga_H, u))
		_ReportError(_T("整形阈值2设置失败"));
}




void CscanerDlg::OnBnClickedButton30()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpga没有打开"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT35);
	if (!Set_PulseWidth(fpga_H, u))
		_ReportError(_T("整形脉宽设置失败"));

}
