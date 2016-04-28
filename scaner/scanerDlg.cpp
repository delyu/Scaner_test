
// scanerDlg.cpp : ʵ���ļ�
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

//ȫ��
BOOL stop_thread_fpga;							//���Կ���fpga�߳̽���
BOOL stop_thread_battery;						//���Կ��Ƶ���߳̽���
BOOL stop_thread_sensor;						//���Կ���������߳̽���
UINT fpga_ComRcvTread(LPVOID pParm);			//fpga���ڽ����߳�
UINT laser_ComRcvTread(LPVOID pParm);			//laser�����߳�
UINT sensor_ComRcvTread(LPVOID pParm);			//sensor�����߳�
UINT battery_ComRcvTread(LPVOID pParm);			//battery�����߳�

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CscanerDlg �Ի���




CscanerDlg::CscanerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CscanerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	laser_montor = 0;
	battery_montor = 0x0d;
	//ȫ��
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


// CscanerDlg ��Ϣ�������

BOOL CscanerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetDlgItemInt(IDC_EDIT1,1500);	//x����ٶ�
	SetDlgItemInt(IDC_EDIT14,1500);	//y����ٶ�
	SetDlgItemInt(IDC_EDIT2,500);
	SetDlgItemInt(IDC_EDIT3,500);
	SetDlgItemInt(IDC_EDIT4,2000);	//�����ٶ�
	SetDlgItemInt(IDC_EDIT5,500);	//������ٶ�
	SetDlgItemInt(IDC_EDIT7,2600);			//APD����
	SetDlgItemInt(IDC_EDIT20,5350);			//�²�����
	SetDlgItemInt(IDC_EDIT21,22);			//�²�ϵ��
	//SetDlgItemInt(IDC_EDIT22,20);			//AD��ʱ
	SetDlgItemInt(IDC_EDIT30,115200);			//FPGA������										
	
	SetDlgItemInt(IDC_EDIT35, 4000);//���� �����ѹ
	SetDlgItemInt(IDC_EDIT36, 149);//������ֵ��ѹ1
	SetDlgItemInt(IDC_EDIT37, 240);//������ֵ��ѹ2
	//���ں�
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
		AfxMessageBox( _T("usb ��ȡ�߳̿���ʧ��") );
		return FALSE;
	}
	m_dwThreadID = (DWORD)uiAddr;



	InitializeCriticalSection( &m_GpsCS );
		

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CscanerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CscanerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//����
void CscanerDlg::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
/*	UINT u = GetDlgItemInt(IDC_EDIT35);
	Set_PulseWidth(nullptr, u);*/
}

void CscanerDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

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

//fpga_com�����߳�
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

		if(rBuffer[0] == 0x5a && rBuffer[1] == 0xa5)	//̽�����¶�
		{
			UINT Tem = (rBuffer[2] & 0x0f) * 256 + rBuffer[3];
			dlg->SetDlgItemInt(IDC_EDIT19,Tem);
			memset(rBuffer,0x00,27);
		}
		if(rBuffer[5] == 0x54)			//�������¶���Ϣ
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
		if(rBuffer[5] == 0x4c)			//������������Ϣ
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
		if(rBuffer[5] == 0x46)			//������Ƶ����Ϣ
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

	/*��������*/
	*p++ = HIGH_4Bit(pBuffer[1]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[1]) + _T('0');
	*p++ = _T('.');

	/*С������*/
	*p++ = HIGH_4Bit(pBuffer[2]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[2]) + _T('0');
	*p++ = HIGH_4Bit(pBuffer[3]) + _T('0');
	*p++ = LOW_4Bit(pBuffer[3]) + _T('0');

	/*�ַ�������*/
	*p = _T('\0');

#undef LOW_4Bit
#undef HIGH_4Bit
}

//sensor_com�����߳�
UINT sensor_ComRcvTread(LPVOID pParm)
{
	CscanerDlg* dlg = (CscanerDlg*)pParm;
	UCHAR rBuffer[18] = {0x00};
	DWORD rnum = 0;

	/*�ַ���������*/
	TCHAR chBuffer[16];

	while( !stop_thread_sensor )
	{
		Sleep(100);/*����100����*/

		if( !ReadFile(dlg->sensor_H, rBuffer, 17 , &rnum , NULL ) )
			continue;

		if(0x68 == rBuffer[0]  && 0x0c == rBuffer[1])						//���յ���Ч����
		{
			BCD2String( rBuffer + 4 , chBuffer );
			dlg->SetDlgItemText(IDC_EDIT17,chBuffer);

			BCD2String( rBuffer + 8 , chBuffer );
			dlg->SetDlgItemText(IDC_EDIT18,chBuffer);
		}
	}

	return 1;
}

