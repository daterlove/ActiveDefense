
// ActiveDefenseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ActiveDefense.h"
#include "ActiveDefenseDlg.h"
#include "afxdialogex.h"
#include "ScmDrvCtrl.h"
#include <Windows.h>
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

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
void CActiveDefenseDlg::OnBnClickedLoadDrv()
{
	CString str;
	BOOL b;
	cDrvCtrl dc;
	//设置驱动名称
	char szSysFile[MAX_PATH] = { 0 };
	char szSvcLnkName[] = "DefenseDriver";;
	GetAppPath(szSysFile);
	strcat(szSysFile, "DefenseDriver.sys");
	//安装并启动驱动
	b = dc.Install(szSysFile, szSvcLnkName, szSvcLnkName);
	b = dc.Start();
	

	//“打开”驱动的符号链接
	b = dc.Open("\\\\.\\DefenseDevice");
	if (b==true)
		MessageBox(L"打开符号链接成功");
	
	//使用控制码控制驱动（0x800：传入一个数字并返回一个数字）
	DWORD x = 100, y = 0, z = 0;
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	dc.IoControl(0x911, msg, strlen(msg) + 1, &y, sizeof(y), &z);
	MessageBox(L"here4");
	/*printf("INPUT=%ld\nOUTPUT=%ld\nReturnBytesLength=%ld\n", x, y, z);
	//使用控制码控制驱动（0x801：在DBGVIEW里显示HELLOWORLD）
	dc.IoControl(0x801, 0, 0, 0, 0, 0);*/
	//关闭符号链接句柄
	//CloseHandle(dc.m_hDriver);
	//停止并卸载驱动
	////
	b = dc.Stop();
	b = dc.Remove();
}
