#pragma once
#include "afxwin.h"
#include "bnc32.h"

// CGetKeyDlg dialog	Показывает окно с ключом при проверке введённого образа

class CGetKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetKeyDlg)

public:
	CGetKeyDlg(CWnd* pParent = NULL);   // standard constructor
	// mode - режим (с обучающим ключом или нет)
	// bres - результат проверки (верный/неверный ключ) (TRUE-верный, FALSE-неверный)
	// secret - TRUE - не отображаем ключ, FALSE - показываем полученный ключ
	// imLrnKey - обучающий ключ
	// imChkKey - полученный ключ
	CGetKeyDlg(int mode, bool bres, bool secret,  NCKEY *learnKey, NCKEY *getKey, CWnd* pParent = NULL);
	virtual ~CGetKeyDlg();

// Dialog Data
	enum { IDD = IDD_KEY_DLG };

protected:
	HICON  m_hIcon;
	HBRUSH m_hWhiteBrush;

	NCKEY *key;				// исходный ключ
	NCKEY *checkKey;		// полученный ключ

	TCHAR checkKeyStr	[NEURON_COUNT+1];	// строка с полученным ключом
	TCHAR checkKeyZvStr [NEURON_COUNT+1];	// полученный ключ со звёздочками
	TCHAR pswStr		[NCKEY_SIZE+1];	// строка с пользовательским паролём

	bool  bTrainMode;		// режим обучения/тестирования
	bool  bTruePsw;			// указывает на верный/неверный рукописный пароль
	bool  bShowKey;			// true - отображать двоичный ключ , false - отображать символьный пароль
	bool  bSecret;			// показывать/не показывать пароль
	bool  blinear;
	CString csAddInf, csErrInf;

	// Выводим двоичный ключ в окне
	void  FillEdits ( TCHAR *psw );

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void    OnClose();
	afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnNotationBtn();
	DECLARE_MESSAGE_MAP()
	
};

