
// ActiveDefenseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"

#pragma comment(lib,"user32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SOFT_CAPTION L"SML主动防御"
// CActiveDefenseDlg 对话框

extern bool g_isProcessOver;//指示程序是否要退出
extern int g_ThreadNum;//记录线程个数
HANDLE g_ThreadHandle;

TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };

CActiveDefenseDlg::CActiveDefenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActiveDefenseDlg::IDD, pParent)
	, m_EDIT_STR(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActiveDefenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, btn_close);
	DDX_Control(pDX, IDC_MIN, btn_min);
	DDX_Text(pDX, IDC_EDIT1, m_EDIT_STR);
}

BEGIN_MESSAGE_MAP(CActiveDefenseDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CLOSE, &CActiveDefenseDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_MIN, &CActiveDefenseDlg::OnBnClickedMin)
	ON_BN_CLICKED(IDC_LOAD_DRV, &CActiveDefenseDlg::OnBnClickedLoadDrv)
	ON_BN_CLICKED(IDC_UNLOAD_DRV, &CActiveDefenseDlg::OnBnClickedUnloadDrv)
	//ON_BN_CLICKED(IDC_START_THREAD, &CActiveDefenseDlg::OnBnClickedStartThread)
	ON_MESSAGE(WM_SHOW_MSG, &CActiveDefenseDlg::OnShowMsg)    // OnCountMsg是自定义的消息处理函数，可以在这个函数里面进行自定义的消息处理代码
	ON_BN_CLICKED(IDC_PROCESS_PROTECT, &CActiveDefenseDlg::OnBnClickedProcessProtect)
	ON_BN_CLICKED(IDC_LOAD_FILTER, &CActiveDefenseDlg::OnBnClickedLoadFilter)
	ON_BN_CLICKED(IDC_PROCESS_FILTER, &CActiveDefenseDlg::OnBnClickedProcessFilter)
	ON_BN_CLICKED(IDC_DRIVER_FILTER, &CActiveDefenseDlg::OnBnClickedDriverFilter)
	ON_BN_CLICKED(IDC_HELP, &CActiveDefenseDlg::OnBnClickedHelp)
END_MESSAGE_MAP()


// CActiveDefenseDlg 消息处理程序
BOOL CActiveDefenseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	ControlInit();//初始化 窗体 控件 
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CActiveDefenseDlg::OnPaint()
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

HCURSOR CActiveDefenseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CActiveDefenseDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rctClient;
	GetClientRect(rctClient);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap m_bmp;
	m_bmp.LoadBitmap(IDB_BACK);
	BITMAP bitmap;
	m_bmp.GetBitmap(&bitmap);
	CBitmap   *pbmpOld = dcMem.SelectObject(&m_bmp);
	CString buff;
	pDC->StretchBlt(0, 0, rctClient.Width(), rctClient.Height(), &dcMem, 0, 0,
		bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);//绘制背景

	/*输出标题 文字*/
	CFont font;
	font.CreatePointFont(125, L"微软雅黑");

	CDC*pdc = GetDC();
	pdc->SetBkMode(TRANSPARENT);
	pdc->SelectObject(&font);
	pdc->SetTextColor(RGB(255, 255, 255));
	pdc->TextOutW(20, 7, SOFT_CAPTION);
	
	ReleaseDC(pdc);
	/**/
	return true;
}

LRESULT CActiveDefenseDlg::OnNcHitTest(CPoint point)
{
	UINT nRet = CDialog::OnNcHitTest(point);
	if (HTCLIENT == nRet)
	{
		ScreenToClient(&point);
	//	if (point.y<34 && point.x<415)
	//	{
			return HTCAPTION;
	//	}
	}
	return nRet;
}
// 刷新，并使文本框 滚动条 最底端显示
void CActiveDefenseDlg::EditBoxToBottom(void)
{
	UpdateData(false);
	CEdit* pedit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pedit->LineScroll(pedit->GetLineCount());
}
void CActiveDefenseDlg::ShowInText(CString *str)
{
	m_EDIT_STR += *str;
	m_EDIT_STR += "\r\n";
	EditBoxToBottom();
}
LRESULT CActiveDefenseDlg::OnShowMsg(WPARAM wParam, LPARAM lParam)
{
	ShowInText((CString *)wParam);
	return 0;
}

// 控件信息初始化
void CActiveDefenseDlg::ControlInit()
{
	CRect rect;
	GetWindowRect(&rect);
	btn_close.MoveWindow(rect.Width() - 43,3, 40, 29);//设置按钮位置和大小
	btn_close.Load(IDP_CLOSE, 45);//载入按钮图片

	btn_min.Load(IDP_MIN, 31);//载入按钮图片
	btn_min.MoveWindow(rect.Width() - 43 - 39, 3, 40, 29);//设置按钮位置和大小
}


