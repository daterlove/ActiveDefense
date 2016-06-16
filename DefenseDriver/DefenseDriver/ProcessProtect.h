#ifndef _PROCESS_PROTECT_H_
#define _PROCESS_PROTECT_H_
#include "common.h"
	INT ProtectProcess(PEPROCESS Process, BOOLEAN bIsProtect, ULONG v);
	INT ProcessProcectByName(char *szName);
#endif
