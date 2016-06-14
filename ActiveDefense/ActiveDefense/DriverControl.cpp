#include "stdafx.h"
#include <Winsvc.h>

#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_ANY_ACCESS                 0
#define METHOD_BUFFERED                 0

DWORD CTL_CODE_GEN(DWORD lngFunction)
{
	return (FILE_DEVICE_UNKNOWN * 65536) | (FILE_ANY_ACCESS * 16384) | (lngFunction * 4) | METHOD_BUFFERED;
}
//szFullPath����·����szName��������iType��������(��װ�����С�ֹͣ���Ƴ�)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType)
{
	SC_HANDLE shOSCM = NULL, shCS = NULL;
	SERVICE_STATUS ss;
	DWORD dwErrorCode = 0;
	BOOL bSuccess = FALSE;

	shOSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!shOSCM)
	{
		//SetWindowText(hwndStatus, TEXT("OpenSCManagerʱ������"));
		//MessageBox(NULL, L"OpenSCManagerʱ������", NULL, NULL);
		ShowInfoInDlg(L"����������OpenSCManager ����");
		return FALSE;
	}

	if (iType) //���������Ͳ���"��װ����"
	{
		shCS = OpenService(shOSCM, szName, SERVICE_ALL_ACCESS);
		if (!shCS)
		{
			dwErrorCode = GetLastError(); //��ȡ������Ϣ
			if (ERROR_INVALID_NAME == dwErrorCode)
				ShowInfoInDlg(L"������������������Ч");
			else if (ERROR_SERVICE_DOES_NOT_EXIST == dwErrorCode)
				ShowInfoInDlg(L"���������������ڴ˷���");
			else
				ShowInfoInDlg(L"����������OpenServiceʧ��");

			CloseServiceHandle(shOSCM); //�رշ�����
			shOSCM = NULL;
			return FALSE;
		}
	}

	switch (iType)
	{
	case 0: //��װ����
		shCS = CreateService(shOSCM, szName, szName,
			SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
			szFullPath, NULL, NULL, NULL, NULL, NULL);

		if (!shCS)
		{
			if (ERROR_SERVICE_EXISTS == GetLastError()) //��������Ѿ�����
				ShowInfoInDlg(L"����������ָ���ķ����Ѿ�����");
			else
				ShowInfoInDlg(L"������������������ ʧ��");

			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;

		//MessageBox(NULL, L"��װ����ɹ�", NULL, NULL);
		ShowInfoInDlg(L"��������������װ�ɹ�");

		break;

	case 1: //���з���
		if (StartService(shCS, 0, NULL))
			ShowInfoInDlg(L"��������������ָ������ɹ�\r\n----------------------------------------");
		else
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_ALREADY_RUNNING == dwErrorCode)
				ShowInfoInDlg(L"����������ָ���ķ����Ѿ�����");
			else
				ShowInfoInDlg(L"��������������ָ���������");

			bSuccess = FALSE;
			break;
		}
		bSuccess = TRUE;
		break;

	case 2: //ֹͣ����
		if (!ControlService(shCS, SERVICE_CONTROL_STOP, &ss))
		{
			dwErrorCode = GetLastError();
			if (ERROR_SERVICE_NOT_ACTIVE == dwErrorCode)
				ShowInfoInDlg(L"����������ָ���ķ���δ����");
			else
				ShowInfoInDlg(L"��������������ֹͣ����");

			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("�ɹ�ֹͣ����"));
		//MessageBox(NULL, L"�ɹ�ֹͣ����", NULL, NULL);
		ShowInfoInDlg(L"�����������ɹ�ֹͣ����");
		bSuccess = TRUE;
		break;

	case 3: //�Ƴ�����
		if (!DeleteService(shCS))
		{
			//SetWindowText(hwndStatus, TEXT("�����Ƴ�����"));
			//MessageBox(NULL, L"�����Ƴ�����", NULL, NULL);
			ShowInfoInDlg(L"���������������Ƴ�����");
			bSuccess = FALSE;
			break;
		}
		//SetWindowText(hwndStatus, TEXT("�ɹ��Ƴ�����"));
		//MessageBox(NULL, L"�ɹ��Ƴ�����", NULL, NULL);
		ShowInfoInDlg(L"�����������ɹ��Ƴ�����\r\n----------------------------------------");
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