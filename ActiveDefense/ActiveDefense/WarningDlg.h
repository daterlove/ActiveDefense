#pragma once
#include "resource.h"

// WarningDlg 对话框

class WarningDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WarningDlg)

public:
	WarningDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WarningDlg();

// 对话框数据
	enum { IDD = IDD_WARING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAccepct();
	afx_msg void OnBnClickedRefuse();
};
