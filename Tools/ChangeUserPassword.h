// ChangeUserPassword.h : header file

#pragma once

// �����, �������� ���������������� ������
class CChangeUserPsw
{
public:
	CChangeUserPsw(void);
	~CChangeUserPsw(void);

	// �������, ����������� ������� ���������������� ������
	// userName  - [in]  ��� ������������
	// oldPsw	 - [in]  ������ ������
	// newPsw	 - [in]  ����� ������
	// c_Message - [out] ��������� - ��������� ����� ������
	// nType	 - [out] ������������ ��� ������ ��� ����������� � �����������
	void ChangePassword ( WCHAR userName[], WCHAR oldPsw[], 
						  WCHAR newPsw[], CString &c_Message, UINT &nType );
	void ChangePassword ( WCHAR userName[], WCHAR oldPsw[], WCHAR newPsw[] );

	// �������, ����������� ���������� ��� � ����� ����������
	// pcName  - [out] ��� ������� ����������
	// domName - [out] ��� ������, � ������� ������ ���������
	void DetermineNameAndDomain ( WCHAR pcName[], WCHAR domName[] );

	// �������, ����������� ���������� ����� ����������
	BOOL IsComputerInDomain ( BOOL *pbIn, WCHAR *DomainName, PDWORD pBufferSize );

};
