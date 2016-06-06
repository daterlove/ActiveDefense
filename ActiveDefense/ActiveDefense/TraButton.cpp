
#include "stdafx.h"
#include "TraButton.h"

CTraButton::CTraButton()
{
	m_btninfoNor.pImg = m_btninfoHot.pImg = m_btninfoPre.pImg = m_btninfoDis.pImg = NULL;//底图
	m_bHot = false;//是否为Hot
	m_bPress = false;//是否按下
	m_bAutoSize = true;//自适应
}

CTraButton::~CTraButton()
{
	if (m_btninfoNor.pImg != NULL)
	{
		delete m_btninfoNor.pImg;
		m_btninfoNor.pImg = NULL;
	}
	if (m_btninfoHot.pImg != NULL)
	{
		delete m_btninfoHot.pImg; 
		m_btninfoHot.pImg = NULL;
	}
	if (m_btninfoPre.pImg != NULL)
	{
		delete m_btninfoPre.pImg;
		m_btninfoPre.pImg = NULL;
	}
	if (m_btninfoDis.pImg!= NULL)
	{
		delete m_btninfoDis.pImg;
		m_btninfoDis.pImg = NULL;
	}
}


BEGIN_MESSAGE_MAP(CTraButton, CButton)
	//{{AFX_MSG_MAP(CTraButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//作用：载入背景图片
void CTraButton::Load(UINT IDBkGroup,int width, int height, const CString& resourceType)
{
	CImage orgImg;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst,MAKEINTRESOURCE(IDBkGroup),resourceType);
	if (hRsrc == NULL)
		return;

	//讲资源加载到内存中
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (lpRsrc == NULL)
		return;

	//为流申请资源
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);

	//加载流
	orgImg.Load(pstm);

	//释放资源
	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);

	if (resourceType == _T("PNG"))
	{
		if (orgImg.GetBPP() == 32)
		{
			//PNG透明
			for(int i = 0; i < orgImg.GetWidth(); i++)  
			{  
				for(int j = 0; j < orgImg.GetHeight(); j++)  
				{  
					unsigned char* pucColor = reinterpret_cast<unsigned char *>(orgImg.GetPixelAddress(i , j));  
					pucColor[0] = pucColor[0] * pucColor[3] / 255;  
					pucColor[1] = pucColor[1] * pucColor[3] / 255;  
					pucColor[2] = pucColor[2] * pucColor[3] / 255;  
				}  
			}
		}
	}

	if (width==0 && height==0)
	{
		width=orgImg.GetHeight();//方形
	}
	if (height==0)
	{
		height = orgImg.GetHeight();
	}

	m_btninfoNor.nWidth = width;
	m_btninfoNor.nHeight = height;
	m_btninfoHot.nWidth = width;
	m_btninfoHot.nHeight = height;
	m_btninfoPre.nWidth = width;
	m_btninfoPre.nHeight = height;
	m_btninfoDis.nWidth = width;
	m_btninfoDis.nHeight = height;
	CImage** imgs[]={&m_btninfoNor.pImg,&m_btninfoHot.pImg,&m_btninfoPre.pImg,&m_btninfoDis.pImg};
	int posX=0;
	for (int i=0;i<4 && posX<=(orgImg.GetWidth()-width);i++,posX+=width)
	{
		CImage* pMap=new CImage();
		if (*imgs[i] != NULL)
		{
			delete *imgs[i];
			*imgs[i] = NULL;
		}
		*imgs[i]=pMap;
 
		if (resourceType == _T("PNG"))
		{
			BOOL bStat = FALSE;
			if (orgImg.GetBPP() == 32)
			{
				bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL, CImage::createAlphaChannel);  
			}else
			{
				bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
			}	
			ASSERT(bStat); 
		}else
		{
			BOOL bStat = pMap->CreateEx(width, height,orgImg.GetBPP(),BI_RGB,NULL);  
			ASSERT(bStat);  
		}
	
		CImageDC imageDC(*pMap);
		orgImg.Draw(imageDC,0,0,width,height,posX,0,width,height);
	}
}

//重绘
void CTraButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;
	CString strText = _T("");
	GetWindowText(strText);

	//绘图
	if(lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		DrawBK(*pDC, m_btninfoDis.pImg, TRA_BTN_DIS);
	}
	else if(lpDrawItemStruct->itemState & ODS_SELECTED
		||(m_bHot && m_bPress) )
	{
		DrawBK(*pDC, m_btninfoPre.pImg, TRA_BTN_PRE);
	}
	else if(m_bHot)
	{
		DrawBK(*pDC, m_btninfoHot.pImg, TRA_BTN_HOT);
	}
	else
	{
		DrawBK(*pDC, m_btninfoNor.pImg, TRA_BTN_NOR);
	}
	//写字
	CString strTemp(strText);
	strTemp.Remove(' ');
	if (!strTemp.IsEmpty())
	{
		if(lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			DrawBtnText(*pDC,strText,0, TRA_BTN_DIS);
		}
		else if(lpDrawItemStruct->itemState & ODS_SELECTED
			||(m_bHot && m_bPress) )
		{
			DrawBtnText(*pDC, strText,1, TRA_BTN_PRE);
		}
		else if(m_bHot)
		{
			DrawBtnText(*pDC, strText,0, TRA_BTN_HOT);
		}
		else
		{
			DrawBtnText(*pDC, strText,0, TRA_BTN_NOR);
		}

	}
}

