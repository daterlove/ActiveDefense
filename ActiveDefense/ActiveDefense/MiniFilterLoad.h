#include "stdafx.h"

BOOLEAN LoadWdmDrv(char *inf, char *szDrvSvcName);
BOOLEAN net_start(char *szDrvSvcName);
BOOLEAN net_stop(char *szDrvSvcName);
BOOLEAN UnloadWdmDrv(char *szDrvSvcName);
