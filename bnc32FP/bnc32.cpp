//файл:			bnc32.h (bnc - биометрико-нейросетевой контейнер/хранитель паролей)
//описание:		Биометрико-нейросетевой контейнер осуществляет:
//              обучение нейронной сети - Ключ+Подписи=Сеть;
//				извлечение ключа из контейнера - Сеть+Подпись=Ключ;
//				тестирования обученной нейронной сети


#include <stdio.h>
#include "stdafx.h"
#include "bnc32.h"

#include "ConversionSFN.h"
#include "NetTraining.h"
#include "NetTesting.h"
#include "NetSimulation.h"
#include "ncmath.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	break;	// подключение библиотеки к процессу
	case DLL_THREAD_ATTACH:		break;	// подключение к потоку
	case DLL_THREAD_DETACH:		break;	// отключение от процесса
	case DLL_PROCESS_DETACH:	break;	// отключение от процесса
	}	//end switch
    return TRUE;

}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// Сохранить коэффициенты в файл
LONG SaveCoefs( const float coefs[], int count, 
                const WCHAR path[], 
                const CHAR info[], 
                const WCHAR mode[] )
{
  FILE *stream;
  if( _wfopen_s(&stream, path, mode) ) { 
    return ERROR_FILE_NOT_FOUND;
  }      
  //fprintf(stream, info);
  for (int i = 0; i < count; i++) {
    fprintf(stream, "%.3f\n", coefs[i]);
  }
  fprintf(stream, "\n\n");
  fclose(stream);

  return ERROR_SUCCESS;
}
// Считать коэффициенты из файла
bool LoadCoefs ( float coefs[], int &count, const WCHAR path[]) {
  FILE *stream;
  if( _wfopen_s(&stream, path, TEXT("r")) ) { 
    return false;
  }
//  CHAR info[MAX_PATH];
  //fscanf( stream, "%s\n", info );   
  int i = 0;
  while ( !feof( stream ) && i < count ) {
    fscanf_s(stream, "%f\n", &coefs[i]);
    i++;
  }
  count = i;
  fclose(stream);

  return true;
}
// Вычислить матожидание и стандартное отклонение и записать в файл
void calcAndWriteMS(float *coefs, int count, const WCHAR path[]) {
  // Входные характеристики (мат ожидание, дисперсия и качество на входе сети)
  NCFOURIER averArr;
  NCFOURIER dispArr;
  
  // Вычисление входного мат. ожидания и входного качества на первом слое
  fadq ( count, NCFOURIER_COUNT, coefs, averArr, dispArr, NULL );

  // Сохранить коэффициенты в файл
  WCHAR filePath[MAX_PATH];
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("mean.txt"));
  SaveCoefs(averArr,NCFOURIER_COUNT,filePath,"Математическое ожидание биометрических параметров обучающего образа:\n",TEXT("w"));  
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("stdev.txt"));
  SaveCoefs(dispArr,NCFOURIER_COUNT,filePath,"Стандартное отклонение биометрических параметров обучающего образа:\n",TEXT("w"));
}
// Подсчитать кол-во коэффициентов, попадающих в три сигмы обучающего образа
void calcAndWriteExtractor(float *coefs, int count, const WCHAR path[]) {
  // Сохранить коэффициенты Фурье в файл
  WCHAR filePath[MAX_PATH];
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("params.txt"));
  SaveCoefs(coefs,count,filePath,"Биометрические параметры последнего введённого рукописного примера:\n",TEXT("w"));
  if (count != NCFOURIER_COUNT) {
    return;
  }
  // Считать матожидание и отклонение обучающего образа
  NCFOURIER averArr;
  NCFOURIER dispArr;
  int cnt = NCFOURIER_COUNT;
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("mean.txt")); 
  if (!LoadCoefs(averArr,cnt,filePath)) {
    return;
  }
  if (cnt != NCFOURIER_COUNT) {
    return;
  }
  cnt = NCFOURIER_COUNT;
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("stdev.txt"));
  if (!LoadCoefs(dispArr,cnt,filePath)) {
    return;
  }
  if (cnt != NCFOURIER_COUNT) {
    return;
  }
  // Подсчитать кол-во параметров, попавших в три сигма
  int *outArr = new int[NCFOURIER_COUNT];
  cnt = 0;
  for (int i = 0; i < NCFOURIER_COUNT; i++) {
    outArr[i] = ((coefs[i] >= averArr[i]-3.0f*dispArr[i])&&(coefs[i] <= averArr[i]+3.0f*dispArr[i])) ? 1 : 0;
    cnt += outArr[i];
  }
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("3sigma.txt"));
  FILE *stream;
  if( !(_wfopen_s(&stream, filePath, TEXT("w"))) ) { 
    char  buffer[200];   
    sprintf( buffer, "В три сигма попало %d параметров из %d\n", cnt, NCFOURIER_COUNT);
    fprintf(stream, buffer);
    for (int i = 0; i < NCFOURIER_COUNT; i++) {
      fprintf(stream, "%d\n", outArr[i]);
    }
    fprintf(stream, "\n");
    fclose(stream);  
  }
  delete [] outArr;

}

