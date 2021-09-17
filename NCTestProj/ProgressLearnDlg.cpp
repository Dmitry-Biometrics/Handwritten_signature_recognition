// ProgressLearnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "ProgressLearnDlg.h"
#include "../Tools/FileManipulations.h"

// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
}
// ������������� �����������
CProgressDlg::CProgressDlg(int imgsCnt, BNCSIGNIMAGE imgs[], sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghtsArr, CWnd* pParent /* = NULL */)
:CDialog(CProgressDlg::IDD, pParent) {
	m_hIcon   = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mode      = 1;
	imgCount  = imgsCnt;			// ���������� ��������� ������� 
	images    = imgs;					// ���������� ���������� �������
	si        = sesInf;				// ��������� ����
	mixKey    = mxKey;				// �������������� ����
	weights   = wghtsArr;			// ������ ����� ����
	stabGroup = 8;
	
}

CProgressDlg::~CProgressDlg() { }

void CProgressDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, OnClose)
	ON_BN_CLICKED(IDC_linear, &CProgressDlg::OnLinear)
END_MESSAGE_MAP()


// CProgressLearnDlg message handlers

BOOL CProgressDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	//��������� ������
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control

}

// ��������� �����
void CProgressDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting

	if ( mode == 1 ) {
		ProgressNeuroNetTrain( stabGroup );
		mode++;
	}
}

// �������, ��������� ��������� ���� � ��������� ��������������� ��������� ���������
void CProgressDlg::ProgressNeuroNetTrain(int iStability)
{
//////////////////////////////////////////////////////////////////////////
//	DWORD dwTime1 = ::GetTickCount();// ����� ������ ��������
//	int iSecond;					 // ����� �������� � ��������
//////////////////////////////////////////////////////////////////////////
	//CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK8);
	//int ChkBox = m_ctlCheck->GetCheck();

	//if (ChkBox == BST_CHECKED)

	//if (IDC_CHECK8->Checked == true)
	//if (IDC_CHECK8 == BST_CHECKED)
	//if (IDC_CHECK1.Checked == true)

	//CheckRadioButton(IDC_linear, IDC_garm, IDC_garm);


//	if (IDC_linear->Checked == true)

	///CheckRadioButton(IDC_linear, IDC_garm, IDC_garm);
	if (linear_garm == true) 
		MessageBox(L"Checkbox marked", L"", MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(L"������� ����", L"", MB_OK | MB_ICONINFORMATION);


		{
		CString headString;

		// ����� ��������� �� �������� ����
		headString = _T("���������, ���� ������� �������� ����...");
		SetWindowText(headString);
		SetDlgItemText(IDC_OUTPUTTEXT, headString);

		stabGroup = iStability;
		// �������� ��������� ����
		//	BncNeuroNetTrain ( si->key, *mixKey, imgCount, images, *weights, &stabGroup ); 
		objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\");


		BncNeuroNetTrain_2(objFileManip.tofileFullPath, si->key, *mixKey, imgCount, images, *weights, &stabGroup);

		// ���������� ��������� ����
		objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\Sign.bnc");
		// ����������� ���������� �����
		NCHASH hash;
		BncCreateHash(si->key, sizeof(NCKEY), hash);
		objFileManip.SaveBNC(objFileManip.tofileFullPath, *weights, hash);

		headString = _T("���������� �������� ��������� ����.");
		SetWindowText(headString);

		// ���������� ����������� ����������� "������"
		float fmyver;
		fmyver = (imgCount * 100) / (float)(imgCount + 1);

		// ���������� ����������� ����������� "������"
		//	��������� ��������� ���� � ������ ������� � ����������� �� ������:
		// ���������
		//	7 ������:	�� 8 �� 12
		//	6 ������:	�� 12 �� 15
		//	5 ������:	�� 15 �� 19
		//	4 ������:	�� 19 �� 23
		//	3 ������:	�� 23 �� 28
		//	2 ������:	�� 28 �� 33
		//	1 ������:	o� 33 �� 35
		CString str;
		switch (stabGroup) {
		case 1: str = "e-33 - e-35."; break;
		case 2: str = "e-28 - e-33."; break;
		case 3: str = "e-23 - e-28."; break;
		case 4: str = "e-19 - e-23."; break;
		case 5: str = "e-15 - e-19.";  break;
		case 6: str = "e-12 - e-15.";   break;
		case 7: str = "e-8 - e-12.";   break;
		}

		// ����� ���������� ���������
		headString.Format(_T("You are in %d stability group.\n The likelihood of recognizing your %.0f"), stabGroup, fmyver);
		headString += _T("% from 1 try.\n Probability of passing someone else");
		headString += str;

		// ����������� ������������ ���������� ���������
		headString += _T("\n\n If you are satisfied with these characteristics, then you should exit the training mode, if not, then follow one of the following recommendations: \n 1. Add additional training examples and retrain the network; \n 2. You can try to change the password word by deleting all the previous examples.");
		SetDlgItemText(IDC_OUTPUTTEXT, headString);

		// �������� CheckBox ������ ������������ � ������� ��������� ������������
		switch (stabGroup) {
		case 1: CheckDlgButton(IDC_CHECK1, BST_CHECKED); break;
		case 2: CheckDlgButton(IDC_CHECK2, BST_CHECKED); break;
		case 3: CheckDlgButton(IDC_CHECK3, BST_CHECKED); break;
		case 4: CheckDlgButton(IDC_CHECK4, BST_CHECKED); break;
		case 5: CheckDlgButton(IDC_CHECK5, BST_CHECKED); break;
		case 6: CheckDlgButton(IDC_CHECK6, BST_CHECKED); break;
		case 7: CheckDlgButton(IDC_CHECK7, BST_CHECKED); break;
		}
	}
	
//////////////////////////////////////////////////////////////////////////
//	iSecond = (::GetTickCount() - dwTime1)/1000;
//////////////////////////////////////////////////////////////////////////

}

// �������
void CProgressDlg::OnClose()
{
	OnOK();
}

void CProgressDlg::OnLinear()
{
	linear_garm = true;
}
