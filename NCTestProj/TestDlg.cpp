// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "TestDlg.h"

#include "../Tools/FileManipulations.h"
#include "../Tools/computation.h"

// CTestDlg dialog

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent) { }

CTestDlg::CTestDlg(UINT regime, BOOL bOnTI, CString fldr, CString tip, sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghts, CWnd* pParent /*= NULL*/)
:CDialog(CTestDlg::IDD,pParent)
{
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mode		  = regime;
	bTestOnTI	= bOnTI;
	folder		= fldr;
	type		  = tip;
	si			  = sesInf;
	mixKey		= mxKey;
	weights	  = wghts;
}
CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_PrBar2);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CLOSE_BTN, &CTestDlg::OnBnClickedCloseBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CTestDlg::OnBnClickedStopBtn)
	ON_BN_CLICKED(IDC_MCD_BTN, &CTestDlg::OnMcdBtn)
END_MESSAGE_MAP()


// CTestDlg message handlers
BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Рисование иконок
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	bStart = true;	// запускаем процесс тестирования при прорисовки окна
	bStop = false;	

	m_PrBar2.SetStep(1);

	if ( bTestOnTI ) SetWindowText(_T("Тестирование на образах из базы"));
	else SetWindowText(_T("Тестирование на белом шуие"));
	SetDlgItemText( IDC_STATIC01, _T("Количество проверенных файлов :") );
	SetDlgItemText( IDC_STATIC02, _T("Количество проверенных образов :") );

  SetDlgItemText(IDC_STATIC1, _T("0"));
  SetDlgItemText(IDC_STATIC2, _T("0"));

	GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GB)->ShowWindow(FALSE);
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_MCD_BTN)->ShowWindow(FALSE);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( bStart ) {
		bStart = false;
		if ( mode == NCTEST_MODE_TIWN)
			TestONTI ();		// тестировать на тестовых образах либо белом шуме
		else
			TestOnMultiTI();	// тестировать на заранее отобранных образах с минимальной мерой Хемминга
	}
}

