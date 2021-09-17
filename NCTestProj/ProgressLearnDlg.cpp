// ProgressLearnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NCTestProj.h"
#include "ProgressLearnDlg.h"
#include "../Tools/FileManipulations.h"

// CProgressDlg dialog

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
}
// Перегруженный конструктор
CProgressDlg::CProgressDlg(int imgsCnt, BNCSIGNIMAGE imgs[], sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghtsArr, CWnd* pParent /* = NULL */)
:CDialog(CProgressDlg::IDD, pParent) {
	m_hIcon   = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mode      = 1;
	imgCount  = imgsCnt;			// количество обучающих образов 
	images    = imgs;					// координаты рукописных образов
	si        = sesInf;				// обучающий ключ
	mixKey    = mxKey;				// перемешивающий ключ
	weights   = wghtsArr;			// вектор весов сети
	stabGroup = 8;
	
}

CProgressDlg::~CProgressDlg() { }

void CProgressDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, OnClose)
	ON_BN_CLICKED(IDC_linear, &CProgressDlg::OnLinear)
END_MESSAGE_MAP()


// CProgressLearnDlg message handlers

BOOL CProgressDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	//Рисование иконки
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control

}

// Рисование формы
void CProgressDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting

	if ( mode == 1 ) {
		ProgressNeuroNetTrain( stabGroup );
		mode++;
	}
}

// Функция, обучающая нейронную сеть и выводящая соответствующее текстовое сообщение
void CProgressDlg::ProgressNeuroNetTrain(int iStability)
{
//////////////////////////////////////////////////////////////////////////
//	DWORD dwTime1 = ::GetTickCount();// время начала обучения
//	int iSecond;					 // врямя обучения в секундах
//////////////////////////////////////////////////////////////////////////
	//CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK8);
	//int ChkBox = m_ctlCheck->GetCheck();

	//if (ChkBox == BST_CHECKED)

	//if (IDC_CHECK8->Checked == true)
	//if (IDC_CHECK8 == BST_CHECKED)
	//if (IDC_CHECK1.Checked == true)

	//CheckRadioButton(IDC_linear, IDC_garm, IDC_garm);


//	if (IDC_linear->Checked == true)

	///CheckRadioButton(IDC_linear, IDC_garm, IDC_garm);
	if (linear_garm == true) 
		MessageBox(L"Checkbox marked", L"", MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(L"Чекбокс снят", L"", MB_OK | MB_ICONINFORMATION);


		{
		CString headString;

		// Вывод сообщения об обучении сети
		headString = _T("Подождите, идет процесс обучения сети...");
		SetWindowText(headString);
		SetDlgItemText(IDC_OUTPUTTEXT, headString);

		stabGroup = iStability;
		// Обучение нейронной сети
		//	BncNeuroNetTrain ( si->key, *mixKey, imgCount, images, *weights, &stabGroup ); 
		objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\");


		BncNeuroNetTrain_2(objFileManip.tofileFullPath, si->key, *mixKey, imgCount, images, *weights, &stabGroup);

		// Сохранение обученной сети
		objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\Sign.bnc");
		// Хеширование обучающего ключа
		NCHASH hash;
		BncCreateHash(si->key, sizeof(NCKEY), hash);
		objFileManip.SaveBNC(objFileManip.tofileFullPath, *weights, hash);

		headString = _T("Результаты обучения нейронной сети.");
		SetWindowText(headString);

		// Определяем вероятность прохождения "Своего"
		float fmyver;
		fmyver = (imgCount * 100) / (float)(imgCount + 1);

		// Определяем вероятность прохождения "Чужого"
		//	Стойкость обученной сети к атакам подбора в зависимости от группы:
		// Стойкость
		//	7 группа:	от 8 до 12
		//	6 группа:	от 12 до 15
		//	5 группа:	от 15 до 19
		//	4 группа:	от 19 до 23
		//	3 группа:	от 23 до 28
		//	2 группа:	от 28 до 33
		//	1 группа:	oт 33 до 35
		CString str;
		switch (stabGroup) {
		case 1: str = "e-33 - e-35."; break;
		case 2: str = "e-28 - e-33."; break;
		case 3: str = "e-23 - e-28."; break;
		case 4: str = "e-19 - e-23."; break;
		case 5: str = "e-15 - e-19.";  break;
		case 6: str = "e-12 - e-15.";   break;
		case 7: str = "e-8 - e-12.";   break;
		}

		// Вывод текстового сообщения
		headString.Format(_T("You are in %d stability group.\n The likelihood of recognizing your %.0f"), stabGroup, fmyver);
		headString += _T("% from 1 try.\n Probability of passing someone else");
		headString += str;

		// Продолжение формирования текстового сообщения
		headString += _T("\n\n If you are satisfied with these characteristics, then you should exit the training mode, if not, then follow one of the following recommendations: \n 1. Add additional training examples and retrain the network; \n 2. You can try to change the password word by deleting all the previous examples.");
		SetDlgItemText(IDC_OUTPUTTEXT, headString);

		// Помечаем CheckBox группы стабильности к которой относится пользователь
		switch (stabGroup) {
		case 1: CheckDlgButton(IDC_CHECK1, BST_CHECKED); break;
		case 2: CheckDlgButton(IDC_CHECK2, BST_CHECKED); break;
		case 3: CheckDlgButton(IDC_CHECK3, BST_CHECKED); break;
		case 4: CheckDlgButton(IDC_CHECK4, BST_CHECKED); break;
		case 5: CheckDlgButton(IDC_CHECK5, BST_CHECKED); break;
		case 6: CheckDlgButton(IDC_CHECK6, BST_CHECKED); break;
		case 7: CheckDlgButton(IDC_CHECK7, BST_CHECKED); break;
		}
	}
	
//////////////////////////////////////////////////////////////////////////
//	iSecond = (::GetTickCount() - dwTime1)/1000;
//////////////////////////////////////////////////////////////////////////

}

// Закрыть
void CProgressDlg::OnClose()
{
	OnOK();
}

void CProgressDlg::OnLinear()
{
	linear_garm = true;
}