//battery_com�����߳�
UINT battery_ComRcvTread(LPVOID pParm)
{
	CscanerDlg* dlg = (CscanerDlg*)pParm;
	UCHAR rBuffer[4] = {0x00};
	int rnum = 0;
	while(1)
	{
		Sleep(1);
		if(stop_thread_battery)					//�ж��Ƿ�Ҫ��������
			return 1;
		else
			rnum = RDataCom_batteryr(dlg->battery_H, rBuffer, 4);
		if(rBuffer[0] == 0x01 && rBuffer[1] == 0x0a)		//ָ��ͳɹ�
		{
			Send_read_order(dlg->battery_H);				//���Ͷ�ȡָ��
			memset(rBuffer,0x00,4);
		}
/*		if(rBuffer[1] == 0x0a && rBuffer[0] != 0x01)		//ָ��δ���ͳɹ�����������
		{
			Get_dump_energy(dlg->battery_H);
			memset(rBuffer,0x00,4);
		}*/

		if(rBuffer[2] == 0x01 && rBuffer[3] == 0x0a)								//��ȡ������
		{
			UINT num = rBuffer[1]*256+rBuffer[0];
			if(num >= 100)
				num = 100;
			dlg->SetDlgItemInt(IDC_EDIT16,num);
			memset(rBuffer,0x00,4);
		}
	}
}

//������
void CscanerDlg::OnBnClickedButton7()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}
	if( !Laser_ON(fpga_H))
		_ReportError( _T("laser on error!!!") );
}

//�ؼ���
void CscanerDlg::OnBnClickedButton8()
{
	
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}
	if( !Laser_OFF(fpga_H))
		_ReportError( _T("Laser off error!!") );
}

//����������
void CscanerDlg::OnBnClickedButton9()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
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
	Set_Prober_Gain(fpga_H,GetDlgItemInt(IDC_EDIT7));									//����̽��������
}

//������ֵ��ѹ
void CscanerDlg::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}

	UINT u = GetDlgItemInt(IDC_EDIT8);
	if( !Set_Vth(fpga_H, u) )
		_ReportError( _T("set vth error!") );

}

//AD��ʱʱ��
void CscanerDlg::OnBnClickedButton18()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}
	
	UINT u =GetDlgItemInt(IDC_EDIT22);
	if( !Set_AD_ns(fpga_H,u) )
		_ReportError( _T("AD��ʱ����ʧ��") );
}

//��ȡ̽�����¶�
void CscanerDlg::OnBnClickedButton13()
{
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}
	unsigned char chRecvBuffer[72];
	if (!Get_Prober_Tem(fpga_H, chRecvBuffer))												//��ȡ̽�����¶�
		_ReportError(_T("��ȡ�¶Ȳ���ʧ��"));

	
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
				str += "APD�¶�: ";
					str += tem;
					str += "\r\n";
					break;
			case 1:
				str += "��ѹ��ѹ: ";
				str += tem;
				str += "\r\n"; 		
				break;
			case 2:
				str += "��ص�ѹ: ";
				str += tem;
				str += "\r\n";
				break;
			case 3:
				str += "��Դ�л���ѹ: ";
				str += tem;
				str += "\r\n";
				break;
			case 4:
				str += "������Ƶ: ";
				str += tem;
				str += "\r\n";
				break;
			case 5:
				str += "���⿪��: ";
				if (tem == "2")
					str += "��";
				else
				{
					str += "��";
				}
				str += "\r\n";
				break;
			case 6:
				str += "��������: ";
				str += tem;
				str += "\r\n";
				break;
			case 7:
				str += "������: ";
				str += tem;
				str += "\r\n";
				break;
			case 8:
				str += "������ֵ1: ";
				str += tem;
				str += "\r\n";
				break;
			case 9:
				str += "������ֵ2: ";
				str += tem;
				str += "\r\n";
				break;
			case 10:
				str += "�²�����M: ";
				str += tem;
				str += "\r\n";
				break;
			case 11:
				str += "�²�����N: ";
				str += tem;
				str += "\r\n";
				break;
			case 12:
				str += "�²�����: ";
				if (tem == "10")
					str += "��";
				else
				{
					str += "��";
				}
				str += "\r\n";
				break;
			case 13:
				str += "TDC��ʱ: ";
				str += tem;
				str += "\r\n";
				break;
			case 14:
				str += "GPS����: ";
				if (tem == "13")
					str += "��";
				else
				{
					str += "��";
				}
				str += "\r\n";
				break;
			case 15:
				str += "��Ǵ���������: ";
				if(tem=="16")
				str += "��";
				else
				{
					str += "��";
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


//�򿪽ڵ翪��
void CscanerDlg::OnBnClickedButton19()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}

	if( !JD_Switch_ON(fpga_H))												//�򿪽ڵ翪��
		_ReportError( _T("�ڵ翪�ش�ʧ��") );
}

