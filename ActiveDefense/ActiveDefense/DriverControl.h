
#define  IOCTL_SEND  0x911// 从应用层给驱动发送一个字符串。
#define  IOCTL_RECV 0x912// 从驱动读取一个字符串
#define  IOCTL_CLOSE 0x913// 应用层关闭信号
#define  IOCTL_PROCESS_PROTECT 0x914// 应用层开启进程保护
#define  IOCTL_PROCESS_UNPROTECT  0x915//应用层关闭进程保护
#define  IOCTL_PROCESS_FILTER 0x916// 应用层开启进程监控
#define  IOCTL_PROCESS_UNFILTER 0x917//应用层关闭进程监控
#define  IOCTL_DRIVER_FILTER  0x918// 应用层开启驱动监控
#define  IOCTL_DRIVER_UNFILTER  0x919//应用层关闭驱动监控


#define CWK_DEV_SYM L"\\\\.\\DefenseDevice"

DWORD CTL_CODE_GEN(DWORD lngFunction);
//szFullPath完整路径、szName服务名、iType操作类型(安装、运行、停止、移除)
BOOL operaType(TCHAR *szFullPath, TCHAR *szName, int iType);
int SendMsgToDriver(int nCode);