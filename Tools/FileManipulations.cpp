// FileManipulations.cpp : implementation file
// Поддерживает работу с файлами

#include "../NCTestProj/StdAfx.h"
#include "FileManipulations.h"

#define  OLD_DATA_FORMAT		0	// старый формат сохранения координат
#define  NEW_DATA_FORMAT		1	// новый  формат сохранения координат
#define  PRESS1_DATA_FORMAT		2	// формат сохранения координат с давлением
#define  PRESS2_DATA_FORMAT		3	// формат сохранения координат с давлением (2 вариант)
#define  UNKNOWN_FORMAT			-1	// неизвестный формат сохранения координат

const CHAR FileSign[4]	= {'B','S','I','G'};
const BYTE  FileVersion	= 1;

CFileManip objFileManip;

CFileManip::CFileManip(void){}
CFileManip::~CFileManip(void){}

// Удаление координат выбранного образа
void CFileManip::DelImageCoord (int imageSel, int imageLast, BNCSIGNIMAGE images[] )
{
	// Удаляем координаты X - Y образа imageNum
	images[imageSel].m_Num = 0;
	delete [] images[imageSel].m_X;
	delete [] images[imageSel].m_Y;
	delete [] images[imageSel].m_P;
	delete [] images[imageSel].m_T;

	if ( imageSel < imageLast)
	{
		// Записываем в освободмвшуюся позицию координаты последнего образа
		// т.е. записываем образ из позиции imageCount в позицию imageNum
		images[imageSel].m_X = new UINT [images[imageLast].m_Num];
		images[imageSel].m_Y = new UINT [images[imageLast].m_Num];
		images[imageSel].m_P = new UINT [images[imageLast].m_Num];
		images[imageSel].m_T = new UINT [images[imageLast].m_Num];
		images[imageSel].m_Num = images[imageLast].m_Num;
		memcpy ( images[imageSel].m_X, images[imageLast].m_X, images[imageLast].m_Num*sizeof(UINT) );
		memcpy ( images[imageSel].m_Y, images[imageLast].m_Y, images[imageLast].m_Num*sizeof(UINT) );
		memcpy ( images[imageSel].m_P, images[imageLast].m_P, images[imageLast].m_Num*sizeof(UINT) );
		memcpy ( images[imageSel].m_T, images[imageLast].m_T, images[imageLast].m_Num*sizeof(UINT) );
		delete [] images[imageLast].m_X;
		delete [] images[imageLast].m_Y;
		delete [] images[imageLast].m_P;
		delete [] images[imageLast].m_T;
	}
}

// Удалить все образы (очистить файлы, содержащие координаты обучающих примеров)
void CFileManip::DelAllImages ( UINT &imageCount, BNCSIGNIMAGE images[] )
{
	for (UINT iCount = 0; iCount < imageCount; iCount++ ) {
		images[iCount].m_Num = 0;
		delete [] images[iCount].m_X;
		delete [] images[iCount].m_Y;
		delete [] images[iCount].m_P;
		delete [] images[iCount].m_T;
	}
	imageCount = 0;
}

//назначение: сохранить/создать биометрико-нейросетевой контейнер
//описание:   записываем в отдельный файл веса обученной нейронной
//			  сети и хеш обучающего ключа
//параметры: path    - [in] путь к файлу
//			 weights - [in] вектор весов нейронной сети
//			 hash    - [in] хеш обучающего ключа
// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::SaveBNC ( CString path, NCWEIGHTS weights, NCHASH hash )
{
	long lErr = ERROR_SUCCESS;	// код ошибки
	size_t wrCount = 0;			// количество реально записанных блоков
	if( !(_tfopen_s(&stream, path, _T("wb"))) )
	{
		// Запись весов
		wrCount += fwrite( weights, sizeof(NCWEIGHTS), 1, stream );
		// Запись хеша
		wrCount += fwrite( hash, sizeof(NCHASH), 1, stream );
		fclose(stream);
	}
	else 
		return lErr = ERROR_FILE_NOT_FOUND;

	// Проверяем на количество удачно записанных блоков
	if ( wrCount != 2 ) return lErr = ERROR_WRITE_FAULT;

	return lErr;
}