//写字
void CTraButton::DrawBtnText(HDC dc, const CString& strText, int nMove, TRA_BTNSTATE btnstate)
{
	///*
	CSize sizeText = CDC::FromHandle(dc)->GetTextExtent(strText);
	CRect rect;
	GetClientRect(&rect);

	rect.DeflateRect(nMove,nMove,0,0);
	CDC::FromHandle(dc)->SetBkMode(TRANSPARENT);

	if (btnstate == TRA_BTN_NOR)
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(30,30,30));
	}else if (btnstate == TRA_BTN_HOT)
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(30,30,30));
	}else if (btnstate == TRA_BTN_PRE)
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(30,30,30));
	}else
	{
		CDC::FromHandle(dc)->SetTextColor(RGB(100,100,100));
	}
	CDC::FromHandle(dc)->DrawText(strText, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER); 
	//*/
}

//绘图
void CTraButton::DrawBK(HDC dc, CImage* img, TRA_BTNSTATE btnstate)
{
	if (!img)
		return;

	CRect rc;
	GetClientRect(&rc);
	CRect tmpRect;
	int nX = 0;
	int nY = 0;
	int nW = 0;
	int nH = 0;

	if (m_bAutoSize == true)
	{
		tmpRect.SetRect(0,0,rc.Width(), rc.Height());
		if (img)
			img->Draw(dc,tmpRect);
	}else
	{

		if(btnstate == TRA_BTN_NOR)
		{
			nW = m_btninfoNor.nWidth;
			nH = m_btninfoNor.nHeight;

		}else if (btnstate == TRA_BTN_HOT)
		{
			nW = m_btninfoHot.nWidth;
			nH = m_btninfoHot.nHeight;

		}else if (btnstate == TRA_BTN_PRE)
		{
			nW = m_btninfoPre.nWidth;
			nH = m_btninfoPre.nHeight;

		}else
		{
			nW = m_btninfoDis.nWidth;
			nH = m_btninfoDis.nHeight;
		}

		nX = (rc.Width()-nW)/2;
		nY = (rc.Height()-nH)/2;
		tmpRect.SetRect(nX,nY,nW+nX, nH+nY);
		if (img)
			img->Draw(dc,tmpRect);
	}
}


//鼠标移动
void CTraButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(rect);
	if(rect.PtInRect(point))	
	{
		if(!(nFlags & MK_LBUTTON))
			m_bPress = FALSE;
		if(GetCapture() != this)
		{			
			SetCapture();
		}
		if (m_bHot == TRUE)
		{
			//
		}else
		{
			m_bHot = TRUE;
			SetFocus();
			Invalidate();
			CRect rcTemp(rect);
			ClientToScreen(&rcTemp);
			CPoint pointTemp(rcTemp.left, rcTemp.top);
			::ScreenToClient(GetParent()->GetSafeHwnd(), &pointTemp);
			rcTemp.SetRect(pointTemp.x, pointTemp.y, pointTemp.x+rect.Width(), pointTemp.y+rect.Height());

			//加入这句虽然支持透明图片，但是有时候会闪屏，所以就去掉
			//GetParent()->InvalidateRect(rcTemp, TRUE);		
		}
	}
	else
	{
		ReleaseCapture();
		m_bHot = FALSE;
		Invalidate();
		CRect rcTemp(rect);
		ClientToScreen(&rcTemp);
		CPoint pointTemp(rcTemp.left, rcTemp.top);
		::ScreenToClient(GetParent()->GetSafeHwnd(), &pointTemp);
		rcTemp.SetRect(pointTemp.x, pointTemp.y, pointTemp.x+rect.Width(), pointTemp.y+rect.Height());
		//加入这句虽然支持透明图片，但是有时候会闪屏，所以就去掉
		//GetParent()->InvalidateRect(rcTemp, TRUE);
	}
	CButton::OnMouseMove(nFlags, point);
}

//鼠标抬起
void CTraButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bPress)
	{
		CRect rect;
		GetClientRect(rect);
		Invalidate();
		CRect rcTemp(rect);
		ClientToScreen(&rcTemp);
		CPoint pointTemp(rcTemp.left, rcTemp.top);
		::ScreenToClient(GetParent()->GetSafeHwnd(), &pointTemp);
		rcTemp.SetRect(pointTemp.x, pointTemp.y, pointTemp.x+rect.Width(), pointTemp.y+rect.Height());
		GetParent()->InvalidateRect(rcTemp, TRUE);
		m_bPress = FALSE;	
	}
	m_bHot = FALSE;
	CButton::OnLButtonUp(nFlags, point);
}

//鼠标落下
void CTraButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(rect);
	CRect rcTemp(rect);
	ClientToScreen(&rcTemp);
	CPoint pointTemp(rcTemp.left, rcTemp.top);
	::ScreenToClient(GetParent()->GetSafeHwnd(), &pointTemp);
	rcTemp.SetRect(pointTemp.x, pointTemp.y, pointTemp.x+rect.Width(), pointTemp.y+rect.Height());
	GetParent()->InvalidateRect(rcTemp, TRUE);
	m_bPress = TRUE;	
	CButton::OnLButtonDown(nFlags, point);
}

//背景
BOOL CTraButton::OnEraseBkgnd(CDC* pDC) 
{
	if(GetButtonStyle() & BS_OWNERDRAW)
		return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}

//自适应背景图
void CTraButton::SetAutoSize(bool bAutoSize)
{
	m_bAutoSize = bAutoSize;
}

//按钮风格
void CTraButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);//设置自绘
	CButton::PreSubclassWindow();
}

