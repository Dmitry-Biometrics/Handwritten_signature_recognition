#pragma once
#include <afxmt.h>//для синхронизации основного потока с вторичным
#include "bnc32.h"
#include "../Tools/FileManipulations.h"

// CNCLearningView form view

class CNCLearningView : public CFormView
{
	DECLARE_DYNCREATE(CNCLearningView)

public:
	CNCLearningView();           // protected constructor used by dynamic creation
	virtual ~CNCLearningView();

public:
	enum { IDD = IDD_TRAIN_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	
	CClientDC *m_objClientDC;	// указатель на клиентскую область окна
	HPEN       mypen;					// перо

	BNCSIGNIMAGE *images;		  // указатель на координаты рукописных образов
	BNCSIGNIMAGE *oneImg;		  // указатель на координаты одного рукописного образа
	BNCSIGNIMAGE *fixImg;		  // указатель на координаты фиксированного образа (для обводки)
	UINT		imageCount;	      // счётчик образов
	UINT		coordCount;	      // счётчик количества координат в одном фрагменте парольной фразы, т.е. кол-во координат в промежуток между установкой пера и отрывом
	UINT		beginTime;		    // время установки пера
	bool    bLButDown;		    // флаг - указатель на нажатие левой клавиши мыши в поле ввода
  sessionInfo si;           // информация о текущей сессии
	NCKEY   checkKey;			    // полученный ключ
	NCKEY	  mixKey;			      // ключ для мешалки
	NCWEIGHTS weightsArr;     // веса 	
	NCHASH	  hashKey;			  // хеш обучающего ключа

  bool    bErr;             // ошибка, если true, то ничего не выполняем
	bool    bSecret;			    // флаг, указывающий на режим работы (явный или тайный)
	bool blinear; //используем линейную сеть
	bool    bObvodkaOn;			  // режим обводки
	bool    bFastModOn;			  // режим быстрого моделирования образов из списка
	bool	  bSlideShowOn;		  // режим слайд-шоу
	CString cXemMera;			    // для режима быстрого моделирования
	
	DECLARE_MESSAGE_MAP()

public:

  bool isReady();

//////////////////////////////////////////////////////////////////////////
	struct THREADDATA	
	{
		HWND pWnd;			// указатель на родительское окно
		UINT count;			// количество рукописных образов
		BNCSIGNIMAGE *img;	// рукописные образы
	};	// передаваемые дополнительному потоку данные
	THREADDATA thData;

	int m_nTimer;				// таймер для отображения образов в режиме слайд-щоу
	CWinThread* m_pRecalcWorkerThread;
	LONG OnThreadFinished(UINT wParam, LONG lParam);//действия при завершении потока
//	CString sMess;
	static CNCLearningView *the_instance;
	static UINT ComputeThreadProc(LPVOID pParam); // дополнительный поток
//////////////////////////////////////////////////////////////////////////

	// Определение ширины области группы образов по пути sPath
	// Возвращает ширину области
	float GetGroupImgWidth(CString sPath);


	bool	bLearnNet;			// true - запускали режим обучения, false - режим обучения не запускался
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void ClearInputArea();
	afx_msg void CheckImage();
	afx_msg void DelImage();
	afx_msg void DelAllImages();
	afx_msg void AddImage();
	afx_msg void LearnNet();
	afx_msg void OnShowImage();
	afx_msg void OnSecretBtn();
	afx_msg void OnPublicBtn();
	afx_msg void SaveImages();
	afx_msg void LoadImages();
	afx_msg void CheckAllImages();
	afx_msg void OnObvodkaReg();
	afx_msg void OnTestOnTI();
	afx_msg void OnTestOnWN();
	afx_msg void OnUpdateObvodka(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCheckAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSaveImages(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLoadImages(CCmdUI *pCmdUI);
	afx_msg void OnSetPswM();
	afx_msg void OnProverkaList();
	afx_msg void OnUpdateProverka(CCmdUI *pCmdUI);
	afx_msg void OnBodyBurdenReg();
	afx_msg void OnCracking();
	afx_msg void OnStability();
	afx_msg void OnTIMultiplication();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnUpdateTIMultiplication(CCmdUI *pCmdUI);
public:
	afx_msg void OnMorfing();
public:
	afx_msg void OnGrWidth();
};