// Вывод на экран количества проверенных файлов и образов
void CTestDlg::UpdateCount ( int percent, int fileCount, int imageCount )
{
	CString mes, str;
	str.Format(_T("%d"),percent);
	mes = _T("Выполнено ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar2.StepIt();

	str.Format(_T("%d"), fileCount);
	SetDlgItemText(IDC_STATIC1, str);
	str.Format(_T("%d"), imageCount);
	SetDlgItemText(IDC_STATIC2, str);
}

// Формирование вектора случайных нейронов
// invPos - [out] в каком порядке рассматривать нейроны
void GetRandVector (int invPos[])
{
	// Формируем таблицу по которой будем вычислять совпадения единиц, пар, троек...
	// 1 - выход нейрона invPos[0] на тестовых образах равен invPos[0] битику обучающего ключа
	// 2 - testKey[invPos[0]] == trainKey[invPos[0]] и  testKey[invPos[1]] == trainKey[invPos[1]]
	// 3 ... и т.д.
	srand(::GetTickCount());
	for ( int i = 0; i < NEURON_COUNT; i++ )
	{
		int r = rand()%NEURON_COUNT;// номер рассматриваемого нейрона
		bool bEquiv = false;
		for ( int j = 0; j < i; j++) 
		{
			if ( invPos[j] == r ) 
			{
				bEquiv = true;
				break;
			}
		}
		if ( bEquiv ) 
		{
			i--;
			continue;
		}
		invPos[i] = r;
	}
}
// Получить вектор с количеством совпадений для единиц, пар, троек выходов случайных
// нейронов с обучающим ключём
void GetKeyCoincidence ( int count, BYTE tempKeys[], int trnKey[], int invPos[], int dest[] )
{
	for ( int i = 0; i < count; i++ ) {
		for ( int j = 0; j < NEURON_COUNT; j++ ) {
			if (tempKeys[i*NEURON_COUNT+invPos[j]] != trnKey[invPos[j]])
				break;
			dest[j] ++;
		}
	}
}

// Создание списка pth_xem_list в котором элементы отсортированы по возрастанию по мерам Хемминга
// path		- [in] путь к тестовым образам
// count	- [in] количество тестовых образов
// xemArr	- [in] вектор мер Хемминга для тестовых образов
void CTestDlg::GetSortPXList(CString path, int count, int xemArr[] )
{
	const int maxSize = 32;		// требуемый размер отсортированного массива
	pth_xem_list tempListElem;
	POSITION pos, tmpPos;
	int tempElem, listElem, tmpSize, maxLstEl;

	if (bestImgsList.GetCount() > 0)
	{
		tempListElem = bestImgsList.GetAt(bestImgsList.FindIndex(0));
		listElem = tempListElem.mera;
		tempListElem = bestImgsList.GetAt(bestImgsList.GetTailPosition());
		maxLstEl = tempListElem.mera;
	}
	else
	{
		tempListElem.mera = listElem = maxLstEl = 256;
		bestImgsList.AddHead(tempListElem);
	}
	for ( int i = 0; i < count; i++ )	
	{
		tempElem = xemArr[i];
		tmpPos = bestImgsList.GetHeadPosition();
		pos = tmpPos;
		tmpSize = 0;
		if (tempElem < maxLstEl || bestImgsList.GetCount() < maxSize)
		{
			while (tempElem > listElem)
			{
				pos = tmpPos;
				tempListElem = bestImgsList.GetNext(tmpPos);
				listElem = tempListElem.mera;
				tmpSize++;
			}
			if (tmpSize < maxSize+1)
			{
				tempListElem.mera = tempElem;
				tempListElem.num = i;	
				lstrcpy(tempListElem.path, path);
				bestImgsList.InsertBefore(pos, tempListElem);
			}
			if (bestImgsList.GetCount()>maxSize+1)
			{
				bestImgsList.RemoveTail();
				tempListElem = bestImgsList.GetAt(bestImgsList.GetTailPosition());
				maxLstEl = tempListElem.mera;
			}
			tempListElem = bestImgsList.GetAt(bestImgsList.FindIndex(0));
			listElem = tempListElem.mera;
		} 
	}
}

// Создание списка mera_coef_list в котором элементы отсортированы по возрастанию по мерам Хемминга
// Заполнение списка bestCoefList
// count	- [in] количество тестовых образов (кандидатов на добавление)
// coefs	- [in] коэффициенты Фурье добавляемых образов
// xemArr	- [in] вектор мер Хемминга для тестовых образов
// maxSize	- [in] максимальный размер отсортированного списка 
void CTestDlg::GetSortMCList ( int count, float coefs[], int xemArr[], int maxSize )
{
	mera_coef_list tempListElem;
	POSITION pos, tmpPos;
	int tempElem, listElem, tmpSize, maxLstEl;

	if (bestCoefList.GetCount() > 0)
	{
		tempListElem = bestCoefList.GetAt(bestCoefList.FindIndex(0));
		listElem = tempListElem.mera;
		tempListElem = bestCoefList.GetAt(bestCoefList.GetTailPosition());
		maxLstEl = tempListElem.mera;
	}
	else
	{
		tempListElem.mera = listElem = maxLstEl = 256;
		bestCoefList.AddHead(tempListElem);
	}
	for ( int i = 0; i < count; i++ )	
	{
		tempElem = xemArr[i];
		tmpPos = bestCoefList.GetHeadPosition();
		pos = tmpPos;
		tmpSize = 0;
		if (tempElem < maxLstEl || bestCoefList.GetCount() < maxSize)
		{
			while (tempElem > listElem)
			{
				pos = tmpPos;
				tempListElem = bestCoefList.GetNext(tmpPos);
				listElem = tempListElem.mera;
				tmpSize++;
			}
			if (tmpSize < maxSize+1)
			{
				tempListElem.mera = tempElem;
				memcpy(tempListElem.coef, coefs+i*NCFOURIER_COUNT, sizeof(NCFOURIER));
				bestCoefList.InsertBefore(pos, tempListElem);
			}
			if (bestCoefList.GetCount()>maxSize+1)
			{
				bestCoefList.RemoveTail();
				tempListElem = bestCoefList.GetAt(bestCoefList.GetTailPosition());
				maxLstEl = tempListElem.mera;
			}
			tempListElem = bestCoefList.GetAt(bestCoefList.FindIndex(0));
			listElem = tempListElem.mera;
		} 
	}
}

// Получить меры Хемминга второго порядка
// count	- [in] количество тестовых образов (м.д. которыми будем вычислыть меру)
// coefs	- [in] коэффициенты Фурье анализируемых образов
// meraArr	- [out] вектор с расстояниями между входными образами
void CTestDlg::GetSecondOrderMera (int count, float coefs[], int meraArr[])
{
/**/
// вычисление мер Хемминга второго порядка
	NCKEY first, second; // ключ на первом и втором образе
	int i = 0;
	for ( int i1 = 0; i1 < count-1; i1++ ) // номер первого образа в паре
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weights, *mixKey, first); 
		for ( int i2 = i1+1; i2 < count; i2++ ) // номер второго образа в паре
		{
			BncCoefsToKey (coefs + i2*NCFOURIER_COUNT, *weights, *mixKey, second); 
			meraArr[i] = BncCompareKeys ( first, second );
			i++;
		}
	}
