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

	// ��������� ������
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	bStart = true;	// ��������� ������� ������������ ��� ���������� ����
	bStop = false;	

	m_PrBar2.SetStep(1);

	if ( bTestOnTI ) SetWindowText(_T("������������ �� ������� �� ����"));
	else SetWindowText(_T("������������ �� ����� ����"));
	SetDlgItemText( IDC_STATIC01, _T("���������� ����������� ������ :") );
	SetDlgItemText( IDC_STATIC02, _T("���������� ����������� ������� :") );

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
			TestONTI ();		// ����������� �� �������� ������� ���� ����� ����
		else
			TestOnMultiTI();	// ����������� �� ������� ���������� ������� � ����������� ����� ��������
	}
}

// ����� �� ����� ���������� ����������� ������ � �������
void CTestDlg::UpdateCount ( int percent, int fileCount, int imageCount )
{
	CString mes, str;
	str.Format(_T("%d"),percent);
	mes = _T("��������� ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar2.StepIt();

	str.Format(_T("%d"), fileCount);
	SetDlgItemText(IDC_STATIC1, str);
	str.Format(_T("%d"), imageCount);
	SetDlgItemText(IDC_STATIC2, str);
}

// ������������ ������� ��������� ��������
// invPos - [out] � ����� ������� ������������� �������
void GetRandVector (int invPos[])
{
	// ��������� ������� �� ������� ����� ��������� ���������� ������, ���, �����...
	// 1 - ����� ������� invPos[0] �� �������� ������� ����� invPos[0] ������ ���������� �����
	// 2 - testKey[invPos[0]] == trainKey[invPos[0]] �  testKey[invPos[1]] == trainKey[invPos[1]]
	// 3 ... � �.�.
	srand(::GetTickCount());
	for ( int i = 0; i < NEURON_COUNT; i++ )
	{
		int r = rand()%NEURON_COUNT;// ����� ���������������� �������
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
// �������� ������ � ����������� ���������� ��� ������, ���, ����� ������� ���������
// �������� � ��������� ������
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

// �������� ������ pth_xem_list � ������� �������� ������������� �� ����������� �� ����� ��������
// path		- [in] ���� � �������� �������
// count	- [in] ���������� �������� �������
// xemArr	- [in] ������ ��� �������� ��� �������� �������
void CTestDlg::GetSortPXList(CString path, int count, int xemArr[] )
{
	const int maxSize = 32;		// ��������� ������ ���������������� �������
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

// �������� ������ mera_coef_list � ������� �������� ������������� �� ����������� �� ����� ��������
// ���������� ������ bestCoefList
// count	- [in] ���������� �������� ������� (���������� �� ����������)
// coefs	- [in] ������������ ����� ����������� �������
// xemArr	- [in] ������ ��� �������� ��� �������� �������
// maxSize	- [in] ������������ ������ ���������������� ������ 
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

// �������� ���� �������� ������� �������
// count	- [in] ���������� �������� ������� (�.�. �������� ����� ��������� ����)
// coefs	- [in] ������������ ����� ������������� �������
// meraArr	- [out] ������ � ������������ ����� �������� ��������
void CTestDlg::GetSecondOrderMera (int count, float coefs[], int meraArr[])
{
/**/
// ���������� ��� �������� ������� �������
	NCKEY first, second; // ���� �� ������ � ������ ������
	int i = 0;
	for ( int i1 = 0; i1 < count-1; i1++ ) // ����� ������� ������ � ����
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weights, *mixKey, first); 
		for ( int i2 = i1+1; i2 < count; i2++ ) // ����� ������� ������ � ����
		{
			BncCoefsToKey (coefs + i2*NCFOURIER_COUNT, *weights, *mixKey, second); 
			meraArr[i] = BncCompareKeys ( first, second );
			i++;
		}
	}
/**/
/*
// ���������� ��� �������� ������� �������
	NCKEY first, second; // ���� �� ������ � ������ ������
	int i = 0;
	for ( int i1 = 0; i1 < count; i1++ )
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weightsArr, *mixKey, first); 
		meraArr[i1] = BncCompareKeys ( *key, first );
	}
*/
}
// ���������� ���������� ��������������� ��� ��� x ��������
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

// ����������� ��������� ���� �� ������� 
// �� ���� �������� �������� ��� ����� ����
void CTestDlg::TestONTI()
{
	LONG	lErr = ERROR_SUCCESS;		// ��� ������
	DWORD	dwTime1 = ::GetTickCount();	// ����� ������ ������������
	int		fileCount;

	// ������������ ���������� ������������� ������ 
	if ( bTestOnTI ) fileCount = objFileManip.GetFileCount ( folder, type );
	else			 fileCount = 50;//50000;	// �������� ������������� �� �������� �������

//--------------------------------------------------------------------------------------
	m_PrBar2.SetRange(0, fileCount);
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(TRUE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(TRUE);
	DelegateFun();
//--------------------------------------------------------------------------------------	

	// ������ �������� �������
	BNCSIGNIMAGE *images;   // ��������� �� ���������� �������� ���������� �������
	if ( bTestOnTI ) images = new BNCSIGNIMAGE[NCIMAGE_COUNT];

	UINT imageCount;					// ���������� ������� � ������� �����
	int totalCount = 0;			// ����� ���-�� ������ � ��������
	int totalIMGCount = 0;		// ����� ���-�� �������
	POINT netOut01[NEURON_COUNT];	// ���-�� ��������� 0 � 1 �� ���� �������� �������
	ZeroMemory(netOut01, sizeof(netOut01));
	BNCMOMENTS params;			// ��������� ��� ��������

	int invPos[NEURON_COUNT];	// � ����� ������� ������������� �������
	GetRandVector(invPos);
	int CoinCountArr[NEURON_COUNT] = {0}; // ������ �� ���������� ���������� [0] - ��� ������ [1] - ��� ��� ...	
	int trnKey[NEURON_COUNT] = {0};       //�������� ��������� ����
	BncConvertKey ( si->key, trnKey );

//////////////////////////////////////////////////////////////////////////
	int maxMera = 0;		// ������������ �������� ���������� � ������ ���� ��������
//////////////////////////////////////////////////////////////////////////

	if ( bTestOnTI )
	{
		// ��������� ���������� �������� �������
		WIN32_FIND_DATA FindFileData;
		wchar_t cPath[MAX_PATH];
		wcscpy_s ( cPath, folder );
		wcscat_s ( cPath, _T("*."));
		wcscat_s ( cPath, type );

		// ���������� ��� ����� � ������������
		HANDLE hFind = FindFirstFile( cPath, &FindFileData );
		if ( hFind != INVALID_HANDLE_VALUE ) 
		{	
			int iFileNum = 0;	// ���������� ��� ����������� ������
			// ���������������� ������������ �� ������� �� ���� ������
			do 
			{
				// ��������� �������� ������
				imageCount = 0;			
				CString str		= FindFileData.cFileName;
				CString toCoord = folder + str;
				lErr = objFileManip.LoadAllImages(toCoord, imageCount, images);
				if ( lErr != ERROR_SUCCESS ) continue;
				totalIMGCount += imageCount;
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// ��������� ������� ��� �������� �� �������� �������
				params = BncGetXemMoments ( TRUE, si->key, *mixKey, images, imageCount, *weights );
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// ��������� ���������� ���� ��������
				TCHAR *sMode;
				if ( totalCount == 0 ) sMode = _T("w");
				else				   sMode = _T("a");
				objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
				objFileManip.SaveXem(objFileManip.tofileFullPath, params.X, imageCount, sMode);
				// ���������� ��������� ������� � ��������
				for ( int i = 0; i < NEURON_COUNT; i++ ) { netOut01[i].x += params.p01[i].x; netOut01[i].y += params.p01[i].y; }
				// ���������� ����������������� ���������� ����� ���������� ����������� ����� � ���������
				//GetKeyCoincidence ( imageCount, params.tmpKey, trnKey, invPos, CoinCountArr );
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------
				// ����� ������ �������
				// ��������� ���������� ������� � ������������ ������ ��������
				// ������� ������: ������� � �������� ������������ 32 ������,
				// ����������� ������� �� ������ ������ (�.�. ���� �������� ���
				// ���� 32 ������� ����� ����������� �� ��������� � ������ ��� ����������
				// �������)
				// ����� ��������� ���������� ��������� ������� � ���� (������ ����� ����
				// ��� ������������ ��� �� ������ �������� ������) ��� ���������� ���������
				GetSortPXList(toCoord, imageCount, params.X);
//--------------------------------------------------------------------------------------
				DelegateFun();
//--------------------------------------------------------------------------------------

				// ������� ���������� ���� ���������� �������
				objFileManip.DelAllImages(imageCount, images); 
				totalCount++;
				iFileNum++;
				// ����� �� �����
				UpdateCount(iFileNum*100/fileCount, totalCount, totalIMGCount);
			} while ( FindNextFile(hFind, &FindFileData) != 0 && !bStop );
			FindClose(hFind);
		}
	}		// ������������ �� �������� �������
	else	// ������������ �� ����� ����
	{
		int iFileNum = 0;	// ���������� ��� ����������� ������
		while ( iFileNum < fileCount && !bStop )
		{
			imageCount = 20;			
			totalIMGCount += imageCount;
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// ��������� ������� ��� �������� �� �������� �������
			params = BncGetXemMoments ( FALSE, si->key, *mixKey, images, imageCount, *weights );
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// ��������� ���������� ���� ��������
			TCHAR *sMode;
			if ( totalCount == 0 ) sMode = _T("w");
			else				   sMode = _T("a");
      objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
			objFileManip.SaveXem(objFileManip.tofileFullPath, params.X, imageCount, sMode);
			totalCount++;
			iFileNum++;
			// ���������� ��������� ������� � ��������
			for ( int i = 0; i < NEURON_COUNT; i++ ) { netOut01[i].x += params.p01[i].x; netOut01[i].y += params.p01[i].y; }
			// ���������� ����������������� ���������� ����� ���������� ����������� ����� � ���������
			//GetKeyCoincidence ( imageCount, params.tmpKey, trnKey, invPos, CoinCountArr );
//--------------------------------------------------------------------------------------
			DelegateFun();
//--------------------------------------------------------------------------------------
			// ����� �� �����
			UpdateCount(iFileNum*100/fileCount, totalCount, totalIMGCount);
		}
	}

	// ��������� ���������� �������� ��� ��������� 0 � 1
/*	for ( int i = 0; i < NEURON_COUNT; i++ )
	{
		netOut01[i].x = roundUp((float)netOut01[i].x*100/totalIMGCount);
		netOut01[i].y = roundUp((float)netOut01[i].y*100/totalIMGCount);
	}
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\NetOut_0_1.txt");
	objFileManip.SaveNetOut(objFileManip.tofileFullPath, netOut01, _T("w"));
*/
	// ��������� ���������� �������� �������� ��� ���������� 1,2,3...
  //objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\coincidence.txt");
	//objFileManip.SaveXem(objFileManip.tofileFullPath, CoinCountArr, NEURON_COUNT, _T("w"));

//////////////////////////////////////////////////////////////////////////
	// ������������ ��� ����������� �������� ������ �������
	// ������ � ������� �������������� ����������� ����� �����������
	// ���������� ������� � ����������� ����� � �������� �������������
	// ����� ����������� ������� �� ����� ������ (���� = 0)
	// ��������� ������ ������ � ��������� ����
	pth_xem_list tempListElem;
	BNCSIGNIMAGE *bestImages;   // ��������� �� ���������� � ����������� ����� ��������
	bestImages = new BNCSIGNIMAGE[bestImgsList.GetCount()];
	// ��������� ������ �� ������
	POSITION pos = bestImgsList.GetHeadPosition();
	UINT imCount = 0;//���������� �������
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
	// ��������� ���������� ������ � ��������� ���� (������ � ����������� ����� ��������)
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\minXemImg.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	objFileManip.SaveAllImages(szPath, imCount, bestImages);
	objFileManip.DelAllImages(imCount, bestImages); 
	delete [] bestImages;

//////////////////////////////////////////////////////////////////////////

// ������������ ���������� ������������

	// ��������� ����������� ���� ��������
	int *xemArr = new int [totalIMGCount];
  objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\mera.txt");
	objFileManip.LoadXem(objFileManip.tofileFullPath, xemArr, totalIMGCount);
	// �������� ������� ��� ��������
	fadq ( totalIMGCount, xemArr, params ); 
	// ��������� ����������� � ������������ ����
	int imin = fminElem ( totalIMGCount, xemArr );
	int imax = fmaxElem ( totalIMGCount, xemArr );
	// ��������� ���������� "�������" �������, �.�. ���-�� ��� == 0
	int zeroCount = fzeroXem ( totalIMGCount, xemArr );
	// ��������� ��������� ������� � ������ �������
	float fstab = Qual2Stab(params.fqual);

	// ������� �������, �������� �� ������������ �������
	DWORD dwTime2 = (::GetTickCount() - dwTime1)/1000;
	int iDay  = dwTime2/(60*60*24);
	int iHour = dwTime2/(60*60)%24;
	int iMinute = dwTime2/60%60;
	int iSecond = dwTime2%60;
	CString strTime;
	strTime.Format(_T("������������ ��������� ��: %02d:%02d:%02d:%02d."), iDay, iHour, iMinute, iSecond);

	// ����� ���������� ����������� �� �����
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GB)->ShowWindow(TRUE);
	CString str;
	str.Format(_T(" ����������� : %.3f\n ��������� : %.3f\n �������� : %.3f\n ����������� �������� : %d\n ������������ �������� : %d\n ���������� ������� � ����� ������ 0 : %d\n ��������� � ������ ������� ����� : 10^%.3f"), params.faver, params.fdisp, params.fqual, imin, imax, zeroCount, fstab);
	SetDlgItemText(IDC_FINAL_STATIC, str);
	SetWindowText(strTime);

	GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
	GetDlgItem(IDC_MCD_BTN)->ShowWindow(TRUE);

	// �������� 
	if(bTestOnTI) delete [] images; 
	delete [] xemArr;
}

// ���������� �������� ������������ ����� � ���������
// �� ��������������� ������� (�������������), �����
// �������� �������� ������� � "������" ������
// coef - [in/out] �������� ������������ ����� (�� ����� ������ ������� ���������������� ������ �������������)
// count - [in] ���������� �������� �������
BNCMOMENTS CTestDlg::MorfingAndSorting ( float coefs[], UINT count )
{
	// "�����������" ������� (����������� count �������� ����������
	// ������� � pairCount*(interpCount) �������)
	// ��������, ��� 32 �������� ������� pairCount = 496 (���������� ��������� ���)
	// interpCount = 11 ���������� 5456 �������
	UINT i1, i2;			// ������ ��������������� �������
	int pairCount = 0;		// ���������� ��� �������
	int interpCount;		// ���������� ������������ ������� ����� i1 � i2 (�������� 
	// � ������� �������� ������������ ��������� ���� ���������������
	// �������), �������� ������ ���� ��������, �.�. ���������� 
	// interpCount-2 ����� ������

//////////////////////////////////////////////////////////////////////////
	// ��������� ������
	int childCount = 0;
	struct child_coef 
	{
		NCFOURIER	coef;	// ������������ �����
	};
	CList<child_coef, child_coef&> childCoefList; // ������ ������������� ����� ���� ��������
	child_coef oneChild;	// ������������ ������ �������
//////////////////////////////////////////////////////////////////////////
	
	BNCMOMENTS params;		// ��������� ��� ��������

//////////////////////////////////////////////////////////////////////////
	// ���������� �������� ������� �� ������������ �����������
	// ����������� ����������� ������� �� ���������� ����� ����������
	// (���� �������� ������� �������)
	// 
	NCKEY first, second;	// ���� �� ������ � ������ ������
	int mera;				// ���� �������� ������� �������
	for ( i1 = 0; i1 < count-1; i1++ ) // ����� ������� ������ � ����
	{
		BncCoefsToKey (coefs + i1*NCFOURIER_COUNT, *weights, *mixKey, first); 
		for ( i2 = i1+1; i2 < count; i2++ ) // ����� ������� ������ � ����
		{
			BncCoefsToKey (coefs + i2*NCFOURIER_COUNT, *weights, *mixKey, second); 
			mera = BncCompareKeys ( first, second );
//			mera = BncCompareKeys ( *key, second );
//			mera = 45;	// ������������� ����������� �����������: 10 �������� + 2 ��������
			interpCount = mera/5 + 1 + 1;	// ���������� �������� ������ 5 
			// ���� 0 - 4 �������� 1 + 2 ��������
			// ���� 5 - 9 �������� 2 + 2 �������� 
			// ���� 10 - 14 �������� 3 + 2 �������� � �.�.
			float *tmpCoefs = new float[mera*NCFOURIER_COUNT];	// ������������ ������ ��������������� �������
			// ���������� ����� � ����� ������
//			for ( int i = 0; i < interpCount+1; i++ )
			for ( int i = 1; i < interpCount; i++ )// ��� ������� ��� ������ � ��� ������ ��������
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
			// �������� ���� �������� ��� ��������������� �������
			params = BncCoefsToXemMoments (si->key, *mixKey, tmpCoefs, interpCount, *weights );

			// ��������� ������������ � ������ ��������� �������
			GetSortMCList ( interpCount, tmpCoefs, params.X, count );

			pairCount++;
			delete [] tmpCoefs;
		}
	}
//////////////////////////////////////////////////////////////////////////
	// ������������� ����������� ����������� (���������� ��� ���� ��� ���������
	// � �� ������� �� ���������� ����� ����������)
/*	float tmpCoefs[interpCount*NCFOURIER_COUNT];	// ������������ ������ ��������������� �������
	for ( i1 = 0; i1 < count-1; i1++ ) // ����� ������� ������ � ����
	{
		for ( i2 = i1+1; i2 < count; i2++ ) // ����� ������� ������ � ����
		{
			// ���������� ����� � ����� ������
			for ( int i = 0; i < interpCount; i++ )
			{
				for ( int j = 0; j < NCFOURIER_COUNT; j++ )
				{
					tmpCoefs[i*NCFOURIER_COUNT+j] = 
						((interpCount-i)/(float)interpCount) * coefs[i1*NCFOURIER_COUNT+j] + 
						(i/(float)interpCount) * coefs[i2*NCFOURIER_COUNT+j];
				}
			}
			// �������� ���� �������� ��� ��������������� �������
			params = BncCoefsToXemMoments (*key, *mixKey, tmpCoefs, interpCount, *weightsArr );

			// ��������� ������������ � ������ ��������� �������
			GetSortMCList ( interpCount, tmpCoefs, params.X, count );

			pairCount++;

//////////////////////////////////////////////////////////////////////////
//��������� ������
// �������� ��������� ������������ �����
			//memcpy(allChildCoef+pdx*interpCount*NCFOURIER_COUNT, tmpCoefs, interpCount*NCFOURIER_COUNT*sizeof(float));
			//pdx++;
//////////////////////////////////////////////////////////////////////////
		}
	}
*/
//////////////////////////////////////////////////////////////////////////
//��������� ������
	// ���������� ���������� ��� �������� ����� ���������� ��������
	childCount = (int)childCoefList.GetCount();
	int paraCount = GetPara(childCount);
	int *meraArr = new int[paraCount];
	float *allChildCoef = new float[childCount*NCFOURIER_COUNT];
	// ������� ������ �� ������ ������������� ����� �������� � ���� �������� ������
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

	// ��������� ��� ��� ������� ��� �������� �� ��������� �������������
	// ��������� ������ �� ������ ���������� ��������� �������
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

// ������������ ���� � ������� ������ ������� ������ � ����������� ����� �������� 
// � ���������� ����� ���������� �������
// ���������� ���������� �������� ������� �� ���� �������� ����������
// ������������� �� 32 ������� �� ����� minXemImg.dat �������� ������ ����������
// �������� �������, ��������� ��������� �������� �������
void CTestDlg::TestOnMultiTI()
{
	// ��������� �������� ������ 
	BNCSIGNIMAGE *images;   // ��������� �� ���������� �������� ���������� �������
	images = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	UINT imageCount = 0;	// ���������� ���������� �������
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\minXemImg.dat");
	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, objFileManip.tofileFullPath);
	long lErr =	objFileManip.LoadAllImages(szPath, imageCount, images);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(imageCount, images); 
		delete [] images;
    MessageBox(_T("�� ������ ���� ������� � ����������� ����� (minXemImg.dat).\n������� ��������� ��������� ������������."), _T("������-�������� �� �������"), MB_ICONWARNING);
    GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
    GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
    return;
	}
	
	// ��������� ������������ ����� �������� �������
	FLOAT *coefs;		// ���������� ������ ������������ ���� �������� �������
	// ��������� ������ ��� ������������ ���� �������
	coefs = new FLOAT[imageCount*NCFOURIER_COUNT];
	BncGetImgParams (images, imageCount, coefs );

	// ����������� ���������� �������� �������
	BNCMOMENTS params;								// ��������� ��� ��������	
	params = BncCoefsToXemMoments ( si->key, *mixKey, coefs, imageCount, *weights );

//////////////////////////////////////////////////////////////////////////
	// ���������� ����� ������������� ��������
//imageCount = 8;	// ���������� �������� ���������� �������-���������	
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
		bestCoefList.RemoveAll();// ������� ������ � ������� �������������
		params = MorfingAndSorting ( coefs, imageCount ); // ����������� ����� ������������ � ��������� � ������
	}
	
	delete [] coefs;
	
	objFileManip.DelAllImages(imageCount, images); 
	delete [] images;

	// ����� ���������� ����������� �� �����
	GetDlgItem(IDC_PROGRESS_BAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GB)->ShowWindow(TRUE);
	CString str = _T("");
//	str.Format(_T(" ����������� : %.3f\n ��������� : %.3f\n �������� : %.3f\n ����������� �������� : %d\n ������������ �������� : %d\n ���������� ������� � ����� ������ 0 : %d\n ��������� � ������ ������� ����� : 10^%.3f"), params.faver, params.fdisp, params.fqual, imin, imax, zeroCount, fstab);
	SetDlgItemText(IDC_FINAL_STATIC, str);
//	SetWindowText(strTime);

	GetDlgItem(IDC_CLOSE_BTN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(FALSE);
	GetDlgItem(IDC_MCD_BTN)->ShowWindow(TRUE);

}

// �������
void CTestDlg::OnBnClickedCloseBtn()
{
	CDialog::OnOK();
}

// ���������� ������������
void CTestDlg::OnBnClickedStopBtn()
{
	bStop = true;
	GetDlgItem(IDC_STOP_BTN)->EnableWindow(FALSE);
}

// ��������� ������������ ����, ������������ 
// ���������� ������������� ��� ��������
void CTestDlg::OnMcdBtn()
{
	// ����� ���� � ���������� ������������	
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\Graf.xmcd");

	// ��������� ���������� ������������
	DWORD dwStatus = 
		(DWORD)ShellExecute ( NULL, NULL, 
		objFileManip.tofileFullPath, 
		NULL, NULL, SW_SHOWNORMAL );
	// ���������� ������� ������
	if (dwStatus < 32) 
	{
		CString sMessage;
		switch(dwStatus)
		{
		case ERROR_FILE_NOT_FOUND:       sMessage = " ���� �� ������ "; break;
		case ERROR_PATH_NOT_FOUND:       sMessage = " ���� �� ������ "; break;
		default:                         sMessage = " ���������� ������� ������ "; 
		}
		MessageBox (sMessage, _T("������ ��� ����������"), MB_ICONWARNING);
	}
}