//назначение: загрузить/считать биометрико-нейросетевой контейнер
//описание:   считываем из файла веса обученной нейронной
//			  сети и хеш обучающего ключа
//параметры: path    - [in] путь к файлу
//			 weights - [in] вектор весов нейронной сети
//			 hash    - [in] хеш обучающего ключа
// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::LoadBNC ( CString path, NCWEIGHTS weights, NCHASH hash )
{
	long lErr = ERROR_SUCCESS;	// код ошибки
	size_t wrCount = 0;			// количество реально считанных блоков

	if( !(_tfopen_s(&stream, path, _T("rb"))) )
	{
		// Чтение весов
		wrCount += fread( weights, sizeof(NCWEIGHTS), 1, stream );
		// Чтение хеша
		wrCount += fread( hash, sizeof(NCHASH), 1, stream );
		fclose(stream);
	}
	else 
		return lErr = ERROR_FILE_NOT_FOUND;

	// Проверяем на количество удачно записанных блоков
	if ( wrCount != 2 ) return lErr = ERROR_READ_FAULT;

	return lErr;
}

//назначение: сохранение пользовательской информации
//описание:   запись в отдельные файлы пароля (Key.dat)
//			      и имени пользователя (Login.dat)  
// login  - [вх] логин
// psw    - [вх] пользовательский пароль
//возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::SaveUserData ( NCLOGIN login, WCHAR psw[] )
{
	long lErr = ERROR_SUCCESS;	// код ошибки
	size_t wrCount = 0;			    // количество реально записанных блоков

	// Запись в файл имени пользователя
	tofileFullPath = fullPath + _T("Data\\");
  tofileFullPath += login;
  tofileFullPath += _T("\\Login.dat");
	if( !(_tfopen_s(&stream, tofileFullPath, _T("wb"))) ) {
		wrCount += fwrite( login, sizeof(NCLOGIN), 1, stream );
		fclose(stream);
	}

	// Сохранение пользовательского пароля
  tofileFullPath = fullPath + _T("Data\\");
  tofileFullPath += login;
  tofileFullPath += _T("\\Key.dat");
  int len = (int)wcslen(psw);
	if( !(_tfopen_s(&stream, tofileFullPath, _T("wb"))) ) {		
    wrCount += fwrite( &len, sizeof(int), 1, stream );
		wrCount += fwrite( psw, len*sizeof(WCHAR), 1, stream );
		fclose(stream);
	}

  // Перезаписать файл index.ini
  tofileFullPath = fullPath + _T("Data\\index.ini");
  sessionInfo si;
  memcpy(si.userName, login, sizeof(NCLOGIN));
  memcpy(&si.keyLen, &len, sizeof(len));
  memset(si.key, 0, sizeof(si.key) );
  for (int i = 0; i < len; i++) {
    si.key[i] = (BYTE)psw[i];
  }
  lErr = WriteIniData(tofileFullPath, si);
  
	// Проверяем на количество удачно записанных блоков
	if ( wrCount != 3 || lErr != ERROR_SUCCESS) return ERROR_WRITE_FAULT;

	return lErr;
}

//назначение: считывание пользовательской информации
//описание:   чтение из отдельного файла пароля (Key.dat)
//			      и имени пользователя (Login.dat)  
// login  - [вых] логин
// psw    - [вых] пользовательский пароль
//возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::LoadUserData ( NCLOGIN login, WCHAR psw[] )
{
  long lErr = ERROR_SUCCESS;	// код ошибки
  size_t wrCount = 0;			    // количество реально считанных блоков

  // Считать имя пользователя
  tofileFullPath = fullPath + _T("Data\\Login.dat");
  if( !(_tfopen_s(&stream, tofileFullPath, _T("rb"))) ) {
    wrCount += fread( login, sizeof(NCLOGIN), 1, stream );
    fclose(stream);
  }

  // Считать пользовательский пароль
  tofileFullPath = fullPath + _T("Data\\Key.dat");
  int len = 0;
  if( !(_tfopen_s(&stream, tofileFullPath, _T("rb"))) ) {
    wrCount += fread( &len, sizeof(int), 1, stream );
    if (len <= sizeof(NCKEY)) {
      wrCount += fread( psw, len*sizeof(WCHAR), 1, stream );
      psw[len] = '\0';
    }
    fclose(stream);
  }

  // Проверяем на количество удачно записанных блоков
  if ( wrCount != 3 ) return lErr = ERROR_WRITE_FAULT;

  return lErr;
}

