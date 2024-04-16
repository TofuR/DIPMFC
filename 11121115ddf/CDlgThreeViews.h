#pragma once
#include "afxdialogex.h"


// CDlgThreeViews 对话框

class CDlgThreeViews : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgThreeViews)

public:
	CDlgThreeViews(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgThreeViews();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_THVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