/**/
/*
// вычисление мер Хемминга первого порядка
	NCKEY first, second; // ключ на первом и втором образе
	int i = 0;
	for ( int i1 = 0; i1 < count; i1++ )
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weightsArr, *mixKey, first); 
		meraArr[i1] = BncCompareKeys ( *key, first );
	}
*/
}
// Вычисление количества неповторяющихся пар для x значений
int GetPara( int x )
{
	int res = 0;
	for ( int i = 0; i < x; i++ )
	{
		res += x - (i+1);
	}
	return res;
}
//////////////////////////////////////////////////////////////////////////

// Тестировать обученную сеть на образах 
// из базы тестовых примеров или белом шуме
void CTestDlg::TestONTI()
{
	LONG	lErr = ERROR_SUCCESS;		// код ошибки
	DWORD	dwTime1 = ::GetTickCount();	// время начала тестирования
	int		fileCount;

	// Подсчитываем количество анализируемых файлов 
	if ( bTestOnTI ) fileCount = objFileManip.GetFileCount ( folder, type );
	else			 fileCount = 50;//50000;	// попробую потестировать на миллионе образов

//--------------------------------------------------------------------------------------
	m_PrBar2.SetRange(0, fileCount);
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(TRUE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(TRUE);
	DelegateFun();
//--------------------------------------------------------------------------------------	

	// Чтение тестовых образов
	BNCSIGNIMAGE *images;   // указатель на координаты тестовых рукописных образов
	if ( bTestOnTI ) images = new BNCSIGNIMAGE[NCIMAGE_COUNT];

	UINT imageCount;					// количество образов в текущем файле
	int totalCount = 0;			// общее кол-во файлов с образами
	int totalIMGCount = 0;		// общее кол-во образов
	POINT netOut01[NEURON_COUNT];	// кол-во появлений 0 и 1 на всех тестовых образах
	ZeroMemory(netOut01, sizeof(netOut01));
	BNCMOMENTS params;			// параметры мер Хемминга

	int invPos[NEURON_COUNT];	// в каком порядке рассматривать нейроны
	GetRandVector(invPos);
	int CoinCountArr[NEURON_COUNT] = {0}; // массив со счётчиками совпадений [0] - для единиц [1] - для пар ...	
	int trnKey[NEURON_COUNT] = {0};       //двоичный обучающий ключ
	BncConvertKey ( si->key, trnKey );

//////////////////////////////////////////////////////////////////////////
	int maxMera = 0;		// максимальное значение хранящейся в списке меры Хемминга
//////////////////////////////////////////////////////////////////////////

	if ( bTestOnTI )
	{
		// Считываем координаты тестовых образов
		WIN32_FIND_DATA FindFileData;
		wchar_t cPath[MAX_PATH];
		wcscpy_s ( cPath, folder );
		wcscat_s ( cPath, _T("*."));
		wcscat_s ( cPath, type );

		// Определяем имя файла с координатами
		HANDLE hFind = FindFirstFile( cPath, &FindFileData );
		if ( hFind != INVALID_HANDLE_VALUE ) 
		{	
			int iFileNum = 0;	// количество уже проверенных файлов
			// Последовательное тестирование на образах из всех файлов
			do 
			{
				// Загружаем тестовые образы
				imageCount = 0;			
				CString str		= FindFileData.cFileName;
				CString toCoord = folder + str;
				lErr = objFileManip.LoadAllImages(toCoord, imageCount, images);
				if ( lErr != ERROR_SUCCESS ) continue;
				totalIMGCount += imageCount;
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// Вычисляем моменты мер Хемминга на тестовых образах
				params = BncGetXemMoments ( TRUE, si->key, *mixKey, images, imageCount, *weights );
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// Сохраняем полученные меры Хемминга
				TCHAR *sMode;
				if ( totalCount == 0 ) sMode = _T("w");
				else				   sMode = _T("a");
				objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
				objFileManip.SaveXem(objFileManip.tofileFullPath, params.X, imageCount, sMode);
				// Количество появлений ноликов и единичек
				for ( int i = 0; i < NEURON_COUNT; i++ ) { netOut01[i].x += params.p01[i].x; netOut01[i].y += params.p01[i].y; }
				// Вычисление экспоненциального уменьшения числа совпадений полученного ключа с обучающим
				//GetKeyCoincidence ( imageCount, params.tmpKey, trnKey, invPos, CoinCountArr );
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// ВЫБОР ЛУЧШИХ ОБРАЗОВ
				// Фиксируем координаты образов с минимальными мерами Хемминга
				// Главная задача: выявить в процессе тестирования 32 образа,
				// максимально похожих на образы СВОЕГО (т.е. мера Хемминга для
				// этих 32 образов будет минимальной по сравнению с мерами для оставшихся
				// образов)
				// Далее сохраняем координаты выбранных образов в файл (только после того
				// как протестируем все до одного тестовые образы) для дальнейшей обработки
				GetSortPXList(toCoord, imageCount, params.X);
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------

				// Удаляем координаты всех рукописных образов
				objFileManip.DelAllImages(imageCount, images); 
				totalCount++;
				iFileNum++;
				// Вывод на экран
				UpdateCount(iFileNum*100/fileCount, totalCount, totalIMGCount);
			} while ( FindNextFile(hFind, &FindFileData) != 0 && !bStop );
			FindClose(hFind);
		}
	}		// тестирование на тестовых образах
	else	// тестирование на белом шуме
	{
		int iFileNum = 0;	// количество уже проверенных файлов
		while ( iFileNum < fileCount && !bStop )
		{
			imageCount = 20;			
			totalIMGCount += imageCount;
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// Вычисляем моменты мер Хемминга на тестовых образах
			params = BncGetXemMoments ( FALSE, si->key, *mixKey, images, imageCount, *weights );
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// Сохраняем полученные меры Хемминга
			TCHAR *sMode;
			if ( totalCount == 0 ) sMode = _T("w");
			else				   sMode = _T("a");
      objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
			objFileManip.SaveXem(objFileManip.tofileFullPath, params.X, imageCount, sMode);
			totalCount++;
			iFileNum++;
			// Количество появлений ноликов и единичек
			for ( int i = 0; i < NEURON_COUNT; i++ ) { netOut01[i].x += params.p01[i].x; netOut01[i].y += params.p01[i].y; }
			// Вычисление экспоненциального уменьшения числа совпадений полученного ключа с обучающим
			//GetKeyCoincidence ( imageCount, params.tmpKey, trnKey, invPos, CoinCountArr );
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// Вывод на экран
			UpdateCount(iFileNum*100/fileCount, totalCount, totalIMGCount);
		}
	}

	// Сохраняем полученные значения для появления 0 и 1
/*	for ( int i = 0; i < NEURON_COUNT; i++ )
	{
		netOut01[i].x = roundUp((float)netOut01[i].x*100/totalIMGCount);
		netOut01[i].y = roundUp((float)netOut01[i].y*100/totalIMGCount);
	}
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\NetOut_0_1.txt");
	objFileManip.SaveNetOut(objFileManip.tofileFullPath, netOut01, _T("w"));
*/
	// Сохраняем полученные значения счётчика для совпадений 1,2,3...
  //objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\coincidence.txt");
	//objFileManip.SaveXem(objFileManip.tofileFullPath, CoinCountArr, NEURON_COUNT, _T("w"));

//////////////////////////////////////////////////////////////////////////
	// ИСПОЛЬЗУЕТСЯ ДЛЯ ДАЛЬНЕЙШЕГО РАЗМЫТИЯ ЛУЧШИХ ОБРАЗОВ
	// ДАЛЬШЕ С ПОМОЩЬЮ АВТОМАТИЧЕСКИХ ГЕНЕРАТОРОВ БУДЕМ УВЕЛИЧИВАТЬ
	// КОЛИЧЕСТВО ОБРАЗОВ С МИНИМАЛЬНОЙ МЕРОЙ И ПЫТАТЬСЯ СГЕНЕРИРОВАТЬ
	// ОБРАЗ МАКСИМАЛЬНО ПОХОЖИЙ НА ОБРАЗ СВОЕГО (МЕРА = 0)
	// Сохраняем лучшие образы в отдельный файл
	pth_xem_list tempListElem;
	BNCSIGNIMAGE *bestImages;   // указатель на координаты с минимальной мерой Хемминга
	bestImages = new BNCSIGNIMAGE[bestImgsList.GetCount()];
	// Считываем данные из списка
	POSITION pos = bestImgsList.GetHeadPosition();
	UINT imCount = 0;//количество образов
	for (int i = 0; i < bestImgsList.GetCount()-1; i++)
	{
		tempListElem = bestImgsList.GetNext(pos);
		lErr = objFileManip.LoadAllImages(tempListElem.path, imageCount, images);
		if ( lErr != ERROR_SUCCESS ) continue;

		bestImages[imCount].m_Num = images[tempListElem.num].m_Num;
		bestImages[imCount].m_X = new UINT [images[tempListElem.num].m_Num];
		bestImages[imCount].m_Y = new UINT [images[tempListElem.num].m_Num];
		bestImages[imCount].m_P = new UINT [images[tempListElem.num].m_Num];
		bestImages[imCount].m_T = new UINT [images[tempListElem.num].m_Num];
		memcpy ( bestImages[imCount].m_X, images[tempListElem.num].m_X, images[tempListElem.num].m_Num*sizeof(UINT) );
		memcpy ( bestImages[imCount].m_Y, images[tempListElem.num].m_Y, images[tempListElem.num].m_Num*sizeof(UINT) );
		memcpy ( bestImages[imCount].m_P, images[tempListElem.num].m_P, images[tempListElem.num].m_Num*sizeof(UINT) );
		memcpy ( bestImages[imCount].m_T, images[tempListElem.num].m_T, images[tempListElem.num].m_Num*sizeof(UINT) );
		bestImages[imCount].m_MaxX = images[tempListElem.num].m_MaxX;
		bestImages[imCount].m_MaxY = images[tempListElem.num].m_MaxY;
		bestImages[imCount].m_MaxP = images[tempListElem.num].m_MaxP;
		objFileManip.DelAllImages(imageCount, images); 
		imCount++;
	}
	// Сохраняем рукописные образы в отдельный файл (образы с минимальной мерой хемминга)
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\minXemImg.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	objFileManip.SaveAllImages(szPath, imCount, bestImages);
	objFileManip.DelAllImages(imCount, bestImages); 
	delete [] bestImages;

//////////////////////////////////////////////////////////////////////////

// ОБРАБАТЫВАЕМ РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ

	// Считываем вычисленные меры Хемминга
	int *xemArr = new int [totalIMGCount];
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
	objFileManip.LoadXem(objFileManip.tofileFullPath, xemArr, totalIMGCount);
	// Вычислям моменты мер Хемминга
	fadq ( totalIMGCount, xemArr, params ); 
	// Вычисляем минимальную и максимальную меру
	int imin = fminElem ( totalIMGCount, xemArr );
	int imax = fmaxElem ( totalIMGCount, xemArr );
	// Вычисляем количество "взломов" системы, т.е. кол-во мер == 0
	int zeroCount = fzeroXem ( totalIMGCount, xemArr );
	// Вычисляем стойкость системы к атакам подбора
	float fstab = Qual2Stab(params.fqual);

	// Рассчёт времени, ушедшего на тестирование системы
	DWORD dwTime2 = (::GetTickCount() - dwTime1)/1000;
	int iDay  = dwTime2/(60*60*24);
	int iHour = dwTime2/(60*60)%24;
	int iMinute = dwTime2/60%60;
	int iSecond = dwTime2%60;
	CString strTime;
	strTime.Format(_T("Тестирование завершено за: %02d:%02d:%02d:%02d."), iDay, iHour, iMinute, iSecond);

	// Вывод полученных результатов на экран
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GB)->ShowWindow(TRUE);
	CString str;
	str.Format(_T(" Матожидание : %.3f\n Дисперсия : %.3f\n Качество : %.3f\n Минимальное значение : %d\n Максимальное значение : %d\n Количество образов с мерой равной 0 : %d\n Стойкость к атакам подбора равна : 10^%.3f"), params.faver, params.fdisp, params.fqual, imin, imax, zeroCount, fstab);
	SetDlgItemText(IDC_FINAL_STATIC, str);
	SetWindowText(strTime);

	GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
	GetDlgItem(IDC_MCD_BTN)->ShowWindow(TRUE);

	// Удаление 
	if(bTestOnTI) delete [] images; 
	delete [] xemArr;
}

