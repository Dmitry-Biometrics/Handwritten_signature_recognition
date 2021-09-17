#pragma once
#include "afxwin.h"
#include "bnc32.h"

// CGetKeyDlg dialog	���������� ���� � ������ ��� �������� ��������� ������

class CGetKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetKeyDlg)

public:
	CGetKeyDlg(CWnd* pParent = NULL);   // standard constructor
	// mode - ����� (� ��������� ������ ��� ���)
	// bres - ��������� �������� (������/�������� ����) (TRUE-������, FALSE-��������)
	// secret - TRUE - �� ���������� ����, FALSE - ���������� ���������� ����
	// imLrnKey - ��������� ����
	// imChkKey - ���������� ����
	CGetKeyDlg(int mode, bool bres, bool secret,  NCKEY *learnKey, NCKEY *getKey, CWnd* pParent = NULL);
	virtual ~CGetKeyDlg();

// Dialog Data
	enum { IDD = IDD_KEY_DLG };

protected:
	HICON  m_hIcon;
	HBRUSH m_hWhiteBrush;

	NCKEY *key;				// �������� ����
	NCKEY *checkKey;		// ���������� ����

	TCHAR checkKeyStr	[NEURON_COUNT+1];	// ������ � ���������� ������
	TCHAR checkKeyZvStr [NEURON_COUNT+1];	// ���������� ���� �� ����������
	TCHAR pswStr		[NCKEY_SIZE+1];	// ������ � ���������������� ������

	bool  bTrainMode;		// ����� ��������/������������
	bool  bTruePsw;			// ��������� �� ������/�������� ���������� ������
	bool  bShowKey;			// true - ���������� �������� ���� , false - ���������� ���������� ������
	bool  bSecret;			// ����������/�� ���������� ������
	bool  blinear;
	CString csAddInf, csErrInf;

	// ������� �������� ���� � ����
	void  FillEdits ( TCHAR *psw );

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void    OnClose();
	afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnNotationBtn();
	DECLARE_MESSAGE_MAP()
	
};

