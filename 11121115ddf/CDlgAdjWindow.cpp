// CDlgAdjWindow.cpp: 实现文件
//

#include "pch.h"
#include "11121115ddf.h"
#include "afxdialogex.h"
#include "CDlgAdjWindow.h"


// CDlgAdjWindow 对话框

IMPLEMENT_DYNAMIC(CDlgAdjWindow, CDialogEx)

CDlgAdjWindow::CDlgAdjWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_ADJWIN, pParent)
	, m_midpoint(0)
	, m_width(0)
{

}

CDlgAdjWindow::~CDlgAdjWindow()
{
}

void CDlgAdjWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_MIDPOINT, m_midpoint);
	DDX_Slider(pDX, IDC_WIDTH, m_width);
	DDX_Control(pDX, IDC_MIDPOINT, m_CSmidpoint);
	DDX_Control(pDX, IDC_WIDTH, m_CSwidth);
}


BEGIN_MESSAGE_MAP(CDlgAdjWindow, CDialogEx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDlgAdjWindow 消息处理程序


BOOL CDlgAdjWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_CSmidpoint.SetRange(0, 255);
	m_CSmidpoint.SetTicFreq(15);
	m_CSmidpoint.SetPos(0);

	m_CSwidth.SetRange(0, 255);
	m_CSwidth.SetTicFreq(15);
	m_CSwidth.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgAdjWindow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_midpoint = m_CSmidpoint.GetPos();
	m_width = m_CSwidth.GetPos();

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
