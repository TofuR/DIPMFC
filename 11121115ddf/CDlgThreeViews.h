#pragma once
#include "afxdialogex.h"
#include "Dib.h"
#include "functions.h"

// CDlgThreeViews 对话框

class CDlgThreeViews : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThreeViews)

public:
	CDlgThreeViews(CDib* pDib = nullptr, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgThreeViews();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_THVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CDib* m_pDib;
	ImageSet m_ImageSet;
	afx_msg void OnPaint();
	CSliderCtrl m_Slider_Tra;
	CSliderCtrl m_Slider_Cor;
	CSliderCtrl m_Slider_Sag;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
