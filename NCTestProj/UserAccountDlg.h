#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "bnc32.h"
#include "..\gui\SButton.h"

#include "..\Tools\ChangeUserPassword.h"

// CUserAccountDlg dialog

class CUserAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserAccountDlg)

public:
	CUserAccountDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserAccountDlg();

// Dialog Data
	enum { IDD = IDD_PSW_DLG };

  CEdit   m_nEditLogin;
  CEdit   m_nEditPsw;
  SButton m_OpenBtn;

  // —генерировать пароль
  void GenerateKey(WCHAR *userPsw);

  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSaveChangesBtn();
  afx_msg void OnCancelBtn();
  afx_msg void OnGenKeyBtn();
  afx_msg void OnClose();
  afx_msg void OnLoadBtn();

protected:
	HICON   m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	NCLOGIN _login;		            // введенный пользователем логин
  WCHAR   _psw [NCKEY_SIZE+1];	// введенный пользователем пароль	

	CChangeUserPsw objChangePsw;

	DECLARE_MESSAGE_MAP()

};
