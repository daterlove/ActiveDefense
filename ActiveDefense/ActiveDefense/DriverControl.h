
DWORD CTL_CODE_GEN(DWORD lngFunction);
//szFullPath完整路径、szName服务名、iType操作类型(安装、运行、停止、移除)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType);