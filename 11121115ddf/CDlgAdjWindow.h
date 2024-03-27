#pragma once
#include "afxdialogex.h"


// CDlgAdjWindow 对话框

class CDlgAdjWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAdjWindow)
public:
	CDlgAdjWindow(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgAdjWindow();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ADJWIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_midpoint;
	int m_width;
	virtual BOOL OnInitDialog();
	CSliderCtrl m_CSmidpoint;
	CSliderCtrl m_CSwidth;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
