#pragma once
#include <afxmt.h>//��� ������������� ��������� ������ � ���������
#include "bnc32.h"
#include "../Tools/FileManipulations.h"

// CNCLearningView form view

class CNCLearningView : public CFormView
{
	DECLARE_DYNCREATE(CNCLearningView)

public:
	CNCLearningView();           // protected constructor used by dynamic creation
	virtual ~CNCLearningView();

public:
	enum { IDD = IDD_TRAIN_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	
	CClientDC *m_objClientDC;	// ��������� �� ���������� ������� ����
	HPEN       mypen;					// ����

	BNCSIGNIMAGE *images;		  // ��������� �� ���������� ���������� �������
	BNCSIGNIMAGE *oneImg;		  // ��������� �� ���������� ������ ����������� ������
	BNCSIGNIMAGE *fixImg;		  // ��������� �� ���������� �������������� ������ (��� �������)
	UINT		imageCount;	      // ������� �������
	UINT		coordCount;	      // ������� ���������� ��������� � ����� ��������� ��������� �����, �.�. ���-�� ��������� � ���������� ����� ���������� ���� � �������
	UINT		beginTime;		    // ����� ��������� ����
	bool    bLButDown;		    // ���� - ��������� �� ������� ����� ������� ���� � ���� �����
  sessionInfo si;           // ���������� � ������� ������
	NCKEY   checkKey;			    // ���������� ����
	NCKEY	  mixKey;			      // ���� ��� �������
	NCWEIGHTS weightsArr;     // ���� 	
	NCHASH	  hashKey;			  // ��� ���������� �����

  bool    bErr;             // ������, ���� true, �� ������ �� ���������
	bool    bSecret;			    // ����, ����������� �� ����� ������ (����� ��� ������)
	bool blinear; //���������� �������� ����
	bool    bObvodkaOn;			  // ����� �������
	bool    bFastModOn;			  // ����� �������� ������������� ������� �� ������
	bool	  bSlideShowOn;		  // ����� �����-���
	CString cXemMera;			    // ��� ������ �������� �������������
	
	DECLARE_MESSAGE_MAP()

public:

  bool isReady();

//////////////////////////////////////////////////////////////////////////
	struct THREADDATA	
	{
		HWND pWnd;			// ��������� �� ������������ ����
		UINT count;			// ���������� ���������� �������
		BNCSIGNIMAGE *img;	// ���������� ������
	};	// ������������ ��������������� ������ ������
	THREADDATA thData;

	int m_nTimer;				// ������ ��� ����������� ������� � ������ �����-���
	CWinThread* m_pRecalcWorkerThread;
	LONG OnThreadFinished(UINT wParam, LONG lParam);//�������� ��� ���������� ������
//	CString sMess;
	static CNCLearningView *the_instance;
	static UINT ComputeThreadProc(LPVOID pParam); // �������������� �����
//////////////////////////////////////////////////////////////////////////

	// ����������� ������ ������� ������ ������� �� ���� sPath
	// ���������� ������ �������
	float GetGroupImgWidth(CString sPath);


	bool	bLearnNet;			// true - ��������� ����� ��������, false - ����� �������� �� ����������
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void ClearInputArea();
	afx_msg void CheckImage();
	afx_msg void DelImage();
	afx_msg void DelAllImages();
	afx_msg void AddImage();
	afx_msg void LearnNet();
	afx_msg void OnShowImage();
	afx_msg void OnSecretBtn();
	afx_msg void OnPublicBtn();
	afx_msg void SaveImages();
	afx_msg void LoadImages();
	afx_msg void CheckAllImages();
	afx_msg void OnObvodkaReg();
	afx_msg void OnTestOnTI();
	afx_msg void OnTestOnWN();
	afx_msg void OnUpdateObvodka(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCheckAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveImages(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLoadImages(CCmdUI *pCmdUI);
	afx_msg void OnSetPswM();
	afx_msg void OnProverkaList();
	afx_msg void OnUpdateProverka(CCmdUI *pCmdUI);
	afx_msg void OnBodyBurdenReg();
	afx_msg void OnCracking();
	afx_msg void OnStability();
	afx_msg void OnTIMultiplication();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnUpdateTIMultiplication(CCmdUI *pCmdUI);
public:
	afx_msg void OnMorfing();
public:
	afx_msg void OnGrWidth();
};


