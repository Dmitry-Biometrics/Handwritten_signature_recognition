// BodyBurdenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "BodyBurdenDlg.h"

#include "../Tools/FileManipulations.h"
#include "../Tools/computation.h"

// CBodyBurdenDlg dialog

IMPLEMENT_DYNAMIC(CBodyBurdenDlg, CDialog)

CBodyBurdenDlg::CBodyBurdenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBodyBurdenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CBodyBurdenDlg::CBodyBurdenDlg ( sessionInfo *sesInf, NCKEY *mxKey, CWnd* pParent /* = NULL */)
:CDialog(CBodyBurdenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	si = sesInf;					  // обучающий ключ
	mixKey = mxKey;					// перемешивающий ключ
}

CBodyBurdenDlg::~CBodyBurdenDlg()
{
}

void CBodyBurdenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_PrBar1);
}

BEGIN_MESSAGE_MAP(CBodyBurdenDlg, CDialog)
	ON_BN_CLICKED(IDC_START_BTN, &CBodyBurdenDlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CBodyBurdenDlg::OnStopBtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SHOWRES_BTN, &CBodyBurdenDlg::OnShowResBtn)
END_MESSAGE_MAP()


// CBodyBurdenDlg message handlers
BOOL CBodyBurdenDlg::OnInitDialog()
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

	resFilePath = _T("");
	bStop = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//	-	-	-	-	-	-	ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ	-	-	-	-	-	-	//

// Выводим % выполненности и пере
void CBodyBurdenDlg::SetTxtAndSetMes ( int iproc )
{
	CString mes, str;
	str.Format(_T("%d"),iproc);
	mes = _T("Анализ образов. Выполнено ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar1.StepIt();
	DelegateFun();
}

// Открываем анализируемый файл (папку с анализируемыми файлами)
// breg - определяем на чём обучать и на чём тестировать
bool CBodyBurdenDlg::OpenAnalizFile ( CString &csBefore, CString &csAfter, bool &breg )
{
	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, NULL, _T("1.dat"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , szFilters, this);

	CString csPath, csFolder;
	csBefore	= _T("До нагрузки");
	csAfter		= _T("После нагрузки");
	int len;
	// Находим путь к папкам с файлами до и после нагрузки
	if ( fileOpenDlg.DoModal() != IDOK ) 
		return FALSE;

	csPath = fileOpenDlg.GetPathName();
	len	 = csPath.Find(csBefore);
	breg = TRUE;
	if ( len == -1 ) {
		len = csPath.Find(csAfter);
		breg = FALSE;
		if ( len == -1 ) {
			MessageBox(_T("Выбрана неверная папка!"));
			return FALSE; 
		}
	}
	csFolder = csPath.Left(len);
	csBefore = csFolder + csBefore + _T("\\");	// папка с файлами до нагрузки
	csAfter  = csFolder + csAfter + _T("\\");	// папка с файлами после нагрузки

	return TRUE;
}

//	-	-	-	-	-	КОНЕЦ ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ	-	-	-	-	-	//

// Запуск процедуры Обучения/Тестирования на образах до и после нагрузки
void CBodyBurdenDlg::OnBnClickedStartBtn()
{
	// Выбор анализируемого файла
	CString csBefore, csAfter;
	if ( OpenAnalizFile ( csBefore, csAfter, bregDO ) != TRUE ) return;

	bStop = false;

	CString trainDir = bregDO ? csBefore : csAfter; // путь к папке с обучающими файлами
	CString checkDir = bregDO ? csAfter : csBefore; // путь к папке с проверочными файлами

//--------------------------------------------------------------------------------------
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(SW_SHOW);
	CString mes = _T("Анализ образов. Выполнено 0%");
	GetDlgItemText(IDC_EXEC_STATIC, mes);
//--------------------------------------------------------------------------------------

	// Подсчитываем количество анализируемых файлов 
	int fileCount = objFileManip.GetFileCount ( trainDir, _T("dat") );

//--------------------------------------------------------------------------------------
	m_PrBar1.SetRange(0, fileCount);
//--------------------------------------------------------------------------------------	
	long lErr;

	// Задаём массив рукописных образов до и после нагрузки
	trainImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];
	checkImages = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	testImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];

	// Загружаем тестовые образы
	UINT testImgCount = 0;
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\5bykv.dat");
	lErr = objFileManip.LoadAllImages(objFileManip.tofileFullPath, testImgCount, testImages);
	if ( lErr != ERROR_SUCCESS ) return;

	// Файл результатов вычислений
	CString str;
	int icor;	// корректирующий коэффициент. Принимает значение либо +1, либо -1.
				// ДО - +1, напр, файлу ДО 1.dat соответствует файл ПОСЛЕ 2.dat 
				//		 1.dat + 1 = 2.dat 
				// ПОСЛЕ - -1, напр, файлу ПОСЛЕ 2.dat соответствует файл ДО 1.dat
				//		 2.dat - 1 = 1.dat 
	if ( bregDO ) {
		str = _T("\tОбучаем сеть на образах ДО нагрузки.\r\n");
		resFilePath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\TestRes_1.txt");
		icor = 1;
	} else {
		str = _T("\tОбучаем сеть на образах ПОСЛЕ нагрузки.\r\n");
    resFilePath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\TestRes_2.txt");
		icor = -1;
	}
	FILE *stream;
	_tfopen_s(&stream, resFilePath, _T("wb"));
	fwrite( str, _tcslen(str)*sizeof(TCHAR), 1, stream );
	fclose(stream);

	UINT trainImgCount = 0;			// счётчик кол-ва образов до нагрузки
	UINT checkImgCount = 0;			// счётчик кол-ва образов после нагрузки

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_SHOW);

	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, trainDir );
	wcsncat_s ( cPath, _T("*.dat"), 5 );

	CString trainFile, checkFile, trainFileName, checkFileName;

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
		trainImgCount = 0;
		trainFileName = FindFileData.cFileName;
		trainFile = trainDir + trainFileName;
		lErr = objFileManip.LoadAllImages(trainFile, trainImgCount, trainImages);
		if ( lErr != ERROR_SUCCESS ) continue;