// Размножаем исходные коэффициенты Фурье и тестируем
// на синтезированных образах (биопараметрах), затем
// выбираем наиболее близкие к "Своему" образы
// coef - [in/out] исходные коэффициенты Фурье (во время работы функции перезаписываются новыми функционалами)
// count - [in] количество исходных образов
BNCMOMENTS CTestDlg::MorfingAndSorting ( float coefs[], UINT count )
{
	// "Размножение" образов (преобразуем count исходных рукописных
	// образов в pairCount*(interpCount) образов)
	// Например, для 32 исходных образов pairCount = 496 (количество различных пар)
	// interpCount = 11 получается 5456 образов
	UINT i1, i2;			// номера рассматриваемых образов
	int pairCount = 0;		// количество пар образов
	int interpCount;		// количество генерируемых образов между i1 и i2 (получаем 
	// с помощью линейной интерполяции координат двух рассматриваемых
	// образов), исходные образы тоже остаются, т.о. получается 
	// interpCount-2 новых образа

//////////////////////////////////////////////////////////////////////////
	// ВРЕМЕННЫЕ ДАННЫЕ
	int childCount = 0;
	struct child_coef 
	{
		NCFOURIER	coef;	// коэффициенты Фурье
	};
	CList<child_coef, child_coef&> childCoefList; // список коэффициентов Фурье всех потомков
	child_coef oneChild;	// коэффициенты одного потомка
//////////////////////////////////////////////////////////////////////////
	
	BNCMOMENTS params;		// параметры мер Хемминга

//////////////////////////////////////////////////////////////////////////
	// количество потомков зависит от коэффициента размножения
	// коэффициент размножения зависит от расстояния между родителями
	// (мера Хемминга второго порядка)
	// 
	NCKEY first, second;	// ключ на первом и втором образе
	int mera;				// мера Хемминга второго порядка
	for ( i1 = 0; i1 < count-1; i1++ ) // номер первого образа в паре
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weights, *mixKey, first); 
		for ( i2 = i1+1; i2 < count; i2++ ) // номер второго образа в паре
		{
			BncCoefsToKey (coefs + i2*NCFOURIER_COUNT, *weights, *mixKey, second); 
			mera = BncCompareKeys ( first, second );
//			mera = BncCompareKeys ( *key, second );
//			mera = 45;	// фиксированный коэффициент размножения: 10 потомков + 2 родителя
			interpCount = mera/5 + 1 + 1;	// количество потомков кратно 5 
			// мера 0 - 4 потомков 1 + 2 родителя
			// мера 5 - 9 потомков 2 + 2 родителя 
			// мера 10 - 14 потомков 3 + 2 родителя и т.д.
			float *tmpCoefs = new float[mera*NCFOURIER_COUNT];	// коэффициенты группы синтезированных образов
			// количество точек в новом образе
//			for ( int i = 0; i < interpCount+1; i++ )
			for ( int i = 1; i < interpCount; i++ )// нет потомка как первый и как второй родитель
			{
				for ( int j = 0; j < NCFOURIER_COUNT; j++ )
				{
					oneChild.coef[j] = 
						((interpCount-i)/(float)interpCount) * coefs[i1*NCFOURIER_COUNT+j] + 
						(i/(float)interpCount) * coefs[i2*NCFOURIER_COUNT+j];
				}
				childCoefList.AddTail(oneChild);
				memcpy(tmpCoefs + i*NCFOURIER_COUNT, oneChild.coef, sizeof(NCFOURIER));
			}
			// Получаем меры Хемминга для синтезированных образов
			params = BncCoefsToXemMoments (si->key, *mixKey, tmpCoefs, interpCount, *weights );

			// Добавляем коэффициенты в список избранных образов
			GetSortMCList ( interpCount, tmpCoefs, params.X, count );

			pairCount++;
			delete [] tmpCoefs;
		}
	}
