#include "common.h"

#define MEM_TAG 110
int g_number = 0;//链表的全局标号
extern INT g_isRefuse;//指示进程是否被放行

LIST_ENTRY		my_list_head;
KSPIN_LOCK	my_list_lock;


void EventListInit()//链表初始化
{
	InitializeListHead(&my_list_head);//初始化链表头
	KeInitializeSpinLock(&my_list_lock);
}

NTSTATUS AddEventToList(PMY_EVENT pEvent)
{

	ExInterlockedInsertTailList(&my_list_head, pEvent, &my_list_lock);//从链表尾部插入
	return STATUS_SUCCESS;
}
PMY_EVENT RemoveEventFromList()//从链表头部删除,并返回删除的结构体
{
	PLIST_ENTRY pEntry;
	PMY_EVENT pEvent;
	pEntry=ExInterlockedRemoveHeadList(&my_list_head, &my_list_lock);//获取删除的链表地址
	pEvent = CONTAINING_RECORD(pEntry,MY_EVENT, list_enty);//从链表地址计算出MY_EVENT结构体地址

	return pEvent;
}

PMY_EVENT GetEvent()
{
	PLIST_ENTRY pEntry = my_list_head.Flink;
	PMY_EVENT pEvent;

	pEvent = CONTAINING_RECORD(pEntry, MY_EVENT, list_enty);//从链表地址计算出MY_EVENT结构体地址
	KdPrint(("GetEvent:%d", pEvent->nType));
	return pEvent;
}
VOID DeleteAllList()//在关闭的时候释放所有链表内容
{
	KdPrint(("DeleteAllList"));
	g_isRefuse = 0;//进程默认允许运行
	while (!IsListEmpty(&my_list_head))//只要链表不为空
	{
		PMY_EVENT pEvent = RemoveEventFromList();//从链表删除一个事件
		KeSetEvent(pEvent->pProcessEvent, IO_NO_INCREMENT, FALSE);//激活回调进程
		KdPrint(("pEvent:%d", pEvent));
		ExFreePool(pEvent);//释放内存
	}
}
void ShowList()
{
	int i = 0;
	PLIST_ENTRY p;
	for (p = my_list_head.Flink; p != &my_list_head.Flink; p = p->Flink)
	{
		PMY_EVENT pEvent = CONTAINING_RECORD(p, MY_EVENT, list_enty);
		KdPrint(("i:%d,pEvent->type=%d", i++, pEvent->nType));
	}
}


