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
	sessionInfo   *si;			      // данные сессии обучения
	NCKEY			    *mixKey;	 	    // перемешивающий ключ
	NCKEY			    checkKey;			  // полученный ключ
	BNCSIGNIMAGE	*trainImages;		// обучающие образы    до/после  нагрузки
	BNCSIGNIMAGE	*checkImages;		// проверочные образы  после/до  нагрузки
	BNCSIGNIMAGE	*testImages;		// тестовые образы (1000 образов)
	bool			    bStop;				  // остановить вычисления
	CString			  resFilePath;		// путь к файлу с результатом
	bool			    bregDO;				  // если true, то обучаем на образах ДО нагрузки,
										// а тестируем на образах ПОСЛЕ нагрузки.
										// false - обучаем на ПОСЛЕ, тестируем на ДО

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// вывод % выполнения
	void SetTxtAndSetMes ( int iproc );	
	// Выбор файла для анализа
	bool OpenAnalizFile  ( CString &csBefore, CString &csAfter, bool &breg );

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_PrBar1;
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnStopBtn();
	afx_msg void OnClose();
	afx_msg void OnShowResBtn();
};
