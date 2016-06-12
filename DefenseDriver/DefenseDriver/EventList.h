#ifndef _EVENT_LIST_H_
#define _EVENT_LIST_H_
#include "common.h"

#define MAX_PATH 512
typedef struct
{
	LIST_ENTRY list_enty;
	INT32	nType; 
	wchar_t wStr[MAX_PATH];
}MY_EVENT, *PMY_EVENT;


void EventListInit();

NTSTATUS AddEventToList(PMY_EVENT pEvent);
PMY_EVENT RemoveEventFromList();//从链表头部删除,并返回删除的结构体
void add();
void remove();

void ShowList();


#endif