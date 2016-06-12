
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

#define SOFT_CAPTION L"SML主动防御引擎"
// CActiveDefenseDlg 对话框

extern bool g_isProcessOver;//指示程序是否要退出
extern int g_ThreadNum;//记录线程个数
HANDLE g_ThreadHandle;
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
	ON_BN_CLICKED(IDC_START_THREAD, &CActiveDefenseDlg::OnBnClickedStartThread)
	ON_MESSAGE(WM_SHOW_MSG, &CActiveDefenseDlg::OnShowMsg)    // OnCountMsg是自定义的消息处理函数，可以在这个函数里面进行自定义的消息处理代码
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


TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"


void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	TCHAR szFullPath[256];
	GetAppPath(szFullPath, L"DefenseDriver.sys");

	operaType(szFullPath, szTitle, 0);
	operaType(szFullPath, szTitle, 1);
	/*
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[100];
	ULONG ret_len;
	HANDLE device = NULL;
	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		MessageBox( L"设备打开错误");
		return;
	}


	if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg) + 1, NULL, 0, &ret_len, 0))
	{
		MessageBox(L"发送错误");
		return;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), msg, strlen(msg) + 1, str, sizeof(str), &ret_len, 0))
	{
		MessageBox(L"接收错误");
		return;
	}
	CloseHandle(device);
	CString Str_Temp(str);
	MessageBox(Str_Temp);
	*/
}

void StartThread()
{
	 g_ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadHandle, NULL, 0, NULL);
	//WaitForSingleObject(handle, INFINITE); //等待线程结束
	//CloseHandle(handle);
}
int SendDrvClose()//发送给设备CLOSE信号，激活线程，防止线程阻塞在内核层
{
	HANDLE device = NULL;
	ULONG ret_len;

	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"主线程：设备打开错误，线程退出");
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
	operaType(szFullPath, szTitle, 3);
}


void CActiveDefenseDlg::OnBnClickedStartThread()
{
	if (g_ThreadNum < 1)//线程数量小于一个
	{
		g_isProcessOver = 0;//线程开启标志位
		StartThread();
	}
	else
	{
		MessageBox(L"只允许开启一个监控线程");
	}
	
}




BOOL CActiveDefenseDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else
	return CDialogEx::PreTranslateMessage(pMsg);
}
