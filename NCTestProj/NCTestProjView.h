// NCTestProjView.h : interface of the CNCTestProjView class
//


#pragma once
#include "NCTestProjDoc.h"

class CNCTestProjView : public CFormView
{
public: // create from serialization only
	CNCTestProjView();
	DECLARE_DYNCREATE(CNCTestProjView)

public:
	enum{ IDD = IDD_FIRST_FORM };

// Attributes
public:
	CNCTestProjDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CNCTestProjView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	CDC     dcMem;
	CBitmap m_cBmp;
	UINT    m_iBmpHeight;
	UINT    m_iBmpWidth;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in NCTestProjView.cpp
inline CNCTestProjDoc* CNCTestProjView::GetDocument() const
   { return reinterpret_cast<CNCTestProjDoc*>(m_pDocument); }
#endif

