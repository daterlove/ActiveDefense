
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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	CTraButton btn_close;
	CTraButton btn_min;
	// 控件信息初始化
	void ControlInit();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMin();
	afx_msg void OnBnClickedLoadDrv();
};
