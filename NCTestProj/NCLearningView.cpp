// NCLearningView.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "NCLearningView.h"

#include "DlgBarEx.h"
#include "ProgressLearnDlg.h"
#include "GetKeyDlg.h"
#include "TestDlg.h"
#include "UserAccountDlg.h"
#include "BodyBurdenDlg.h"
#include "StabilityDlg.h"


#include "../Tools/computation.h"

//////////////////////////////////////////////////////////////////////////
// события для слайдшоу образов
CString sMessage;
CEvent g_eventStart; // запустить второй поток на выполнение
CEvent g_eventKill;  // приостановить выполнение второго потока
volatile int g_nCount;
CNCLearningView* CNCLearningView::the_instance;
#define WM_THREADFINISHED  (WM_USER + 2)
//////////////////////////////////////////////////////////////////////////

// CNCLearningView

IMPLEMENT_DYNCREATE(CNCLearningView, CFormView)

CNCLearningView::CNCLearningView()
	: CFormView(CNCLearningView::IDD)
{
	// Обнуляем перемешивающий ключ
	ZeroMemory (mixKey, NCKEY_SIZE);

  ZeroMemory(&si, sizeof(si));

	// Задаём массив рукописных образов
	images = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	imageCount = 0;
	ZeroMemory(weightsArr, sizeof(weightsArr));

	// Для записи координат временного образа
	oneImg = new BNCSIGNIMAGE[1];
	oneImg[0].m_X = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_Y = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_P = new UINT [NCMAXCOORD_QUAN];
	oneImg[0].m_T = new UINT [NCMAXCOORD_QUAN];

	// Для режима обводки координаты фиксированного образа
	fixImg = new BNCSIGNIMAGE[1];
	fixImg[0].m_X = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_Y = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_P = new UINT [NCMAXCOORD_QUAN];
	fixImg[0].m_T = new UINT [NCMAXCOORD_QUAN];
}

CNCLearningView::~CNCLearningView()
{
	// Удаление клиентской области
	delete m_objClientDC;
	
	// Удаление всех рукописных образов
	if ( images != NULL )	// т.е. если массив рукописных образов создан
	{
		// Удаляем координаты всех рукописных образов
		for (unsigned iCount = 0; iCount < imageCount; iCount++ ) {
			delete [] images[iCount].m_X;
			delete [] images[iCount].m_Y;
			delete [] images[iCount].m_P;
			delete [] images[iCount].m_T;
		}
		delete [] images; 
	}

	// Удаление "временного" рукописного образа
	if ( oneImg != NULL ) {
		delete [] oneImg[0].m_X;
		delete [] oneImg[0].m_Y;
		delete [] oneImg[0].m_P;
		delete [] oneImg[0].m_T;
		delete [] oneImg; 
	}

	// Удаление "обводимого" рукописного образа
	if ( fixImg != NULL ) {
		delete [] fixImg[0].m_X;
		delete [] fixImg[0].m_Y;
		delete [] fixImg[0].m_P;
		delete [] fixImg[0].m_T;
		delete [] fixImg; 
	}
}

void CNCLearningView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNCLearningView, CFormView)
	ON_MESSAGE(WM_THREADFINISHED, OnThreadFinished)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CLEAR_BTN,	 &CNCLearningView::ClearInputArea)
	ON_BN_CLICKED(IDC_CHECK_BTN,	 &CNCLearningView::CheckImage)
	ON_BN_CLICKED(IDC_DELIMAGE_BTN,  &CNCLearningView::DelImage)
	ON_BN_CLICKED(IDC_CLEARLIST_BTN, &CNCLearningView::DelAllImages)
	ON_BN_CLICKED(IDC_ADD_BTN,		 &CNCLearningView::AddImage)
	ON_BN_CLICKED(IDC_LEARN_BTN,	 &CNCLearningView::LearnNet)
	ON_LBN_SELCHANGE(IDC_IMAGE_LIST, &CNCLearningView::OnShowImage)
	ON_BN_CLICKED(IDC_SECRET_RADIO, &CNCLearningView::OnSecretBtn)
	ON_BN_CLICKED(IDC_PUBLIC_RADIO, &CNCLearningView::OnPublicBtn)
	ON_COMMAND(ID_SAVE_IMAGES,		&CNCLearningView::SaveImages)
	ON_COMMAND(ID_LOAD_IMAGES,		&CNCLearningView::LoadImages)
	ON_COMMAND(ID_CHECK_ALL,		&CNCLearningView::CheckAllImages)
	ON_COMMAND(ID_OBVODKA,			&CNCLearningView::OnObvodkaReg)
	ON_COMMAND(ID_TESTON_TI,		&CNCLearningView::OnTestOnTI)
	ON_COMMAND(ID_TESTON_WN,		&CNCLearningView::OnTestOnWN)
	ON_UPDATE_COMMAND_UI(ID_OBVODKA, &CNCLearningView::OnUpdateObvodka)
//	ON_COMMAND(ID_LOAD_BDTI, &CNCLearningView::OnLoadTIfromBDi)
	ON_UPDATE_COMMAND_UI(ID_CHECK_ALL,	 &CNCLearningView::OnUpdateCheckAll)
	ON_UPDATE_COMMAND_UI(ID_SAVE_IMAGES, &CNCLearningView::OnUpdateSaveImages)
	ON_UPDATE_COMMAND_UI(ID_LOAD_IMAGES, &CNCLearningView::OnUpdateLoadImages)
	ON_COMMAND(ID_SET_PSW,  &CNCLearningView::OnSetPswM)
	ON_COMMAND(ID_PROVERKA, &CNCLearningView::OnProverkaList)
	ON_UPDATE_COMMAND_UI(ID_PROVERKA, &CNCLearningView::OnUpdateProverka)
	ON_COMMAND(ID_BURDEN,	 &CNCLearningView::OnBodyBurdenReg)
	ON_COMMAND(ID_CRACKING,  &CNCLearningView::OnCracking)
	ON_COMMAND(ID_STABILITY, &CNCLearningView::OnStability)
	ON_COMMAND(ID_TI_MULTIPLICATION, &CNCLearningView::OnTIMultiplication)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_TI_MULTIPLICATION, &CNCLearningView::OnUpdateTIMultiplication)
	ON_COMMAND(ID_MORFING, &CNCLearningView::OnMorfing)
	ON_COMMAND(ID_GRWIDTH, &CNCLearningView::OnGrWidth)
END_MESSAGE_MAP()

void CNCLearningView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
//////////////////////////////////////////////////////////////////////////
	///....
	CNCLearningView::the_instance=this;
	//...
//////////////////////////////////////////////////////////////////////////
	// Создание клиентской области
	m_objClientDC = new CClientDC(this);
	// Создаем и выбираем перо	
	mypen = CreatePen( PS_SOLID, 2, RGB(5,3,86));
	m_objClientDC->SelectObject(mypen);

	bSecret       = false;		// по умолчанию помечена кнопка "Открыто"
	oneImg[0].m_Num = 0;	    // обнуление счётчика координат
	bObvodkaOn	  = false;	  // по умолчанию режим обводки отключён
	bFastModOn	  = false;	  // по умолчанию режим быстрого моделирования отключён
	bLButDown	    = false;	  // по умолчанию левая кнопка мышки не нажата
	bSlideShowOn  = false;	  // по умолчанию режим слайд-шоу отключен
  bErr          = false;

	// Если выбран режим обучения
	if ( m_wndRightBar.mode == NCTRAIN_MODE ) {
		// Указываем, что режим обучения пока не запускался
		bLearnNet = FALSE;		
	} else {	// то есть выбран режим тестирования
		m_wndRightBar.iLight = 0;		// вначале светофор выключен
	}

  // Чтение данных последней сессии
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\index.ini");
  if ( objFileManip.LoadIniData(objFileManip.tofileFullPath,si) != ERROR_SUCCESS ) {
    MessageBox(_T("Отсутствует файл с ключом."), _T("Обучающий ключ не задан"), MB_ICONWARNING);
    bErr = true;
    return;
  }

	// Загружаем биометрический контейнер (веса и хеш)
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\Sign.bnc");
	if ( objFileManip.LoadBNC(objFileManip.tofileFullPath, weightsArr, hashKey ) != ERROR_SUCCESS ) {
		MessageBox(_T("Отсутствует биоконтейнер.\nНеобходимо обучить сеть."), _T("Сеть не обучена"), MB_ICONWARNING);
	}

}

