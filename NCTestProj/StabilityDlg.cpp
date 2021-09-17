// CStabilityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "StabilityDlg.h"

#include "../Tools/FileManipulations.h"
#include "../Tools/computation.h"

// CStabilityDlg dialog

IMPLEMENT_DYNAMIC(CStabilityDlg, CDialog)

CStabilityDlg::CStabilityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStabilityDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CStabilityDlg::CStabilityDlg ( CString fldr, NCKEY *lnKey, NCKEY *mxKey, CWnd* pParent /* = NULL */)
:CDialog(CStabilityDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	folder	= fldr;
	key		= lnKey;					// обучающий ключ
	mixKey	= mxKey;					// перемешивающий ключ
}

CStabilityDlg::~CStabilityDlg()
{
}

void CStabilityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_PrBar1);
}

BEGIN_MESSAGE_MAP(CStabilityDlg, CDialog)
	ON_BN_CLICKED(IDC_START_BTN, &CStabilityDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CStabilityDlg::OnStopBtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SHOWRES_BTN, &CStabilityDlg::OnShowResBtn)
END_MESSAGE_MAP()


// CBodyBurdenDlg message handlers
BOOL CStabilityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Рисование иконок
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Инициализация внешнего вида окна
	m_PrBar1.SetStep(1);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SHOWRES_BTN)->EnableWindow(FALSE);
	CString mes = _T("Анализ образов. Выполнено 0%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	SetWindowText(_T("Выявление групп стойкости"));

	resFilePath = _T("");
	bStop = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//	-	-	-	-	-	-	ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ	-	-	-	-	-	-	//

// Выводим % выполненности и пере
void CStabilityDlg::SetTxtAndSetMes ( int iproc )
{
	CString mes, str;
	str.Format(_T("%d"),iproc);
	mes = _T("Анализ образов. Выполнено ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar1.StepIt();
	DelegateFun();
}
//	-	-	-	-	-	КОНЕЦ ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ	-	-	-	-	-	//

// Запуск процедуры Обучения/Тестирования на образах до и после нагрузки
void CStabilityDlg::OnBnClickedStartBtn()
{
/**/
	bStop = false;

	// Подсчитываем количество анализируемых файлов 
	int fileCount = objFileManip.GetFileCount ( folder, _T("dat") );

	// Результирующие вектора качества на белом шуме и на тестовых образах
	float *qualWN = new float [fileCount];
	float *qualTI = new float [fileCount];

//--------------------------------------------------------------------------------------
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(SW_SHOW);
	CString mes = _T("Анализ образов. Выполнено 0%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
	m_PrBar1.SetRange(0, fileCount);
//--------------------------------------------------------------------------------------	
	long lErr;

	// Задаём массивы обучающих и тестовых рукописных образов
	trnImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];
	tstImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];

	// Загружаем все Чужие образы (база из 210 образов с временем)
	UINT tstCount = 0;	// количество Чужих образов
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\210TI.bim");
	lErr = objFileManip.LoadAllImages(objFileManip.tofileFullPath, tstCount, tstImages);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(tstCount, tstImages); 
		delete [] tstImages; 
		return;
	}

	// Файл результатов вычислений
	CString str;
	str = _T("\tРезультаты оценки стойкости системы.\r\n");
	str += _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
	str += _T("1. Результаты тестирования на белом шуме (матожидание, дисперсия, качество, стойкость к атакам подбора 10^):\r\n");
	str += _T("2. Результаты тестирования на тестовых образах (матожидание, дисперсия, качество, минимальное значение, максимальное значение, количество образов с мерой равной 0, стойкость):\r\n");
	str += _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
	resFilePath = folder + _T("StabRes.txt");

	FILE *stream;
	_tfopen_s(&stream, resFilePath, _T("wb"));
	fwrite( str, _tcslen(str)*sizeof(TCHAR), 1, stream );
	fclose(stream);

	UINT trnCount = 0;			// счётчик кол-ва обучающих образов

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_SHOW);

	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, folder );
	wcsncat_s ( cPath, _T("*.dat"), 5 );

	CString trainFile, checkFile, trnFileName, checkFileName;

	// Последовательный анализ всех файлов
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( cPath, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE ) 
	{
	for ( int iFileNum = 0; iFileNum < fileCount; iFileNum++ )
	{	
		// Останавливаем вычисления
		if ( bStop ) break;

//--------------------------------------------------------------------------------------
		SetTxtAndSetMes((iFileNum+1)*100/fileCount);
//--------------------------------------------------------------------------------------
		// Загружаем обучающие образы
		trnCount = 0;
		trnFileName = FindFileData.cFileName;
		trainFile = folder + trnFileName;
		lErr = objFileManip.LoadAllImages(trainFile, trnCount, trnImages);
		FindNextFile(hFind, &FindFileData);
		if ( lErr != ERROR_SUCCESS ) continue;
//--------------------------------------------------------------------------------------
		DelegateFun();	
//--------------------------------------------------------------------------------------
		
		// Обучение нейронной сети
		NCWEIGHTS	weightsArr;			// вектор весов сети
		UINT		stabGroup;			// номер группы стабильности к которой относится пользователь
		BncNeuroNetTrain ( *key, *mixKey, trnCount, trnImages, weightsArr, &stabGroup ); 

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------

		// Тестирование на белом шуме
		BNCMOMENTS paramsWN;	// параметры мер Хемминга
		paramsWN = BncGetXemMoments ( FALSE, *key, *mixKey, NULL, 1000, weightsArr );
		// Вычисляем стойкость системы к атакам подбора
		float fstabWN = 0.87f+0.221f*paramsWN.fqual*paramsWN.fqual;
		qualWN[iFileNum] = paramsWN.fqual;
		CString csWN;
		csWN.Format(_T("%.3f\t%.3f\t%.3f\t%.3f\t"), paramsWN.faver, paramsWN.fdisp, paramsWN.fqual, fstabWN);

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		
		// Тестирование на тестовых образах
		BNCMOMENTS paramsTI;	// параметры мер Хемминга
		paramsTI = BncGetXemMoments ( TRUE, *key, *mixKey, tstImages, tstCount, weightsArr );
		// Вычисляем стойкость системы к атакам подбора
		float fstabTI = 0.87f+0.221f*paramsTI.fqual*paramsTI.fqual;
		qualTI[iFileNum] = paramsTI.fqual;
		// Обрабатываем результаты тестирования
		// Вычисляем минимальную и максимальную меру
		int imin = fminElem ( tstCount, paramsTI.X );
		int imax = fmaxElem ( tstCount, paramsTI.X );
		// Вычисляем количество "взломов" системы, т.е. кол-во мер == 0
		int zeroCount = fzeroXem ( tstCount, paramsTI.X );
		CString csTI;
		csTI.Format(_T("%.3f\t%.3f\t%.3f\t%d\t%d\t%d\t%.3f\r\n"), paramsTI.faver, paramsTI.fdisp, paramsTI.fqual, imin, imax, zeroCount, fstabTI);

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------

		// Запись результатов в файл
		size_t wrCount = 0;			// количество реально записанных блоков
		_tfopen_s(&stream, resFilePath, _T("ab"));
		CString csRes;
		csRes = csWN;
		csRes += csTI;
		wrCount = fwrite( csRes, _tcslen(csRes)*sizeof(TCHAR), 1, stream );
		fclose(stream);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		objFileManip.DelAllImages(trnCount, trnImages); 
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
	}
		FindClose(hFind);
	}

	objFileManip.DelAllImages(tstCount, tstImages);

