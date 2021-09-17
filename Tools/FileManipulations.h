// FileManipulations.h : header file
// Поддерживает работу с файлами

#pragma once
#include "../NCTestProj/bnc32.h"

class CFileManip
{
public:
	CFileManip(void);
	~CFileManip(void);
	
	// Удаление координат выбранного образа
	// imageSel  - номер выбранного образа
	// imageLast - номер последнего образа
	// images	- координаты рукописных образов
	void DelImageCoord ( int imageSel, int imageLast, BNCSIGNIMAGE images[] );

	// Удалить все образы (очистить файлы, содержащие координаты обучающих примеров)
	// imageCount - общее количество рукописных образов
	// images	  - координаты рукописных образов
	void DelAllImages ( UINT &imageCount, BNCSIGNIMAGE images[] );

	//назначение: сохранить/создать биометрико-нейросетевой контейнер
	//описание:   записываем в отдельный файл веса обученной нейронной
	//			  сети и хеш обучающего ключа
	//параметры: path    - [in] путь к файлу
	//			 weights - [in] вектор весов нейронной сети
	//			 hash    - [in] хеш обучающего ключа
	// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG SaveBNC ( CString path, NCWEIGHTS weights, NCHASH hash );

	//назначение: загрузить/считать биометрико-нейросетевой контейнер
	//описание:   считываем из файла веса обученной нейронной
	//			  сети и хеш обучающего ключа
	//параметры: path    - [in] путь к файлу
	//			 weights - [in] вектор весов нейронной сети
	//			 hash    - [in] хеш обучающего ключа
	//возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG LoadBNC ( CString path, NCWEIGHTS weights, NCHASH hash );

	//назначение: сохранение пользовательской информации
	//описание:   запись в отдельные файлы пароля (Key.dat)
	//			      и имени пользователя (Login.dat)  
	// login  - [вх] логин
	// psw    - [вх] пользовательский пароль
	//возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG SaveUserData ( NCLOGIN login, WCHAR psw[] );

  //назначение: считывание пользовательской информации
  //описание:   чтение из отдельного файла пароля (Key.dat)
  //			      и имени пользователя (Login.dat)  
  // login  - [вых] логин
  // psw    - [вых] пользовательский пароль
  //возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
  LONG LoadUserData ( NCLOGIN login, WCHAR psw[] );

	// Прочитать пароль, находящийся по адресу pathName
	// userKey  - [out] пароль/обучающий ключ
	// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG ReadPswFile ( NCKEY userKey );

	// Прочитать имя пользователя, находящийся по адресу pathName
	// userLogin  - [out] обучающий ключ
	// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG ReadLoginFile ( NCLOGIN userLogin );

  // Сохранить ключи
  LONG SaveKeys( CString path, NCKEY key, TCHAR *sMode);

  // Сохранить коэффициенты в файл
  // path   - [вх] путь к файлу
  // info   - [вх] дополнительная информация, записываемая перед данными
  // coefs  - [вх] записываемые в файл данные
  // count  - [вх] кол-во элементов в массиве coefs
  // mode   - [вх] режим записи данных в файл (перезаписывать или дописывать)
  LONG SaveCoefs( const CString & path, const char * info, const float coefs[], int count, const CString & mode );
  
	//назначение: сохранить рукописные образы
	//описание:   функция сохраняет все рукописные образы из обучающей выборки в файл
	//			  по указанному пользователем пути. 
	//параметры:  pszPath		- [in] путь к файлу с образами
	//			  imageCount	- [in] количество образов
	//			  images		- [in] рукописные образы
	// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG SaveAllImages ( LPWSTR pszPath, int imageCount, BNCSIGNIMAGE images[] );

	// Загрузить все обучающие образы с диска
	// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
	LONG LoadAllImages ( CString filePath, UINT &count, BNCSIGNIMAGE images[] );

	// Сохранить меры Хемминга в файл
	// xemArr - [in] массив мер
	// count  - [in] кол-во мер
	LONG SaveXem ( CString filePath, int xemArr[], int count, TCHAR *sMode );
	// Считать меры Хемминга из файла
	// xemArr - [in] массив мер
	// count  - [in] кол-во мер
	LONG LoadXem ( CString filePath, int xemArr[], int &count );

	// Сохранить массив со счётчиками появления ноликов и единичек на
	// выходе нейронной сети при тестировании на группе образов
	// netOutArr - [in] [i].x - количество появлений нолика на выходе i-го нейрона,
	//					[i].y - количество появлений единички на выходе i-го нейрона
	LONG SaveNetOut(CString filePath, POINT netOutArr[], TCHAR *sMode );

	// Подсчитываем количество файлов в папке
	// szPath - [in] путь к анализируемой папке
	// szType - [in] тип искомых файлов, например, "*.dat", "*.bnc"
	// Возвращает количество файлов типа szType, найденных в папке szPath
	int GetFileCount ( const TCHAR szPath[], const TCHAR szType[] );

  LONG WriteIniData( CString path, sessionInfo &si );
  LONG LoadIniData( CString path, sessionInfo &si );

  // Проверка существования каталога с данными пользователя
  bool HasUserDir(CString path, NCLOGIN login);
  // Создание каталога (пользователя)
  bool CreateDir(CString path, NCLOGIN login);

	CString fullPath;			// Полный путь к программе
	CString tofileFullPath;		// Полный путь к файлу

private:
	FILE *stream;
};
extern CFileManip objFileManip;