// CNCLearningView diagnostics

#ifdef _DEBUG
void CNCLearningView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNCLearningView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CNCLearningView message handlers

// ********************************** РИСОВАНИЕ ********************************* //
void CNCLearningView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// сетка
	CPen BluePen;
	CPen RedPen;

	BluePen.CreatePen(  PS_SOLID, 1, RGB(139,136,247) );
	RedPen.CreatePen (  PS_SOLID, 1, RGB(253,126,104) );

	// получаем границы области рисования
	RECT rc;
	GetClientRect( &rc );

	// рисуем белое поле
	dc.SelectStockObject( WHITE_PEN );
//	dc.SelectObject(BluePen);
	dc.Rectangle(rc.left, rc.top, rc.right, rc.bottom);

	dc.SelectObject( BluePen );
	// рисуем горизонтальные линии
	int i;
	for (int step = 48, i = rc.top + step; i <= rc.bottom; step = (step == 48) ? 24 : 48, i += step)
	{
		dc.MoveTo( rc.left + 1,  i );
		dc.LineTo( rc.right - 1, i );
	}
	// рисуем наклонные линии
	int dx = (int)(0.4663 * (rc.bottom - rc.top - 2));
  for (i = 82; i < rc.right - rc.left + dx - 1; i += 162){
    dc.MoveTo( i, rc.top + 1 );
    dc.LineTo( i-dx, rc.bottom - 1 );
  }
	dc.SelectObject( RedPen );
	// красные поля 
	dc.MoveTo( rc.right - 70, rc.top + 1);
	dc.LineTo( rc.right - 70, rc.bottom - 1);
	// конец сетки

	// Перерисовываем рукописный образ
	if ( oneImg[0].m_Num > 0 && !bSecret ) 
	{
		int x, y;
		x = oneImg[0].m_X[0];
		y = oneImg[0].m_Y[0];
		if ( x != 0 && y != 0 ) m_objClientDC->MoveTo ( x, y );
		else					m_objClientDC->MoveTo ( oneImg[0].m_X[1], oneImg[0].m_Y[1] );

		for ( unsigned iCount = 1; iCount < oneImg[0].m_Num; iCount++ ) {
			if ( oneImg[0].m_X[iCount] != 0 && oneImg[0].m_Y[iCount] != 0 ) 
				m_objClientDC->LineTo ( oneImg[0].m_X[iCount], oneImg[0].m_Y[iCount] );
			else 
				m_objClientDC->MoveTo ( oneImg[0].m_X[iCount+1], oneImg[0].m_Y[iCount+1] );
		}
	}

	// Задаём шрифт выводимых сообщений
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       
	lf.lfHeight = 16;      
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName,  _T("MS Sans Serif"));
	VERIFY(font.CreateFontIndirect(&lf));  
	dc.SelectObject(&font);
	// Выбор фона шрифта
	dc.SetBkMode(TRANSPARENT);
	// Выбор цвета шрифта
	dc.SetTextColor(RGB(255, 0, 0));
	
  dc.TextOut(rc.left+10, rc.bottom-20, sMessage, (int)wcslen(sMessage));

	TCHAR strBuffer[40];	// выводимое на экран сообщение

	// Если включён режим обводки, выводим сообщение на экран
	if ( bObvodkaOn )
	{
		// Рисование "обводимого" образа
		CPen *objPen;
		objPen = new CPen;
		objPen->CreatePen(PS_SOLID, 1, RGB(200,0,0));
		m_objClientDC->SelectObject(objPen);
		m_objClientDC->MoveTo ( fixImg[0].m_X[0], fixImg[0].m_Y[0] );
		for ( unsigned iCount = 1; iCount < fixImg[0].m_Num; iCount++ ) {
			if ( fixImg[0].m_X[iCount] != 0 && fixImg[0].m_Y[iCount] != 0 ) 
				m_objClientDC->LineTo ( fixImg[0].m_X[iCount], fixImg[0].m_Y[iCount] );
			else 
				m_objClientDC->MoveTo ( fixImg[0].m_X[iCount+1], fixImg[0].m_Y[iCount+1] );
		}
		objPen->DeleteObject();
		m_objClientDC->SelectObject(mypen);
	
		// Вывод сообщения
		wcscpy_s(strBuffer, _T("Включён режим обводки"));
		dc.TextOut(rc.left+10, rc.bottom-20, strBuffer, (int)wcslen(strBuffer));
	}

	// Если включён режим быстрого моделирования, выводим сообщение на экран
	if ( bFastModOn ) {
		wcscpy_s(strBuffer, _T("Включён режим проверки"));	// Вывод сообщения
		dc.TextOut(rc.left+10, rc.bottom-20, strBuffer, (int)wcslen(strBuffer));
		// Вывод величины меры Хемминга
		dc.TextOut(rc.left+10, rc.top+9, cXemMera, (int)wcslen(cXemMera));
	}

//////////////////////////////////////////////////////////////////////////
	// ВРЕМЕННАЯ СТРОКА ДЛЯ ОТЛАДКИ
//	dc.TextOut(rc.left+80, rc.top+115, sMess, (int)wcslen(sMess));
//////////////////////////////////////////////////////////////////////////
	
}

// Нажатие левой клавиши мыши
void CNCLearningView::OnLButtonDown(UINT nFlags, CPoint point)
{
	bLButDown = true; // указываем на нажатие мышки

	// Перемещение указателя мыши
	m_objClientDC->MoveTo(point);
	if ( !bSecret ) 
		m_objClientDC->Ellipse(point.x-1, point.y-1, point.x+1, point.y+1);	// рисуем точку

	// Определяем время установки пера в исходную позицию
	beginTime = ::GetTickCount();
	coordCount = 1;			// обнуление счётчика координат фрагмента (начало нового фрагмента)

	oneImg[0].m_T[oneImg[0].m_Num] = beginTime;
	oneImg[0].m_X[oneImg[0].m_Num] = point.x;
	oneImg[0].m_Y[oneImg[0].m_Num] = point.y;
	oneImg[0].m_Num++; 

	CFormView::OnLButtonDown(nFlags, point);
}

// Отпускание левой клавиши мыши
void CNCLearningView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( bLButDown )
	{
		bLButDown = false;

		// Зная время начала и конца написания слова(фрагмента без отрыва),
		// заполняем вектор отсчётов времени, считаем что все координаты
		// появляются через равномерные промежутки времени
		oneImg[0].m_T[oneImg[0].m_Num-1] = ::GetTickCount();		// фиксируем время отрыва пера
		beginTime = oneImg[0].m_T[oneImg[0].m_Num-1] - beginTime;	// определяем время написания одного фрагмента
		UINT lag = beginTime/coordCount;								// время м/д соседними координатами
		for ( unsigned i = 2; i < coordCount; i++ )
			oneImg[0].m_T[oneImg[0].m_Num-i] = 
			oneImg[0].m_T[oneImg[0].m_Num - i + 1] - lag;

		// Ставим указатель на разрыв 
		oneImg[0].m_X[oneImg[0].m_Num] = 0;
		oneImg[0].m_Y[oneImg[0].m_Num] = 0;
		oneImg[0].m_P[oneImg[0].m_Num] = 0;
		oneImg[0].m_T[oneImg[0].m_Num] = 0;
		oneImg[0].m_Num++;
	}

	CFormView::OnLButtonUp(nFlags, point);
}

