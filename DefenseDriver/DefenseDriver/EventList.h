#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_
#include "common.h"

#define MAX_PATH 512
typedef struct
{
	LIST_ENTRY list_enty;
	INT32	nType; 
	INT32  nLength;//字符串长度
	wchar_t wStr[MAX_PATH];
	PKEVENT pProcessEvent;//用于唤醒进程回调函数的事件
}MY_EVENT, *PMY_EVENT;


void EventListInit();

NTSTATUS AddEventToList(PMY_EVENT pEvent);
PMY_EVENT RemoveEventFromList();//从链表头部删除,并返回删除的结构体
PMY_EVENT GetEvent(); 

void ShowList();


#endif