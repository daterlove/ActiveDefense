#include "stdafx.h"
#include <Winsvc.h>

#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0

DWORD CTL_CODE_GEN(DWORD lngFunction)
{
	return (FILE_DEVICE_UNKNOWN * 65536) | (FILE_ANY_ACCESS * 16384) | (lngFunction * 4) | METHOD_BUFFERED;
}
//szFullPath完整路径、szName服务名、iType操作类型(安装、运行、停止、移除)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType)
{
	SC_HANDLE shOSCM = NULL, shCS = NULL;
	SERVICE_STATUS ss;
	DWORD dwErrorCode = 0;
	BOOL bSuccess = FALSE;

	shOSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!shOSCM)
	{
		//SetWindowText(hwndStatus, TEXT("OpenSCManager时出错！"));
		//MessageBox(NULL, L"OpenSCManager时出错！", NULL, NULL);
		ShowInfoInDlg(L"驱动操作：OpenSCManager 出错");
		return FALSE;
	}

	if (iType) //若操作类型不是"安装服务"
	{
		shCS = OpenService(shOSCM, szName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			dwErrorCode = GetLastError(); //获取错误信息
			if (ERROR_INVALID_NAME == dwErrorCode)
				ShowInfoInDlg(L"驱动操作：服务名无效");
			else if (ERROR_SERVICE_DOES_NOT_EXIST == dwErrorCode)
				ShowInfoInDlg(L"驱动操作：不存在此服务\r\n----------------------------------------");
			else
				ShowInfoInDlg(L"驱动操作：OpenService失败");

			CloseServiceHandle(shOSCM); //关闭服务句柄
			shOSCM = NULL;
			return FALSE;
		}
	}

	switch (iType)
	{
	case 0: //安装服务
		shCS = CreateService(shOSCM, szName, szName,
			SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
			szFullPath, NULL, NULL, NULL, NULL, NULL);

		if (!shCS)
		{
			/*int nError = GetLastError();
			if (nError == ERROR_SERVICE_EXISTS || nError == ERROR_SERVICE_MARKED_FOR_DELETE)
			{
				CString temp;
				temp.Format(L"nError == GetLastError():%d", GetLastError());
				ShowInfoInDlg(temp);
				shCS == OpenService(shOSCM, szName, SERVICE_ALL_ACCESS);
			}*/
			
			if (ERROR_SERVICE_EXISTS == GetLastError()) //如果服务已经存在
				ShowInfoInDlg(L"驱动操作：指定的服务已经存在");
			else
			{
				CString temp;
				temp.Format(L"GetLastError():%d", GetLastError());
				ShowInfoInDlg(L"驱动操作：创建服务 失败");
				ShowInfoInDlg(temp);
				break;
			}
			/*
			if (!shCS)
			{
				CString temp;
				temp.Format(L"here== GetLastError():%d", GetLastError());
				ShowInfoInDlg(temp);
				ShowInfoInDlg(L"驱动操作：创建服务 失败");
				bSuccess = FALSE;
				break;
			}*/

			
			//
		}
		bSuccess = TRUE;

		//MessageBox(NULL, L"安装服务成功", NULL, NULL);
		ShowInfoInDlg(L"驱动操作：服务安装成功");

		break;

	case 1: //运行服务
		if (StartService(shCS, 0, NULL))
			ShowInfoInDlg(L"驱动操作：运行指定服务成功\r\n----------------------------------------");
		else
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_ALREADY_RUNNING == dwErrorCode)
				ShowInfoInDlg(L"驱动操作：指定的服务已经运行");
			else
				ShowInfoInDlg(L"驱动操作：运行指定服务出错\r\n----------------------------------------");

			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;
		break;

	case 2: //停止服务
		if (!ControlService(shCS, SERVICE_CONTROL_STOP, &ss))
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_NOT_ACTIVE == dwErrorCode)
				ShowInfoInDlg(L"驱动操作：指定的服务并未启动");
			else
				ShowInfoInDlg(L"驱动操作：不能停止服务");

			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("成功停止服务！"));
		//MessageBox(NULL, L"成功停止服务", NULL, NULL);
		ShowInfoInDlg(L"驱动操作：成功停止服务");
		bSuccess = TRUE;
		break;

	case 3: //移除服务
		if (!DeleteService(shCS))
		{
			//SetWindowText(hwndStatus, TEXT("不能移除服务！"));
			//MessageBox(NULL, L"不能移除服务", NULL, NULL);
			ShowInfoInDlg(L"驱动操作：不能移除服务");
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("成功移除服务！"));
		//MessageBox(NULL, L"成功移除服务", NULL, NULL);
		ShowInfoInDlg(L"驱动操作：成功移除服务\r\n----------------------------------------");
		bSuccess = TRUE;
		break;

	default:
		break;
	}

	if (shCS)
	{
		CloseServiceHandle(shCS);
		shCS = NULL;
	}
	if (shOSCM)
	{
		CloseServiceHandle(shOSCM);
		shOSCM = NULL;
	}

	return bSuccess;
}

int SendMsgToDriver(int nCode)
{
	HANDLE device = NULL;
	ULONG ret_len;


	//打开设备
	device = CreateFile(CWK_DEV_SYM, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
	if (device == INVALID_HANDLE_VALUE)
	{
		ShowInfoInDlg(L"主线程：设备打开错误");
		return -1;
	}
	if (!DeviceIoControl(device, CTL_CODE_GEN(nCode), NULL, 0, NULL, 0, &ret_len, 0))//发送
	{
		ShowInfoInDlg(L"主线程：向驱动发送消息失败");
	}

	CloseHandle(device);
	return ret_len;
}