// Движение указателя мыши
void CNCLearningView::OnMouseMove(UINT nFlags, CPoint point)
{
	// Если при написании образа вышли за границы области ввода (ввод некорректного образа)
	if ( nFlags != MK_LBUTTON && bLButDown  )
	{
		bLButDown = false;
		MessageBox(_T("Выход за границы поля ввода!"), _T("Ошибка ввода"), MB_ICONWARNING);
		ClearInputArea();
		return;
	}

	// Если нажата левая клавиша
	if( nFlags == MK_LBUTTON && bLButDown )
	{
		// Запись координат рукописного образа 		
		if ( oneImg[0].m_Num < NCMAXCOORD_QUAN ) {
			oneImg[0].m_X[oneImg[0].m_Num] = point.x;
			oneImg[0].m_Y[oneImg[0].m_Num] = point.y;
			// Рисование линии
			if ( !bSecret ) {
				m_objClientDC->LineTo(point.x, point.y);
			}
			// Подсчёт количества считываемых координат
			oneImg[0].m_Num++;
			coordCount++;
		} else {
			MessageBox(_T("Длина введённого рукописного образа\nпревышает установленный лимит!"), _T("Слишком длинное слово"), MB_ICONWARNING);
			ClearInputArea();
		}
	}

	CFormView::OnMouseMove(nFlags, point);
}

// Очистить область ввода
void CNCLearningView::ClearInputArea()
{
	oneImg[0].m_Num = 0;			// обнуление счётчика координат
	ZeroMemory(oneImg[0].m_X, NCMAXCOORD_QUAN*sizeof(UINT)); // обнуление массива координат
	ZeroMemory(oneImg[0].m_Y, NCMAXCOORD_QUAN*sizeof(UINT)); // обнуление массива координат
	ZeroMemory(oneImg[0].m_P, NCMAXCOORD_QUAN*sizeof(UINT)); // обнуление массива давления
	ZeroMemory(oneImg[0].m_T, NCMAXCOORD_QUAN*sizeof(UINT)); // обнуление массива отсчётов времени
	cXemMera = _T("");				// используется в режиме проверки
	RedrawWindow();					// перерисовать область

	// Снять выделение
//	m_wndBotDlgBar.m_ClearBtn.SetCheck(BST_CHECKED);
//	m_wndRightBar.m_AddImageBtn.SetCursor( ::LoadCursor(NULL, IDC_APPSTARTING) );
//	UINT i = m_wndBotDlgBar.m_ClearBtn.GetState();
//	m_wndBotDlgBar.m_ClearBtn.SetState( !(m_wndBotDlgBar.m_ClearBtn.GetState() & 0x0008) );

}

// ******************************* КОНЕЦ РИСОВАНИЕ ****************************** //

//
bool CNCLearningView::isReady()
{
  return true;
}
// Проверка введенного образа в режиме обучения/тестирования
void CNCLearningView::CheckImage()
{
  if (bErr)
    return;

	// Проверка количества координат
	if( oneImg[0].m_Num == 0 ) {
		MessageBox(_T("You must enter a handwritten image!"),_T("Enter the image"), MB_ICONSTOP);
		RedrawWindow();
		return;
	}
	// Проверка наличия весов
	if ( weightsArr[0] == 0.0f ) {
		MessageBox(_T("Отсутствует файл весов.\nВначале необходимо обучить сеть."), _T("Сеть не обучена"), MB_ICONWARNING);
		return;
	}

	// Моделирование сети на полученных данных, вычисление количества несовпадений
	int errorCount;								// количество несовпадений

	// Проверка введённого образа в режиме тестирования
	if ( m_wndRightBar.mode == NCTEST_MODE )
	{

		// Преобразовываем рукописный образ в ключ и 
		// подсчитываем количество выходов для светофора
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\");
		int yellowSvet =
						BncExtractKey_2 ( objFileManip.tofileFullPath, oneImg[0], weightsArr, mixKey, checkKey );

		// Хеширование полученного ключа
		NCHASH checkHash;
		BncCreateHash ( checkKey, sizeof(NCKEY), checkHash );

		// Сравнение хеша исходного ключа и полученного 
		BOOL bEqual = BncCompareHash ( hashKey, checkHash );

//////////////////////////////////////////////////////////////////////////
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\testKeys.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, checkKey, _T("a"));
//////////////////////////////////////////////////////////////////////////

		if ( bEqual ) {		// если введён правильный образ
			// Загорается зелёный свет
			m_wndRightBar.iLight = 3;
			m_wndRightBar.RedrawWindow();
			MessageBox(_T("Correct image entered"),_T("Result of checking"), MB_ICONASTERISK | MB_OK);
			// Выключаем светофор
			m_wndRightBar.iLight = 0;
			m_wndRightBar.RedrawWindow();
		} else {
			// Т.е. если более 90% выходов попали в интервал,
			// то загорается жёлтый свет, иначе красный
			if ( yellowSvet > NEURON_COUNT*0.9 ) {
				// Загорается желтый свет
				m_wndRightBar.iLight = 2;
				m_wndRightBar.RedrawWindow();
				MessageBox(_T("The entered image is close to the correct one \n Please try again"),_T("Result of checking"), MB_ICONWARNING | MB_OK);
				// Выключаем светофор
				m_wndRightBar.iLight = 0;
				m_wndRightBar.RedrawWindow();
			} else {
				// Загорается красный свет
				m_wndRightBar.iLight = 1;
				m_wndRightBar.RedrawWindow();
				MessageBox(_T("Wrong image entered"),_T("Result of checking"), MB_ICONWARNING | MB_OK);
				// Выключаем светофор
				m_wndRightBar.iLight = 0;
				m_wndRightBar.RedrawWindow();
			}
		}
		ClearInputArea();
	}
	
	// Проверка введённого образа в режиме обучения
	if ( m_wndRightBar.mode == NCTRAIN_MODE )
	{
		// Преобразовываем рукописный образ в ключ
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\");
		BncExtractKey_2 ( objFileManip.tofileFullPath, oneImg[0], weightsArr, mixKey, checkKey );
//////////////////////////////////////////////////////////////////////////
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\testKeys.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, checkKey, _T("a"));
//////////////////////////////////////////////////////////////////////////
		// Сравниваем полученный ключ с обучающим
		errorCount = BncCompareKeys( si.key, checkKey );

		if ( errorCount == 0 ) {	
			CGetKeyDlg objGetKeyDlg(NCTRAIN_MODE, TRUE, FALSE, &si.key, &checkKey, NULL);
			objGetKeyDlg.DoModal();
			ClearInputArea();
		} else  {
			CGetKeyDlg objGetKeyDlg(NCTRAIN_MODE, FALSE, FALSE, &si.key, &checkKey, NULL);
			objGetKeyDlg.DoModal();

			// Если образ близок к обучающим образам, предлагаем пользователю
			// добавить образ в обучающую выборку и переобучить сеть
			if ( errorCount < NEURON_COUNT*1 ) {
				CFont font;
				LOGFONT lf;
				TCHAR strBuffer[51];
				memset(&lf, 0, sizeof(LOGFONT));       
				lf.lfHeight = 16;      
				lf.lfWeight = FW_BOLD;
				wcscpy_s(lf.lfFaceName,  _T("MS Sans Serif"));
				VERIFY(font.CreateFontIndirect(&lf));  
				CClientDC dc(this);
				dc.SelectObject(&font);
				wcscpy_s(strBuffer, _T("Если Вы хотите, чтобы программа лучше Вас узнавала"));
				dc.TextOut(20, 42, strBuffer, (int)wcslen(strBuffer));
				wcscpy_s(strBuffer, _T("нажмите \"Добавить\", а затем \"Обучить\""));
				dc.TextOut(50, 65, strBuffer, (int)wcslen(strBuffer));
			}
			else ClearInputArea();
		}
	}
}

// Удалить выделенный образ (из списка)
void CNCLearningView::DelImage()
{
	// Номер выбранного образа
	int iCheckedImage = m_wndRightBar.m_ImageList.GetCurSel();

	if( iCheckedImage != -1 )
	{
		if ( MessageBox ( _T("Вы уверены, что хотите удалить выбранный образ?"), _T("Удаление обучающего образа"), MB_ICONQUESTION|MB_YESNO) == IDYES )
		{
			objFileManip.DelImageCoord( iCheckedImage, imageCount-1, images );
			
			ClearInputArea();
			imageCount--;
								
			// Уменьшаем количество отображаемых обучающих образов
			m_wndRightBar.m_ImageList.DeleteString(imageCount);
			m_wndRightBar.m_ImageList.SetCurSel(imageCount-1);

//			MessageBox ( _T("Выбранный образ успешно удален."), _T(""), MB_ICONASTERISK);
		}
	} else {
		MessageBox ( _T("Нет выбранного образа."), _T("Образ не выбран"), MB_ICONASTERISK);
	} // конец удаления образа		

}