//////////////////////////////////////////////////////////////////////////
	// фиксированный коэффициент размножения (одинаковый для всех пар родителей
	// и не зависит от расстояния между родителями)
/*	float tmpCoefs[interpCount*NCFOURIER_COUNT];	// коэффициенты группы синтезированных образов
	for ( i1 = 0; i1 < count-1; i1++ ) // номер первого образа в паре
	{
		for ( i2 = i1+1; i2 < count; i2++ ) // номер второго образа в паре
		{
			// количество точек в новом образе
			for ( int i = 0; i < interpCount; i++ )
			{
				for ( int j = 0; j < NCFOURIER_COUNT; j++ )
				{
					tmpCoefs[i*NCFOURIER_COUNT+j] = 
						((interpCount-i)/(float)interpCount) * coefs[i1*NCFOURIER_COUNT+j] + 
						(i/(float)interpCount) * coefs[i2*NCFOURIER_COUNT+j];
				}
			}
			// Получаем меры Хемминга для синтезированных образов
			params = BncCoefsToXemMoments (*key, *mixKey, tmpCoefs, interpCount, *weightsArr );

			// Добавляем коэффициенты в список избранных образов
			GetSortMCList ( interpCount, tmpCoefs, params.X, count );

			pairCount++;

//////////////////////////////////////////////////////////////////////////
//ВРЕМЕННЫЕ ДАННЫЕ
// Временно фиксируем коэффициенты детей
			//memcpy(allChildCoef+pdx*interpCount*NCFOURIER_COUNT, tmpCoefs, interpCount*NCFOURIER_COUNT*sizeof(float));
			//pdx++;
//////////////////////////////////////////////////////////////////////////
		}
	}
*/
//////////////////////////////////////////////////////////////////////////
//ВРЕМЕННЫЕ ДАННЫЕ
	// вычисление расстояний мер Хемминга между различными образами
	childCount = (int)childCoefList.GetCount();
	int paraCount = GetPara(childCount);
	int *meraArr = new int[paraCount];
	float *allChildCoef = new float[childCount*NCFOURIER_COUNT];
	// Заносим данные из списка коэффициентов Фурье потомков в один огромный массив
	POSITION pos = childCoefList.GetHeadPosition();
	for ( int i = 0; i < childCount; i++ )
	{
		oneChild = childCoefList.GetNext(pos);
		memcpy ( allChildCoef + i*NCFOURIER_COUNT, oneChild.coef, sizeof(NCFOURIER) );
	}
	GetSecondOrderMera (childCount, allChildCoef, meraArr);
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\SecondOrderMeraN.txt");
	objFileManip.SaveXem(objFileManip.tofileFullPath, meraArr, paraCount, _T("w"));
