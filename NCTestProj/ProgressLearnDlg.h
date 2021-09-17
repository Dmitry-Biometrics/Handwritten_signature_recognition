//назначение: отображение заставки во время обучения, обучение нейронной сети, 
//            отображение результатов обучения

#pragma once
#include "bnc32.h"

// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	// imgsCnt	- количество обучающих образов
	// imgs		  - координаты обучающих образов
	// sesInf   - данные сессии обучения
	// mxKey    - перемешивающий ключ
	// wghtsArr - веса
	CProgressDlg(int imgsCnt, BNCSIGNIMAGE imgs[], sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghtsArr, CWnd* pParent = NULL);
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESS_DLG };

private:
	// Функция, обучающая нейронную сеть и выводящая соответствующее текстовое сообщение
	void ProgressNeuroNetTrain(int iStability);
	
	int          mode;					// указатель на первый запуск окна/формы
	UINT         imgCount;		  // количество обучающих образов
	BNCSIGNIMAGE *images;		    // обучающие рукописные образы
	sessionInfo  *si;			      // данные сессии обучения
	NCKEY        *mixKey;	  	  // перемешивающий ключ
	NCWEIGHTS    *weights;	    // вектор весов сети
	UINT         stabGroup;			// номер группы стабильности к которой относится пользователь
	

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	bool linear_garm;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void    OnPaint();
	afx_msg void    OnClose();
	afx_msg void    OnLinear();

};