// Удалить все образы (очистить список)
void CNCLearningView::DelAllImages()
{
	if ( imageCount == 0 ) {
		MessageBox( _T("Список пуст"), _T(""), MB_ICONASTERISK );
		return;
	}
	if ( MessageBox ( _T("Вы уверены, что хотите удалить все сохраненные образы?"), _T("Очистить обучающее множество"), MB_ICONQUESTION|MB_YESNO) == IDYES )
	{
		// Удаляем координаты всех рукописных образов
		objFileManip.DelAllImages ( imageCount, images );		
		m_wndRightBar.m_ImageList.ResetContent();
		ClearInputArea();
		//MessageBox ( _T("Все образы успешно удалены."), _T(""), MB_ICONASTERISK);
	}
}

// Добавление нового образа
void CNCLearningView::AddImage()
{
	// Проверка количества координат
	if( oneImg[0].m_Num == 0 ) {
		MessageBox(_T("Нельзя добавлять пустой образ!"),_T("Нет образа"), MB_ICONSTOP);
		RedrawWindow();
		return;
	}

	if ( imageCount < NCIMAGE_COUNT ) 
	{
		if ( (oneImg[0].m_T[oneImg[0].m_Num-2] - oneImg[0].m_T[0]) < 0 ) {
			MessageBox(_T("Данный образ не может быть добавлен в выборку"),_T("Неверный образ"), MB_ICONSTOP);
			RedrawWindow();
			return;
		}

		// Заносим введённые координаты в массив координат
		images[imageCount].m_Num = oneImg[0].m_Num;
		images[imageCount].m_X = new UINT [oneImg[0].m_Num];
		images[imageCount].m_Y = new UINT [oneImg[0].m_Num];
		images[imageCount].m_P = new UINT [oneImg[0].m_Num];
		images[imageCount].m_T = new UINT [oneImg[0].m_Num];
		for ( unsigned i = 0; i < oneImg[0].m_Num; i++ )	{
			images[imageCount].m_X[i] = oneImg[0].m_X[i];
			images[imageCount].m_Y[i] = oneImg[0].m_Y[i];
			images[imageCount].m_P[i] = oneImg[0].m_P[i];
			images[imageCount].m_T[i] = oneImg[0].m_T[i];
		}
		// Увеличение счётчика образов
		imageCount++;
		// Отображение списка сохранённых образов
		CString str;
		str.Format(_T("     %d"), imageCount);
		m_wndRightBar.m_ImageList.InsertString(imageCount-1, str);
		m_wndRightBar.m_ImageList.SetCurSel(imageCount-1);
	}
	else MessageBox( _T("Добавление образа невозможно!\nДостигнуто максимальное количество\nобрабатываемых системой образов!"), _T("Ошибка добавления"), MB_ICONWARNING);

	// Очистка области ввода	
	ClearInputArea();
}

// Обучение нейронной сети
void CNCLearningView::LearnNet()
{
  if (bErr)
    return;

	// Проверка достаточного размера выборки
	if ( imageCount > 2 )
	{
		// Обучение нейронной сети
		CProgressDlg objDlg ( imageCount, images, &si, &mixKey, &weightsArr, NULL );
		objDlg.DoModal();
		bLearnNet = TRUE;
//////////////////////////////////////////////////////////////////////////
    // Сохранить обучающий ключ
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\trainKey.txt");
    objFileManip.SaveKeys(objFileManip.tofileFullPath, si.key, _T("w"));
    // Сохранить веса
    objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si.userName + _T("\\weights.txt");
    objFileManip.SaveCoefs(objFileManip.tofileFullPath, "Таблица весовых коэффициентов:\n", weightsArr, sizeof(NCWEIGHTS)/sizeof(weightsArr[0]), _T("w"));
//////////////////////////////////////////////////////////////////////////
	}
	else
		MessageBox( _T("Недостаточно образов для обучения"), _T("Добавьте обучающих образов"), MB_ICONASTERISK );
}

// Активация элемента списка образов
void CNCLearningView::OnShowImage()
{
	ClearInputArea();

	// Номер выбранного образа
	int iCheckedImage = m_wndRightBar.m_ImageList.GetCurSel();

	if ( iCheckedImage == -1) return;	// т.е. в поле щёлкнули, а образ не выбрали

	// Чтение координат выбранного образа
	if ( bObvodkaOn ) {		// если режим обводки включён
		fixImg[0].m_Num = images[iCheckedImage].m_Num;
		memcpy ( fixImg[0].m_X, images[iCheckedImage].m_X, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_Y, images[iCheckedImage].m_Y, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_P, images[iCheckedImage].m_P, fixImg[0].m_Num*sizeof(UINT) );
		memcpy ( fixImg[0].m_T, images[iCheckedImage].m_T, fixImg[0].m_Num*sizeof(UINT) );

	} else {				// если не используется обводка
		oneImg[0].m_Num = images[iCheckedImage].m_Num;
		memcpy ( oneImg[0].m_X, images[iCheckedImage].m_X, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_Y, images[iCheckedImage].m_Y, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_P, images[iCheckedImage].m_P, oneImg[0].m_Num*sizeof(UINT) );
		memcpy ( oneImg[0].m_T, images[iCheckedImage].m_T, oneImg[0].m_Num*sizeof(UINT) );
	}

	// Если включён режим быстрой проверки, вычисляем меру Хемминга
	if ( bFastModOn )
	{
		// Преобразовываем рукописный образ в ключ
		BncExtractKey ( oneImg[0], weightsArr, mixKey, checkKey );
		// Сравниваем полученный ключ с обучающим
		int errCount = BncCompareKeys( si.key, checkKey );
		cXemMera.Format(_T("Мера Хемминга : %d"),errCount);
	}

	RedrawWindow();	// перерисовать область

}

// Тайный режим работы
void CNCLearningView::OnSecretBtn()
{
	bSecret = true;
	ClearInputArea();
}


// Открытый режим работы
void CNCLearningView::OnPublicBtn()
{
	bSecret = false;
	ClearInputArea();
}

// Режим создания пользовательского пароля
void CNCLearningView::OnSetPswM()
{
	// Если выбран режим обучения
	if ( m_wndRightBar.mode != NCTRAIN_MODE )
    return;

  CUserAccountDlg objDlg;
	INT_PTR nResponse = objDlg.DoModal();
	if ( nResponse != IDOK )
    return;

  // Указываем, что режим обучения пока не запускался
	bLearnNet = FALSE;		
	// Считываем новый обучающий ключ
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\index.ini");
  if ( objFileManip.LoadIniData(objFileManip.tofileFullPath,si) != ERROR_SUCCESS ) {
    MessageBox(_T("Ошибка чтения обучающего ключа."), _T("Обучающий ключ не задан"), MB_ICONWARNING);
    bErr = true;
  }
  bErr = false;
}

