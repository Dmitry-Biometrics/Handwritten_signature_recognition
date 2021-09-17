#pragma once
#include "bnc32.h"
#include "afxcmn.h"

// CTestDlg dialog

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	// regime	  - [in] режим работы окна тестирования: NCTEST_MODE_TIWN или NCTEST_MODE_MORFING
	// bOnTI	  - [in] TRUE - тестировать на тестовых образах, FALSE - на белом шуме
	// fldr		  - [in] путь к папке с тестовыми образами
	// tip		  - [in] расширение файлов с образами
	// sesInf   - [in] данные сессии обучения
	// mxKey	  - [in] перемешивающий ключ
	// weights  - [in] веса обученной сети
	CTestDlg(UINT regime, BOOL bOnTI, CString fldr, CString tip, sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghts, CWnd* pParent = NULL);
	virtual ~CTestDlg();

// Dialog Data
	enum { IDD = IDD_TST_DLG };

protected:
	HICON		      m_hIcon;
	int			      mode;			  // режим работы окна тестирования: NCTEST_MODE_TIWN или NCTEST_MODE_MORFING
	bool		      bStart;			// указатель на первый запуск окна/формы (запуск тестирования)
	bool		      bStop;			// остановить тестирование
	BOOL		      bTestOnTI;  // TRUE - тестировать на тестовых образах, FALSE - на белом шуме
	CString		    folder;			// папка с биообразами
	CString		    type;			  // тип файлов биообразов (напр .dat)
	sessionInfo   *si;			  // данные сессии обучения
	NCKEY		      *mixKey;	  // перемешивающий ключ
	NCWEIGHTS	    *weights;   // веса 

//////////////////////////////////////////////////////////////////////////
	// Список лучших образов (самая маленькая мера Хемминга)
	struct pth_xem_list  {
		int		mera;			// мера Хемминга на "хорошем" образе
		int		num;			// номер "хорошего/похожего/близкого" образа
		WCHAR	path[MAX_PATH];	// путь к тестовым образам
	};
	CList<pth_xem_list, pth_xem_list&> bestImgsList; // отсортированные по возрастанию меры лучщие образы
	// Список лучших образов (коэффициенты с самой маленькой мерой Хемминга)
	struct mera_coef_list {
		int			mera;	// мера Хемминга для образа
		NCFOURIER	coef;	// коэффициенты Фурье
	};
	CList<mera_coef_list, mera_coef_list&> bestCoefList; // отсортированные по возрастанию меры лучщие коэф-ты образов
//////////////////////////////////////////////////////////////////////////
	
	void TestONTI();		// тестировать на образах из базы или белом шуме
	void TestOnMultiTI();	// тестировать на заранее отобранных образах с минимальной мерой Хемминга
	// Вывод на экран количества проверенных файлов и образов
	void UpdateCount ( int percent, int fileCount, int imageCount );
	// Создание списка pth_xem_list в котором элементы отсортированы по возрастанию по мерам Хемминга
	// path		- [in] путь к тестовым образам
	// count	- [in] количество тестовых образов
	// xemArr	- [in] вектор мер Хемминга для тестовых образов
	void GetSortPXList(CString path, int count, int xemArr[] );

	// Создание списка mera_coef_list в котором элементы отсортированы по возрастанию по мерам Хемминга
	// Заполнение списка bestCoefList
	// count	- [in] количество тестовых образов (кандидатов на добавление)
	// coefs	- [in] коэффициенты Фурье добавляемых образов
	// xemArr	- [in] вектор мер Хемминга для тестовых образов
	// maxSize	- [in] максимальный размер отсортированного списка 
	void GetSortMCList ( int count, float coefs[], int xemArr[], int maxSize );

	// Размножаем исходные коэффициенты Фурье и тестируем
	// на синтезированных образах (биопараметрах), затем
	// выбираем наиболее близкие к "Своему" образы
	// coefs - [in/out] исходные коэффициенты Фурье (во время работы функции перезаписываются новыми функционалами)
	// count - [in] количество исходных образов
	BNCMOMENTS MorfingAndSorting ( float coefs[], UINT count );

	// Получить меры Хемминга второго порядка
	// count	- [in] количество тестовых образов (м.д. которыми будем вычислыть меру)
	// coefs	- [in] коэффициенты Фурье анализируемых образов
	// meraArr	- [out] вектор с расстояниями между входными образами
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
