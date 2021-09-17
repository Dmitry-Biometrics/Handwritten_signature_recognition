#pragma once
#include "bnc32.h"
#include "afxcmn.h"

// CBodyBurdenDlg dialog

class CBodyBurdenDlg : public CDialog
{
	DECLARE_DYNAMIC(CBodyBurdenDlg)

public:
	CBodyBurdenDlg(CWnd* pParent = NULL);   // standard constructor
	CBodyBurdenDlg(sessionInfo *sesInf, NCKEY *mxKey, CWnd* pParent = NULL);
	virtual ~CBodyBurdenDlg();

// Dialog Data
	enum { IDD = IDD_BURDEN_DLG };

protected:
	HICON			    m_hIcon;
	sessionInfo   *si;			      // ������ ������ ��������
	NCKEY			    *mixKey;	 	    // �������������� ����
	NCKEY			    checkKey;			  // ���������� ����
	BNCSIGNIMAGE	*trainImages;		// ��������� ������    ��/�����  ��������
	BNCSIGNIMAGE	*checkImages;		// ����������� ������  �����/��  ��������
	BNCSIGNIMAGE	*testImages;		// �������� ������ (1000 �������)
	bool			    bStop;				  // ���������� ����������
	CString			  resFilePath;		// ���� � ����� � �����������
	bool			    bregDO;				  // ���� true, �� ������� �� ������� �� ��������,
										// � ��������� �� ������� ����� ��������.
										// false - ������� �� �����, ��������� �� ��

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// ����� % ����������
	void SetTxtAndSetMes ( int iproc );	
	// ����� ����� ��� �������
	bool OpenAnalizFile  ( CString &csBefore, CString &csAfter, bool &breg );

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_PrBar1;
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnStopBtn();
	afx_msg void OnClose();
	afx_msg void OnShowResBtn();
};
