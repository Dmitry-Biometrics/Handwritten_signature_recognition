// ChangeUserPassword.h : header file

#pragma once

//  ласс, мен€ющий пользовательский пароль
class CChangeUserPsw
{
public:
	CChangeUserPsw(void);
	~CChangeUserPsw(void);

	// ‘ункци€, позвол€юща€ сменить пользовательский пароль
	// userName  - [in]  им€ пользовател€
	// oldPsw	 - [in]  старый пароль
	// newPsw	 - [in]  новый пароль
	// c_Message - [out] сообщение - результат смены парол€
	// nType	 - [out] возвращаемый тип иконки дл€ отображени€ с соообщением
	void ChangePassword ( WCHAR userName[], WCHAR oldPsw[], 
						  WCHAR newPsw[], CString &c_Message, UINT &nType );
	void ChangePassword ( WCHAR userName[], WCHAR oldPsw[], WCHAR newPsw[] );

	// ‘ункци€, позвол€юща€ определить им€ и домен компьютера
	// pcName  - [out] им€ данного компьютера
	// domName - [out] им€ домена, в который входит компьютер
	void DetermineNameAndDomain ( WCHAR pcName[], WCHAR domName[] );

	// ‘ункци€, позвол€юща€ определить домен компьютера
	BOOL IsComputerInDomain ( BOOL *pbIn, WCHAR *DomainName, PDWORD pBufferSize );

};
