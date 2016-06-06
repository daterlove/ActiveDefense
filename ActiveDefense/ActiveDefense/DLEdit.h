#pragma once

/*
											CDLEdit 2012年2月3日14:07:01
受到网友大牛们的启发，自己也操刀跃跃欲试。
基类：CEdit
实现功能：实现了分别为FOCUS_DEFAULT，FOCUS_BKCOLOR，FOCUS_FONTBOLD，
					FOCUS_SHADOW四种用户输入焦点风格；悬停边框高亮功能（与qq登录
					窗口的编辑框相类似）；可以自定义字体和字体的大小。
尚未实现的功能：密码原点密文（不是*），编辑框自定义背景，还有大家都很熟悉
								的鼠标光标风格；自定义圆角边框。

启发收获：可真应了大牛说的：设计模式很重要。即便对于这么小的一个工程来说，
					如果熟悉设计模式的人，在MFC基础扎实过硬的基础下，可以节省额很多
					的时间，提高程序设计效率。
*/

// CDLEdit
enum				//枚举焦点风格
{
	FOCUS_DEFAULT,
	FOCUS_BKCOLOR,
	FOCUS_FONTBOLD,
	FOCUS_SHADOW
};

class CDLEdit : public CEdit
{
	DECLARE_DYNAMIC(CDLEdit)

public:
	CDLEdit();
	virtual ~CDLEdit();

//编辑框需要的元素
public:
	CRect m_rctEdit;		//编辑框rct，screen数据而非client数据
	CFont m_Font;			//字体

	BOOL m_bHover;		//is悬停
	BOOL m_bTrack;		//is检测
	BOOL m_bUser;		//is被用户修改，setWindowText都会触发change消息
	BOOL m_bEmpty;		//is文本为空

	COLORREF m_clrShadow;				//阴影	
	COLORREF m_clrBkGndNormal;		//默认输入焦点底色
	COLORREF m_clrBkGndFocus;		//输入焦点底色
	COLORREF m_clrBorderNormal;		//默认边框条颜色
	COLORREF m_clrBorderHorver;		//悬停边框条颜色
	COLORREF m_clrBorderEdit;			//编辑框框条颜色

	int m_nFocusStyle;						//焦点风格
	CString m_strGrayString;

	CString m_strFaceName;
	int m_nPointSize;

protected:
	DECLARE_MESSAGE_MAP()

//风格定制函数
public:
	void SetFont(int nPointSize, LPCTSTR lpszFaceName);
	void SetFocusStyle(int nStyle = FOCUS_DEFAULT);
	void SetBkGndColor(
		COLORREF clrNormal = RGB(255,255,255),
		COLORREF clrFocus = RGB(255,255,255)
		);
	void SetBorderColor(
		COLORREF clrBorderEdit = RGB(0xEA,0xEA,0xEA),
		COLORREF clrBorderNormal = RGB(51, 139, 192),
		COLORREF clrBorderHover = RGB(96, 200, 253));
	void SetShadowColor(COLORREF clrShadow = RGB(0xB8,0xB8,0xB8));
	void SetGrayString(CString str);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnChange();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