//	objFileManip.SaveXem(objFileManip.tofileFullPath, meraArr, childCount, _T("w"));
	delete [] meraArr;

	delete [] allChildCoef;
//////////////////////////////////////////////////////////////////////////

	// вычисляем ещё раз моменты мер Хемминга на выбранных коэффициентах
	// Считываем данные из списка отобранных ближайших образов
	mera_coef_list tempListElem;
	/*POSITION*/ pos = bestCoefList.GetHeadPosition();
	for ( int i = 0; i < bestCoefList.GetCount()-1; i++ )
	{
		tempListElem = bestCoefList.GetNext(pos);
		memcpy ( coefs + i*NCFOURIER_COUNT, tempListElem.coef, sizeof(NCFOURIER) );
	}
	params = BncCoefsToXemMoments (si->key, *mixKey, coefs, count, *weights );

	return params;
}

// Тестирование сети с помощью отбора образов Чужого с минимальной мерой Хемминга 
// и увеличение числа отобранных образов
// Увеличение количества тестовых образов за счёт размытия параметров
// Автоматически из 32 образов из папки minXemImg.dat получаем нужное количество
// тестовых образов, размываем параметры исходных образов
void CTestDlg::TestOnMultiTI()
{
	// Загружаем исходные образы 
	BNCSIGNIMAGE *images;   // указатель на координаты тестовых рукописных образов
	images = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT imageCount = 0;	// количество рукописных образов
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\minXemImg.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, imageCount, images);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(imageCount, images); 
		delete [] images;
    MessageBox(_T("Не найден файл образов с минимальной мерой (minXemImg.dat).\nСначала запустите процедуру тестирования."), _T("Образы-родители не найдены"), MB_ICONWARNING);
    GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
    GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
    return;
	}
	
	// Вычисляем коэффициенты Фурье исходных образов
	FLOAT *coefs;		// одномерный массив функционалов всех тестовых образов
	// Выделение памяти под коэффициенты всех образов
	coefs = new FLOAT[imageCount*NCFOURIER_COUNT];
	BncGetImgParams (images, imageCount, coefs );

	// Увеличиваем количество исходных образов
	BNCMOMENTS params;								// параметры мер Хемминга	
	params = BncCoefsToXemMoments ( si->key, *mixKey, coefs, imageCount, *weights );

