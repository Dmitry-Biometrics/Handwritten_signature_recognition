// NCTestProjDoc.cpp : implementation of the CNCTestProjDoc class
//

#include "stdafx.h"
#include "NCTestProj.h"

#include "NCTestProjDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNCTestProjDoc

IMPLEMENT_DYNCREATE(CNCTestProjDoc, CDocument)

BEGIN_MESSAGE_MAP(CNCTestProjDoc, CDocument)
END_MESSAGE_MAP()


// CNCTestProjDoc construction/destruction

CNCTestProjDoc::CNCTestProjDoc()
{
	// TODO: add one-time construction code here

}

CNCTestProjDoc::~CNCTestProjDoc()
{
}

BOOL CNCTestProjDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CNCTestProjDoc serialization

void CNCTestProjDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CNCTestProjDoc diagnostics

#ifdef _DEBUG
void CNCTestProjDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNCTestProjDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNCTestProjDoc commands