// Прочитать пользовательский пароль из файла
LONG CFileManip::ReadPswFile ( NCKEY userKey )
{
  ZeroMemory(userKey, sizeof(NCKEY));

	// Путь к файлу с ключом
	tofileFullPath = fullPath + _T("Data\\Key.dat");

	long lErr = ERROR_SUCCESS;	// код ошибки
	size_t wrCount = 0;			    // количество реально считанных блоков
  int len = 0;

	if ( !(_tfopen_s(&stream, tofileFullPath, _T("rb"))) ) {
    wrCount += fread( &len, sizeof(int), 1, stream );
    if (len <= sizeof(NCKEY)) {
      WCHAR *psw = new WCHAR[len+1];
      wrCount += fread( psw, len*sizeof(WCHAR), 1, stream );      
      for (int i = 0; i < len; i++) {
        userKey[i] = (BYTE)psw[i];
      }
      delete [] psw;
    }
    fclose(stream);
	}
	else 
		return lErr = ERROR_FILE_NOT_FOUND;
 
	// Проверяем на количество удачно записанных блоков
	if ( wrCount != 2 ) return lErr = ERROR_READ_FAULT;

	return lErr;
}

// Прочитать имя пользователя, находящийся по адресу pathName
// userLogin  - [out] обучающий ключ
// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::ReadLoginFile ( NCLOGIN userLogin )
{
	// Путь к файлу с именем
	tofileFullPath = fullPath + _T("Data\\Login.dat");

	long lErr = ERROR_SUCCESS;	// код ошибки
	size_t wrCount = 0;			// количество реально считанных блоков

	if( !(_tfopen_s(&stream, tofileFullPath, _T("rb"))) )
	{
		wrCount += fread( userLogin, sizeof(NCLOGIN), 1, stream );
		fclose(stream);
	}
	else 
		return lErr = ERROR_FILE_NOT_FOUND;

	// Проверяем на количество удачно записанных блоков
	if ( wrCount != 1 ) return lErr = ERROR_READ_FAULT;

	return lErr;
}

// Сохранить ключи
LONG CFileManip::SaveKeys( CString path, NCKEY key, TCHAR *sMode)
{
  size_t wrCount = 0;			// количество реально записанных блоков
  int   keyArr[NEURON_COUNT];
  TCHAR hexStr[NEURON_COUNT/4+1];	// строка с пользовательским паролём

	if( !(_tfopen_s(&stream, path, sMode)) ) { 
    BncConvertKey (key, keyArr);
    // Запись символьного ключа
    fprintf(stream, "Cимвольный пароль:\n");
    for (int i = 0; i < NCKEY_SIZE; i++) {
      fprintf(stream, "%c", key[i]);
    }
    fprintf(stream, "\n");
    // Запись шестнадцатиричного ключа
    BncKeyToPassw ( key, hexStr, TRUE );
    fprintf(stream, "Шестнадцатиричный ключ:\n");
    for (int i = 0; i < NEURON_COUNT/4; i++) {
      fprintf(stream, "%c", hexStr[i]);
    }
    fprintf(stream, "\n");
    // Запись двоичного ключа
    fprintf(stream, "Двоичный ключ:\n");
    for (int i = 0; i < NEURON_COUNT/8; i++) {
      for (int j = 0; j < 8; j++) {
        fprintf(stream, "%d", keyArr[i*8+j]);
      }
    }
    fprintf(stream, "\n\n");
		fclose(stream);
	}
	else 
    return ERROR_FILE_NOT_FOUND;

	return ERROR_SUCCESS;
}

// Сохранить коэффициенты в файл
LONG CFileManip::SaveCoefs( const CString & path, const char * info, const float coefs[], int count, const CString & mode )
{
  if( !(_tfopen_s(&stream, path, mode)) ) { 
    fprintf(stream, info);
    for (int i = 0; i < count; i++) {
      fprintf(stream, "%.3f\n", coefs[i]);
    }
    fprintf(stream, "\n\n");
    fclose(stream);
  }
  else 
    return ERROR_FILE_NOT_FOUND;

  return ERROR_SUCCESS;
}

//LONG CFileManip::SaveAllImages (
//					CString filePath, int imageCount, BNCSIGNIMAGE images[], TCHAR *sMode )
//{
//	long lErr = ERROR_SUCCESS;	// код ошибки
//
//	if( !(_tfopen_s(&stream, filePath, sMode)) )
//	{
//		// Количества образов
//		fprintf(stream, "%d\n", imageCount);
//
//		// Последовательная запись всех образов
//		for ( int iCount = 0; iCount < imageCount; iCount++ )
//		{
//			// Запись в файл координат i-го образа
//			for ( unsigned jCount = 0; jCount < images[iCount].m_Num; jCount++ ) {
//				fprintf (stream, "%d\t%d\t%d\n", images[iCount].m_X[jCount], images[iCount].m_Y[jCount], images[iCount].m_T[jCount]);
//			}
//			// Разделитель
//			fprintf(stream, "%d\t%d\t%d\n", -1, -1, -1);
//		}
//		fclose(stream);
//	}
//	else lErr = ERROR_FILE_NOT_FOUND;
//	return lErr;
//}

