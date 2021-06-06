﻿
// SolarSystemView.h: CSolarSystemView 클래스의 인터페이스
//

#pragma once



class CSolarSystemView : public CView
{
protected: // serialization에서만 만들어집니다.
	CSolarSystemView() noexcept;
	DECLARE_DYNCREATE(CSolarSystemView)

	// 특성입니다.
public:
	CSolarSystemDoc* GetDocument() const;

	HDC m_hDC;  // Windows 버전의 Device Context
	HGLRC m_hglRC; // OpenGL 버전의 Device Context

	int m_texNum;
	CWordArray m_globTexture;
	int m_view_type;
	enum VIEW_TYPE {
		VIEW_DEFAULT,
		VIEW_TEXTURE,
	};

	bool ssTimer;

	// 작업입니다.
public:
	void InitGL(void);
	void ReSizeGLScene(GLsizei width, GLsizei height);
	void DrawGLScene(void);
	void Display();
	void RotateSphere(float rot);
	void DrawSphere2();
	void drawSphere(double r, int lats, int longs);

 // 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 구현입니다.
public:
	virtual ~CSolarSystemView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	BOOL SetPixelformat(HDC hdc);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
};

#ifndef _DEBUG  // SolarSystemView.cpp의 디버그 버전
inline CSolarSystemDoc* CSolarSystemView::GetDocument() const
{
	return reinterpret_cast<CSolarSystemDoc*>(m_pDocument);
}
#endif