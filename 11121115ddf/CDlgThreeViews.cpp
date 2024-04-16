// CDlgThreeViews.cpp: 实现文件
//

#include "pch.h"
#include "11121115ddf.h"
#include "afxdialogex.h"
#include "CDlgThreeViews.h"
#include "Dib.h"


// CDlgThreeViews 对话框

IMPLEMENT_DYNAMIC(CDlgThreeViews, CDialogEx)

CDlgThreeViews::CDlgThreeViews(CDib* pDib /*=nullptr*/, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_THVIEW, pParent)
{
	m_pDib = pDib;
	m_ImageSet = m_pDib->TovectorMatrix();
}

CDlgThreeViews::~CDlgThreeViews()
{
}

void CDlgThreeViews::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLI_TRA, m_Slider_Tra);
	DDX_Control(pDX, IDC_SLI_COR, m_Slider_Cor);
	DDX_Control(pDX, IDC_SLI_SAG, m_Slider_Sag);
}


BEGIN_MESSAGE_MAP(CDlgThreeViews, CDialogEx)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDlgThreeViews 消息处理程序


void CDlgThreeViews::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	CRect rect;

	CWnd* pWnd = GetDlgItem(IDC_TXT_TRA);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	CDC* pDC = &dc;
	pWnd->UpdateWindow();
	pDC->Rectangle(rect);
	m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());

	pWnd = GetDlgItem(IDC_TXT_COR);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	pWnd->UpdateWindow();
	pDC->Rectangle(rect);
	m_pDib->Show(Coronal(m_ImageSet, 250, 458, 512));
	m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());

	pWnd = GetDlgItem(IDC_TXT_SAG);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	pWnd->UpdateWindow();
	pDC->Rectangle(rect);
	m_pDib->Show(Sagittal(m_ImageSet, 250, 458, 512));
	m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());
}


void CDlgThreeViews::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	CDC* pDC = GetDC();
	if (pScrollBar == (CScrollBar*)&m_Slider_Tra)
	{
		int num = m_Slider_Tra.GetPos();
		CWnd* pWnd = GetDlgItem(IDC_TXT_TRA);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->UpdateWindow();
		pDC->Rectangle(rect);
		m_pDib->Show(Transverse(m_ImageSet, num, 512, 512));
		m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());
	}
	else if (pScrollBar == (CScrollBar*)&m_Slider_Cor)
	{
		int num = m_Slider_Cor.GetPos();
		CWnd* pWnd = GetDlgItem(IDC_TXT_COR);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->UpdateWindow();
		pDC->Rectangle(rect);
		m_pDib->Show(Coronal(m_ImageSet, num, 458, 512));
		m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());
	}
	else if (pScrollBar == (CScrollBar*)&m_Slider_Sag)
	{
		int num = m_Slider_Sag.GetPos();
		CWnd* pWnd = GetDlgItem(IDC_TXT_SAG);
		pWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		pWnd->UpdateWindow();
		pDC->Rectangle(rect);
		m_pDib->Show(Sagittal(m_ImageSet, num, 458, 512));
		m_pDib->Draw(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height());
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CDlgThreeViews::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Slider_Tra.SetRange(0, m_ImageSet.size() - 1);
	m_Slider_Cor.SetRange(0, m_ImageSet[0].size() - 1);
	m_Slider_Sag.SetRange(0, m_ImageSet[0][0].size() - 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
