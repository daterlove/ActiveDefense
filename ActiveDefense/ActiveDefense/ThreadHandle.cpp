#include "stdafx.h"
bool g_isProcessOver = 0;//指示程序是否要退出
int g_ThreadNum = 0;//记录线程个数
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"
 
unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	WarningDlg myDlg;
	int nDlgRet;
	CString CS_Str;
	HANDLE device = NULL;
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[512+8];
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

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, str, sizeof(str), &ret_len, 0))//读取
		{
			ShowInfoInDlg(L"线程消息：从驱动读取消息失败");
			continue;
		}
		/*
		CString temp;
		temp.Format(L"ret_len:%d", ret_len);
		ShowInfoInDlg(temp);
		*/
		

		int nType, nLength;
		int *pNum;
		pNum = (int *)&str;
		PWCHAR pWchr=PWCHAR(str+8);
		nType = *pNum;
		nLength = *(pNum + 1);
		CS_Str.Format(L"T:%d,L:%d,re:%d", nType, nLength, ret_len);

		ShowInfoInDlg(CS_Str);

		CS_Str = L"";
	
		//memcpy(CS_Str.GetBuffer(), &str[8], nLength);
		unsigned short *pShort=(unsigned short  *)&str[8];
		pShort += 4;//忽略前4个字节
		for (unsigned int i = 0; i<nLength/2-4; i++)
		{
			if (*pShort!=0)//没有到结束符
				CS_Str.AppendChar(*pShort);
			pShort++;
		}
		ShowInfoInDlg(CS_Str);

		if (ret_len == 0) continue;
		nDlgRet = myDlg.DoModal();  //弹出警告窗体
		CS_Str.Format(L"nDlgRet:%d", nDlgRet);
		ShowInfoInDlg(CS_Str);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), &nDlgRet, sizeof(nDlgRet), NULL, 0, &ret_len, 0))//发送处理结果
		//if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg), NULL, 0, &ret_len, 0))//发送处理结果
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