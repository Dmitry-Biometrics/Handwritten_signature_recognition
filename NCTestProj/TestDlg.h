#pragma once
#include "bnc32.h"
#include "afxcmn.h"

// CTestDlg dialog

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	// regime	  - [in] ����� ������ ���� ������������: NCTEST_MODE_TIWN ��� NCTEST_MODE_MORFING
	// bOnTI	  - [in] TRUE - ����������� �� �������� �������, FALSE - �� ����� ����
	// fldr		  - [in] ���� � ����� � ��������� ��������
	// tip		  - [in] ���������� ������ � ��������
	// sesInf   - [in] ������ ������ ��������
	// mxKey	  - [in] �������������� ����
	// weights  - [in] ���� ��������� ����
	CTestDlg(UINT regime, BOOL bOnTI, CString fldr, CString tip, sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghts, CWnd* pParent = NULL);
	virtual ~CTestDlg();

// Dialog Data
	enum { IDD = IDD_TST_DLG };

protected:
	HICON		      m_hIcon;
	int			      mode;			  // ����� ������ ���� ������������: NCTEST_MODE_TIWN ��� NCTEST_MODE_MORFING
	bool		      bStart;			// ��������� �� ������ ������ ����/����� (������ ������������)
	bool		      bStop;			// ���������� ������������
	BOOL		      bTestOnTI;  // TRUE - ����������� �� �������� �������, FALSE - �� ����� ����
	CString		    folder;			// ����� � �����������
	CString		    type;			  // ��� ������ ���������� (���� .dat)
	sessionInfo   *si;			  // ������ ������ ��������
	NCKEY		      *mixKey;	  // �������������� ����
	NCWEIGHTS	    *weights;   // ���� 

//////////////////////////////////////////////////////////////////////////
	// ������ ������ ������� (����� ��������� ���� ��������)
	struct pth_xem_list  {
		int		mera;			// ���� �������� �� "�������" ������
		int		num;			// ����� "��������/��������/��������" ������
		WCHAR	path[MAX_PATH];	// ���� � �������� �������
	};
	CList<pth_xem_list, pth_xem_list&> bestImgsList; // ��������������� �� ����������� ���� ������ ������
	// ������ ������ ������� (������������ � ����� ��������� ����� ��������)
	struct mera_coef_list {
		int			mera;	// ���� �������� ��� ������
		NCFOURIER	coef;	// ������������ �����
	};
	CList<mera_coef_list, mera_coef_list&> bestCoefList; // ��������������� �� ����������� ���� ������ ����-�� �������
//////////////////////////////////////////////////////////////////////////
	
	void TestONTI();		// ����������� �� ������� �� ���� ��� ����� ����
	void TestOnMultiTI();	// ����������� �� ������� ���������� ������� � ����������� ����� ��������
	// ����� �� ����� ���������� ����������� ������ � �������
	void UpdateCount ( int percent, int fileCount, int imageCount );
	// �������� ������ pth_xem_list � ������� �������� ������������� �� ����������� �� ����� ��������
	// path		- [in] ���� � �������� �������
	// count	- [in] ���������� �������� �������
	// xemArr	- [in] ������ ��� �������� ��� �������� �������
	void GetSortPXList(CString path, int count, int xemArr[] );

	// �������� ������ mera_coef_list � ������� �������� ������������� �� ����������� �� ����� ��������
	// ���������� ������ bestCoefList
	// count	- [in] ���������� �������� ������� (���������� �� ����������)
	// coefs	- [in] ������������ ����� ����������� �������
	// xemArr	- [in] ������ ��� �������� ��� �������� �������
	// maxSize	- [in] ������������ ������ ���������������� ������ 
	void GetSortMCList ( int count, float coefs[], int xemArr[], int maxSize );

	// ���������� �������� ������������ ����� � ���������
	// �� ��������������� ������� (�������������), �����
	// �������� �������� ������� � "������" ������
	// coefs - [in/out] �������� ������������ ����� (�� ����� ������ ������� ���������������� ������ �������������)
	// count - [in] ���������� �������� �������
	BNCMOMENTS MorfingAndSorting ( float coefs[], UINT count );

	// �������� ���� �������� ������� �������
	// count	- [in] ���������� �������� ������� (�.�. �������� ����� ��������� ����)
	// coefs	- [in] ������������ ����� ������������� �������
	// meraArr	- [out] ������ � ������������ ����� �������� ��������
	void GetSecondOrderMera (int count, float coefs[], int meraArr[]);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_PrBar2;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCloseBtn();
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnMcdBtn();
};