//////////////////////////////////////////////////////////////////////////////////////
//назначение: функция обучения нейронной сети
//описание:   осуществляется преобразование рукописных образов пользователя и 
//			  обучающего ключа в таблицу весовых коэффициентов нейронной сети
//параметры:  key		- [in]  обучающий ключ
//			  mixKey	- [in]  перемешивающий ключ
//			  imgs		- [in]  обучающие образы
//			  count		- [in]  количество обучающих образов
//			  weights	- [out] таблица весовых коэффициентов
//			  group		- [out] номер группы стабильности пользователя
BNC32_API HRESULT BncNeuroNetTrain (
									const NCKEY key, 
									const NCKEY mixKey,
									UINT  count,
									const BNCSIGNIMAGE imgs[],
									NCWEIGHTS weights,
									UINT  *group )
{
	HRESULT result = ERROR_SUCCESS;
	if (!count || !key || !imgs || !weights){
		return ERROR_BAD_ARGUMENTS;	// все параметры д.б. заданы
	}
	if ( count > NCIMAGE_COUNT ){
		return ERROR_BAD_ARGUMENTS;	// количество образов не д.б. больше NCIMAGE_COUNT
	}

	FLOAT *coefs;		// одномерный массив коэффициентов Фурье всех обучающих образов
//	FLOAT *testCoefs;	// одномерный массив коэффициентов Фурье всех образов Чужие
	// Выделение памяти под коэффициенты всех Своих образов
	coefs = new FLOAT[count*NCFOURIER_COUNT];

	// Вычисление коэффициентов Фурье всех обучающих образов
	NcCalcImagesCoef ( count, imgs, coefs );

	// Обучение нейронной сети
	NeuroNetTrain ( count, coefs, key, mixKey, weights );

	// Определяем группу стойкости пользователя
	*group = CalculateStabilityGroup ( key, mixKey, weights );

	// Освобождение памяти
	delete[] coefs;

	return result;
}
//функция с сохранением коэффициентов
BNC32_API HRESULT BncNeuroNetTrain_2 (
                                    const WCHAR path[],
                                    const NCKEY key, 
                                    const NCKEY mixKey,
                                    UINT  count,
                                    const BNCSIGNIMAGE imgs[],
                                    NCWEIGHTS weights,
                                    UINT  *group )
{
  HRESULT result = ERROR_SUCCESS;
  if (!count || !key || !imgs || !weights){
    return ERROR_BAD_ARGUMENTS;	// все параметры д.б. заданы
  }
  if ( count > NCIMAGE_COUNT ){
    return ERROR_BAD_ARGUMENTS;	// количество образов не д.б. больше NCIMAGE_COUNT
  }

  FLOAT *coefs;		// одномерный массив коэффициентов Фурье всех обучающих образов
  //	FLOAT *testCoefs;	// одномерный массив коэффициентов Фурье всех образов Чужие
  // Выделение памяти под коэффициенты всех Своих образов
  coefs = new FLOAT[count*NCFOURIER_COUNT];

  // Вычисление коэффициентов Фурье всех обучающих образов
  NcCalcImagesCoef ( count, imgs, coefs );

  // Вычислить матожидание и стандартное отклонение и записать в файл
  calcAndWriteMS(coefs, count, path);

  // Обучение нейронной сети
  NeuroNetTrain ( count, coefs, key, mixKey, weights );

  // Определяем группу стойкости пользователя
  *group = CalculateStabilityGroup ( key, mixKey, weights );

  // Освобождение памяти
  delete[] coefs;

  return result;
}

