#pragma once
#include "afxdialogex.h"


// CDlgFilter 对话框

class CDlgFilter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFilter)

public:
	CDlgFilter(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFilter();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit_D0;
	CEdit m_Edit_N;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	double m_D0;
	int m_N;
	CComboBox m_Combo_Filter;
	CComboBox m_Combo_Type;
	afx_msg void OnSelchangeFilter();
	CString m_Filter;
	CString m_Type;
};