//////////////////////////////////////////////////////////////////////////
	// Расстояния между родительскими образами
//imageCount = 8;	// специально уменьшил количество образов-родителей	
/*
	int paraCount = GetPara(imageCount);
	int *meraArr = new int[paraCount];
	GetSecondOrderMera (imageCount, coefs, meraArr);
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\Parent8.txt");
	objFileManip.SaveXem(objFileManip.tofileFullPath, meraArr, paraCount, _T("w"));
	delete [] meraArr;
*/
//////////////////////////////////////////////////////////////////////////

	while ( params.fdisp != 0 )
	{
		bestCoefList.RemoveAll();// Очищаем список с лучшими функционалами
		params = MorfingAndSorting ( coefs, imageCount ); // синтезируем новые коэффициенты и добавляем в список
	}
	
	delete [] coefs;
	
	objFileManip.DelAllImages(imageCount, images); 
	delete [] images;

	// Вывод полученных результатов на экран
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GB)->ShowWindow(TRUE);
	CString str = _T("");
//	str.Format(_T(" Матожидание : %.3f\n Дисперсия : %.3f\n Качество : %.3f\n Минимальное значение : %d\n Максимальное значение : %d\n Количество образов с мерой равной 0 : %d\n Стойкость к атакам подбора равна : 10^%.3f"), params.faver, params.fdisp, params.fqual, imin, imax, zeroCount, fstab);
	SetDlgItemText(IDC_FINAL_STATIC, str);