///////////////////////////////////////////////////////////////////////////////
//назначение: получение ключа из нейронной сети
//описание:   преобразование введённого пользователем  
//			  рукописного образа в ключ
//параметры:  imgs    - [in]  рукописный образ/подпись пользователя
//			  weights - [out] таблица весовых коэффициентов обученной сети
//			  mixKey  - [in]  перемешивающий ключ
//			  key     - [in]  получаемый ключ
//возвращает: количество выходов первого слоя, попавших в указанный интервал
//			  используется для разделения СВОИХ и ЧУЖИХ
BNC32_API INT BncExtractKey (
							 const BNCSIGNIMAGE img,
							 const NCWEIGHTS weights,
							 const NCKEY mixKey,
							 NCKEY key )
{
	NCFOURIER fourier; 	// массив с коэффициентами Фурье
	INT count;			// счётчик количества попаданий

	// Преобразование координат в коэффициенты Фурье
	NcCalcCoefZero2416 ( img, fourier );

	// Извлечение ключа
	count = ImageSimulation ( fourier, weights, mixKey, key );

	return count;
}
//функция с сохранением коэффициентов
BNC32_API INT BncExtractKey_2 (
                             const WCHAR path[],
                             const BNCSIGNIMAGE img,
                             const NCWEIGHTS weights,
                             const NCKEY mixKey,
                             NCKEY key )
{
  NCFOURIER fourier; 	// массив с коэффициентами Фурье
  INT count;			// счётчик количества попаданий

  // Преобразование координат в коэффициенты Фурье
  NcCalcCoefZero2416 ( img, fourier );

  // Подсчитать кол-во коэффициентов, попадающих в три сигмы обучающего образа
  calcAndWriteExtractor(fourier, NCFOURIER_COUNT, path);
 
  // Извлечение ключа
  count = ImageSimulation ( fourier, weights, mixKey, key );

  return count;
}

///////////////////////////////////////////////////////////////////////////////
//назначение: вычисление моментов мер Хемминга на тестовых образах
//описание:   моделирование сети на тестовых образах и вычисление
//			  матожидания, дисперсии и качества мер Хемминга
//параметры:  on_TI   - [in]  TRUE - тестирование на тестовых образах, 
//							  FALSE - тестирование на белом шуме
//			  key     - [in]  обучающий ключ
//			  mixKey  - [in]  перемешивающий ключ
//			  imgs    - [in]  тестовые образы
//			  count	  - [in]  количество тестовых образов
//			  weights - [in] таблица весовых коэффициентов
BNC32_API BNCMOMENTS BncGetXemMoments (
									   const BOOL on_TI,
									   const NCKEY key, 
									   const NCKEY mixKey,
									   const BNCSIGNIMAGE imgs[],
									   UINT  count,
									   NCWEIGHTS weights )
{
	BNCMOMENTS params;	// параметры мер Хемминга
	FLOAT *coefs;		// одномерный массив функционалов всех тестовых образов
	// Выделение памяти под коэффициенты всех образов
	coefs = new FLOAT[count*NCFOURIER_COUNT];

	// Определяем на чём тестировать
	if ( on_TI == TRUE ) {		// тестирование на тестовых образах
		// Вычисление коэффициентов Фурье всех обучающих образов
		NcCalcImagesCoef ( count, imgs, coefs );
		NetTestOnTF ( count, key, mixKey, weights, coefs, params );
	} else {					// тестирование на белом шуме
		NetTestOnWN(count, key, mixKey, weights, params);
	}
	 
	// Освобождение памяти
	delete [] coefs;

	return params;
}

