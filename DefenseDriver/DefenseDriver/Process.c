#include "common.h"
NTKERNELAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);

#define MEM_TAG 111

BOOLEAN g_isProcessRoutine=0;//记录是否安装进程回调
INT g_isRefuse = 0;//指示进程是否被放行
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
	KEVENT ProcessEvent;
	//初始化事件,同步事件
	KeInitializeEvent(&ProcessEvent, SynchronizationEvent, FALSE);
	PMY_EVENT pEvent = (PMY_EVENT)ExAllocatePoolWithTag(PagedPool, sizeof(MY_EVENT), MEM_TAG);//申请内存
	if (pEvent == NULL)
	{
		return;
	}
	if (CreateInfo != NULL)	//进程创建事件
	{
		pEvent->nType = 1;
		pEvent->nLength = CreateInfo->ImageFileName->Length;
		pEvent->pProcessEvent = &ProcessEvent;

		RtlCopyMemory(pEvent->wStr, CreateInfo->ImageFileName->Buffer, pEvent->nLength);//拷贝字符串
		KdPrint(("ProcessRoutine:nTtype:%d,nLen:%d", pEvent->nType, pEvent->nLength));
		
		AddEventToList(pEvent);//加入链表

		DbgPrint("[进程创建:][PID:%d]路径: %wZ --",
			ProcessId,
			CreateInfo->ImageFileName);

		KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//激活事件,MyDeviceControl函数继续运行

		KeWaitForSingleObject(&ProcessEvent, Executive, KernelMode, FALSE, 0);//等待事件的处理信号 
		KdPrint(("g_isRefuse:%d", g_isRefuse));
		if (g_isRefuse)
		{
			DbgPrint("禁止创建进程！");
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