//	SetWindowText(strTime);

	GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
	GetDlgItem(IDC_MCD_BTN)->ShowWindow(TRUE);

}

// Закрыть
void CTestDlg::OnBnClickedCloseBtn()
{
	CDialog::OnOK();
}

// Остановить тестирование
void CTestDlg::OnBnClickedStopBtn()
{
	bStop = true;
	GetDlgItem(IDC_STOP_BTN)->EnableWindow(FALSE);
}

// Запускаем маткадовский файл, показывающий 
// полученное распределение мер Хемминга
void CTestDlg::OnMcdBtn()
{
	// Задаём путь к инструкции пользователя	
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\Graf.xmcd");

	// Открываем инструкцию пользователя
	DWORD dwStatus = 
		(DWORD)ShellExecute ( NULL, NULL, 
		objFileManip.tofileFullPath, 
		NULL, NULL, SW_SHOWNORMAL );
	// Определяем наличие ошибок
	if (dwStatus < 32) 
	{
		CString sMessage;
		switch(dwStatus)
		{
		case ERROR_FILE_NOT_FOUND:       sMessage = " Файл не найден "; break;
		case ERROR_PATH_NOT_FOUND:       sMessage = " Путь не найден "; break;
		default:                         sMessage = " Невозможно открыть маткад "; 
		}
		MessageBox (sMessage, _T("Ошибка при открывании"), MB_ICONWARNING);
	}
}
