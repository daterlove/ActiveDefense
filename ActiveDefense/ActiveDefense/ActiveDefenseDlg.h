
// ActiveDefenseDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "TraButton.h"
#include "DLEdit.h"


// CActiveDefenseDlg 对话框
class CActiveDefenseDlg : public CDialogEx
{
// 构造
public:
	CActiveDefenseDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ACTIVEDEFENSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMin();
	afx_msg void OnBnClickedLoadDrv();
	afx_msg void OnBnClickedUnloadDrv();
	afx_msg void OnBnClickedProcessProtect();
	afx_msg void OnBnClickedLoadFilter();
	afx_msg void OnBnClickedProcessFilter();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnShowMsg(WPARAM, LPARAM);

	CTraButton btn_close;
	CTraButton btn_min;
	CString m_EDIT_STR;

	// 控件信息初始化
	void ControlInit();
	void EditBoxToBottom(void);
	void ShowInText(CString *str);

	
	

};
