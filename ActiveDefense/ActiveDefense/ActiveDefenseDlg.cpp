
// ActiveDefenseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"
#include "ScmDrvCtrl.h"

#include "DriverContrl.h"
#pragma comment(lib,"user32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SOFT_CAPTION L"SML主动防御引擎"
// CActiveDefenseDlg 对话框



CActiveDefenseDlg::CActiveDefenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActiveDefenseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActiveDefenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, btn_close);
	DDX_Control(pDX, IDC_MIN, btn_min);
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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


//HRESULT CActiveDefenseDlg::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
//{
//	// TODO:  在此添加专用代码和/或调用基类
//
//	return CDialogEx::accHitTest(xLeft, yTop, pvarChild);
//}


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

void GetAppPath(char *szCurFile) //最后带斜杠
{
	GetModuleFileNameA(0, szCurFile, MAX_PATH);
	for (SIZE_T i = strlen(szCurFile) - 1; i >= 0; i--)
	{
		if (szCurFile[i] == '\\')
		{
			szCurFile[i + 1] = '\0';
			break;
		}
	}
}

TCHAR szFullPath[] = { L"C:\\Users\\Administrator\\DeskTop\\DefenseDriver.sys" };
TCHAR szTitle[MAX_PATH] = { L"DefenseDriver" };
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
/*
// 从应用层给驱动发送一个字符串。
#define  CWK_DVC_SEND_STR \
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x911,METHOD_BUFFERED, \
	FILE_WRITE_DATA)

// 从驱动读取一个字符串
#define  CWK_DVC_RECV_STR \
	(ULONG)CTL_CODE( \
	FILE_DEVICE_UNKNOWN, \
	0x912,METHOD_BUFFERED, \
	FILE_READ_DATA)
	*/
void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	operaType(szFullPath, szTitle, 0);
	operaType(szFullPath, szTitle, 1);
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
}


void CActiveDefenseDlg::OnBnClickedUnloadDrv()
{/*
	CString str;
	str.Format(L"hwnd:%d", dc.m_hService);
	MessageBox(str);

	//关闭符号链接句柄
	CloseHandle(dc.m_hDriver);
	BOOL b;
	b = dc.Stop();
	if (b == true)
		MessageBox(L"驱动停止成功");
	else
	{
		str.Format(L"stop error:%d", dc.m_dwLastError);
		MessageBox(str); 
	}
	b = dc.Remove();
	if (b == true)
		MessageBox(L"驱动卸载成功");
	else
	{
		str.Format(L"remove error:%d", dc.m_dwLastError);
		MessageBox(str);
	}
	*/

	
	operaType(szFullPath, szTitle, 2);
	operaType(szFullPath, szTitle, 3);
}