//назначение: вычисление моментов мер Хемминга для коэффициентов Фурье
//описание:   моделирование сети функционалах рукописных образов и вычисление
//			  матожидания, дисперсии и качества мер Хемминга
//параметры:  
//			  key      - [in]  обучающий ключ
//			  mixKey   - [in]  перемешивающий ключ
//			  imgsCoef - [in]  функционалы всех образов в одном векторе столбце
//			  count	   - [in]  количество тестовых образов
//			  weights  - [in]  таблица весовых коэффициентов
BNC32_API BNCMOMENTS BncCoefsToXemMoments (
										const NCKEY key, 
										const NCKEY mixKey,
										const FLOAT imgsCoef[],
										UINT  count,
										NCWEIGHTS weights )
{
	BNCMOMENTS params;	// параметры мер Хемминга

	NetTestOnTF ( count, key, mixKey, weights, imgsCoef, params );

	return params;
}

//////////////////////////////////////////////////////////////////////////
//назначение: вычисление коэффициентов Фурье входных образов
//описание:	  используется только на этапе исследований и экспериментов
//параметры:  imgs  - [in] исходные рукописные образы
//			  count - [in] количество рукописных образов
//			  bioParam - [out] вычисленные параметры рукописных образов (коэф-ты Фурье)
BNC32_API void BncGetImgParams (
								const BNCSIGNIMAGE imgs[],
								UINT  count, 
								FLOAT bioParam[] )
{
	NcCalcImagesCoef ( count, imgs, bioParam );
}
//////////////////////////////////////////////////////////////////////////
//назначение: преобразование коэффициентов Фурье в ключ
//описание:	  используется только на этапе исследований и экспериментов
//параметры:  coef    - [in]  коэффициенты Фурье одного образа
//			  weights - [in]  таблица весовых коэффициентов обученной сети
//			  mixKey  - [in]  перемешивающий ключ
//			  key     - [out] получаемый ключ
BNC32_API void BncCoefsToKey (
							  const NCFOURIER coef,
							  const NCWEIGHTS weights,
							  const NCKEY mixKey,
							  NCKEY key )
{
	ImageSimulation ( coef, weights, mixKey, key );
}


///////////////////////////////////////////////////////////////////////////////
//назначение: Сравнение ключей и подсчет количества отличающихся битов
//параметры:  first     - [in] первый ключ
//	          second    - [in] второй ключ
//возвращает: количество отличий между ключами (0, если ключи равны)
BNC32_API INT BncCompareKeys ( const NCKEY first, const NCKEY second )
{
	return getbitdiffs(first,second,NCKEY_BITS);
}
///////////////////////////////////////////////////////////////////////////////
//назначение: получение двоичного ключа
//описание:   преобразование строкового ключа NCKEY 
//			  в двоичный ключ типа INT
//параметры:  
//  key	   - [in]  строка с ключём
//	keyArr - [out] двоичный ключ (одномерный массив, каждая ячейка
//							   которого является элементом двоичного ключа)
//возвращает: код ошибки
BNC32_API HRESULT BncConvertKey ( const NCKEY key, INT keyArr[] )
{
	if ( !key )	return ERROR_BAD_ARGUMENTS;	

  //раньше
  /*for ( int i = 0; i < NEURON_COUNT; i++ ) {
  keyArr[i] = getbit(key,i);
  }*/
  //для Hex паролей
  int k = 0;
  int j = 0;
  for ( int i = 0; i < NEURON_COUNT; i++ ) {
    k = i / 8;
    j = i % 8;
    keyArr[k*8+7-j] = getbit(key,i);
  }
		
	return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
//назначение: хеширование входных данных
//параметры:  pbData    - [in]  входные/хешируемые данные
//			  dwDataLen - [in]  размер буфера pbData
//			  hash	    - [out] хеш
//возвращает: код ошибки
BNC32_API HRESULT BncCreateHash ( const BYTE *pbData, DWORD dwDataLen, NCHASH hash )
{
	if (!pbData || !hash){
		return ERROR_BAD_ARGUMENTS;
	}

	TCHAR		szName[]  = TEXT("bnc32.dll");
	HRESULT		result	  = -1;	
	HCRYPTPROV	hProv	  = NULL;
	HCRYPTHASH	hHash	  = NULL;
	DWORD		dwHashLen = sizeof(NCHASH);

	// открытие существующего контейнера
	if (!CryptAcquireContext(&hProv,szName,NULL,PROV_RSA_FULL,NULL)){
		if (GetLastError() != NTE_BAD_KEYSET)
			goto exit;
		// создание нового контейнера
		if (!CryptAcquireContext(&hProv,szName,NULL,PROV_RSA_FULL,CRYPT_NEWKEYSET))
			goto exit;
	}
	// создание объекта хеша
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)){
		goto exit;
	}
	// вычисление хеш-функции
	if (!CryptHashData(hHash, pbData, dwDataLen, 0)){
		goto exit;
	}

	ZeroMemory(hash,sizeof(NCHASH));
	// чтение размера хеша	
	if(!CryptGetHashParam( hHash, HP_HASHVAL, NULL, &dwHashLen, 0)){
		goto exit;
	}
	// чтение хеша
	if(!CryptGetHashParam( hHash, HP_HASHVAL, hash, &dwHashLen, 0)){
		goto exit;
	}
	result = ERROR_SUCCESS;
