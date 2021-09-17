#pragma once
#include "bnc32.h"
#include "afxcmn.h"

// CStabilityDlg dialog

class CStabilityDlg : public CDialog
{
	DECLARE_DYNAMIC(CStabilityDlg)

public:
	CStabilityDlg(CWnd* pParent = NULL);   // standard constructor
	CStabilityDlg(CString fldr, NCKEY *lnKey, NCKEY *mxKey, CWnd* pParent = NULL);
	virtual ~CStabilityDlg();

// Dialog Data
	enum { IDD = IDD_BURDEN_DLG };

protected:

	HICON			m_hIcon;
	CString			folder;			// папка обучающих образов
	NCKEY			*key;			// обучающий ключ
	NCKEY			*mixKey;	    // перемешивающий ключ
	NCKEY			checkKey;		// полученный ключ
	BNCSIGNIMAGE	*trnImages;		// обучающие образы
	BNCSIGNIMAGE	*tstImages;		// тестовые образы (210 образов с временем)
	bool			bStop;			// остановить вычисления
	CString			resFilePath;	// путь к файлу с результатом

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// вывод % выполнения
	void SetTxtAndSetMes ( int iproc );	

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_PrBar1;
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnStopBtn();
	afx_msg void OnClose();
	afx_msg void OnShowResBtn();
};
