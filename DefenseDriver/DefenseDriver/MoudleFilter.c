#include "common.h"
#define MEM_TAG 112
extern KEVENT g_kEvent;	//全局事件对象
extern INT g_isRefuse;//指示进程是否被放行
INT g_isMoudleFilterLoad = 0;

BOOLEAN VxkCopyMemory(PVOID pDestination, PVOID pSourceAddress, SIZE_T SizeOfCopy)
{
	PMDL pMdl = NULL;
	PVOID pSafeAddress = NULL;
	pMdl = IoAllocateMdl(pSourceAddress, (ULONG)SizeOfCopy, FALSE, FALSE, NULL);//建立MDL内存 
	if (!pMdl) 
		return FALSE;
	__try
	{
		MmProbeAndLockPages(pMdl, KernelMode, IoReadAccess);//锁定内存页
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		IoFreeMdl(pMdl);//释放
		return FALSE;
	}
	pSafeAddress = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);//Buffer映射到内核地址空间
	if (!pSafeAddress) 
		return FALSE;
	RtlCopyMemory(pDestination, pSafeAddress, SizeOfCopy);//拷贝
	MmUnlockPages(pMdl);//解锁内存页
	IoFreeMdl(pMdl);//释放
	return TRUE;
}
PVOID GetDriverEntryByImageBase(PVOID ImageBase)//返回可执行映像 的执行入口地址
{
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS64 pNTHeader;
	PVOID pEntryPoint;
	pDOSHeader = (PIMAGE_DOS_HEADER)ImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS64)((ULONG64)ImageBase + pDOSHeader->e_lfanew);
	pEntryPoint = (PVOID)((ULONG64)ImageBase + pNTHeader->OptionalHeader.AddressOfEntryPoint);
	return pEntryPoint;
}

void DenyLoadDriver(PVOID DriverEntry)//在入口地址写入禁止执行的机器码
{
	//Mov eax,c0000022h  B8 22 00 00 C0
	//Ret							 C3
	UCHAR code[] = "\xB8\x22\x00\x00\xC0\xC3";
	if (MmIsAddressValid(DriverEntry))
	{
		VxkCopyMemory(DriverEntry, code, sizeof(code));
	}
	
}

VOID LoadImageNotifyRoutine//模块加载 过滤函数
(
__in_opt PUNICODE_STRING  FullImageName,
__in HANDLE  ProcessId,
__in PIMAGE_INFO  ImageInfo
)
{
	

	PVOID pDrvEntry;
	//char szFullImageName[260] = { 0 };
	KEVENT ImageEvent;
	PMY_EVENT pEvent;//本次事件
	
	if (FullImageName != NULL && MmIsAddressValid(FullImageName))
	{
		if (ProcessId == 0)//父调用对象是系统，所以加载的是驱动
		{
			KdPrint(("ImageName:%wZ\n", FullImageName));

			//初始化事件,同步事件
			KeInitializeEvent(&ImageEvent, SynchronizationEvent, FALSE);
			pEvent = (PMY_EVENT)ExAllocatePoolWithTag(PagedPool, sizeof(MY_EVENT), MEM_TAG);//申请内存
			if (pEvent == NULL)
			{
				return;
			}
			pEvent->nType =3;
			pEvent->nLength = FullImageName->Length;
			pEvent->pProcessEvent = &ImageEvent;
			RtlCopyMemory(pEvent->wStr, FullImageName->Buffer, pEvent->nLength);//拷贝字符串

			AddEventToList(pEvent);//加入链表
			KeSetEvent(&g_kEvent, IO_NO_INCREMENT, FALSE);//激活事件,MyDeviceControl函数继续运行

			KeWaitForSingleObject(&ImageEvent, Executive, KernelMode, FALSE, 0);//等待事件的处理信号 

			
			

			if (g_isRefuse)
			{
				pDrvEntry = GetDriverEntryByImageBase(ImageInfo->ImageBase);
				KdPrint(("禁止加载驱动，DriverEntry: %p\n", pDrvEntry));
				DenyLoadDriver(pDrvEntry);
			}
/*		UnicodeToChar(FullImageName, szFullImageName);
		if (strstr(_strlwr(szFullImageName), "win64ast.sys"))
			{
				DbgPrint("Deny load [WIN64AST.SYS]");
				//禁止加载win64ast.sys
				
			}*/
			KdPrint(("LoadImage处理结束"));
		}
	}
}

INT MoudleFilterLoad()
{
	KdPrint(("MoudleFilterLoad"));
	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)LoadImageNotifyRoutine);
	g_isMoudleFilterLoad = 1;
	return 0;
}
INT MoudleFilterUnLoad()
{
	KdPrint(("MoudleFilterUnLoad"));

	if (g_isMoudleFilterLoad)
	{
		PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)LoadImageNotifyRoutine);
		g_isMoudleFilterLoad = 0;
	}
	
	return 0;
}