//назначение: сохранить рукописные образы
//описание:   функция сохраняет все рукописные образы из обучающей выборки в файл
//			  по указанному пользователем пути. 
//параметры:  pszPath		- [in] путь к файлу с образами
//			  imageCount	- [in] количество образов
//			  images		- [in] рукописные образы
// возвращает ERROR_SUCCESS, если всё нормально, либо код ошибки
LONG CFileManip::SaveAllImages ( LPWSTR pszPath, int imageCount, BNCSIGNIMAGE images[] )
{
	DWORD	dwRes;
	BOOL	bRes;
	HANDLE	hFile;
	LONG	lErr = ERROR_READ_FAULT;	// код ошибки

	hFile = CreateFile( pszPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE) {
		return ERROR_FILE_NOT_FOUND;
	}
	__try {
		// write file signature
		bRes	= WriteFile( hFile, FileSign, sizeof(FileSign), &dwRes,NULL );
		if ( !bRes) __leave;
		// write version & flags
		BYTE	CurVersion	= 1;
		BYTE	Flags		= 0;
		bRes	= WriteFile( hFile, &CurVersion, sizeof(CurVersion), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= WriteFile( hFile, &Flags, sizeof(Flags), &dwRes,NULL );
		if ( !bRes) __leave;

		DWORD	MaxX = images[0].m_MaxX, MaxY = images[0].m_MaxY, MaxP = images[0].m_MaxP;

		// write resolution
		bRes	= WriteFile( hFile, &MaxX, sizeof(MaxX), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= WriteFile( hFile, &MaxY, sizeof(MaxY), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= WriteFile( hFile, &MaxP, sizeof(MaxP), &dwRes,NULL );
		if ( !bRes) __leave;

		// write Signatures
		DWORD	dwNumSign	= imageCount;	// количество сохранённых образов
		bRes	= WriteFile( hFile, &dwNumSign, sizeof(dwNumSign), &dwRes,NULL );
		if ( !bRes) __leave;

		// Последовательная запись координат и давления всех образов
		for ( unsigned iCount = 0; iCount < dwNumSign; iCount++ )
		{
			// записываем количество координат в образе
			bRes = WriteFile( hFile, &images[iCount].m_Num, sizeof(images[iCount].m_Num), &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = WriteFile( hFile, images[iCount].m_X, sizeof(images[iCount].m_X[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = WriteFile( hFile, images[iCount].m_Y, sizeof(images[iCount].m_Y[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = WriteFile( hFile, images[iCount].m_P, sizeof(images[iCount].m_P[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = WriteFile( hFile, images[iCount].m_T, sizeof(images[iCount].m_T[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;

			lErr = ERROR_SUCCESS;
		}

	}__finally{
		CloseHandle( hFile);
	}

	return lErr;

}
// Определить формат считываемых координат (новый или старый)
// Новый формат  - в файл записаны координаты x,y и время t
// Старый формат - в файл записаны координаты x,y, а затем коэффициенты Фурье
INT DefineDataFormat ( CString filePath )
{
	FILE *stream;
	int count, x, y, t;
	const char FileSign[4]	= {'B','S','I','G'};	// подпись файлов с давлением
	INT iRes = UNKNOWN_FORMAT;		// возвращаемый результат

	if( !(_tfopen_s(&stream, filePath, _T("r"))) )
	{
		// Считываем количество обучающих примеров либо подпись файла
		fscanf_s ( stream, "%d\n", &count );
		if ( stream->_base[0] == FileSign[0] )
		{
			// т.е. перед нами файл, хранящий X,Y,P,T + maxX, maxY, maxP
			char str[6];
			fscanf_s ( stream, "%5s", str, 6);
			switch ( str[4] )
			{
			case 1:  // т.е. версия файла == 1 (использовалось ещё Диманом)
				iRes = PRESS1_DATA_FORMAT;	break;	
			case 2:  // т.е. версия файла == 2 (используется мной для хранения всех тестовых образов в одном файле) 
				iRes = PRESS2_DATA_FORMAT;	break;
			default: 
				iRes = UNKNOWN_FORMAT;
			}
		}
		else
		{
			// т.е. перед нами файлы, без давления (а некоторые и без времени)
			count = 0;
			while ( count < NCMAXCOORD_QUAN ) {
				count++;
				fscanf_s ( stream, "%d\t%d\t%d\n", &x, &y, &t );
				if ( x == -1 && y == -1 && t == -1) {
					iRes = NEW_DATA_FORMAT;			// новый формат
					break;
				}
			}
			// Если в файле данные, сохранённые в старом формате
			if ( iRes == UNKNOWN_FORMAT )
			{
				fpos_t pos;
				pos = 0;
				fsetpos(stream, &pos);
				fscanf_s ( stream, "%d\n", &count );
				count = 0;
				while ( count < NCMAXCOORD_QUAN ) {
					count++;
					fscanf_s ( stream, "%d\t%d\n", &x, &y );
					if ( (x == 0 && y == 1) || (x == 1 && y == 1) || (x == -1 && y == -1) ) {
						iRes = OLD_DATA_FORMAT;			// старый формат
						break;
					}
				}
			}
		}
		fclose(stream);
	}

	return iRes;

}

// Загрузить координаты образов, записанных в старом формате
// Т.е. только координаты x,y без отсчётов времени 
LONG LoadOldFormatImages (
				CString filePath, UINT &imageCount, BNCSIGNIMAGE images[] )
{
	int	  x, y;					// координаты x и y	и время
	long  lErr = ERROR_SUCCESS;	// код ошибки
	PUINT coordX = new UINT [NCMAXCOORD_QUAN];
	PUINT coordY = new UINT [NCMAXCOORD_QUAN];
	PUINT coordT = new UINT [NCMAXCOORD_QUAN];
	FILE  *stream;

	if( !(_tfopen_s(&stream, filePath, _T("r"))) )
	{
		// Чтение количества обучающих примеров
		fscanf_s ( stream, "%d\n", &imageCount );

		// Если в файле записано больше образов, чем предусмотрено 
		// т.е. imageCount > NCIMAGE_COUNT, то останавлваем процесс считывания
		if ( imageCount > NCIMAGE_COUNT ) 
			return ERROR_READ_FAULT;

		// Последовательное чтение всех образов
		for ( UINT iCount = 0; iCount < imageCount; iCount++ )
		{
			int jCount = 0;
			fscanf_s ( stream, "%d\t%d\n", &x, &y );
			// Считываем координаты до тех пор, пока не встретим разрыв.
			// Разрыв в первой редакции был x = 0, y = 1, затем шли
			// коэффициенты Фурье до разрыва x = 1, fcoef = 0.
			// Во второй редакции разрыв обозначался x = 1, y = 1.
			// А в третьей редакции - x = -1, y = -1.
			// Следовательно, необходимо учитывать все варианты разрывов
			while ( (x != 0 || y != 1) && (x != 1 || y != 1) && (x != -1 || y != -1) ) 
			{
				coordX[jCount] = x;
				coordY[jCount] = y;
				fscanf_s ( stream, "%d\t%d\n", &x, &y );
				jCount++;
			}

			// Пропускаем хранящиеся в старом формате коэффициенты Фурье
			if ( x == 0 && y == 1)
			{
				float fcoef;	// временный коэффициент
				fscanf_s ( stream, "%d\t%f\n", &x, &fcoef);
				while( x != 1 || fcoef != 0 ) {
					fscanf_s ( stream, "%d\t%f\n", &x, &fcoef);
				}
			}
			
			// Записываем считанные координаты в массив образов
			images[iCount].m_Num = jCount;
			images[iCount].m_X = new UINT [jCount];
			memcpy (images[iCount].m_X, coordX, jCount*sizeof(UINT));
			images[iCount].m_Y = new UINT [jCount];
			memcpy (images[iCount].m_Y, coordY, jCount*sizeof(UINT));
			images[iCount].m_P = new UINT [jCount];
			ZeroMemory(images[iCount].m_P, jCount*sizeof(UINT));
			images[iCount].m_T = new UINT [jCount];
			ZeroMemory(images[iCount].m_T, jCount*sizeof(UINT));
		}
		fclose(stream);
	}
	else lErr = ERROR_FILE_NOT_FOUND;

	delete [] coordX;
	delete [] coordY;
	delete [] coordT;
	
	return lErr;

}

// Загрузить координаты образов, записанных в новом формате
LONG LoadNewFormatImages (
						  CString filePath, UINT &imageCount, BNCSIGNIMAGE images[] )
{
	int x, y, t;				// координаты x и y	и время
	long lErr = ERROR_SUCCESS;	// код ошибки
	PUINT coordX = new UINT [NCMAXCOORD_QUAN];
	PUINT coordY = new UINT [NCMAXCOORD_QUAN];
	PUINT coordT = new UINT [NCMAXCOORD_QUAN];
	FILE *stream;
	
	if( !(_tfopen_s(&stream, filePath, _T("r"))) )
	{
		// Чтение количества обучающих примеров
		fscanf_s ( stream, "%d\n", &imageCount );

		// Если в файле записано больше образов, чем предусмотрено 
		// т.е. imageCount > NCIMAGE_COUNT, то останавлваем процесс считывания
		if ( imageCount > NCIMAGE_COUNT ) 
			return ERROR_READ_FAULT;

		// Последовательное чтение всех образов
		for ( UINT iCount = 0; iCount < imageCount; iCount++ )
		{
			int jCount = 0;
			fscanf_s ( stream, "%d\t%d\t%d\n", &x, &y, &t );
			while ( (x != -1 || y != -1) ) {
				coordX[jCount] = x;
				coordY[jCount] = y;
				coordT[jCount] = t;
				fscanf_s ( stream, "%d\t%d\t%d\n", &x, &y, &t );
				jCount++;
			}
			
			// Записываем считанные координаты в массив образов
			images[iCount].m_Num = jCount;
			images[iCount].m_X = new UINT [jCount];
			memcpy (images[iCount].m_X, coordX, jCount*sizeof(UINT));
			images[iCount].m_Y = new UINT [jCount];
			memcpy (images[iCount].m_Y, coordY, jCount*sizeof(UINT));
			images[iCount].m_P = new UINT [jCount];
			ZeroMemory(images[iCount].m_P, jCount*sizeof(UINT));
			images[iCount].m_T = new UINT [jCount];
			memcpy (images[iCount].m_T, coordT, jCount*sizeof(UINT));
		}
		fclose(stream);
	}
	else lErr = ERROR_FILE_NOT_FOUND;

	delete [] coordX;
	delete [] coordY;
	delete [] coordT;

	return lErr;

}

// Загрузить образы, записанные в формате X+Y+P+T
LONG LoadXYPTFormatImages (
						   const TCHAR szFileName[MAX_PATH], UINT &imageCount, BNCSIGNIMAGE images[] )
{
	DWORD	dwRes;
	BOOL	bRes;
	HANDLE	hFile;
	LONG	lErr = ERROR_READ_FAULT;	// код ошибки

	hFile	= CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( hFile == INVALID_HANDLE_VALUE) {
		return ERROR_FILE_NOT_FOUND;
	}
	__try {
		// read file signature
		char	rFileSign[4];
		bRes	= ReadFile( hFile, rFileSign, sizeof(rFileSign), &dwRes,NULL );
		if ( !bRes) __leave;
		// read version & flags
		BYTE	Version;
		BYTE	Flags;
		bRes	= ReadFile( hFile, &Version, sizeof(Version), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, &Flags, sizeof(Flags), &dwRes,NULL );
		if ( !bRes) __leave;

		// read resolution
		DWORD	MaxX,MaxY,MaxPressure; // максимальное значение X, Y и давления
		bRes	= ReadFile( hFile, & MaxX, sizeof(MaxX), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, & MaxY, sizeof(MaxY), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, & MaxPressure, sizeof(MaxPressure), &dwRes,NULL );
		if ( !bRes) __leave;

		// read Signatures
		DWORD	dwNumSign	= 0;	// количество сохранённых образов
		bRes	= ReadFile( hFile, &dwNumSign, sizeof(dwNumSign), &dwRes,NULL );
		if ( !bRes) __leave;
		imageCount = (UINT)dwNumSign;

		DWORD NumPoint;
		// Последовательное считывание координат и давления всех хранящихся образов
		for ( unsigned iCount = 0; iCount < dwNumSign; iCount++ )
		{
			// считываем количество координат в образе
			bRes = ReadFile( hFile, &NumPoint, sizeof(NumPoint), &dwRes,NULL );
			if ( !bRes) __leave;

			images[iCount].m_Num	= NumPoint;
			images[iCount].m_X		= new UINT [NumPoint];
			images[iCount].m_Y		= new UINT [NumPoint];
			images[iCount].m_P		= new UINT [NumPoint];
			images[iCount].m_T		= new UINT [NumPoint];
			bRes = ReadFile( hFile, images[iCount].m_X, sizeof(images[iCount].m_X[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_Y, sizeof(images[iCount].m_Y[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_P, sizeof(images[iCount].m_P[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_T, sizeof(images[iCount].m_T[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;

			images[iCount].m_MaxX = MaxX;
			images[iCount].m_MaxY = MaxY;
			images[iCount].m_MaxP = MaxPressure;

			lErr = ERROR_SUCCESS;
		}

	}__finally{
		CloseHandle( hFile);
	}

	return lErr;

}
// Загрузить образы, записанные в формате X+Y+P+T (в одном файле более 2000 образов)
LONG LoadALLXYPTFormatImages (
							  const TCHAR szFileName[MAX_PATH], UINT &imageCount, BNCSIGNIMAGE images[] )
{
	DWORD	dwRes;
	BOOL	bRes;
	HANDLE	hFile;
	LONG	lErr = ERROR_READ_FAULT;	// код ошибки

	hFile	= CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if ( hFile == INVALID_HANDLE_VALUE) {
		return ERROR_FILE_NOT_FOUND;
	}
	__try {
		// read file signature
		char	rFileSign[4];
		bRes	= ReadFile( hFile, rFileSign, sizeof(rFileSign), &dwRes,NULL );
		if ( !bRes) __leave;
		// read version & flags
		BYTE	Version;
		BYTE	Flags;
		bRes	= ReadFile( hFile, &Version, sizeof(Version), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, &Flags, sizeof(Flags), &dwRes,NULL );
		if ( !bRes) __leave;

		// read resolution
		DWORD	MaxX,MaxY,MaxPressure; // максимальное значение X, Y и давления
		bRes	= ReadFile( hFile, & MaxX, sizeof(MaxX), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, & MaxY, sizeof(MaxY), &dwRes,NULL );
		if ( !bRes) __leave;
		bRes	= ReadFile( hFile, & MaxPressure, sizeof(MaxPressure), &dwRes,NULL );
		if ( !bRes) __leave;

		// read Signatures
		DWORD	dwNumSign	= 0;	// количество сохранённых образов
		bRes	= ReadFile( hFile, &dwNumSign, sizeof(dwNumSign), &dwRes,NULL );
		if ( !bRes) __leave;
		imageCount = (UINT)dwNumSign;

		DWORD NumPoint;
		// Последовательное считывание координат и давления всех хранящихся образов
		for ( unsigned iCount = 0; iCount < dwNumSign; iCount++ )
		{
			// считываем количество координат в образе
			bRes = ReadFile( hFile, &NumPoint, sizeof(NumPoint), &dwRes,NULL );
			if ( !bRes) __leave;

			images[iCount].m_Num	= NumPoint;
			images[iCount].m_X		= new UINT [NumPoint];
			images[iCount].m_Y		= new UINT [NumPoint];
			images[iCount].m_P		= new UINT [NumPoint];
			images[iCount].m_T		= new UINT [NumPoint];
			bRes = ReadFile( hFile, images[iCount].m_X, sizeof(images[iCount].m_X[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_Y, sizeof(images[iCount].m_Y[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_P, sizeof(images[iCount].m_P[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, images[iCount].m_T, sizeof(images[iCount].m_T[0])*images[iCount].m_Num, &dwRes,NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, &images[iCount].m_MaxX,  sizeof(images[iCount].m_MaxX), &dwRes, NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, &images[iCount].m_MaxY,  sizeof(images[iCount].m_MaxY), &dwRes, NULL );
			if ( !bRes) __leave;
			bRes = ReadFile( hFile, &images[iCount].m_MaxP,  sizeof(images[iCount].m_MaxP), &dwRes, NULL );
			if ( !bRes) __leave;

			lErr = ERROR_SUCCESS;
		}


	}__finally{
		CloseHandle( hFile);
	}

	return lErr;

}



// Загрузить все обучающие образы с диска 
// Чтение рукописных образов из одного файла
LONG CFileManip::LoadAllImages ( 
					CString filePath, UINT &count, BNCSIGNIMAGE images[] )
{
	long lErr = ERROR_SUCCESS;	// код ошибки

	// Определяем в каком формате сохранены данные (новый или старый формат)
	INT ires = DefineDataFormat(filePath);

	// Считываем координаты, сохранённые в разных форматах
	switch ( ires )
	{
	case OLD_DATA_FORMAT:  
		lErr = LoadOldFormatImages (filePath, count, images); break;
	case NEW_DATA_FORMAT:   
		lErr = LoadNewFormatImages (filePath, count, images); break;
	case PRESS1_DATA_FORMAT: 
		lErr = LoadXYPTFormatImages (filePath, count, images); break;
	case PRESS2_DATA_FORMAT: 
		lErr = LoadALLXYPTFormatImages(filePath, count, images); break;
	default: 
		lErr = ERROR_INVALID_FUNCTION;
	}

	return lErr;;
}

// Сохранить меры Хемминга в файл
LONG CFileManip::SaveXem ( CString filePath, int xemArr[], int count, TCHAR *sMode )
{
	long lErr = ERROR_SUCCESS;	// код ошибки

	if( !(_tfopen_s(&stream, filePath, sMode)) )
	{
		for ( int i = 0; i < count; i++ ) {
			fprintf(stream, "%d\n", xemArr[i]);
		}
		fclose(stream);
	}
	else lErr = ERROR_FILE_NOT_FOUND;

	return lErr;
}
// Считать меры Хемминга из файла
// xemArr - [in] массив мер
// count  - [in] кол-во мер
LONG CFileManip::LoadXem ( CString filePath, int xemArr[], int &count )
{
	long lErr = ERROR_SUCCESS;	// код ошибки

	if( !(_tfopen_s(&stream, filePath, _T("r"))) )
	{ 
		int i = 0;
		while ( !feof( stream ) && i < count ) {
			fscanf_s(stream, "%d\n", &xemArr[i]);
			i++;
		}
		fclose(stream);
	}
	else lErr = ERROR_FILE_NOT_FOUND;

	return lErr;
}

// Сохранить массив со счётчиками появления ноликов и единичек на
// выходе нейронной сети при тестировании на группе образов
// netOutArr - [in] [i].x - количество появлений нолика на выходе i-го нейрона,
//					[i].y - количество появлений единички на выходе i-го нейрона
LONG CFileManip::SaveNetOut(CString filePath, POINT netOutArr[], TCHAR *sMode )
{
	long lErr = ERROR_SUCCESS;	// код ошибки

	if( !(_tfopen_s(&stream, filePath, sMode)) )
	{
		for ( int i = 0; i < NEURON_COUNT; i++ ) {
			fprintf(stream, "%d\t%d\n",netOutArr[i].x, netOutArr[i].y);
		}
		fclose(stream);
	}
	else lErr = ERROR_FILE_NOT_FOUND;

	return lErr;
}

// Подсчитываем количество файлов в папке
// szPath - [in] путь к анализируемой папке
// szType - [in] тип искомых файлов, например, "*.dat", "*.bnc"
// Возвращает количество файлов типа szType, найденных в папке szPath
int CFileManip::GetFileCount ( const TCHAR szPath[], const TCHAR szType[] )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE	hFind = INVALID_HANDLE_VALUE;
	wchar_t DirSpec[MAX_PATH];  // directory specification
	int		fileCount = 0;		// счётчик количества файлов

	// Подсчитываем кол-во файлов в данной директории
	wcscpy_s ( DirSpec, szPath );
	wcscat_s ( DirSpec, _T("*."));
	// считаем все файлы типа szType
	wcscat_s ( DirSpec, szType);

	hFind = FindFirstFile(DirSpec, &FindFileData);
	if ( hFind != INVALID_HANDLE_VALUE ) {
		fileCount++;
		while ( FindNextFile(hFind, &FindFileData) != 0 ) {
			fileCount++;
		}
		FindClose(hFind);
	}

	return fileCount;
}

//////////////////////////////////////////////////////////////////////////
// Записать данные в файл  настроек
// path - [вх] путь к файлу настроек
// si   - [вх] записываемые данные
LONG CFileManip::WriteIniData(CString path, sessionInfo &si) {
  DWORD	  dwRes;
  BOOL	  bRes;
  HANDLE	hFile;
  LONG	  lErr = ERROR_WRITE_FAULT;	// код ошибки

  hFile = CreateFile( path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if ( hFile == INVALID_HANDLE_VALUE) {
    return ERROR_FILE_NOT_FOUND;
  }

  bRes	= WriteFile( hFile, &si, sizeof(si), &dwRes, NULL );
  if ( bRes) lErr = ERROR_SUCCESS;

  CloseHandle( hFile);

  return lErr;
}

//////////////////////////////////////////////////////////////////////////
// Загрузить данные из файла настроек
// path - [вх]  путь к файлу настроек
// si   - [вых] считываемые данные
LONG CFileManip::LoadIniData( CString path, sessionInfo &si )
{
  DWORD	  dwRes;
  BOOL	  bRes;
  HANDLE	hFile;
  LONG	  lErr = ERROR_READ_FAULT;	// код ошибки

  hFile	= CreateFile( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if ( hFile == INVALID_HANDLE_VALUE) {
    return ERROR_FILE_NOT_FOUND;
  }

  bRes  = ReadFile( hFile, &si, sizeof(si), &dwRes,NULL );
  if ( bRes && dwRes != 0 ) lErr = ERROR_SUCCESS;

  CloseHandle( hFile);

  return lErr;

}

// Проверка существования каталога с данными пользователя
bool CFileManip::HasUserDir(CString path, NCLOGIN login)
{

  return true;
}

// Создание каталога (пользователя)
bool CFileManip::CreateDir(CString path, NCLOGIN login) {
  path += login;
  path += _T("\\");
  bool res = (bool)CreateDirectory(path, NULL);
  if (!res) {
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      res = true;
    }
  }
  return res;
}

 