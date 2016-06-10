#include "stdafx.h"

CString g_str_info;
void ShowInfoInDlg(CString str)
{
	g_str_info = str;
	::SendMessage((AfxGetApp())->m_pMainWnd->m_hWnd, WM_SHOW_MSG, (WPARAM)&g_str_info, 0);
}