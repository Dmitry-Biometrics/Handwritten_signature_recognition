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
	key		= lnKey;					// ��������� ����
	mixKey	= mxKey;					// �������������� ����
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

	// ��������� ������
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// ������������� �������� ���� ����
	m_PrBar1.SetStep(1);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SHOWRES_BTN)->EnableWindow(FALSE);
	CString mes = _T("������ �������. ��������� 0%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	SetWindowText(_T("��������� ����� ���������"));

	resFilePath = _T("");
	bStop = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//	-	-	-	-	-	-	��������������� �������	-	-	-	-	-	-	//

// ������� % ������������� � ����
void CStabilityDlg::SetTxtAndSetMes ( int iproc )
{
	CString mes, str;
	str.Format(_T("%d"),iproc);
	mes = _T("������ �������. ��������� ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar1.StepIt();
	DelegateFun();
}
//	-	-	-	-	-	����� ��������������� �������	-	-	-	-	-	//

// ������ ��������� ��������/������������ �� ������� �� � ����� ��������
void CStabilityDlg::OnBnClickedStartBtn()
{
/**/
	bStop = false;

	// ������������ ���������� ������������� ������ 
	int fileCount = objFileManip.GetFileCount ( folder, _T("dat") );

	// �������������� ������� �������� �� ����� ���� � �� �������� �������
	float *qualWN = new float [fileCount];
	float *qualTI = new float [fileCount];

//--------------------------------------------------------------------------------------
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(SW_SHOW);
	CString mes = _T("������ �������. ��������� 0%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
	m_PrBar1.SetRange(0, fileCount);
//--------------------------------------------------------------------------------------	
	long lErr;

	// ����� ������� ��������� � �������� ���������� �������
	trnImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];
	tstImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];

	// ��������� ��� ����� ������ (���� �� 210 ������� � ��������)
	UINT tstCount = 0;	// ���������� ����� �������
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\210TI.bim");
	lErr = objFileManip.LoadAllImages(objFileManip.tofileFullPath, tstCount, tstImages);
	if ( lErr != ERROR_SUCCESS ) { 
		objFileManip.DelAllImages(tstCount, tstImages); 
		delete [] tstImages; 
		return;
	}

	// ���� ����������� ����������
	CString str;
	str = _T("\t���������� ������ ��������� �������.\r\n");
	str += _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
	str += _T("1. ���������� ������������ �� ����� ���� (�����������, ���������, ��������, ��������� � ������ ������� 10^):\r\n");
	str += _T("2. ���������� ������������ �� �������� ������� (�����������, ���������, ��������, ����������� ��������, ������������ ��������, ���������� ������� � ����� ������ 0, ���������):\r\n");
	str += _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
	resFilePath = folder + _T("StabRes.txt");

	FILE *stream;
	_tfopen_s(&stream, resFilePath, _T("wb"));
	fwrite( str, _tcslen(str)*sizeof(TCHAR), 1, stream );
	fclose(stream);

	UINT trnCount = 0;			// ������� ���-�� ��������� �������

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_SHOW);

	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, folder );
	wcsncat_s ( cPath, _T("*.dat"), 5 );

	CString trainFile, checkFile, trnFileName, checkFileName;

	// ���������������� ������ ���� ������
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile( cPath, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE ) 
	{
	for ( int iFileNum = 0; iFileNum < fileCount; iFileNum++ )
	{	
		// ������������� ����������
		if ( bStop ) break;

//--------------------------------------------------------------------------------------
		SetTxtAndSetMes((iFileNum+1)*100/fileCount);
//--------------------------------------------------------------------------------------
		// ��������� ��������� ������
		trnCount = 0;
		trnFileName = FindFileData.cFileName;
		trainFile = folder + trnFileName;
		lErr = objFileManip.LoadAllImages(trainFile, trnCount, trnImages);
		FindNextFile(hFind, &FindFileData);
		if ( lErr != ERROR_SUCCESS ) continue;
//--------------------------------------------------------------------------------------
		DelegateFun();	
//--------------------------------------------------------------------------------------
		
		// �������� ��������� ����
		NCWEIGHTS	weightsArr;			// ������ ����� ����
		UINT		stabGroup;			// ����� ������ ������������ � ������� ��������� ������������
		BncNeuroNetTrain ( *key, *mixKey, trnCount, trnImages, weightsArr, &stabGroup ); 

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------

		// ������������ �� ����� ����
		BNCMOMENTS paramsWN;	// ��������� ��� ��������
		paramsWN = BncGetXemMoments ( FALSE, *key, *mixKey, NULL, 1000, weightsArr );
		// ��������� ��������� ������� � ������ �������
		float fstabWN = 0.87f+0.221f*paramsWN.fqual*paramsWN.fqual;
		qualWN[iFileNum] = paramsWN.fqual;
		CString csWN;
		csWN.Format(_T("%.3f\t%.3f\t%.3f\t%.3f\t"), paramsWN.faver, paramsWN.fdisp, paramsWN.fqual, fstabWN);

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		
		// ������������ �� �������� �������
		BNCMOMENTS paramsTI;	// ��������� ��� ��������
		paramsTI = BncGetXemMoments ( TRUE, *key, *mixKey, tstImages, tstCount, weightsArr );
		// ��������� ��������� ������� � ������ �������
		float fstabTI = 0.87f+0.221f*paramsTI.fqual*paramsTI.fqual;
		qualTI[iFileNum] = paramsTI.fqual;
		// ������������ ���������� ������������
		// ��������� ����������� � ������������ ����
		int imin = fminElem ( tstCount, paramsTI.X );
		int imax = fmaxElem ( tstCount, paramsTI.X );
		// ��������� ���������� "�������" �������, �.�. ���-�� ��� == 0
		int zeroCount = fzeroXem ( tstCount, paramsTI.X );
		CString csTI;
		csTI.Format(_T("%.3f\t%.3f\t%.3f\t%d\t%d\t%d\t%.3f\r\n"), paramsTI.faver, paramsTI.fdisp, paramsTI.fqual, imin, imax, zeroCount, fstabTI);

//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------

		// ������ ����������� � ����
		size_t wrCount = 0;			// ���������� ������� ���������� ������
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
	// ������������ ���������� ����������
	//1. ��������� ������� ��������
	//���������� ������� �������� (� ����������� �� ����������� ��������)		
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

	MessageBox( resFilePath, _T("���������� �������� � ����"), MB_ICONASTERISK);

	// �������� ���� ���������� �������
	delete [] trnImages; 
	delete [] tstImages;
	delete [] qualWN;
	delete [] qualTI;

/**/
}

// ���������� ������� ����������
void CStabilityDlg::OnStopBtn()
{
	bStop = true;
}
void CStabilityDlg::OnClose()
{
	if ( !bStop ) return;
	
	CDialog::OnClose();
}

// ������� ���� ���������� �����������
void CStabilityDlg::OnShowResBtn()
{
	if ( resFilePath == _T("")) return;
		
	DWORD dwStatus = 
		(DWORD)ShellExecute ( NULL, NULL, 
		resFilePath, 
		NULL, NULL, SW_SHOWNORMAL );
	// ���������� ������� ������
	if (dwStatus < 32) 
	{
		CString sMessage;
		switch(dwStatus) {
			case ERROR_FILE_NOT_FOUND:       sMessage = " ���� �� ������ "; break;
			case ERROR_PATH_NOT_FOUND:       sMessage = " ���� �� ������ "; break;
			default:                         sMessage = " ���������� ������� ���� "; 
		}
		MessageBox (sMessage, _T("������ ��� ����������"), MB_ICONWARNING);
	}
}
