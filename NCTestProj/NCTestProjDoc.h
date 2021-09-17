// NCTestProjDoc.h : interface of the CNCTestProjDoc class
//


#pragma once


class CNCTestProjDoc : public CDocument
{
protected: // create from serialization only
	CNCTestProjDoc();
	DECLARE_DYNCREATE(CNCTestProjDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CNCTestProjDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