//--------------------------------------------------------------------------------------
		DelegateFun();	
//--------------------------------------------------------------------------------------
		// Загружаем тестовые образы
		checkImgCount = 0;
		checkFileName = FindFileData.cFileName;
		int iNum = _ttoi(checkFileName.Left(checkFileName.Find(_T(".dat"))));
		checkFileName.Format(_T("%d.dat"), iNum+icor);
		checkFile = checkDir + checkFileName;
		lErr = objFileManip.LoadAllImages(checkFile, checkImgCount, checkImages);
		FindNextFile(hFind, &FindFileData);
		if ( lErr != ERROR_SUCCESS ) continue;
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// Обучение нейронной сети
		NCWEIGHTS	weightsArr;			// вектор весов сети
		UINT		stabGroup;			// номер группы стабильности к которой относится пользователь
		BncNeuroNetTrain ( si->key, *mixKey, trainImgCount, trainImages, weightsArr, &stabGroup ); 
//		MessageBox(_T("Функция обучения закомментирована!!!"), _T("Работает не так как надо!!!"), MB_ICONASTERISK);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// Тестирование на белом шуме
		BNCMOMENTS paramsWN;	// параметры мер Хемминга
		paramsWN = BncGetXemMoments ( FALSE, si->key, *mixKey, NULL, 1000, weightsArr );
		// Вычисляем стойкость системы к атакам подбора
		float fstabWN = 0.87f+0.221f*paramsWN.fqual*paramsWN.fqual;
		CString csWN;
		csWN.Format(_T("\tМатожидание : %.3f\r\n\tДисперсия : %.3f\r\n\tКачество : %.3f\r\n\tСтойкость к атакам подбора равна : 10^%.3f\r\n"), paramsWN.faver, paramsWN.fdisp, paramsWN.fqual, fstabWN);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// Тестирование на тестовых образах
		BNCMOMENTS paramsTI;	// параметры мер Хемминга
		paramsTI = BncGetXemMoments ( TRUE, si->key, *mixKey, testImages, testImgCount, weightsArr );
		// Вычисляем стойкость системы к атакам подбора
		float fstabTI = 0.87f+0.221f*paramsTI.fqual*paramsTI.fqual;
		// Обрабатываем результаты тестирования
		// Вычисляем минимальную и максимальную меру
		int imin = fminElem ( testImgCount, paramsTI.X );
		int imax = fmaxElem ( testImgCount, paramsTI.X );
		// Вычисляем количество "взломов" системы, т.е. кол-во мер == 0
		int zeroCount = fzeroXem ( testImgCount, paramsTI.X );
		CString csTI;
		csTI.Format(_T("\tМатожидание : %.3f\r\n\tДисперсия : %.3f\r\n\tКачество : %.3f\r\n\tМинимальное значение : %d\r\n\tМаксимальное значение : %d\r\n\tКоличество образов с мерой равной 0 : %d\r\n\tСтойкость к атакам подбора равна : 10^%.3f\r\n"), paramsTI.faver, paramsTI.fdisp, paramsTI.fqual, imin, imax, zeroCount, fstabTI);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// Проверка на антогонистических образах, т.е. если обучали на образах ДО НАГРУЗКИ, 
		// то проверяем на образах ПОСЛЕ НАГРУЗКИ, и наоборот
		// Вычисляем процент прохождения образов
		int count = 0;	// количество образов СВОЕГО
		// Последовательно вычисляем меры Хемминга на всех образах из списка
		for ( UINT i = 0; i < checkImgCount; i++ )
		{
			BncExtractKey ( checkImages[i], weightsArr, *mixKey, checkKey );
			int mera = BncCompareKeys ( si->key, checkKey );
			if ( mera == 0 ) count++;
		}
		// Переводим в проценты
		float ferr = (float)count/checkImgCount*100;
		CString csAnt;
		csAnt.Format(_T("\t%.1f"), ferr );
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// Запись результатов в файл
		size_t wrCount = 0;			// количество реально записанных блоков
		_tfopen_s(&stream, resFilePath, _T("ab"));
		CString csRes;
		csRes = _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
		csRes += _T("Анализируемый файл:\t");
		csRes += trainFileName;
		csRes += _T("\r\n\r\nРезультаты тестирования на белом шуме:\r\n");
		csRes += csWN;
		csRes += _T("\r\nРезультаты тестирования на тестовых образах:\r\n");
		csRes += csTI;
		csRes += _T("\r\nМоделирование на образах-антагонистах. Файл:\t");
		csRes += checkFileName;
		csRes += _T("\r\n");
		csRes += csAnt;
		csRes += _T(" % образов идентифицировано как \"СВОЙ\"\r\n");
		csRes += _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n\r\n");

		wrCount = fwrite( csRes, _tcslen(csRes)*sizeof(TCHAR), 1, stream );
		fclose(stream);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		objFileManip.DelAllImages(trainImgCount, trainImages); 
		objFileManip.DelAllImages(checkImgCount, checkImages);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
	}
		FindClose(hFind);
	}

	objFileManip.DelAllImages(testImgCount, testImages);

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SHOWRES_BTN)->EnableWindow(TRUE);
	m_PrBar1.SetPos(0);
	bStop = true;

	MessageBox( resFilePath, _T("Результаты записаны в файл"), MB_ICONASTERISK);

	// Удаление всех рукописных образов
	delete [] trainImages; 
	delete [] checkImages; 
	delete [] testImages;

}

// Остановить процесс вычисления
void CBodyBurdenDlg::OnStopBtn()
{
	bStop = true;
}
void CBodyBurdenDlg::OnClose()
{
	if ( !bStop ) return;
	
	CDialog::OnClose();
}

// Открыть файл полученных результатов
void CBodyBurdenDlg::OnShowResBtn()
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