void CActiveDefenseDlg::OnBnClickedClose()
{
	OnBnClickedUnloadDrv();
	//g_isProcessOver = 1;//关闭线程
	ExitProcess(0);
	//EndDialog(0);//关闭窗体
	/*if (IDYES == MessageBox(L"您真的要退出本程序吗？",L"系统提示", MB_ICONQUESTION | MB_YESNO))
	{
		EndDialog(0);//关闭窗体
	}*/
}


void CActiveDefenseDlg::OnBnClickedMin()
{
	ShowWindow(SW_SHOWMINIMIZED);//最小化
}

void GetAppPath(TCHAR *szCurFile, TCHAR *szSysName) //返回本地目录下sys路径
{
	GetModuleFileNameW(0, szCurFile, MAX_PATH);
	
	for (SIZE_T i = wcslen(szCurFile) - 1; i >= 0; i--)
	{
		if (szCurFile[i] == '\\')
		{
			szCurFile[i + 1] = '\0';
			break;
		}
	}
	wcscat(szCurFile, szSysName);
	
	//MessageBox(NULL,szCurFile,NULL,NULL);
}




void StartThread()
{
	//开启线程
	if (g_ThreadNum < 1)//线程数量小于一个
	{
		g_isProcessOver = 0;//线程开启标志位
		g_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadHandle, NULL, 0, NULL);
	}
	else
	{
		ShowInfoInDlg (L"只允许开启一个监控线程");
	}
	
}

void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");

	BOOLEAN bRet;
	bRet=operaType(szFullPath, szTitle, 0);
	if (!bRet)
	{
		return;
	}
		
	bRet = operaType(szFullPath, szTitle, 1);
	if (!bRet)
	{
		return;
	}

	//开启线程
	StartThread();
}


int SendDrvClose()//发送给设备CLOSE信号，激活线程，防止线程阻塞在内核层
{
	HANDLE device = NULL;
	ULONG ret_len;

	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"主线程：设备打开错误，线程退出\r\n----------------------------------------------");
		return -1;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x913), NULL, NULL, NULL, 0, &ret_len, 0))//发送
	{
		ShowInfoInDlg(L"主线程：向驱动发送消息失败");
	}

	CloseHandle(device);
	return 0;
}
void CActiveDefenseDlg::OnBnClickedUnloadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");
	g_isProcessOver = 1;//线程退出标志位
	SendDrvClose();
	
	operaType(szFullPath, szTitle, 2);
	//operaType(szFullPath, szTitle, 3);
	StopDriver(L"FsFilter");
}




BOOL CActiveDefenseDlg::PreTranslateMessage(MSG* pMsg)
{
	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else
	return CDialogEx::PreTranslateMessage(pMsg);
}


int StartProcessProtect()//向驱动发送消息开启进程保护
{
	int nReturn = 0;
	HANDLE device = NULL;
	ULONG ret_len;
	char Buffer[MAX_PATH];
	memset(Buffer, 0, MAX_PATH);

	//UNICODE 转ANSI
	USHORT *pShort = (USHORT *)AfxGetApp()->m_pszExeName;
	UCHAR *pChr = (UCHAR *)Buffer;
	while (*pShort != 0)
	{
		*pChr = *pShort;
		pChr++;
		pShort++;
	}
	strcat(Buffer, ".exe");
	if (strlen(Buffer) >= 14)//大于14个字节就截取
	{

		Buffer[14] = 0;
	}

	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"主线程：设备打开错误\r\n----------------------------------------------");
		return -1;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x914), Buffer, sizeof(Buffer), NULL, 0, &ret_len, 0))//发送
	{

		ShowInfoInDlg(L"主线程：向驱动发送消息失败");
		nReturn = -1;
	}
	if (ret_len == 0)
	{
		ShowInfoInDlg(L"进程保护 正确开启,可以尝试结束本进程==\r\n----------------------------------------------");
		nReturn = 0;
	}
	else
	{
		ShowInfoInDlg(L"进程保护 开启失败\r\n---------------------------------------------");
		nReturn = -1;
	}

	CloseHandle(device);
	return nReturn;
}
void CActiveDefenseDlg::OnBnClickedProcessProtect()
{
	CString caption;
	GetDlgItem(IDC_PROCESS_PROTECT)->GetWindowText(caption);//获取按钮名称
	if (caption == L"开启进程保护")
	{
		int nRet = StartProcessProtect();
		if (nRet == 0)//成功开启保护
		{
			GetDlgItem(IDC_PROCESS_PROTECT)->SetWindowText(L"进程保护中");
		}
	}
	else
	{
		ShowInfoInDlg(L"本进程 正在 被保护中==\r\n----------------------------------------------");
	}

}

