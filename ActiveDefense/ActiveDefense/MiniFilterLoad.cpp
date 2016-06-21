#include "stdafx.h"
#pragma comment(lib,"Advapi32.lib")

char *cs(char *str1, char *str2) //connect string
{
	SIZE_T newstrlen = strlen(str1) + strlen(str2) + 1;
	char *newstr = (char*)malloc(newstrlen);
	memcpy(newstr, str1, strlen(str1));
	memcpy(newstr + strlen(str1), str2, strlen(str2) + 1);
	return newstr;
}

BOOLEAN LoadWdmDrv(char *inf, char *szDrvSvcName)
{
	CHAR exe[] = "c:\\windows\\system32\\InfDefaultInstall.exe ";
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	char *cmd = cs(exe, inf);
	if (!CreateProcessA(NULL, cmd, NULL, NULL, 0, 0, NULL, NULL, &si, &pi))
		return FALSE;
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hSvcHandle = OpenServiceA(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
	if (hSvcHandle)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hSvcHandle);
		return TRUE;
	}
	else
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}
}

BOOLEAN net_start(char *szDrvSvcName)
{
	BOOLEAN b = FALSE;
	SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hSvcHandle = OpenServiceA(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
	if (hSvcHandle)
	{
		b = StartServiceA(hSvcHandle, NULL, NULL);
		CloseServiceHandle(hSvcHandle);
	}
	CloseServiceHandle(hSCM);
	return b;
}

BOOLEAN net_stop(char *szDrvSvcName)
{
	BOOLEAN b = FALSE;
	SERVICE_STATUS lpSt;
	SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hSvcHandle = OpenServiceA(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
	if (hSvcHandle)
	{
		b = ControlService(hSvcHandle, SERVICE_CONTROL_STOP, &lpSt);
		CloseServiceHandle(hSvcHandle);
	}
	CloseServiceHandle(hSCM);
	return b;
}

BOOLEAN UnloadWdmDrv(char *szDrvSvcName)
{
	BOOLEAN b = FALSE;
	SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hSvcHandle = OpenServiceA(hSCM, szDrvSvcName, SERVICE_ALL_ACCESS);
	if (hSvcHandle)
	{
		b = DeleteService(hSvcHandle);
		CloseServiceHandle(hSvcHandle);
	}
	CloseServiceHandle(hSCM);
	return b;
}