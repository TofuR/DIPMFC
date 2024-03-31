
// 11121115ddfDoc.h: CMy11121115ddfDoc 类的接口
//


#pragma once


class CDib;

class CMy11121115ddfDoc : public CDocument
{
protected: // 仅从序列化创建
	CMy11121115ddfDoc() noexcept;
	DECLARE_DYNCREATE(CMy11121115ddfDoc)

	// 特性
public:

	// 操作
public:

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// 实现
public:
	virtual ~CMy11121115ddfDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	CDib* m_pDib;
	CDib* m_pBuffer;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnInvert();
	afx_msg void OnRestore();
	afx_msg void OnStretching();
	afx_msg void OnBitplane();
	afx_msg void OnHisto();
	afx_msg void OnHistoEqua();
	afx_msg void OnSmoothing();
	afx_msg void OnSobel();
	afx_msg void OnLaplace();
	afx_msg void OnGaussian1D();
	afx_msg void OnGaussian2D();
	afx_msg void OnCTBone();
	afx_msg void OnCTLung();
	afx_msg void OnAdjustwindow();
	afx_msg void OnWhiterect();
	afx_msg void OnAmplitude();
	afx_msg void OnPhase();
	afx_msg void OnIdealLp();
	afx_msg void OnSelcfilter();
};