// Делаем пунк меню неактивным в режиме тестирования 
// и активным в режиме обучения
void EnableMyMenu ( CCmdUI *pCmdUI )
{
	if ( m_wndRightBar.mode == NCTEST_MODE ) 
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

// Сохранить все обучающие образы на диск
void CNCLearningView::SaveImages()
{
	// Если обучающие образы существуют, то сохраняем их в файл
	if ( imageCount != 0 ) {
		TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
		CFileDialog fileOpenDlg (FALSE, _T("my"), _T("MyImages.dat"),
			OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

		if ( fileOpenDlg.DoModal () == IDOK ) {
			CString str = fileOpenDlg.GetPathName();	// путь по которому следует записать обучающие образы
			WCHAR szPath[MAX_PATH];
			lstrcpy(szPath, str);
			long lErr = objFileManip.SaveAllImages( szPath, imageCount, images );
			//images
		
			
			// Проверяем записан ли файл с обучающими примерами на диск
			if ( lErr == ERROR_SUCCESS ) 
				str = _T("Все обучающие примеры успешно сохранены!");
			else 
				str = _T("Программа не смогла сохранить все обучающие примеры.");

			MessageBox(str, _T("Сохранение образов"), MB_ICONASTERISK);
		}

	}
	else
		MessageBox(_T("В обучающей выборке нет ни одного примера."), _T("Обучающая выборка пуста."), MB_ICONASTERISK);

}
void CNCLearningView::OnUpdateSaveImages(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// Загрузить обучающие образы с диска
void CNCLearningView::LoadImages()
{
	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), _T("MyImages.dat"),
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
		
	if ( fileOpenDlg.DoModal() == IDOK ) {
		CString str = fileOpenDlg.GetPathName();	    // путь к файлу со словами
		objFileManip.DelAllImages(imageCount, images);  // удалить все старые обучающие образы
		imageCount = 0;

		// Загружаем новые обучающие образы
		long lErr =	objFileManip.LoadAllImages(str, imageCount, images);

		// Если все образы успешно прочитаны, то выводим соответствующее сообщение
		if( lErr == ERROR_SUCCESS )
			str.Format(_T("Программа смогла прочитать %d образов."), imageCount);
		else
			str = " Программа не смогла прочитать все обучающие примеры.";

		// Вывод сообщения о успешной\неудачной загрузке
		MessageBox(str, _T("Чтение обучающих примеров"), MB_ICONASTERISK);

		// Очистка области ввода
		ClearInputArea();

		// Отображение списка сохранённых образов
		m_wndRightBar.m_ImageList.ResetContent();
		for( unsigned iCount = 0; iCount < imageCount; iCount++) {
			str.Format(_T("     %d"), iCount+1);
			m_wndRightBar.m_ImageList.InsertString(iCount,str);
		}	
	}
}

void CNCLearningView::OnUpdateLoadImages(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// Групповое моделирование (проверить все образы из списка)
void CNCLearningView::CheckAllImages()
{
	ClearInputArea();

	// Если в списке есть образы, то тестируем, иначе - нет
	if ( imageCount == 0 ) {
		MessageBox(_T("Для выполнения процедуры проверки в списке\nдолжен содержаться хотя бы один образ."), _T("Список пуст"), MB_ICONASTERISK);
		return;
	}

	int count = 0;	// количество образов СВОЕГО
	// Последовательно вычисляем меры Хемминга на всех образах из списка
	for ( unsigned i = 0; i < imageCount; i++ )
	{
		// Преобразуем образ в ключ
		BncExtractKey ( images[i], weightsArr, mixKey, checkKey );
		// Сравниваем исходный ключ и полученный
		int mera = BncCompareKeys ( si.key, checkKey );
		if ( mera == 0 ) count++;
	}

	// Вывод результатов проверки на экран
	CString str;
	str.Format(_T("Всего проверено образов : %d"), imageCount);
	m_objClientDC->ExtTextOut(5,5,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("Результаты проверки : ");
	m_objClientDC->ExtTextOut(5,25,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("Количество образов \"Своего\" : %d. Количество образов \"Чужого\" : %d."), count, imageCount-count);
	m_objClientDC->ExtTextOut(5,40,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);

/*
	ClearInputArea();

	CConversionSFN objSFN;
	CSimNet objSim;
	// Массив под коэффициенты Фурье всех рукописных образов
	float *coefficientsArr = new float [imageCount*NCFOURIER_COEF];
	objSFN.NcCalcImagesCoef ( imageCount, images, coefficientsArr );

	// Моделируем работу сети на полученных коэффициентах
//////////////////////////////////////////////////////////////////////////
	int xemArr[100];
	objSim.NetRecognition(imageCount,coefficientsArr, weightsArr, binKeyArr,xemArr);
	float aver = 0;
	for (int i =0; i<imageCount; i++) {
		aver += xemArr[i]; 
	}
	aver /= imageCount;
//////////////////////////////////////////////////////////////////////////
	int errArr[2] = {0};
	objSim.GroupSimulationForLayerCompare (
		imageCount, coefficientsArr, weightsArr, binKeyArr, errArr );
	delete [] coefficientsArr;
	CString str;
	str.Format(_T("Всего проверено образов : %d"), imageCount);
	m_objClientDC->ExtTextOut(5,5,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("Результаты проверки на первом слое : ");
	m_objClientDC->ExtTextOut(5,25,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("Количество образов \"Своего\" : %d. Количество образов \"Чужого\" : %d."), errArr[0], imageCount-errArr[0]);
	m_objClientDC->ExtTextOut(5,40,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str = _T("Результаты проверки на втором слое : ");
	m_objClientDC->ExtTextOut(5,60,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
	str.Format(_T("Количество образов \"Своего\" : %d. Количество образов \"Чужого\" : %d."), errArr[1], imageCount-errArr[1]);
	m_objClientDC->ExtTextOut(5,75,ETO_CLIPPED, NULL, str, (UINT)wcslen(str), NULL);
*/	
}
void CNCLearningView::OnUpdateCheckAll(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
}

// С помощью базы тестовых образов пытаюсь угадать обучающий ключ
void CNCLearningView::OnCracking()
{
//////////////////////////////////////////////////////////////////////////
// Убрал до лучших времён
  return;
//////////////////////////////////////////////////////////////////////////


	LONG lErr = ERROR_SUCCESS;

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() != IDOK ) return;
	
	CString path = fileOpenDlg.GetPathName();	    // путь к файлу со словами
	CString name = fileOpenDlg.GetFileName();		// имя файла
	CString folder;									// папка с биообразами

	// Определяем путь к папке с файлами
	folder = path.Left( path.Find(name, 0) );

	// Загружаем образы на которых будем подбирать ключ
	BNCSIGNIMAGE *tstImg;   
	tstImg = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT count = 0;			// количество образов в одном файле
	UINT totalCount = 0;	// общее количество образов
	int	sumKey[NEURON_COUNT] = {0}; // сумма элементов всех ключей
	int keyArr[NEURON_COUNT] = {0}; 
	int crKey[NEURON_COUNT]  = {0};

	// Считываем координаты тестовых образов
	WIN32_FIND_DATA FindFileData;
	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, folder );
	wcsncat_s ( cPath, _T("*.dat"), 5 );
	// Определяем имя файла с координатами
	HANDLE hFind = FindFirstFile( cPath, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE ) 
	{
		// Последовательное тестирование на образах из всех файлов
		do 
		{	
			// Загружаем тестовые образы
			count = 0;			
			CString str		= FindFileData.cFileName;
			CString toCoord = folder + str;
			lErr = objFileManip.LoadAllImages(toCoord, count, tstImg);
			if ( lErr != ERROR_SUCCESS ) continue;
			totalCount += count;

			// Последовательное вычисление ключей
			for ( unsigned i = 0; i < count; i++ )
			{
				// Преобразуем образ в ключ
				BncExtractKey ( tstImg[i], weightsArr, mixKey, checkKey );
				BncConvertKey ( checkKey, keyArr );
				for ( int i = 0; i < NEURON_COUNT; i++ ) {
					sumKey[i] += keyArr[i];
				}
			}
			objFileManip.DelAllImages(count, tstImg);

		} while ( FindNextFile(hFind, &FindFileData) != 0 );

		FindClose(hFind);

	}

	// Обрабатываем полученные результаты

	// Элемент, который чаще появляется на выходе данного нейрона,
	// рассматриваем как "правильный" код/бит
	for ( int i = 0; i < NEURON_COUNT; i++ )  {
		crKey[i] = (sumKey[i] < (int)(totalCount/2)) ? 0 : 1;
	}
	// Нейроны, выходы которых не удалось угадать тестированием на базе,
	// правильно обучены. Вычисляем их количество.
	BncConvertKey ( si.key, keyArr );//двоичный обучающий ключ
	int goodN = 0;	// количество "правильно" обученных нейронов
	for ( int i = 0; i < NEURON_COUNT; i++ ) {
		if ( keyArr[i] != crKey[i] )	goodN++;		
	}
	CString str;
	str.Format(_T("Количество правильно обученных нейронов: %d"), goodN);
 	MessageBox( str, _T(""), MB_OK);

	delete [] tstImg;

}

// Включить режим обводки (позволяет обводить образы из списка)
void CNCLearningView::OnObvodkaReg()
{
	// Включаем/выключаем режим обводки	
	bObvodkaOn ^= true;
	if ( bObvodkaOn ) {
		fixImg[0].m_Num = 0;
		MessageBox(_T("Режим обводки включён"));
	}
	else
		MessageBox(_T("Режим обводки выключен"));
	
	ClearInputArea();
}
void CNCLearningView::OnUpdateObvodka(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	
	// Ставим либо снимаем галочку
	if ( bObvodkaOn )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
// Режим быстрой проверки образов 
// При выборе обучающего образа из списка выводится
// сам образ и Мера Хемминга
void CNCLearningView::OnProverkaList()
{
	// Включаем/выключаем режим проверки	
	bFastModOn ^= true;
	if ( bFastModOn )	MessageBox(_T("Режим проверки включён"));
	else				MessageBox(_T("Режим проверки выключен"));
	ClearInputArea();
}
void CNCLearningView::OnUpdateProverka(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	// Ставим либо снимаем галочку
	if ( bFastModOn )	pCmdUI->SetCheck(1);
	else				pCmdUI->SetCheck(0);
}

// Пытаюсь автоматически разделить людей на группы
// Для этого обучаю сеть на образах различных пользователей, 
// затем тестирую обученную сеть и по полученным результатам разбиваю на группы
void CNCLearningView::OnStability()
{
  //////////////////////////////////////////////////////////////////////////
  // Убрал до лучших времён
  return;
  //////////////////////////////////////////////////////////////////////////

	ClearInputArea();

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() == IDOK ) 
	{
		CString path = fileOpenDlg.GetPathName();	    // путь к файлу со словами
		CString name = fileOpenDlg.GetFileName();		// имя файла
		CString folder;									// папка с биообразами

		// Определяем путь к папке с файлами
		folder = path.Left( path.Find(name, 0) );

		CStabilityDlg objDlg ( folder, &si.key, &mixKey );
		objDlg.DoModal();
	}
}
// Режим анализа влияния нагрузок на стабильность и стойкость
void CNCLearningView::OnBodyBurdenReg()
{
  //////////////////////////////////////////////////////////////////////////
  // Убрал до лучших времён
  return;
  //////////////////////////////////////////////////////////////////////////

	// Анализ влияния нагрузок на стабильность и стойкость
	CBodyBurdenDlg objDlg(&si, &mixKey, NULL);
	objDlg.DoModal();
}


// ВРЕМЕННАЯ ФУНКЦИЯ (ОПРЕДЕЛЕНИЕ КОЛИЧЕСТВА ОБРАЗОВ У КОТОРЫХ ВЫХОДЫ СЛУЧАЙНОГО НЕЙРОНА
// СОВПАДАЮТ С ВЫХОДАМИ ЭТОГО ЖЕ НЕЙРОНА НА СВОИХ)
// count	- [in] количество образов
// allKeys	- [in] ключи, полученные на count тестовых образах
// pos		- [in] номера образов у которых выход r-го нейрона совпал с обучающим ключом
// trainKey - [in] обучающий ключ
CString GetCountAndPos( int count, int allKeys[], int pos[], int trainKey[] )
{
	CString str;

	int *tmpKey = new int [count*NEURON_COUNT];
	for ( int i = 0; i < count; i++ ) 
	{
		for ( int j = 0; j < NEURON_COUNT; j++ ) 
		{
			tmpKey[i*NEURON_COUNT+j] = allKeys[pos[i]*NEURON_COUNT+j];
		}
	}
	int r = rand()%256;	// номер рассматриваемого нейрона
	int coin = 0;
	for ( int i = 0; i < count; i++ ) 
	{
		if ( tmpKey[i*NEURON_COUNT + r] == trainKey[r] ) 
		{ 
			pos[coin] = i;
			coin++;
		}
	}
	str.Format(_T("%d\t%d "), count, coin);
	if ( count > 10) {
		str += GetCountAndPos ( coin, tmpKey, pos, trainKey );
	}
	delete [] tmpKey;

	return str;
}

// Протестировать обученную сеть на тестовых образах
void CNCLearningView::OnTestOnTI()
{
//////////////////////////////////////////////////////////////////////////
// НОВАЯ ПРОБНАЯ ФУНКЦИЯ ТЕСТИРОВАНИЯ
	// Загружаем полученные ключи
//	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKey_all.txt");
//	int totalIMGCount = 10189;
	//objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKeyW.txt");
	//const int totalIMGCount = 100000;
	//int *tempKey = new int [totalIMGCount*NEURON_COUNT];
	//int iCount = totalIMGCount*NEURON_COUNT;
	//objFileManip.LoadXem(objFileManip.tofileFullPath, tempKey, iCount);
	//int trnKey[NEURON_COUNT] = {0}; 
	//BncConvertKey ( trainKey, trnKey );//двоичный обучающий ключ

	//srand(::GetTickCount());

	//// Определяем количество образов у которых выход случайного нейрона совпал с обучающим ключем	
	//int pos[totalIMGCount];		// номера образов у которых выход r-го нейрона совпал с обучающим ключом
	//for ( int i = 0; i < totalIMGCount; i++ ) pos[i] = i;
	//CString str = GetCountAndPos ( totalIMGCount, tempKey, pos, trnKey );

	//return;
//////////////////////////////////////////////////////////////////////////

/**/
	ClearInputArea();

	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, _T("my"), NULL,
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if ( fileOpenDlg.DoModal() == IDOK ) 
	{
		CString path = fileOpenDlg.GetPathName();	    // путь к файлу со словами
		CString name = fileOpenDlg.GetFileName();		// имя файла
		CString	type = fileOpenDlg.GetFileExt();		// тип файла
		CString folder;									// папка с биообразами

		// Определяем путь к папке с файлами
		folder = path.Left( path.Find(name, 0) );
				
		CTestDlg objTest ( NCTEST_MODE_TIWN, TRUE, folder, type, &si, &mixKey, &weightsArr );
		objTest.DoModal();
	}
/**/	
/*
	ClearInputArea();

	// Вычисляем качество мер Хемминга на тестовых образах
	int   tstImageCount = 0;			// количество тестовых образов

	// Чтение тестовых образов
	BNCSIGNIMAGE *tstImages;			// указатель на координаты тестовых рукописных образов
	tstImages = new BNCSIGNIMAGE[NCIMAGE_COUNT];

	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\TImages.dat");
	// Считываем координаты тестовых образов из файла по указанному пути
	long lErr = 
		objFileManip.LoadAllImages(objFileManip.tofileFullPath, tstImageCount, tstImages);
	// Если все образы успешно прочитаны, то выводим соответствующее сообщение
	if( lErr != ERROR_SUCCESS ) {
		MessageBox(_T("Тестовые образы не найдены"), _T("Тестирование прервано!"), MB_ICONWARNING);
		return;
	}
	
	// Вычисляем моменты мер Хемминга на тестовых образах
	BNCMOMENTS params;	// параметры мер Хемминга
	params = BncGetXemMoments ( TRUE, trainKey, mixKey, tstImages, 
								tstImageCount, weightsArr );

	// Удаление тестовых образов
	for ( int iCount = 0; iCount < tstImageCount; iCount++ ) {
		delete [] tstImages[iCount].points;
		delete [] tstImages[iCount].m_T;
	}
	delete [] tstImages; 
	
	CString str;
	str.Format(_T(" Матожидание : %.3f\n Дисперсия : %.3f\n Качество : %.3f\n"), params.faver, params.fdisp, params.fqual);
	MessageBox(str, _T("Тестирование завершено!"), MB_ICONASTERISK);
*/
}

// Протестировать обученную сеть на белом шуме
void CNCLearningView::OnTestOnWN()
{
	ClearInputArea();

//	BNCMOMENTS params;	// параметры мер Хемминга
	CTestDlg objTest ( NCTEST_MODE_TIWN, FALSE, NULL, NULL, &si, &mixKey, &weightsArr );
	objTest.DoModal();
//////////////////////////////////////////////////////////////////////////
	//POINT netOut01[NEURON_COUNT];	// кол-во появлений 0 и 1 на всех тестовых образах
	//ZeroMemory(netOut01, sizeof(netOut01));
	//int i = 0;
	//while ( i < 5000 )
	//{
	//	int count = 20;
	//	params = BncGetXemMoments ( FALSE, trainKey, mixKey, 
	//								NULL, count, weightsArr );
	//	for ( int j = 0; j < NEURON_COUNT; j++ ) {
	//		netOut01[j].x += params.p01[j].x;
	//		netOut01[j].y += params.p01[j].y;
	//	}
	//	//Сохраняем полученные ключи
	//	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\tempKeyW.txt");
	//	objFileManip.SaveXem(objFileManip.tofileFullPath, params.tmpKey, count*NEURON_COUNT, _T("a"));
	//	i++;
	//}
//////////////////////////////////////////////////////////////////////////
	/*params = BncGetXemMoments ( FALSE, trainKey, mixKey, 
								NULL, 1000, weightsArr );
	// Вычисляем стойкость системы к атакам подбора
	float fstab = 0.87f+0.221f*params.fqual*params.fqual;

	CString str;
	str.Format(_T("\tМатожидание : %.3f\n \tДисперсия : %.3f\n \tКачество : %.3f\nСтойкость к атакам подбора равна : 10^%.3f"), params.faver, params.fdisp, params.fqual, fstab);
	MessageBox(str, _T("Тестирование завершено!"), MB_ICONASTERISK);*/
}

// Второй поток для режима слайд-шоу
UINT CNCLearningView::ComputeThreadProc(LPVOID pParam)
{
	::WaitForSingleObject(g_eventStart, INFINITE);
	g_nCount = 1;
	THREADDATA *sourceData;		// исходные данные
	sourceData = (THREADDATA*) pParam;
	bool bEnd = false;			// завершение потока
	while (::WaitForSingleObject(g_eventKill, 0) != WAIT_OBJECT_0)
	{
		if (bEnd) break;
		// "Размножение" образов (преобразуем sourceData->count исходных рукописных
		// образов в pairCount*(interpCount) образов)
		// Например, для 32 исходных образов pairCount = 496 (количество различных пар)
		// interpCount = 11 получается 5456 образов
		UINT i1, i2;	// номера рассматриваемых образов
		int pairCount = 0;		// количество пар образов
		int interpCount = 12;	// количество генерируемых образов между i1 и i2 (получаем 
								// с помощью линейной интерполяции координат двух рассматриваемых
								// образов), исходные образы тоже остаются, т.о. получается 
								// interpCount-2 новых образа
		for ( i1 = 0; i1 < sourceData->count-1; i1++ ) // номер первого образа в паре
		{
      int tmpCount = 0;
			if (bEnd) break;
			for ( i2 = i1+1; i2 < sourceData->count; i2++ ) // номер второго образа в паре
			{
				if (bEnd) break;
				// количество точек в новом образе
				int pointCount = 
					(sourceData->img[i1].m_Num < sourceData->img[i2].m_Num) ? 
					sourceData->img[i1].m_Num : sourceData->img[i2].m_Num;
				for ( int i = 0; i < interpCount; i++ )
				{
					if (bEnd) break;
					for ( int j = 0; j < pointCount; j++ )
					{
						the_instance->oneImg[0].m_X[j] = 
							(UINT)(((interpCount-i)/(float)interpCount) * sourceData->img[i1].m_X[j] + 
							(i/(float)interpCount) * sourceData->img[i2].m_X[j]);
						the_instance->oneImg[0].m_Y[j] = 
							(UINT)(((interpCount-i)/(float)interpCount) * sourceData->img[i1].m_Y[j] + 
							((i)/(float)interpCount) * sourceData->img[i2].m_Y[j]);
					}
					the_instance->oneImg[0].m_Num = pointCount;
          tmpCount++;
          sMessage.Format(_T("%d"),tmpCount);
					// приостанавливаем поток (для перерисовки полученного образа)
					g_eventStart.ResetEvent();
					::WaitForSingleObject(g_eventStart, INFINITE);

					if (::WaitForSingleObject(g_eventKill, 0) == WAIT_OBJECT_0)
					{
						bEnd = TRUE;
					}

				}
				pairCount++;
			}
		}
	}
	// Сообщаем родительскому окну о завершении работы потока
	::PostMessage(sourceData->pWnd, WM_THREADFINISHED, 0, 0);
	g_nCount = 0;

	return 0; // ends the thread

}

// Удаляем созданный таймер для слайд-шоу после завершения дочернего потока
LONG CNCLearningView::OnThreadFinished(UINT wParam, LONG lParam)
{
	KillTimer(m_nTimer); 
	return 0;
}
// Увеличение количества тестовых образов за счёт размытия параметров
// Автоматически из 32 образов из папки minXemImg.dat получаем нужное количество
// тестовых образов, размываем параметры исходных образов
void CNCLearningView::OnTIMultiplication()
{
  //////////////////////////////////////////////////////////////////////////
  // Убрал до лучших времён
  return;
  //////////////////////////////////////////////////////////////////////////

// ДАННАЯ ПРОЦЕДУРА НЕ ОСУЩЕСТВЛЯЕТ МОДЕЛИРОВАНИЕ НЕЙРОННОЙ СЕТИ НА ГЕНЕРИРУЕМЫХ ОБРАЗАХ!!!
// ОСУЩЕСТВЛЯЕТСЯ ТОЛЬКО ВИЗУАЛЬНАЯ ДЕМОНСТРАЦИЯ ПРОЦЕДУРЫ МОРФИНГА

	bSlideShowOn ^= true;

	// Если поток для слайд-шоу запущен, останавливаем его выполнение
	if (g_nCount != 0)
	{
		g_eventKill.SetEvent();
		return;
	}

  // Загружаем исходные образы 
	if (imageCount != 0) {
		objFileManip.DelAllImages(imageCount, images); 
	}
	// Используем не исходные образы из файла Data\\minXemImg.dat, а уже промасштабированные
	// образы из файла Data\\AfterScalImg.dat (т.к. для размножения нужно чтобы в каждом
	// образе было одинаковое количество точек)
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\AfterScalImg2.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, imageCount, images);
	if ( lErr != ERROR_SUCCESS ) { 
		return;
	}
	// Очистка области ввода
	ClearInputArea();
	// Отображение списка сохранённых образов
	m_wndRightBar.m_ImageList.ResetContent();
	CString str;
	for( unsigned iCount = 0; iCount < imageCount; iCount++) {
		str.Format(_T("     %d"), iCount+1);
		m_wndRightBar.m_ImageList.InsertString(iCount,str);
	}	

	// устанавливаем таймер для просмотра слайд-шоу
	m_nTimer = (int)SetTimer(1, 1000, NULL);//через каждую секунду
 
	// Создать второй поток (для режима слайд-шоу)
	// Данные для передачи потоку
	thData.pWnd = GetSafeHwnd();
	thData.count = imageCount;
	thData.img = images;
	m_pRecalcWorkerThread =	AfxBeginThread(ComputeThreadProc, &thData);

	// Запустить второй поток на выполнение
	g_eventStart.SetEvent();

}
// Ставим либо снимаем галочку для режима слайд-шоу
void CNCLearningView::OnUpdateTIMultiplication(CCmdUI *pCmdUI)
{
	EnableMyMenu(pCmdUI);
	// Ставим либо снимаем галочку
	if ( bSlideShowOn )	pCmdUI->SetCheck(1);
	else				pCmdUI->SetCheck(0);
}
// В режиме слайдшоу показываем сгенерированные образы
void CNCLearningView::OnTimer(UINT_PTR nIDEvent)
{
	RedrawWindow();				// перерисовываем поле ввода
	g_eventStart.SetEvent();	// возобновляем выполнение потока
	CFormView::OnTimer(nIDEvent);
}

// Тестирование сети с помощью отбора образов Чужого с минимальной мерой Хемминга 
// и увеличение числа отобранных образов
// Увеличение количества тестовых образов за счёт размытия параметров
// Автоматически из 32 образов из папки minXemImg.dat получаем нужное количество
// тестовых образов, размываем параметры исходных образов
void CNCLearningView::OnMorfing()
{
//////////////////////////////////////////////////////////////////////////
// Убрал до лучших времён
  return;
//////////////////////////////////////////////////////////////////////////

	CTestDlg objTest ( NCTEST_MODE_MORFING, TRUE, NULL, NULL, &si, &mixKey, &weightsArr );
	objTest.DoModal();
}

// Определение ширины области группы образов по пути sPath
// Возвращает ширину области
float CNCLearningView::GetGroupImgWidth(CString sPath)
{
	// Загружаем исходные образы 
	BNCSIGNIMAGE *tstimages;
	tstimages = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT tstimageCount = 0;	
	WCHAR szPath[MAX_PATH];	
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, tstimageCount, tstimages);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(tstimageCount, tstimages); 
		delete [] tstimages;
		return 0.0;
	}
	// Вычисляем коэффициенты Фурье исходных образов
	FLOAT *coefs;		// одномерный массив функционалов всех тестовых образов
	// Выделение памяти под коэффициенты всех образов
	coefs = new FLOAT[tstimageCount*NCFOURIER_COUNT];
	BncGetImgParams (tstimages, tstimageCount, coefs );

	// Вычисляем центр области
	float *averArr = new NCFOURIER;
	float *dispArr = new NCFOURIER;
	float *qualArr = new NCFOURIER;
	fadq ( tstimageCount, NCFOURIER_COUNT, coefs, averArr, dispArr, qualArr );
	float width = faverElem(NCFOURIER_COUNT, dispArr);	// ширина области
	float step = width/100;								// ширина шага
	float goal = 0;										// найденная ширина области
	// Моделируем обученную нейронную сеть на образе-центр
	NCKEY key1, key2;
	BncCoefsToKey ( averArr, weightsArr, mixKey, key1 );
	int errorCount = 0;
	do 
	{
		goal += step; 
		float *tmpArr = new NCFOURIER;
		for ( int i = 0; i < NCFOURIER_COUNT; i++ )
			tmpArr[i] = averArr[i] + goal;
		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
		errorCount = BncCompareKeys( key1, key2 );
		delete [] tmpArr;
	} while ( errorCount < 1 );

	//float res = goal/width;

	errorCount = BncCompareKeys( si.key, key2 );

	delete [] averArr;
	delete [] dispArr;
	delete [] qualArr;

	delete [] coefs;

	objFileManip.DelAllImages(tstimageCount, tstimages); 
	delete [] tstimages;
