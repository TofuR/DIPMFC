// CDlgCutoff.cpp: 实现文件
//

#include "pch.h"
#include "11121115ddf.h"
#include "afxdialogex.h"
#include "CDlgCutoff.h"


// CDlgCutoff 对话框

IMPLEMENT_DYNAMIC(CDlgCutoff, CDialogEx)

CDlgCutoff::CDlgCutoff(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CUTOFF, pParent)
	, m_cutoff(0)
{

}

CDlgCutoff::~CDlgCutoff()
{
}

void CDlgCutoff::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CUTOFF, m_cutoff);
	DDX_Control(pDX, IDC_CUTOFF, m_edit_cutoff);
}


BEGIN_MESSAGE_MAP(CDlgCutoff, CDialogEx)
END_MESSAGE_MAP()


// CDlgCutoff 消息处理程序


BOOL CDlgCutoff::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_edit_cutoff.SetWindowTextW(L"150");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
