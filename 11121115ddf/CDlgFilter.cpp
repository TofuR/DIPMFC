// CDlgFilter.cpp: 实现文件
//

#include "pch.h"
#include "11121115ddf.h"
#include "afxdialogex.h"
#include "CDlgFilter.h"


// CDlgFilter 对话框

IMPLEMENT_DYNAMIC(CDlgFilter, CDialogEx)

CDlgFilter::CDlgFilter(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_FILTER, pParent)
	, m_D0(0)
	, m_N(0)
	, m_Filter(_T(""))
	, m_Type(_T(""))
{

}

CDlgFilter::~CDlgFilter()
{
}

void CDlgFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_D0, m_Edit_D0);
	DDX_Control(pDX, IDC_FILTER_N, m_Edit_N);
	DDX_Text(pDX, IDC_D0, m_D0);
	DDX_Text(pDX, IDC_FILTER_N, m_N);
	DDX_Control(pDX, IDC_Filter, m_Combo_Filter);
	DDX_Control(pDX, IDC_Type, m_Combo_Type);
	DDX_CBString(pDX, IDC_Filter, m_Filter);
	DDX_CBString(pDX, IDC_Type, m_Type);
}


BEGIN_MESSAGE_MAP(CDlgFilter, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgFilter::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_Filter, &CDlgFilter::OnSelchangeFilter)
END_MESSAGE_MAP()


// CDlgFilter 消息处理程序


BOOL CDlgFilter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Edit_D0.SetWindowTextW(L"5");
	m_Edit_N.SetWindowTextW(L"2");
	m_Combo_Filter.SetCurSel(0);
	m_Combo_Type.SetCurSel(0);
	m_D0 = 5;
	m_N = 2;
	m_Filter = "Ideal";
	m_Type = "lowpass";
	m_Edit_N.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgFilter::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
	UpdateData(TRUE);
}


void CDlgFilter::OnSelchangeFilter()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	//获取滤波器类型
	m_Combo_Filter.GetLBText(m_Combo_Filter.GetCurSel(), m_Filter);
	if (m_Filter == "Butterworth")
	{
		m_Edit_N.EnableWindow(TRUE);
	}
	else
	{
		m_Edit_N.EnableWindow(FALSE);
	}
}
