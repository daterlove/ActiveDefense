#include "stdafx.h"
bool g_isProcessOver = 0;//指示程序是否要退出
int g_ThreadNum = 0;//记录线程个数
#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"

unsigned int __stdcall ThreadHandle(VOID *pParam)
{
	
	HANDLE device = NULL;
	char *msg = { "Hello driver, this is a message from app.\r\n" };
	char str[100];
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
		Sleep(2000);
		if (!DeviceIoControl(device, CTL_CODE_GEN(0x912), NULL, 0, str, sizeof(str), &ret_len, 0))//读取
		{
			ShowInfoInDlg(L"线程消息：从驱动读取消息失败");
			break;
		}

		if (!DeviceIoControl(device, CTL_CODE_GEN(0x911), msg, strlen(msg) + 1, NULL, 0, &ret_len, 0))//发送
		{
			ShowInfoInDlg(L"线程消息：向驱动发送消息失败");
			break;
		}

	}
	
	CloseHandle(device);
	g_ThreadNum--;//线程总个数-1
	//MessageBox(NULL, L"线程退出", L"", NULL);
	ShowInfoInDlg(L"线程消息：监控线程退出");
	return 0;
}