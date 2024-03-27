
// 11121115ddfView.cpp: CMy11121115ddfView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11121115ddf.h"
#endif

#include "11121115ddfDoc.h"
#include "11121115ddfView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Dib.h"

// CMy11121115ddfView

IMPLEMENT_DYNCREATE(CMy11121115ddfView, CView)

BEGIN_MESSAGE_MAP(CMy11121115ddfView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy11121115ddfView 构造/析构

CMy11121115ddfView::CMy11121115ddfView() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy11121115ddfView::~CMy11121115ddfView()
{
}

BOOL CMy11121115ddfView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy11121115ddfView 绘图

void CMy11121115ddfView::OnDraw(CDC* pDC)
{
	CMy11121115ddfDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (pDoc->m_pDib != NULL)
		pDoc->m_pDib->Draw(GetDC()->m_hDC, 0, 0);
}


// CMy11121115ddfView 打印

BOOL CMy11121115ddfView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy11121115ddfView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy11121115ddfView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy11121115ddfView 诊断

#ifdef _DEBUG
void CMy11121115ddfView::AssertValid() const
{
	CView::AssertValid();
}

void CMy11121115ddfView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy11121115ddfDoc* CMy11121115ddfView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy11121115ddfDoc)));
	return (CMy11121115ddfDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy11121115ddfView 消息处理程序