exit:
	if ( hHash )	CryptDestroyHash(hHash);
	if ( hProv )	CryptReleaseContext(hProv,0);

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//назначение: сравнение хешей
//параметры:  first	 - [in] первый хеш
//			  second - [in] второй хеш
//возвращает: TRUE, если хеши равны; FALSE, если хеши не равны
BNC32_API BOOL BncCompareHash( const NCHASH first, const NCHASH second) {
	return (0 == getbitdiffs ( first, second, NCHASH_LEN*sizeof(BYTE)));
}

///////////////////////////////////////////////////////////////////////////////
//назначение: Преобразование ключа в пароль
//параметры:  key	 - [in]  ключ
//			  passw	 - [out] пароль в виде строки
//			  as_hex - [in]  флаг вывода в 16-м режиме (в этом случае 
//					 	     размер буфера должен быть 65 символов)
//возвращает: TRUE, если всё нормально, иначе возвращает FALSE
BNC32_API BOOL BncKeyToPassw ( 
							  const NCKEY key,
							  TCHAR passw[NCKEY_SIZE+1],
							  BOOL as_hex
							 )
{
	if (!key || !passw)	return FALSE;

	if ( as_hex ) {
		// шестнадцатиричный режим полного пароля
		TCHAR hex[] = TEXT("0123456789ABCDEF");
		for ( INT i = 0; i < NCKEY_SIZE; i++ ) {
			/*passw[2*i]	= hex[key[i] & 0x0F];
			passw[2*i+1]= hex[(key[i]>>4) & 0x0F];*/
      passw[2*i+1]	= hex[key[i] & 0x0F];
      passw[2*i]= hex[(key[i]>>4) & 0x0F];
		}
		passw[2*NCKEY_SIZE] = NULL;
	} else {
		//режим буквенно-цифрового пароля
		//(нулевой символ байт пароля соотв. расширяется в символ окончания строки)
		for ( INT i = 0; i < NCKEY_SIZE; i++ ){
			if ( key[i] >= 128 ) 
				passw[i] = (WORD)(key[i] + 1024 - 128); // кодировка для рус. букв
			else
				passw[i] = (WORD)key[i];				// кодировка для латиницы
		}
		passw[NCKEY_SIZE] = NULL;
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//назначение: преобразование пароля в ключ
//параметры:  key	 - [in]  ключ
//			  passw	 - [out] пароль в виде строки
//			  as_hex - [in]  флаг, определяющий, что пароль задан в виде 16-ых чисел
//возвращает: TRUE, если всё нормально, иначе возвращает FALSE
BNC32_API BOOL BncPasswToKey (
							  NCKEY key,
							  const TCHAR passw[],
							  BOOL as_hex
							 )
{
	if (!key || !passw)	return FALSE;
	
	ZeroMemory(key,sizeof(NCKEY));
	
	if (as_hex){
		// шестнадцатиричный режим полного пароля
		BYTE val;
		for (INT i=0; i<2*NCKEY_SIZE; i++) {
			 if (passw[i] >= TEXT('0') && passw[i] <= TEXT('9'))
				 val = (BYTE)(passw[i] - TEXT('0'));
			 else if (passw[i] >= TEXT('A') && passw[i] <= TEXT('F'))
				 val = (BYTE)(passw[i] - TEXT('A') + 10);
			 else if (passw[i] >= TEXT('a') && passw[i] <= TEXT('f'))
				 val = (BYTE)(passw[i] - TEXT('a') + 10);
			 else return FALSE;

			if (i & 0x01)	key[i>>1] |= (val<<4);
			else			key[i>>1] = val;
		}
	} else {
		// режим буквенно-цифрового пароля
		// (нулевой символ автоматически прекращает перекодировку)
		for (INT i=0; i<NCKEY_SIZE; i++){
			if (passw[i] == 0) return TRUE;
			if (passw[i]>=128) key[i] = (BYTE)(passw[i] - 1024 + 128);
			else 			   key[i] = (BYTE)(passw[i]);
		}
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//назначение: генерация псевдослучайного ключа
//параметры:  key	 - [out] ключ
//			  length - [in] длина ключа (ост. байты обнуляются)
//			  mode	 - [in] набор флагов, определяющих режим генерации
//возвращает: код ошибки
BNC32_API HRESULT BncGenerateKey ( 
								  NCKEY key,
								  int length,
								  unsigned mode								  
								 )
{
	if (!key || length>NCKEY_SIZE){
		return ERROR_BAD_ARGUMENTS;
	}

	ZeroMemory(key,sizeof(NCKEY));
	srand(GetTickCount());

	unsigned page_mode = mode & (NC_MODE_LATIN | NC_MODE_HEX);
	const char latin_table[]= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char hex_table[]	= "0123456789ABCDEF";

	for (int index=0; index<length; index++) 
	{
		switch (page_mode) {
		//-------------------------------------------------------------------//
		//КЛЮЧ ИЗ ЛАТИНСКИХ БУКВ И ЦИФР
		//-------------------------------------------------------------------//
		case NC_MODE_LATIN:		
			key[index] = latin_table[rand()%62];
			break;
		//-------------------------------------------------------------------//
		//КЛЮЧ ИЗ ЧИСЕЛ В 16-Й С.С.
		//-------------------------------------------------------------------//
		case NC_MODE_HEX:		
			key[index] = hex_table[rand()%16];
			break;
		//-------------------------------------------------------------------//
		//КЛЮЧ ИЗ ЧИСЕЛ [1..254]
		//-------------------------------------------------------------------//
		case NC_MODE_DEFAULT:	
			{
				while (key[index]==0 || key[index]==0xff)
					key[index] = rand() % 256;
			}
			break;
		}
	}
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//назначение: увеличение размера обучающего ключа до размера контейнера
//параметры:  key	    - [in]  исходный обучающий ключ
//			  pbDestKey - [out] полученный ключ
//			  dwKeyLen	- [in]  длина выходного ключа
//возвращает: код ошибки
BNC32_API HRESULT BncResizeTrainKey ( 
									 NCKEY key,
									 BYTE *pbDestKey,
									 DWORD dwKeyLen )
{
	if (!key || !pbDestKey || !dwKeyLen){
		return ERROR_BAD_ARGUMENTS;
	}
	
	ZeroMemory(pbDestKey, dwKeyLen);

	// Размер увеличиваем путём дописывания хеша от предыдущей части ключа
	NCHASH hash;		// хеш
	DWORD dwLen;		// длина заполненной части ключа
	UINT   iStep;		// шаг: количество дописываемых элементов
	dwLen = NCKEY_SIZE;
	memcpy(pbDestKey, key, dwLen); // записываем обуч ключ в начало длинного ключа
	iStep = NCHASH_LEN;			   // дописываем оставшуюся чась хешами	
	while ( dwLen < dwKeyLen )	   // пока обучающий ключ не станет равным контейнеру
	{
		BncCreateHash ( pbDestKey, dwLen, hash ); // вычисляем хеш полученного ключа
		memcpy(pbDestKey+dwLen, hash, iStep);	  // дописываем хеш в конец
		dwLen += iStep;							  // полученная длина ключа
		if ( dwKeyLen - dwLen < iStep )			  // чтобы не уйти за границы ключа
			iStep = dwKeyLen - dwLen;
	}

	return ERROR_SUCCESS;
}