/*
	CString str;
	// d - среднее значение дисперсии центрального образа,
	// k - коэффициент масштабирования дисперсии,
	// w - найденная ширина области
	str.Format(_T("Ширина рассматриваемой области (группы образов) равна:\n d * k = w\n %.3f * %.3f = %.3f"), width, res, goal);
	MessageBox(str, _T("Полученный результат"), MB_OK);
*/
//	return goal;
	return (float)errorCount;
}

// Определяем ширину области "Свой" и "Чужой"
void CNCLearningView::OnGrWidth()
{
//////////////////////////////////////////////////////////////////////////
// Убрал до лучших времён
  return;
//////////////////////////////////////////////////////////////////////////

	// 1. Загрузить образы "Свой"
	// 2. Обучение нейронной сети на образах "Свой"
	// 3. Определяем центр и ширину (W0) области "Свой"
	// 4. Загрузить образы "Чужой" (слово-пароль группы "Свой" и группы "Чужой" должны совпадать)
	// 5. Определить центр и ширину (W1) области "Чужой"
	// 6. Загрузить 1 образ "Чужой" отличный от слова-пароля группы "Свой"
	// 7. Определить ширину (W3) области "Чужой", меру Хемминга при W0 и W3.

// ШАГИ 4 И 5
	// Для Чужого, знающего пароль
	float widthArr[8];	// ширина i-й группы образов
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Захаров_20.dat");
	widthArr[0] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Иванов_20.dat");
	widthArr[1] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Капитуров_20.dat");
	widthArr[2] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Ктото_14.dat");
	widthArr[3] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Майоров_12.dat");
	widthArr[4] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Надеев_19.dat");
	widthArr[5] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Тришин_8.dat");
	widthArr[6] = GetGroupImgWidth(objFileManip.tofileFullPath);
	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\Пенза_Хозин_19.dat");
	widthArr[7] = GetGroupImgWidth(objFileManip.tofileFullPath);
	// Для Чужого
//	objFileManip.tofileFullPath = _T("D:\\Work\\Олег\\Проект_НЕЙРОКРИПТОН\\Манипуляции с рукописными образами\\Определение ширины области\\100 средних слов.dat");
//	widthArr[0] = GetGroupImgWidth(objFileManip.tofileFullPath);
	
//////////////////////////////////////////////////////////////////////////
// ДЛЯ ЧУЖОГО (ШАГИ 6 И 7)
	int imgNum[] = {1,2,8,29,35,51,54,60,70,80};	// номера рассматриваемых образов из файла "100 средних слов.dat"
	float	goalArr[10];	// ширина i-го образа
	float	meraArr1[10];
	float	meraArr2[10];
	// Если в списке есть образы, то тестируем, иначе - нет
	if ( imageCount == 0 ) {
		MessageBox(_T("Для выполнения процедуры в списке\nдолжен содержаться хотя бы один образ."), _T("Список пуст"), MB_ICONASTERISK);
		return;
	}
	// Вычисляем коэффициенты Фурье исходных образов
	FLOAT *coefs;		// одномерный массив функционалов всех тестовых образов
	// Выделение памяти под коэффициенты всех образов
	coefs = new FLOAT[imageCount*NCFOURIER_COUNT];
	BncGetImgParams (images, imageCount, coefs );
	NCFOURIER averArr;
	for ( int i = 0; i < sizeof(imgNum)/sizeof(imgNum[0]); i++ )
	{
		// Определяем центр области
		memcpy(averArr, coefs+imgNum[i]*NCFOURIER_COUNT, sizeof(NCFOURIER));
		float width = faverElem(NCFOURIER_COUNT, averArr);	// ширина области
		float step = width/1000;							// ширина шага
		goalArr[i] = 0;										// найденная ширина области
		NCFOURIER tmpArr;
		// Моделируем обученную нейронную сеть на образе-центр
		NCKEY key1, key2;
		BncCoefsToKey ( averArr, weightsArr, mixKey, key1 );
		int errorCount = 0;
		do 
		{
			goalArr[i] += step; 
			for ( int j = 0; j < NCFOURIER_COUNT; j++ )
				tmpArr[j] = averArr[j] + goalArr[i];
			BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
			errorCount = BncCompareKeys( key1, key2 );
		} while ( errorCount < 1 );

		for ( int j = 0; j < NCFOURIER_COUNT; j++ )
			tmpArr[j] = averArr[j] + goalArr[i];
		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
		meraArr1[i] = (float)BncCompareKeys( si.key, key2 );

//		for ( int j = 0; j < NCFOURIER_COUNT; j++ )
//			tmpArr[j] = averArr[j] + widthArr[номер своего];	// меняем номер своего вручную
//		BncCoefsToKey ( tmpArr, weightsArr, mixKey, key2 );
//		meraArr2[i] = BncCompareKeys( trainKey, key2 );

	}
	float width = faverElem(10, goalArr);	// ширина области
	float mera1 = faverElem(10, meraArr1);	// мера 1
	float mera2 = faverElem(10, meraArr2);	// мера 2
//////////////////////////////////////////////////////////////////////////

	CString str;
	// d - среднее значение дисперсии центрального образа,
	// k - коэффициент масштабирования дисперсии,
	// w - найденная ширина области
//	str.Format(_T("Ширина рассматриваемой области (группы образов) равна:\n d * k = w\n %.3f * %.3f = %.3f"), width, res, goal);
//	MessageBox(str, _T("Полученный результат"), MB_OK);
	MessageBox(_T("Вычисления завершены"), _T("Полученный результат"), MB_OK);
//////////////////////////////////////////////////////////////////////////

}
