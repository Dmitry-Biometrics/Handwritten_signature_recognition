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
	si = sesInf;					  // ��������� ����
	mixKey = mxKey;					// �������������� ����
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

	// ��������� ������
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// ������������� �������� ���� ����
	m_PrBar1.SetStep(1);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SHOWRES_BTN)->EnableWindow(FALSE);
	CString mes = _T("������ �������. ��������� 0%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);

	resFilePath = _T("");
	bStop = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//	-	-	-	-	-	-	��������������� �������	-	-	-	-	-	-	//

// ������� % ������������� � ����
void CBodyBurdenDlg::SetTxtAndSetMes ( int iproc )
{
	CString mes, str;
	str.Format(_T("%d"),iproc);
	mes = _T("������ �������. ��������� ") + str;
	mes += _T("%");
	SetDlgItemText(IDC_EXEC_STATIC, mes);
	m_PrBar1.StepIt();
	DelegateFun();
}

// ��������� ������������� ���� (����� � �������������� �������)
// breg - ���������� �� ��� ������� � �� ��� �����������
bool CBodyBurdenDlg::OpenAnalizFile ( CString &csBefore, CString &csAfter, bool &breg )
{
	TCHAR szFilters[] = _T("Image Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg (TRUE, NULL, _T("1.dat"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , szFilters, this);

	CString csPath, csFolder;
	csBefore	= _T("�� ��������");
	csAfter		= _T("����� ��������");
	int len;
	// ������� ���� � ������ � ������� �� � ����� ��������
	if ( fileOpenDlg.DoModal() != IDOK ) 
		return FALSE;

	csPath = fileOpenDlg.GetPathName();
	len	 = csPath.Find(csBefore);
	breg = TRUE;
	if ( len == -1 ) {
		len = csPath.Find(csAfter);
		breg = FALSE;
		if ( len == -1 ) {
			MessageBox(_T("������� �������� �����!"));
			return FALSE; 
		}
	}
	csFolder = csPath.Left(len);
	csBefore = csFolder + csBefore + _T("\\");	// ����� � ������� �� ��������
	csAfter  = csFolder + csAfter + _T("\\");	// ����� � ������� ����� ��������

	return TRUE;
}

//	-	-	-	-	-	����� ��������������� �������	-	-	-	-	-	//

// ������ ��������� ��������/������������ �� ������� �� � ����� ��������
void CBodyBurdenDlg::OnBnClickedStartBtn()
{
	// ����� �������������� �����
	CString csBefore, csAfter;
	if ( OpenAnalizFile ( csBefore, csAfter, bregDO ) != TRUE ) return;

	bStop = false;

	CString trainDir = bregDO ? csBefore : csAfter; // ���� � ����� � ���������� �������
	CString checkDir = bregDO ? csAfter : csBefore; // ���� � ����� � ������������ �������

//--------------------------------------------------------------------------------------
	GetDlgItem(IDC_EXEC_STATIC)->ShowWindow(SW_SHOW);
	CString mes = _T("������ �������. ��������� 0%");
	GetDlgItemText(IDC_EXEC_STATIC, mes);
//--------------------------------------------------------------------------------------

	// ������������ ���������� ������������� ������ 
	int fileCount = objFileManip.GetFileCount ( trainDir, _T("dat") );

//--------------------------------------------------------------------------------------
	m_PrBar1.SetRange(0, fileCount);
//--------------------------------------------------------------------------------------	
	long lErr;

	// ����� ������ ���������� ������� �� � ����� ��������
	trainImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];
	checkImages = new BNCSIGNIMAGE[NCIMAGE_COUNT];
	testImages	= new BNCSIGNIMAGE[NCIMAGE_COUNT];

	// ��������� �������� ������
	UINT testImgCount = 0;
	objFileManip.tofileFullPath = objFileManip.fullPath + _T("Data\\5bykv.dat");
	lErr = objFileManip.LoadAllImages(objFileManip.tofileFullPath, testImgCount, testImages);
	if ( lErr != ERROR_SUCCESS ) return;

	// ���� ����������� ����������
	CString str;
	int icor;	// �������������� �����������. ��������� �������� ���� +1, ���� -1.
				// �� - +1, ����, ����� �� 1.dat ������������� ���� ����� 2.dat 
				//		 1.dat + 1 = 2.dat 
				// ����� - -1, ����, ����� ����� 2.dat ������������� ���� �� 1.dat
				//		 2.dat - 1 = 1.dat 
	if ( bregDO ) {
		str = _T("\t������� ���� �� ������� �� ��������.\r\n");
		resFilePath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\TestRes_1.txt");
		icor = 1;
	} else {
		str = _T("\t������� ���� �� ������� ����� ��������.\r\n");
    resFilePath = objFileManip.fullPath + _T("Data\\") + si->userName + _T("\\TestRes_2.txt");
		icor = -1;
	}
	FILE *stream;
	_tfopen_s(&stream, resFilePath, _T("wb"));
	fwrite( str, _tcslen(str)*sizeof(TCHAR), 1, stream );
	fclose(stream);

	UINT trainImgCount = 0;			// ������� ���-�� ������� �� ��������
	UINT checkImgCount = 0;			// ������� ���-�� ������� ����� ��������

	GetDlgItem(IDC_START_BTN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STOP_BTN)->ShowWindow(SW_SHOW);

	wchar_t cPath[MAX_PATH];
	wcscpy_s ( cPath, trainDir );
	wcsncat_s ( cPath, _T("*.dat"), 5 );

	CString trainFile, checkFile, trainFileName, checkFileName;

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
		trainImgCount = 0;
		trainFileName = FindFileData.cFileName;
		trainFile = trainDir + trainFileName;
		lErr = objFileManip.LoadAllImages(trainFile, trainImgCount, trainImages);
		if ( lErr != ERROR_SUCCESS ) continue;
//--------------------------------------------------------------------------------------
		DelegateFun();	
//--------------------------------------------------------------------------------------
		// ��������� �������� ������
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
		// �������� ��������� ����
		NCWEIGHTS	weightsArr;			// ������ ����� ����
		UINT		stabGroup;			// ����� ������ ������������ � ������� ��������� ������������
		BncNeuroNetTrain ( si->key, *mixKey, trainImgCount, trainImages, weightsArr, &stabGroup ); 
//		MessageBox(_T("������� �������� ����������������!!!"), _T("�������� �� ��� ��� ����!!!"), MB_ICONASTERISK);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// ������������ �� ����� ����
		BNCMOMENTS paramsWN;	// ��������� ��� ��������
		paramsWN = BncGetXemMoments ( FALSE, si->key, *mixKey, NULL, 1000, weightsArr );
		// ��������� ��������� ������� � ������ �������
		float fstabWN = 0.87f+0.221f*paramsWN.fqual*paramsWN.fqual;
		CString csWN;
		csWN.Format(_T("\t����������� : %.3f\r\n\t��������� : %.3f\r\n\t�������� : %.3f\r\n\t��������� � ������ ������� ����� : 10^%.3f\r\n"), paramsWN.faver, paramsWN.fdisp, paramsWN.fqual, fstabWN);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// ������������ �� �������� �������
		BNCMOMENTS paramsTI;	// ��������� ��� ��������
		paramsTI = BncGetXemMoments ( TRUE, si->key, *mixKey, testImages, testImgCount, weightsArr );
		// ��������� ��������� ������� � ������ �������
		float fstabTI = 0.87f+0.221f*paramsTI.fqual*paramsTI.fqual;
		// ������������ ���������� ������������
		// ��������� ����������� � ������������ ����
		int imin = fminElem ( testImgCount, paramsTI.X );
		int imax = fmaxElem ( testImgCount, paramsTI.X );
		// ��������� ���������� "�������" �������, �.�. ���-�� ��� == 0
		int zeroCount = fzeroXem ( testImgCount, paramsTI.X );
		CString csTI;
		csTI.Format(_T("\t����������� : %.3f\r\n\t��������� : %.3f\r\n\t�������� : %.3f\r\n\t����������� �������� : %d\r\n\t������������ �������� : %d\r\n\t���������� ������� � ����� ������ 0 : %d\r\n\t��������� � ������ ������� ����� : 10^%.3f\r\n"), paramsTI.faver, paramsTI.fdisp, paramsTI.fqual, imin, imax, zeroCount, fstabTI);
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// �������� �� ����������������� �������, �.�. ���� ������� �� ������� �� ��������, 
		// �� ��������� �� ������� ����� ��������, � ��������
		// ��������� ������� ����������� �������
		int count = 0;	// ���������� ������� ������
		// ��������������� ��������� ���� �������� �� ���� ������� �� ������
		for ( UINT i = 0; i < checkImgCount; i++ )
		{
			BncExtractKey ( checkImages[i], weightsArr, *mixKey, checkKey );
			int mera = BncCompareKeys ( si->key, checkKey );
			if ( mera == 0 ) count++;
		}
		// ��������� � ��������
		float ferr = (float)count/checkImgCount*100;
		CString csAnt;
		csAnt.Format(_T("\t%.1f"), ferr );
//--------------------------------------------------------------------------------------
		DelegateFun();
//--------------------------------------------------------------------------------------
		// ������ ����������� � ����
		size_t wrCount = 0;			// ���������� ������� ���������� ������
		_tfopen_s(&stream, resFilePath, _T("ab"));
		CString csRes;
		csRes = _T("-\t--\t--\t--\t--\t--\t--\t--\t--\t--\t-\r\n");
		csRes += _T("������������� ����:\t");
		csRes += trainFileName;
		csRes += _T("\r\n\r\n���������� ������������ �� ����� ����:\r\n");
		csRes += csWN;
		csRes += _T("\r\n���������� ������������ �� �������� �������:\r\n");
		csRes += csTI;
		csRes += _T("\r\n������������� �� �������-������������. ����:\t");
		csRes += checkFileName;
		csRes += _T("\r\n");
		csRes += csAnt;
		csRes += _T(" % ������� ���������������� ��� \"����\"\r\n");
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

	MessageBox( resFilePath, _T("���������� �������� � ����"), MB_ICONASTERISK);

	// �������� ���� ���������� �������
	delete [] trainImages; 
	delete [] checkImages; 
	delete [] testImages;

}

// ���������� ������� ����������
void CBodyBurdenDlg::OnStopBtn()
{
	bStop = true;
}
void CBodyBurdenDlg::OnClose()
{
	if ( !bStop ) return;
	
	CDialog::OnClose();
}

// ������� ���� ���������� �����������
void CBodyBurdenDlg::OnShowResBtn()
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
