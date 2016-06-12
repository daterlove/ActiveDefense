#include "common.h"
NTKERNELAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);

BOOLEAN g_isProcessRoutine=0;//记录是否安装进程回调
extern KEVENT g_kEvent;	//全局事件对象



PCHAR GetProcessNameByProcessId(HANDLE ProcessId)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PEPROCESS ProcessObj = NULL;
	PCHAR string = NULL;
	st = PsLookupProcessByProcessId(ProcessId, &ProcessObj);
	if (NT_SUCCESS(st))
	{
		string = PsGetProcessImageFileName(ProcessObj);
		ObfDereferenceObject(ProcessObj);
	}
	return string;
}

VOID MyCreateProcessNotifyEx//进程事件 回调处理函数
(
__inout   PEPROCESS Process,
__in      HANDLE ProcessId,
__in_opt  PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	char xxx[32] = { 0 };
	if (CreateInfo != NULL)	//进程创建事件
	{
		strcpy(xxx, GetProcessNameByProcessId(ProcessId));
		DbgPrint("[进程创建:][%s-PID:%d]路径: %wZ --",
			xxx,
			ProcessId,
			CreateInfo->ImageFileName);
		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//激活事件

		if (!_stricmp(xxx, "calc.exe"))
		{
			DbgPrint("禁止创建计算器进程！");
			//CreateInfo->CreationStatus = STATUS_UNSUCCESSFUL;	//禁止创建进程
			CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}
	}
	else
	{
		DbgPrint("进程退出: %s", PsGetProcessImageFileName(Process));
	}
	
}
VOID UnLoadProcessRoutine()
{
	if (g_isProcessRoutine)//如果回调函数存在
	{
		PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)MyCreateProcessNotifyEx, TRUE);
		g_isProcessRoutine = 0;
	}
	KdPrint(("UnLoadProcessRoutine"));
}
NTSTATUS CreateProcessRoutine()
{
	NTSTATUS status = STATUS_SUCCESS;
	status = PsSetCreateProcessNotifyRoutineEx((PCREATE_PROCESS_NOTIFY_ROUTINE_EX)MyCreateProcessNotifyEx, FALSE);
	if (status == STATUS_SUCCESS)
	{
		g_isProcessRoutine = 1;//表示创建回调函数成功
	}
	KdPrint(("PsSetCreateProcessNotifyRoutineEx return: %x", status));
	return status;
}