//�رսڵ翪��
void CscanerDlg::OnBnClickedButton20()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( !fpga_H )
	{
		AfxMessageBox( _T("fpgaû�д�") );
		return;
	}


	if( !JD_Switch_OFF(fpga_H) )												//�رսڵ翪��
		_ReportError( _T("�ڵ翪�عر�ʧ��") );
}

//���ü���������
void CscanerDlg::OnBnClickedButton11()
{

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpgaû�д�"));
		return;
	}

	UINT uCurrent = GetDlgItemInt(IDC_EDIT9);
	if( !Set_Laser_Currents( fpga_H , uCurrent ) )/*20141103*/
		_ReportError( _T("��������ʧ��") );
		
}

//״̬��ȡ
void CscanerDlg::OnTimer(UINT_PTR nIDEvent)
{
	BOOL bNeedRead = TRUE;

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (TIMER_laser == nIDEvent)
	{/*20141103*/
		switch (laser_montor)
		{
		case 0:
			Get_Laser_Tem(fpga_H);													//��ȡ�������¶�
			laser_montor++;
			break;
		case 1:
			Get_Laser_I(fpga_H);													//��ȡ����������
			laser_montor++;
			break;
		case 2:
			Get_Laser_F(fpga_H);													//��ȡ������Ƶ��
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

			if (rBuffer[0] == 0x5a && rBuffer[1] == 0xa5)	//̽�����¶�
			{
				UINT Tem = (rBuffer[2] & 0x0f) * 256 + rBuffer[3];
				SetDlgItemInt(IDC_EDIT19, Tem);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x54)			//�������¶���Ϣ
			{
				CString cs;
				int i;
				for (i = 5; i < 17; i++)
					cs.Format(_T("%c"), rBuffer[i]);

				SetDlgItemText(IDC_EDIT10, cs);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x4c)			//������������Ϣ
			{
				CString cs;
				int i;
				for (i = 5; i < 22; i++)
					cs.Format(_T("%c"), rBuffer[i]);

				SetDlgItemText(IDC_EDIT11, cs);
				memset(rBuffer, 0x00, 27);
			}
			if (rBuffer[5] == 0x46)			//������Ƶ����Ϣ
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
		Get_Position(xmotor_H, &xp);								//��ȡ���λ��
		SetDlgItemInt(IDC_EDIT6, xp);
	}
	if (TIMER_ymotor == nIDEvent)
	{
		long yv = 0;
		Get_Vel(ymotor_H, &yv);								//��ȡ���λ��
		SetDlgItemInt(IDC_EDIT29, yv);
	}

	CDialogEx::OnTimer(nIDEvent);
	

}

//�����ת
void CscanerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Vel_mode(xmotor_H,GetDlgItemInt(IDC_EDIT1),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3));
}

//ֹͣ��ת
void CscanerDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Stop_Vel(xmotor_H);
}

//�����λ
void CscanerDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Position_mode(xmotor_H,GetDlgItemInt(IDC_EDIT1),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3),GetDlgItemInt(IDC_EDIT23));
}

//ֹͣ��λ
void CscanerDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Stop_Pos(xmotor_H);
}

//�������
void CscanerDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSpeed = GetDlgItemInt( IDC_EDIT4 );
	int nAcc = GetDlgItemInt( IDC_EDIT5 );

	BOOL bSuccess = Homing_xmotor(xmotor_H, nAcc , nSpeed );
	if( !bSuccess )
	{
		AfxMessageBox(  _T("x home failed...") );
	}
}

//ֹͣ����
void CscanerDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Homing_stop(xmotor_H);
}


