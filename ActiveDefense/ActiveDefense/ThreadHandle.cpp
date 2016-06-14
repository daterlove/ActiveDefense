#include "stdafx.h"
bool g_isProcessOver = 0;//指示程序是否要退出
int g_ThreadNum = 0;//记录线程个数
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
#define TYPE_PROCESS_CREAT 1
#define TYPE_PROCESS_EXIT	2
struct EventStrInfo g_EventStrInfo;

VOID AnalysisStr(PCHAR pStr)
{
	int nType, nLength;//事件类型，字符串长度
	CString szTemp;

	int *pNum=(int *)pStr;//指向缓冲区头部
	nType = *pNum;
	nLength = *(pNum + 1);
	//szTemp.Format(L"nType:%d,nLength:%d", nType, nLength);
	//	ShowInfoInDlg(szTemp);

	//解析出进程路径
	USHORT *pShort = (USHORT  *)(pStr);//指向UCHAR字符串地址
	pShort += 8;//忽略前4个字节
	g_EventStrInfo.szPath = L"";
	for (int i = 0; i<nLength / 2 - 4; i++)
	{
		//if (*pShort != 0)//没有到结束符
		g_EventStrInfo.szPath.AppendChar(*pShort);
		pShort++;
	}
	//szTemp = L"For Test";
	
	//------------------------------------------------------------------------
	switch (nType)//解析事件类型
	{
	case TYPE_PROCESS_CREAT:
		g_EventStrInfo.szType = L"事件类型：进程创建";
		break;
	case TYPE_PROCESS_EXIT:
		g_EventStrInfo.szType = L"事件类型：进程退出";
		break;

	default:
		g_EventStrInfo.szType = L"事件类型：未定义事件类型";
		break;
	}
	
	g_EventStrInfo.szName = g_EventStrInfo.szPath;
	int nPos = szTemp.Find('\\');
	while (nPos)//解析出进程名
	{
		g_EventStrInfo.szName = g_EventStrInfo.szName.Mid(nPos + 1, g_EventStrInfo.szName.GetLength() - nPos);  //取'\'右边字符串
		nPos = g_EventStrInfo.szName.Find('\\');   //不包含'\'，函数值返回-1 

		if (nPos == -1)
		{
			nPos = 0;
		}
	}

	g_EventStrInfo.szCaption = L"进程事件";
	g_EventStrInfo.szName = L"进程名:" + g_EventStrInfo.szName;

	g_EventStrInfo.szDescribe = "进程路径:";
	ShowInfoInDlg(g_EventStrInfo.szType);
	ShowInfoInDlg(g_EventStrInfo.szName);
	ShowInfoInDlg(g_EventStrInfo.szPath);
	
}
unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	/*
	g_EventStrInfo.szCaption = L"进程事件";
	g_EventStrInfo.szType = L"事件类型:进程创建";
	g_EventStrInfo.szName =L"进程名:360Safe.exe";
	g_EventStrInfo.szDescribe = "进程路径:";
	g_EventStrInfo.szPath= L"C:\\Program Files (x86)\\360\\360safe\\Program Files (x86)\\360\\360safe\\360Safe.exe";
	myDlg.DoModal();  //弹出警告窗体*/
	WarningDlg myDlg;

	int nDlgRet;
	CString CS_Str;
	HANDLE device = NULL;
	//char *msg = { "Hello driver, this is a message from app.\r\n" };
	char Buffer[512+8];
	ULONG ret_len;
	g_ThreadNum++;//线程总个数+1
	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		//MessageBox(NULL, L"线程：设备打开错误", L"", NULL);
		ShowInfoInDlg(L"线程消息：设备打开错误，线程退出");
		g_ThreadNum--;//线程总个数-1
		return -1;
	}
	ShowInfoInDlg(L"线程消息：监控线程开启");
	while (!g_isProcessOver)
	{

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, Buffer, sizeof(Buffer), &ret_len, 0))//读取
		{
			ShowInfoInDlg(L"线程消息：从驱动读取消息失败");
			continue;
		}


		AnalysisStr(Buffer);

		if (ret_len == 0)
		{
			ShowInfoInDlg(L"Recv信息为0,continue");
			continue;
		}
			
		nDlgRet = myDlg.DoModal();  //弹出警告窗体
		CS_Str.Format(L"nDlgRet:%d", nDlgRet);
		ShowInfoInDlg(CS_Str);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), &nDlgRet, sizeof(nDlgRet), NULL, 0, &ret_len, 0))//发送处理结果
		{
			ShowInfoInDlg(L"线程消息：向驱动发送消息失败");
			continue;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//线程总个数-1
	//MessageBox(NULL, L"线程退出", L"", NULL);
	ShowInfoInDlg(L"线程消息：监控线程退出");
	return 0;
}