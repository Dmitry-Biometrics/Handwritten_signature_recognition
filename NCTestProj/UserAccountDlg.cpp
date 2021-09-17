// UserAccountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "UserAccountDlg.h"

#include "../Tools/FileManipulations.h"

// CUserAccountDlg dialog

IMPLEMENT_DYNAMIC(CUserAccountDlg, CDialog)
// �����������
CUserAccountDlg::CUserAccountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserAccountDlg::IDD, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
// ����������
CUserAccountDlg::~CUserAccountDlg() { }

void CUserAccountDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_LOGIN_EDIT, m_nEditLogin );
	DDX_Control( pDX, IDC_PSW_EDIT,   m_nEditPsw );	
  DDX_Control(pDX,  IDC_LOAD_BTN,   m_OpenBtn);
}

BEGIN_MESSAGE_MAP(CUserAccountDlg, CDialog)
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnSaveChangesBtn)
	ON_BN_CLICKED(IDCANCEL, OnCancelBtn)
	ON_BN_CLICKED(IDC_GENKEY_BTN, OnGenKeyBtn)
	ON_BN_CLICKED(IDC_LOAD_BTN, &CUserAccountDlg::OnLoadBtn)
END_MESSAGE_MAP()

// CUserAccountDlg message handlers

// ����������� �������������
BOOL CUserAccountDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// ��������� ������
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  // �������� ������ �� ������ � ����� �� ������� ���
  m_OpenBtn.SetImages( IDI_PATH_2, IDI_PATH );
  m_OpenBtn.SetFrameWidth(SButton::NO_FRAME);

	// ��������� ������������� ���������� �������� ��� ������� Edit'�
	m_nEditLogin.SetLimitText(20);
	m_nEditPsw.SetLimitText(NCKEY_SIZE);
	
	DWORD dwSize;

	// ����� ������ �� ����� � Edit
	if ( objFileManip.ReadLoginFile(_login) != ERROR_SUCCESS ) {
		// ���� ��� ������������ �� ������� � �����, 
		// �� ������� ��� �������� ������������
		dwSize = 21;
		GetUserName( _login, &dwSize ) ;
	}
	 	
	SetDlgItemText(IDC_LOGIN_EDIT, _login);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// ��������� ������
HCURSOR CUserAccountDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

// ������
void CUserAccountDlg::OnCancelBtn() {
	CDialog::OnCancel();
}

// ��������� ���������
void CUserAccountDlg::OnSaveChangesBtn() {
	CString loginStr, pswStr;
	LONG    lErr = ERROR_INVALID_FUNCTION;

  // ������ ���������� ����� ������������
  GetDlgItemText( IDC_LOGIN_EDIT, loginStr );
  // ������ ���������� ������
  GetDlgItemText( IDC_PSW_EDIT, pswStr );

  if (loginStr.IsEmpty() || pswStr.IsEmpty()) {
    return;
  }

  // ���������� ��������� ����� ������������ � ������
	if ( MessageBox (_T("������������ ��� �������� �������� ������?"), _T("���������������� 2.0"), MB_ICONQUESTION|MB_YESNO) != IDYES )
    return;
  ZeroMemory(_login, sizeof(_login));
  for (int i = 0; i < loginStr.GetLength(); i++) {
    _login[i] = loginStr[i];
  }
  ZeroMemory(_psw, sizeof(_psw));
  for (int i = 0; i < pswStr.GetLength(); i++) {
    _psw[i] = pswStr[i];
  }

  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\");
  if (objFileManip.CreateDir(objFileManip.tofileFullPath, _login)) {
    lErr = objFileManip.SaveUserData ( _login, _psw );
  } 

	// ����� ��������� �� �������� / ��������� ��������� ������
	if ( lErr == ERROR_SUCCESS )
		MessageBox( _T("����� ���������� � �������� �������."), _T("������ �����"), MB_ICONASTERISK | MB_OK);
	else
		MessageBox( _T("������ ���������� ������."), _T("�� ������� �������� ������"), MB_ICONWARNING | MB_OK);

	// ���� ��������� ������, ������ � ����. ������ �������
	// ��������� ������� ������
	if ( lErr == ERROR_SUCCESS )
		CDialog::OnOK();
}

// ������������� ������������� ������/����
void CUserAccountDlg::OnGenKeyBtn()
{
	// ���������� ���������(���������) ����
	NCKEY trKey;
	BncGenerateKey ( trKey, NCKEY_SIZE, NC_MODE_LATIN );
	ZeroMemory ( _psw, sizeof(_psw));
	for ( int i = 0; i < NCKEY_SIZE; i++ ) {
		_psw[i] = trKey[i];
	}
	SetDlgItemText ( IDC_PSW_EDIT, _psw );
}

// �������� ����� �� ��������
void CUserAccountDlg::OnClose()
{
	CDialog::OnClose();
}

// ��������� ������
void CUserAccountDlg::OnLoadBtn()
{
  TCHAR szFilters[] = _T("Key Files (*.dat)|*.dat|All Files (*.*)|*.*||");
  CFileDialog fileOpenDlg (TRUE, _T("my"), _T("Key.dat"),
    OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

  if ( fileOpenDlg.DoModal() != IDOK )
    return;
  
	objFileManip.tofileFullPath = fileOpenDlg.GetPathName();
	LONG res = objFileManip.LoadUserData( _login, _psw );
	if (res != ERROR_SUCCESS) {
		MessageBox(_T("����������� ���� � ������"), _T("��������� ������ �� ������"), MB_ICONWARNING);
		return;
	}
	
	SetDlgItemText(IDC_LOGIN_EDIT, _login);
  SetDlgItemText(IDC_PSW_EDIT, _psw);

  CString str;
  GetDlgItemText( IDC_PSW_EDIT, str );
  int i =  str.GetLength();
  i = 0;
}
