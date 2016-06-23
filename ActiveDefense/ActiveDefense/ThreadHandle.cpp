#include "stdafx.h"
bool g_isProcessOver = 0;//指示程序是否要退出
int g_ThreadNum = 0;//记录线程个数
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
#define TYPE_PROCESS_CREAT 1
#define TYPE_PROCESS_EXIT	2
#define TYPE_DRIVER_LOAD 	3
struct EventStrInfo g_EventStrInfo;
INT AnalysisType(PCHAR pStr)
{
	int nType;//事件类型，字符串长度
	int *pNum = (int *)pStr;//指向缓冲区头部
	nType = *pNum;
	return nType;
}
VOID AnalysisStr(PCHAR pStr,INT *p_nType)
{
	int nType, nLength;//事件类型，字符串长度
	CString szTemp;

	int *pNum=(int *)pStr;//指向缓冲区头部
	nType = *pNum;
	nLength = *(pNum + 1);
	*p_nType = nType;//返回记录的类型


	USHORT *pShort = (USHORT  *)(pStr);
	pShort += 4;//指向UCHAR字符串地址
	g_EventStrInfo.szPath = L"";

	if (nType != TYPE_PROCESS_EXIT)//不是进程退出消息
	{
		pShort += 4;//略过前4个字节
		for (int i = 0; i<nLength / 2 - 4; i++)
		{
			g_EventStrInfo.szPath.AppendChar(*pShort);
			pShort++;
		}
	}
	else
	{
		for (int i = 0; i<nLength / 2; i++)
		{
			g_EventStrInfo.szPath.AppendChar(*pShort);
			pShort++;
		}

	}

	//------------------------------------------------------------------------
	switch (nType)//解析事件类型
	{
	case TYPE_PROCESS_CREAT:
		g_EventStrInfo.szType = L"事件类型：进程创建";
		break;
	case TYPE_PROCESS_EXIT:
		g_EventStrInfo.szType = L"事件类型：进程退出";
		break;
	case TYPE_DRIVER_LOAD:
		g_EventStrInfo.szType = L"事件类型：驱动加载";
		break;

	default:
		g_EventStrInfo.szType = L"事件类型：未定义事件类型";
		break;
	}
	ShowInfoInDlg(g_EventStrInfo.szType);

	if (nType == TYPE_PROCESS_CREAT)
	{
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
		g_EventStrInfo.szDescribe = "进程路径:";
		g_EventStrInfo.szName = L"进程名:" + g_EventStrInfo.szName;
	}
	else if (nType == TYPE_DRIVER_LOAD)
	{
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
		g_EventStrInfo.szCaption = L"驱动事件";
		g_EventStrInfo.szDescribe = "驱动路径:";
		g_EventStrInfo.szName = L"驱动名:" + g_EventStrInfo.szName;
	}
	

	
	if (nType == TYPE_PROCESS_EXIT)
	{
		ShowInfoInDlg(L"进程名:"+g_EventStrInfo.szPath);
		ShowInfoInDlg(L"----------------------------------------------");
	}
	else if (nType == TYPE_PROCESS_CREAT)
	{
		ShowInfoInDlg(g_EventStrInfo.szName);
		ShowInfoInDlg(L"路径:"+g_EventStrInfo.szPath);
	}
	else if (nType == TYPE_DRIVER_LOAD)
	{
		ShowInfoInDlg(g_EventStrInfo.szName);
		ShowInfoInDlg(L"路径:" + g_EventStrInfo.szPath);
	}
	
	
	
}
VOID ShowHandleInfo(int nDlgRet)
{
	CString szTemp;
	switch (nDlgRet)
	{
	case 0:
		szTemp.Format(L"用户选择允许该行为继续。\r\n----------------------------------------------");
		break;
	case 1:
		szTemp.Format(L"用户选择禁止该行为继续。\r\n----------------------------------------------");
		break;

	default:
		szTemp.Format(L"用户返回值遭遇意外。\r\n----------------------------------------------");
		break;
	}
	ShowInfoInDlg(szTemp);
}
unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	
/*	g_EventStrInfo.szCaption = L"进程事件";
	g_EventStrInfo.szType = L"事件类型:进程创建";
	g_EventStrInfo.szName =L"进程名:360Safe.exe";
	g_EventStrInfo.szDescribe = "进程路径:";
	g_EventStrInfo.szPath= L"C:\\Program Files (x86)\\360\\360safe\\Program Files (x86)\\360\\360safe\\360Safe.exe";
	myDlg.DoModal();
*/
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
	ShowInfoInDlg(L"线程消息：监控线程开启\r\n----------------------------------------------");
	while (!g_isProcessOver)
	{

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, Buffer, sizeof(Buffer), &ret_len, 0))//读取
		{
			ShowInfoInDlg(L"线程消息：从驱动读取消息失败");
			continue;
		}
		

		if (ret_len == 0)
		{
			ShowInfoInDlg(L"读取到的返回信息为0,线程继续\r\n----------------------------------------------");
			continue;
		}
		int nType;
		AnalysisStr(Buffer, &nType);//解析受到的数据
		if (nType != TYPE_PROCESS_CREAT && nType != TYPE_DRIVER_LOAD)
			continue;

		nDlgRet = myDlg.DoModal();  //弹出警告窗体
		ShowHandleInfo(nDlgRet);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), &nDlgRet, sizeof(nDlgRet), NULL, 0, &ret_len, 0))//发送处理结果
		{
			ShowInfoInDlg(L"线程消息：向驱动发送消息失败");
			continue;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//线程总个数-1
	//MessageBox(NULL, L"线程退出", L"", NULL);
	ShowInfoInDlg(L"线程消息：监控线程退出\r\n----------------------------------------------");
	return 0;
}