void CreateProtectFile()
{
	DeleteFile(L"README.TXT");
	CFile file;
	char *szContent = "你并不能修改 it\n";
	if (file.Open(L"README.TXT", CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		file.Write(szContent, strlen(szContent));
		file.Close();
	}

}
 
void CActiveDefenseDlg::OnBnClickedLoadFilter()
{
	CString caption;
	GetDlgItem(IDC_LOAD_FILTER)->GetWindowText(caption);//获取按钮名称
	if (caption == L"开启文件保护")
	{
		CreateProtectFile();
		CFileFind fFind;
		if (fFind.FindFile(L"FsFilter.sys"))
		{
			ShowInfoInDlg(L"检查：文件过滤驱动存在");
		}
		else
		{
			ShowInfoInDlg(L"文件过滤驱动不存在，请检查文件！\r\n----------------------------------------------");
			return;
		}
		BOOLEAN bRet;
		
	//InstallDriver("HelloDDK", ".\\HelloDDK.sys", "370030"/*Altitude*/);
		//加载驱动
		bRet = InstallDriver("FsFilter", ".\\FsFilter.sys", "370030");
		if (bRet)
		{
			ShowInfoInDlg(L"文件过滤驱动加载成功==");
		}
		else
		{
			ShowInfoInDlg(L"文件过滤驱动加载失败==");
			return;
		}
		//开启驱动
		bRet = StartDriver (L"FsFilter");
		if (bRet)
		{
			ShowInfoInDlg(L"文件过滤驱动启动成功==\r\n----------------------------------------------");
			ShowInfoInDlg(L"本目录下README.TXT即保护的文件\r\n----------------------------------------------");
		}
		else
		{
			ShowInfoInDlg(L"文件过滤驱动启动失败==\r\n----------------------------------------------");
			return;
		}

		GetDlgItem(IDC_LOAD_FILTER)->SetWindowText(L"关闭文件保护");
	}
	else
	{
		StopDriver(L"FsFilter");
		ShowInfoInDlg(L"文件过滤驱动已经关闭\r\n----------------------------------------------");
		GetDlgItem(IDC_LOAD_FILTER)->SetWindowText(L"开启文件保护");
	}
	
}


void CActiveDefenseDlg::OnBnClickedProcessFilter()
{
	CString caption;
	GetDlgItem(IDC_PROCESS_FILTER)->GetWindowText(caption);//获取按钮名称
	if (caption == L"开启进程监控")
	{
		int nRet = SendMsgToDriver(IOCTL_PROCESS_FILTER);
		if (nRet==0)
		{
			ShowInfoInDlg(L"开启进程监控成功==\r\n----------------------------------------------");
			GetDlgItem(IDC_PROCESS_FILTER)->SetWindowText(L"关闭进程监控");
		}
		else
		{
			ShowInfoInDlg(L"开启进程监控失败==\r\n----------------------------------------------");
		}
			
	}
	else
	{
		int nRet = SendMsgToDriver(IOCTL_PROCESS_UNFILTER);
		if (nRet==0)
		{
			ShowInfoInDlg(L"关闭进程监控成功==\r\n----------------------------------------------");
			GetDlgItem(IDC_PROCESS_FILTER)->SetWindowText(L"开启进程监控");
		}
		else
		{
			ShowInfoInDlg(L"关闭进程监控失败==\r\n----------------------------------------------");
		}
		
	}
}


void CActiveDefenseDlg::OnBnClickedDriverFilter()
{
	CString caption;
	GetDlgItem(IDC_DRIVER_FILTER)->GetWindowText(caption);//获取按钮名称
	if (caption == L"开启驱动监控")
	{
		int nRet = SendMsgToDriver(IOCTL_DRIVER_FILTER);
		if (nRet == 0)
		{
			ShowInfoInDlg(L"开启驱动监控 成功==\r\n----------------------------------------------");
			GetDlgItem(IDC_DRIVER_FILTER)->SetWindowText(L"关闭驱动监控");
		}
		else
		{
			ShowInfoInDlg(L"开启驱动监控 失败==\r\n----------------------------------------------");
		}

	}
	else
	{
		int nRet = SendMsgToDriver(IOCTL_DRIVER_UNFILTER);
		if (nRet == 0)
		{
			ShowInfoInDlg(L"关闭驱动监控 成功==\r\n----------------------------------------------");
			GetDlgItem(IDC_DRIVER_FILTER)->SetWindowText(L"开启驱动监控");
		}
		else
		{
			ShowInfoInDlg(L"关闭驱动监控失败==\r\n----------------------------------------------");
		}

	}
}


void CActiveDefenseDlg::OnBnClickedHelp()
{
	CString temp;
	temp = "1.驱动在Win7 64位下调试通过，并未进行签名，如果加载失败请进入 禁用强制驱动程序签名 模式，再重新打开程序。\n";
	temp += "2.进程监控是通过注册回调函数来监控进程的行为。\n";
	temp += "3.进程保护采用DKOM方式操作EPROCESS修改FLAGS标志位实现进程保护。\n";
	temp += "4.使用MiniFilter框架过滤文件，实现文件保护。\n";
	temp += "5.通过设置映像加载通告例程，实现驱动加载的监控。\n";
	MessageBox(temp, L"软件说明", 0);
}