//////////////////////////////////////////////////////////////////////////
	// Обрабатываем полученные результаты
	//1. Сортируем массивы качества
	//Сортировка массива качества (с наименьшего до наибольшего качества)		
	//int izamena = 10;
	//while ( izamena != 0)
	//{
	//	izamena = 0;
	//	for (int i_Sort = 0; i_Sort < weightsNum-1; i_Sort++)
	//	{
	//		if (neuronQualitySortArr[i_Sort] > neuronQualitySortArr[i_Sort+1])
	//		{
	//			fMin = neuronQualitySortArr[i_Sort+1];
	//			fMax = neuronQualitySortArr[i_Sort];
	//			neuronQualitySortArr[i_Sort]   = fMin;
	//			neuronQualitySortArr[i_Sort+1] = fMax;
	//			izamena = izamena+1;
	//		}
	//	}
	//}
//////////////////////////////////////////////////////////////////////////

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SHOWRES_BTN)->EnableWindow(TRUE);
	m_PrBar1.SetPos(0);
	bStop = true;

	MessageBox( resFilePath, _T("Результаты записаны в файл"), MB_ICONASTERISK);

	// Удаление всех рукописных образов
	delete [] trnImages; 
	delete [] tstImages;
	delete [] qualWN;
	delete [] qualTI;

/**/
}

// Остановить процесс вычисления
void CStabilityDlg::OnStopBtn()
{
	bStop = true;
}
void CStabilityDlg::OnClose()
{
	if ( !bStop ) return;
	
	CDialog::OnClose();
}

// Открыть файл полученных результатов
void CStabilityDlg::OnShowResBtn()
{
	if ( resFilePath == _T("")) return;
		
	DWORD dwStatus = 
		(DWORD)ShellExecute ( NULL, NULL, 
		resFilePath, 
		NULL, NULL, SW_SHOWNORMAL );
	// Определяем наличие ошибок
	if (dwStatus < 32) 
	{
		CString sMessage;
		switch(dwStatus) {
			case ERROR_FILE_NOT_FOUND:       sMessage = " Файл не найден "; break;
			case ERROR_PATH_NOT_FOUND:       sMessage = " Путь не найден "; break;
			default:                         sMessage = " Невозможно открыть файл "; 
		}
		MessageBox (sMessage, _T("Ошибка при открывании"), MB_ICONWARNING);
	}
}
