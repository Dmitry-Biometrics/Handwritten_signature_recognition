// FileManipulations.h : header file
// ������������ ������ � �������

#pragma once
#include "../NCTestProj/bnc32.h"

class CFileManip
{
public:
	CFileManip(void);
	~CFileManip(void);
	
	// �������� ��������� ���������� ������
	// imageSel  - ����� ���������� ������
	// imageLast - ����� ���������� ������
	// images	- ���������� ���������� �������
	void DelImageCoord ( int imageSel, int imageLast, BNCSIGNIMAGE images[] );

	// ������� ��� ������ (�������� �����, ���������� ���������� ��������� ��������)
	// imageCount - ����� ���������� ���������� �������
	// images	  - ���������� ���������� �������
	void DelAllImages ( UINT &imageCount, BNCSIGNIMAGE images[] );

	//����������: ���������/������� ����������-������������ ���������
	//��������:   ���������� � ��������� ���� ���� ��������� ���������
	//			  ���� � ��� ���������� �����
	//���������: path    - [in] ���� � �����
	//			 weights - [in] ������ ����� ��������� ����
	//			 hash    - [in] ��� ���������� �����
	// ���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG SaveBNC ( CString path, NCWEIGHTS weights, NCHASH hash );

	//����������: ���������/������� ����������-������������ ���������
	//��������:   ��������� �� ����� ���� ��������� ���������
	//			  ���� � ��� ���������� �����
	//���������: path    - [in] ���� � �����
	//			 weights - [in] ������ ����� ��������� ����
	//			 hash    - [in] ��� ���������� �����
	//���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG LoadBNC ( CString path, NCWEIGHTS weights, NCHASH hash );

	//����������: ���������� ���������������� ����������
	//��������:   ������ � ��������� ����� ������ (Key.dat)
	//			      � ����� ������������ (Login.dat)  
	// login  - [��] �����
	// psw    - [��] ���������������� ������
	//���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG SaveUserData ( NCLOGIN login, WCHAR psw[] );

  //����������: ���������� ���������������� ����������
  //��������:   ������ �� ���������� ����� ������ (Key.dat)
  //			      � ����� ������������ (Login.dat)  
  // login  - [���] �����
  // psw    - [���] ���������������� ������
  //���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
  LONG LoadUserData ( NCLOGIN login, WCHAR psw[] );

	// ��������� ������, ����������� �� ������ pathName
	// userKey  - [out] ������/��������� ����
	// ���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG ReadPswFile ( NCKEY userKey );

	// ��������� ��� ������������, ����������� �� ������ pathName
	// userLogin  - [out] ��������� ����
	// ���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG ReadLoginFile ( NCLOGIN userLogin );

  // ��������� �����
  LONG SaveKeys( CString path, NCKEY key, TCHAR *sMode);

  // ��������� ������������ � ����
  // path   - [��] ���� � �����
  // info   - [��] �������������� ����������, ������������ ����� �������
  // coefs  - [��] ������������ � ���� ������
  // count  - [��] ���-�� ��������� � ������� coefs
  // mode   - [��] ����� ������ ������ � ���� (�������������� ��� ����������)
  LONG SaveCoefs( const CString & path, const char * info, const float coefs[], int count, const CString & mode );
  
	//����������: ��������� ���������� ������
	//��������:   ������� ��������� ��� ���������� ������ �� ��������� ������� � ����
	//			  �� ���������� ������������� ����. 
	//���������:  pszPath		- [in] ���� � ����� � ��������
	//			  imageCount	- [in] ���������� �������
	//			  images		- [in] ���������� ������
	// ���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG SaveAllImages ( LPWSTR pszPath, int imageCount, BNCSIGNIMAGE images[] );

	// ��������� ��� ��������� ������ � �����
	// ���������� ERROR_SUCCESS, ���� �� ���������, ���� ��� ������
	LONG LoadAllImages ( CString filePath, UINT &count, BNCSIGNIMAGE images[] );

	// ��������� ���� �������� � ����
	// xemArr - [in] ������ ���
	// count  - [in] ���-�� ���
	LONG SaveXem ( CString filePath, int xemArr[], int count, TCHAR *sMode );
	// ������� ���� �������� �� �����
	// xemArr - [in] ������ ���
	// count  - [in] ���-�� ���
	LONG LoadXem ( CString filePath, int xemArr[], int &count );

	// ��������� ������ �� ���������� ��������� ������� � �������� ��
	// ������ ��������� ���� ��� ������������ �� ������ �������
	// netOutArr - [in] [i].x - ���������� ��������� ������ �� ������ i-�� �������,
	//					[i].y - ���������� ��������� �������� �� ������ i-�� �������
	LONG SaveNetOut(CString filePath, POINT netOutArr[], TCHAR *sMode );

	// ������������ ���������� ������ � �����
	// szPath - [in] ���� � ������������� �����
	// szType - [in] ��� ������� ������, ��������, "*.dat", "*.bnc"
	// ���������� ���������� ������ ���� szType, ��������� � ����� szPath
	int GetFileCount ( const TCHAR szPath[], const TCHAR szType[] );

  LONG WriteIniData( CString path, sessionInfo &si );
  LONG LoadIniData( CString path, sessionInfo &si );

  // �������� ������������� �������� � ������� ������������
  bool HasUserDir(CString path, NCLOGIN login);
  // �������� �������� (������������)
  bool CreateDir(CString path, NCLOGIN login);

	CString fullPath;			// ������ ���� � ���������
	CString tofileFullPath;		// ������ ���� � �����

private:
	FILE *stream;
};
extern CFileManip objFileManip;
