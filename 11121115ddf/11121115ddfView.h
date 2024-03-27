
// 11121115ddfView.h: CMy11121115ddfView 类的接口
//

#pragma once


class CMy11121115ddfView : public CView
{
protected: // 仅从序列化创建
	CMy11121115ddfView() noexcept;
	DECLARE_DYNCREATE(CMy11121115ddfView)

// 特性
public:
	CMy11121115ddfDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMy11121115ddfView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // 11121115ddfView.cpp 中的调试版本
inline CMy11121115ddfDoc* CMy11121115ddfView::GetDocument() const
   { return reinterpret_cast<CMy11121115ddfDoc*>(m_pDocument); }
#endif

