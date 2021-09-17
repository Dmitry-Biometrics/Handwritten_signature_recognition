// ChangeUserPassword.cpp : implementation file

#include "../NCTestProj/StdAfx.h"
#include "ChangeUserPassword.h"
#include <lm.h>
#include <Windows.h>

#include "ntsecapi.h"

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS              ((NTSTATUS)0x00000000L)
#define STATUS_INVALID_PARAMETER    ((NTSTATUS)0xC000000DL)
#endif

CChangeUserPsw::CChangeUserPsw(void)
{
}

CChangeUserPsw::~CChangeUserPsw(void)
{
}

//�������, ����������� ������� ���������������� ������
void CChangeUserPsw::ChangePassword (
							WCHAR userName[], WCHAR oldPsw[], 
							WCHAR newPsw[], CString &c_Message, UINT &nType )
{
	// ������ ���������������� ������
	NET_API_STATUS dwStatus = 
					NetUserChangePassword ( NULL, userName, oldPsw, newPsw );
	
	// ���������� ������� ��������� ��������� ��� ���
	switch ( dwStatus )
	{
		case NERR_Success:		         c_Message = " ������ ��� �������. "; nType = MB_ICONASTERISK; break;
		case ERROR_ACCESS_DENIED:        c_Message = " ������������ �� ����� ������� � ������������� ����������. "; nType = MB_ICONWARNING; break;
		case ERROR_INVALID_PASSWORD:     c_Message = " �������� ������ ������. ������ �������� � ������ ��������. \n ���������, �� ������ �� �������� ������� Caps Lock. "; nType = MB_ICONWARNING; break;
		case NERR_InvalidComputer:       c_Message = " �������� ��� ����������. ��������� � ����� ������ �� ������."; nType = MB_ICONWARNING; break;
		case NERR_NotPrimary:			 c_Message = " ����� ������ ������ ����������, ��� ��� ����� ����������"; nType = MB_ICONWARNING; break;
		case ERROR_CANT_ACCESS_DOMAIN_INFO:	c_Message = " ����� ������ ������ ����������, ��� ��� ����� ����������"; nType = MB_ICONWARNING; break;
		case NERR_UserNotFound:          c_Message = " �������� ��� ������������. ������������ � ����� ������ �� ������. "; nType = MB_ICONWARNING; break;
		case ERROR_PASSWORD_RESTRICTION: c_Message = " Unable to update the password because a password update rule has been violated. "; nType = MB_ICONWARNING; break;
		case NERR_PasswordTooShort:      c_Message = " �������� ������ �� ������������� ����������� ��������� ��������.\n ����� ������ �� ����� ��������� ��� ���������� ������.\n ������� ������ �������. "; nType = MB_ICONWARNING; break;
		default:                         c_Message = " ��������� �� ������ �������� ���������������� ������. "; nType = MB_ICONWARNING;
	}
}

void CChangeUserPsw::ChangePassword ( 
							 WCHAR userName[], WCHAR oldPsw[], WCHAR newPsw[] )
{
	// ������ ���������������� ������
	NET_API_STATUS dwStatus = 
						NetUserChangePassword ( NULL, userName, oldPsw, newPsw );
}


//�������, ����������� ���������� ��� � ����� ����������
void CChangeUserPsw::DetermineNameAndDomain ( WCHAR pcName[], WCHAR domName[] )
{
	// �������, ������������ ����� ���������� � ��� ���
	BOOL  bDomain = TRUE;
	DWORD dwSize;
	
	// ������� ����� ����������
	WCHAR DomainNameW[MAX_COMPUTERNAME_LENGTH + 1];
	ZeroMemory(DomainNameW,sizeof(DomainNameW));
	IsComputerInDomain(&bDomain, DomainNameW, &dwSize);
	memcpy(domName, DomainNameW, sizeof(DomainNameW));

	// ������� ��� ����������
	dwSize = MAX_COMPUTERNAME_LENGTH;
	GetComputerNameW ( pcName, &dwSize );
}

//****************************************************************************************
//�������, ������������ ��� ����������� ������ ����������

void InitLsaString(  PLSA_UNICODE_STRING LsaString, LPWSTR String )
{
	DWORD StringLength;

	if(String == NULL) {
		LsaString->Buffer = NULL;
		LsaString->Length = 0;
		LsaString->MaximumLength = 0;

		return;
	}

	StringLength = lstrlenW(String);
	LsaString->Buffer = String;
	LsaString->Length = (USHORT) StringLength * sizeof(WCHAR);
	LsaString->MaximumLength = (USHORT) (StringLength + 1) *
		sizeof(WCHAR);
}

NTSTATUS OpenPolicy( LPWSTR ServerName, DWORD DesiredAccess, PLSA_HANDLE PolicyHandle )
{
	PLSA_UNICODE_STRING Server;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	LSA_UNICODE_STRING ServerString;

	//
	// Always initialize the object attributes to all zeroes
	//
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	if(ServerName != NULL) {
		//
		// Make a LSA_UNICODE_STRING out of the LPWSTR passed in
		//
		InitLsaString(&ServerString, ServerName);
		Server = &ServerString;
	} else {
		Server = NULL; // default to local machine
	}

	//
	// Attempt to open the policy and return NTSTATUS
	//
	return LsaOpenPolicy(
		Server,
		&ObjectAttributes,
		DesiredAccess,
		PolicyHandle
		);
}
//�������, ������������ ����� ����������
BOOL CChangeUserPsw::IsComputerInDomain(BOOL *pbIn, WCHAR *DomainName, PDWORD pBufferSize)
{
	LSA_HANDLE PolicyHandle;
	NTSTATUS Status;

	PPOLICY_PRIMARY_DOMAIN_INFO pprimpdi;

	if (!pbIn)
		return FALSE;

	Status = OpenPolicy(
		NULL,
		POLICY_VIEW_LOCAL_INFORMATION,//POLICY_VIEW_AUDIT_INFORMATION,
		&PolicyHandle
		);
	if(Status == STATUS_SUCCESS)
	{
		//�������� ��� ������
		Status = LsaQueryInformationPolicy(PolicyHandle, 
			PolicyPrimaryDomainInformation, (PVOID *)&pprimpdi);

		if(Status == STATUS_SUCCESS)
		{
			if (pBufferSize)
			{
				if (*pBufferSize >= pprimpdi->Name.Length)
				{
					//		memset(DomainName, 0, *pBufferSize);

					CopyMemory(DomainName, pprimpdi->Name.Buffer, pprimpdi->Name.Length);
				}
				*pBufferSize = pprimpdi->Name.Length;
			}

			*pbIn = TRUE;

			if (NULL == pprimpdi->Sid)
				*pbIn = FALSE;

			LsaFreeMemory(pprimpdi);
			LsaClose(PolicyHandle);
			return TRUE;
		}
		LsaClose(PolicyHandle);

	}
	return FALSE;
}
//**************************************************************************************
