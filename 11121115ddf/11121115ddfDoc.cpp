
// 11121115ddfDoc.cpp: CMy11121115ddfDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11121115ddf.h"
#endif

#include "11121115ddfDoc.h"

#include <propkey.h>

#include "Dib.h"
#include "CDlgHistogram.h"
#include "CDlgAdjWindow.h"
#include "CDlgFilter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy11121115ddfDoc

IMPLEMENT_DYNCREATE(CMy11121115ddfDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy11121115ddfDoc, CDocument)
	ON_COMMAND(ID_Invert, &CMy11121115ddfDoc::OnInvert)
	ON_COMMAND(ID_Restore, &CMy11121115ddfDoc::OnRestore)
	ON_COMMAND(ID_Stretching, &CMy11121115ddfDoc::OnStretching)
	ON_COMMAND(ID_BitPlane, &CMy11121115ddfDoc::OnBitplane)
	ON_COMMAND(ID_Histo, &CMy11121115ddfDoc::OnHisto)
	ON_COMMAND(ID_Histo_Equa, &CMy11121115ddfDoc::OnHistoEqua)
	ON_COMMAND(ID_Smoothing, &CMy11121115ddfDoc::OnSmoothing)
	ON_COMMAND(ID_Sobel, &CMy11121115ddfDoc::OnSobel)
	ON_COMMAND(ID_Laplace, &CMy11121115ddfDoc::OnLaplace)
	ON_COMMAND(ID_Gaussian1D, &CMy11121115ddfDoc::OnGaussian1D)
	ON_COMMAND(ID_Gaussian2D, &CMy11121115ddfDoc::OnGaussian2D)
	ON_COMMAND(ID_BONE, &CMy11121115ddfDoc::OnCTBone)
	ON_COMMAND(ID_LUNG, &CMy11121115ddfDoc::OnCTLung)
	ON_COMMAND(ID_ADJUSTWINDOW, &CMy11121115ddfDoc::OnAdjustwindow)
	ON_COMMAND(ID_WhiteRect, &CMy11121115ddfDoc::OnWhiterect)
	ON_COMMAND(ID_Amplitude, &CMy11121115ddfDoc::OnAmplitude)
	ON_COMMAND(ID_Phase, &CMy11121115ddfDoc::OnPhase)
	ON_COMMAND(ID_IDEAL_LP, &CMy11121115ddfDoc::OnIdealLp)
	ON_COMMAND(ID_SelcFilter, &CMy11121115ddfDoc::OnSelcfilter)
	ON_COMMAND(ID_Median, &CMy11121115ddfDoc::OnMedian)
END_MESSAGE_MAP()


// CMy11121115ddfDoc 构造/析构

CMy11121115ddfDoc::CMy11121115ddfDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	m_pDib = NULL;
	m_pBuffer = NULL;
}

CMy11121115ddfDoc::~CMy11121115ddfDoc()
{
	if (m_pDib != NULL)
	{
		delete m_pDib;
	}
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
}

BOOL CMy11121115ddfDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMy11121115ddfDoc 序列化

void CMy11121115ddfDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMy11121115ddfDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMy11121115ddfDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMy11121115ddfDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMy11121115ddfDoc 诊断

#ifdef _DEBUG
void CMy11121115ddfDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy11121115ddfDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy11121115ddfDoc 命令


BOOL CMy11121115ddfDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	// 获取文件格式
	if (m_pDib != NULL)
	{
		delete m_pDib;
	}
	m_pDib = new CDib;
	m_pDib->LoadFile(lpszPathName);
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
	m_pBuffer = new CDib(*m_pDib);

	UpdateAllViews(NULL);
	return TRUE;
}


void CMy11121115ddfDoc::OnInvert()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Invert();
		//m_pDib->Invert_Palette();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnRestore()
{
	// TODO: 在此添加命令处理程序代码

	if (m_pDib != NULL)
	{
		delete m_pDib;
		if (m_pBuffer != NULL)
		{
			m_pDib = new CDib(*m_pBuffer);
		}
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnStretching()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Stretching();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnBitplane()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->BitPlane_Slicing();	// 修改形参以改变位平面，default = 7
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnHisto()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDlgHistogram dlg(m_pDib);
		dlg.DoModal();
	}

}


void CMy11121115ddfDoc::OnHistoEqua()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Histogram_Equalization();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnSmoothing()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Smoothing();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnSobel()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Sobel();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnLaplace()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Laplace();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnGaussian1D()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		DWORD t1, t2;
		t1 = GetTickCount64();
		m_pDib->Gaussian1D();
		UpdateAllViews(NULL);
		t2 = GetTickCount64();
		CString str;
		str.Format(_T("Time:%dms"), t2 - t1);
		AfxMessageBox(str, MB_OK | MB_ICONINFORMATION, 0);
	}
}


void CMy11121115ddfDoc::OnGaussian2D()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		DWORD t1, t2;
		t1 = GetTickCount64();
		m_pDib->Gaussian2D();
		UpdateAllViews(NULL);
		t2 = GetTickCount64();
		CString str;
		str.Format(_T("Time:%dms"), t2 - t1);
		AfxMessageBox(str, MB_OK | MB_ICONINFORMATION, 0);
	}
}


void CMy11121115ddfDoc::OnCTBone()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->BoneWindow();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnCTLung()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->LungWindow();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnAdjustwindow()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDlgAdjWindow dlg;
		dlg.m_midpoint = 127.5;
		dlg.m_width = 255;
		if (dlg.DoModal() == IDOK)
		{
			m_pDib->Window_1(dlg.m_midpoint, dlg.m_width);
			UpdateAllViews(NULL);
		}
	}
}


void CMy11121115ddfDoc::OnWhiterect()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
	}
	m_pDib = new CDib;
	m_pDib->CreateWhiteRect(512, 512, 50, 100);
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
	}
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
}


void CMy11121115ddfDoc::OnAmplitude()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Amplitude();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnPhase()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Phase();
		UpdateAllViews(NULL);
	}
}


void CMy11121115ddfDoc::OnIdealLp()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDlgFilter dlg;
		dlg.m_D0 = 5;
		if (dlg.DoModal() == IDOK)
		{
			m_pDib->Filter(L"Ideal", L"lowpass", dlg.m_D0, dlg.m_N);
			UpdateAllViews(NULL);
		}
	}
}


void CMy11121115ddfDoc::OnSelcfilter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		CDlgFilter dlg;
		dlg.m_D0 = 50;
		dlg.m_N = 2;
		if (dlg.DoModal() == IDOK)
		{
			m_pDib->Filter(dlg.m_Filter, dlg.m_Type, dlg.m_D0, dlg.m_N);
			UpdateAllViews(NULL);
		}
	}
}


void CMy11121115ddfDoc::OnMedian()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->MedianFilter();
		UpdateAllViews(NULL);
	}
}
