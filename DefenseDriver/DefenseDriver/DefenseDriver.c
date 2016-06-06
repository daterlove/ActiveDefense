#include "ntddk.h"
VOID MyUnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	KdPrint(("Unload"));
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	KdPrint(("Driver load"));
	pDriverObject->DriverUnload = MyUnloadDriver;
	return STATUS_SUCCESS;
}