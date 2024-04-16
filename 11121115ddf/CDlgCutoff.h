#pragma once
#include "afxdialogex.h"


// CDlgCutoff 对话框

class CDlgCutoff : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCutoff)

public:
	CDlgCutoff(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgCutoff();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CUTOFF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_cutoff;
	CEdit m_edit_cutoff;
	virtual BOOL OnInitDialog();
};