void _CalculateSpeed( WORD dwTcicks , DWORD dwSumBytes , CString& sResult )
{
	DWORD dwKBytesPerMs =  dwSumBytes / dwTcicks;
	//sResult.Format( _T("%d") , (int)dwKBytesPerMs );
	sResult.Format( _T("%d.%03d mb/s") , (int)(dwKBytesPerMs / 1000) , (int)(dwKBytesPerMs % 1000) );
}
// USb���ݽ���
unsigned __stdcall CscanerDlg::ReadUsbThread( void* pArgs )
{
	MSG msg;
	BOOL bQuit = FALSE;
	char* pBuffer = new char[BufferSize];
	CFT2232Module* pModule = &(((CscanerDlg*)pArgs)->m_2232);
	CEdit* pEdit = (CEdit*)( ((CscanerDlg*)pArgs)->GetDlgItem(IDC_EDIT_RECV) );

	pEdit->SetWindowText( _T("�߳̿�ʼ����") );

	while ( !bQuit&&GetMessage( &msg , NULL,0,0 ) )
	{
		if( MsgQuitThread == msg.message )
			break;
		if( MsgStartRead != msg.message  )
			continue;

		pEdit->SetWindowText( _T("��ʼ����") );
		CString *pStr = (CString*)msg.wParam;

		pEdit->SetWindowText( _T("���ڴ��ļ���������") );
		/*���ļ�*/
		FILE* p = _tfopen( (LPCTSTR)(*pStr) , _T("w+b") );
		delete pStr;
		if( !p )
		{
			pEdit->SetWindowText( _T("�ļ���ʧ�ܣ�")  );
			continue;
		}

		pEdit->SetWindowText( _T("���ڴ�ft2232.������") );
		if( !pModule->Open(0 , pEdit ) )
		{
			pEdit->SetWindowText(  _T("usb open error��") );
			fclose( p );
			continue;
		}
		pEdit->SetWindowText( _T("��ft2232�ɹ�") );

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

			/*д���ļ�*/
			DWORD readSize;
			size_t writeSize;
			if( pModule->Read(  pBuffer , BufferSize , &readSize  ) )
			{
				writeSize = fwrite( pBuffer , 1 , readSize , p );
				dwSum += readSize;
			}
			
			DWORD dwTicksNow = GetTickCount();
			if( dwTicksNow > dwTick )/*����ʱ���*/
			{
				CString str;
				_CalculateSpeed( dwTicksNow - dwTick + 500/*��500�����ʱ������ȥ*/, dwSum , str );
				pEdit->SetWindowText( (LPCTSTR)str );
				/*500����֮�����¼���*/
				dwTick = dwTicksNow + 500;
				dwSum = 0;
			}
		}

		pEdit->SetWindowText( _T("���������") );
		pModule->Close();
		/*�ر��ļ�*/
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
		AfxMessageBox( _T("�ļ���Ϊ��") );
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

//���ͽ���GNSS���ݵ�ָ��
void CscanerDlg::OnBnClickedButton23()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_GNSS(fpga_H);
}

