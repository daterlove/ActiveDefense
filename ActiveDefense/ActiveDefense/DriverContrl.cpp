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
		MessageBox(NULL, L"OpenSCManager时出错！", NULL, NULL);
		return FALSE;
	}

	if (iType) //若操作类型不是"安装服务"
	{
		shCS = OpenService(shOSCM, szName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			dwErrorCode = GetLastError(); //获取错误信息
			if (ERROR_INVALID_NAME == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("服务名无效！"));
				MessageBox(NULL, L"服务名无效！", NULL, NULL);
			else if (ERROR_SERVICE_DOES_NOT_EXIST == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("不存在此服务！"));
				MessageBox(NULL, L"不存在此服务", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("OpenService时出错！"));
				MessageBox(NULL, L"OpenService时出错！", NULL, NULL);

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
			if (ERROR_SERVICE_EXISTS == GetLastError()) //如果服务已经存在
				MessageBox(NULL, L"指定的服务已经存在", NULL, NULL);
			else
				MessageBox(NULL, L"CreateService", NULL, NULL);

			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;

		MessageBox(NULL, L"安装服务成功", NULL, NULL);
		break;

	case 1: //运行服务
		if (StartService(shCS, 0, NULL))
			//SetWindowText(hwndStatus, TEXT("运行指定服务成功！"));
			MessageBox(NULL, L"运行指定服务成功", NULL, NULL);
		else
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_ALREADY_RUNNING == dwErrorCode)
				//SetWindowText(hwndStatus, TEXT("指定的服务已经运行！"));
				MessageBox(NULL, L"指定的服务已经运行", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("运行指定服务出错！"));
				MessageBox(NULL, L"运行指定服务出错", NULL, NULL);
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
				//SetWindowText(hwndStatus, TEXT("指定的服务并未启动！"));
				MessageBox(NULL, L"指定的服务并未启动", NULL, NULL);
			else
				//SetWindowText(hwndStatus, TEXT("不能停止服务！"));
				MessageBox(NULL, L"不能停止服务", NULL, NULL);
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("成功停止服务！"));
		MessageBox(NULL, L"成功停止服务", NULL, NULL);
		bSuccess = TRUE;
		break;

	case 3: //移除服务
		if (!DeleteService(shCS))
		{
			//SetWindowText(hwndStatus, TEXT("不能移除服务！"));
			MessageBox(NULL, L"不能移除服务", NULL, NULL);
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("成功移除服务！"));
		MessageBox(NULL, L"成功移除服务", NULL, NULL);
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