//FPGA���ڴ�
void CscanerDlg::OnBnClickedButton31()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( !fpga_H )
	{
		CString comstr;
		Set_fpga_brate(GetDlgItemInt(IDC_EDIT30));
		GetDlgItemText(IDC_EDIT24,comstr);//fpga
		fpga_H = Init_fpga_rs232(comstr);
		if(fpga_H == 0)
		{
			AfxMessageBox(_T("FPGA COM�ڴ�ʧ�ܣ�"));
			return;
		}

		
		/*DWORD dwThreadID_fpga;
		pThread_fpga = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)fpga_ComRcvTread, this, 0, &dwThreadID_fpga);
		if(!pThread_fpga)
		{
			AfxMessageBox(_T("FPGA�����̴߳���ʧ�ܣ�"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( fpga_H )
	{
		stop_thread_fpga =true;
		Sleep(100);
		
		KillTimer(TIMER_laser);
		OnBnClickedButton8();		//�ؼ���
		CloseHandle(fpga_H);
		GetDlgItem(IDC_BUTTON31)->EnableWindow(TRUE);
		pThread_fpga = NULL;
		fpga_H = NULL;
		m_bAskForLaserInfo = FALSE;
		GetDlgItem(IDC_BUTTON_GET_INFO)->SetWindowText( _T("��ȡ��������Ϣ") );
	}
		
}

void CscanerDlg::OnBnClickedButton25()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Vel_mode(ymotor_H,GetDlgItemInt(IDC_EDIT14),GetDlgItemInt(IDC_EDIT2),GetDlgItemInt(IDC_EDIT3));
}

void CscanerDlg::OnBnClickedButton26()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Stop_Vel(ymotor_H);
}

void CscanerDlg::OnBnClickedButton27()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Homing_ymotor(ymotor_H,GetDlgItemInt(IDC_EDIT5),GetDlgItemInt(IDC_EDIT4));
}

void CscanerDlg::OnBnClickedButton28()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Homing_stop(ymotor_H);
}


//void CscanerDlg::OnBnClickedButton33()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	char pro_data[3]={0xff,0xa5,0xa5};
//	if(SDataCom_fpga(fpga_H,(UCHAR*)pro_data,3) == -1);
//	
//}


void CscanerDlg::OnBnClickedButton38()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString comstr;
	
	GetDlgItemText(IDC_EDIT27,comstr);//�����
	sensor_H = Init_inclinometer_rs232(comstr);
	if(sensor_H == 0)
		AfxMessageBox(_T("����� COM�ڴ�ʧ�ܣ�"));
	DWORD dwThreadID_sensor;
	pThread_sensor = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)sensor_ComRcvTread, this, 0, &dwThreadID_sensor);
	if(!pThread_sensor)
		AfxMessageBox(_T("����ǽ����̴߳���ʧ�ܣ�"));
	GetDlgItem(IDC_BUTTON38)->EnableWindow(FALSE);	
}


void CscanerDlg::OnBnClickedButton39()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString comstr;
	GetDlgItemText(IDC_EDIT28,comstr);//���
	battery_H = Init_battery_rs232(comstr);
	if(battery_H == 0)
		AfxMessageBox(_T("��� COM�ڴ�ʧ�ܣ�"));
	DWORD dwThreadID_battery;
	pThread_battery = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)battery_ComRcvTread, this, 0, &dwThreadID_battery);
	if(!pThread_battery)
		AfxMessageBox(_T("��ؽ����̴߳���ʧ�ܣ�"));
	SetTimer( TIMER_battery, 6000, NULL);		//6s
	GetDlgItem(IDC_BUTTON40)->EnableWindow(FALSE);
}


void CscanerDlg::OnBnClickedButton41()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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


void CscanerDlg::OnBnClickedButton34()	//x������޸�
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!xmotor_H)
	{
		char buf[32];
		if( !_GetMultByteStringFromCtrl(buf,32,IDC_EDIT25 ) )
		{
			AfxMessageBox( _T("���ӶϿ�Ϊ�գ�") );
			return;
		}
		

		xmotor_H = Open_motor( buf );
		if(!xmotor_H)
		{
			AfxMessageBox(_T("x�����ʧ�ܣ�"));
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


void CscanerDlg::OnBnClickedButton36()//y������޸�
{
	if(!ymotor_H)
	{	
		char chBuffer[32];
		if( !_GetMultByteStringFromCtrl(chBuffer,32,IDC_EDIT26) )
		{
			AfxMessageBox(_T("���Ӷ˿ں�Ϊ��"));
			return;
		}
		
		
		ymotor_H = Open_motor( chBuffer );
		if(!ymotor_H)
			AfxMessageBox(_T("y�����ʧ�ܣ�"));
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
		AfxMessageBox(_T("fpgaû�д�"));
		return;
	}
	
	/*CWnd* pWnd = GetDlgItem( IDC_BUTTON_GET_INFO );
	if( !m_bAskForLaserInfo )
	{
		SetTimer( TIMER_laser , 1000 , NULL );
		pWnd->SetWindowText( _T("ֹͣ��ȡ") );
	}
	else
	{
		pWnd->SetWindowText( _T("��ȡ��������Ϣ") );
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
	UINT uVODA = GetDlgItemInt( IDC_EDIT7 );/*������*/
	UINT uV2AD = GetDlgItemInt( IDC_EDIT_WARM_V2AD );
	UINT uN = GetDlgItemInt( IDC_EDIT_WARM_N );

	UINT uConstant = 0;/*�²�����*/
	UINT uFactor = 0;/*�²�ϵ��*/

	/*�������begin*/
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
// 		AfxMessageBox( _T("�������Ѿ���") );
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
// 		AfxMessageBox( _T("Laser ���������ڴ�ʧ��") );
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
// 		AfxMessageBox( _T("������û�д�") );
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

//������ֵ1
void CscanerDlg::OnBnClickedButton42()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpgaû�д�"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT36);
	if (!Set_PulseVth1(fpga_H, u))
		_ReportError(_T("������ֵ1����ʧ��"));	
}


void CscanerDlg::OnBnClickedButton43()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpgaû�д�"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT37);
	if (!Set_PulseVth2(fpga_H, u))
		_ReportError(_T("������ֵ2����ʧ��"));
}




void CscanerDlg::OnBnClickedButton30()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!fpga_H)
	{
		AfxMessageBox(_T("fpgaû�д�"));
		return;
	}
	UINT u = GetDlgItemInt(IDC_EDIT35);
	if (!Set_PulseWidth(fpga_H, u))
		_ReportError(_T("������������ʧ��"));

}
