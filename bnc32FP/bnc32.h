//	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ

//файл:		bnc32.h (bnc - биометрико-нейросетевой контейнер/хранитель паролей)
//описание: объявление экспортируемых элементов.
//			Биометрико-нейросетевой контейнер осуществляет:
//          обучение нейронной сети - Ключ+Подписи=Сеть;
//			извлечение ключа из контейнера - Сеть+Подпись=Ключ;
//			тестирования обученной нейронной сети



//	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ

#ifndef _BNC32_H
#define _BNC32_H

#include <windows.h>
#include <wincrypt.h>
#include <stdlib.h>

//==================================================================================//
//						ОПИСАНИЕ ИСПОЛЬЗУЕМЫХ КОНСТАНТ		                        //
//==================================================================================//
#define NCMAXCOORD_QUAN		5000		// максимальное количество координат
#define NCSCALE_SIZE		  512			// длина преобразованной строки координат
#define NCFOURIER_COUNT		416			// количество коэффициентов Фурье

#define NEURON_COUNT		256					// количество нейронов
#define NCKEY_BITS			NEURON_COUNT		// длина ключа в битах
#define NCKEY_SIZE			(NCKEY_BITS/8)		//длина ключа в байтах

#define NC_L1_WEIGHT		24					// кол. связей нейрона 1-го слоя
#define NC_L2_WEIGHT		0					// кол. связей нейрона 2-го слоя
#define NC_L1_COUNT			(NC_L1_WEIGHT*NEURON_COUNT)	// кол. связей 1го слоя
#define NC_L2_COUNT			(NC_L2_WEIGHT*NEURON_COUNT)	// кол. связей 2го слоя
#define NCWEIGHTS_COUNT		(NC_L1_COUNT + NC_L2_COUNT)	// размер таблицы весов
#define NCOFFSET_COUNT		NEURON_COUNT		// размер таблицы смещений 
//(смещение пороговой функции активации относительно нуля для нейронов выходного слоя)

// Выбранный режим
#define NCTRAIN_MODE		0		// режим обучения	
#define NCTEST_MODE			1		// режим тестирования
// Режим работы окна тестирования
#define NCTEST_MODE_TIWN	  0		// обычный режим тестирования на тестовых образах или на белом шуме
#define NCTEST_MODE_MORFING 1		// тестирование с использованием морфинга (выбираем образы и размножаем)

#define NCHASH_MD5_BITS		128
#define NCHASH_MD5_SIZE		(NCHASH_MD5_BITS/8)
#define NCHASH_LEN			  NCHASH_MD5_SIZE		//число элементов в хеше (длина хеша)
#define NC_MAX_PASSW_SIZE	(NCKEY_SIZE*2+1)	//макс. длина текста пароля в 16-чной форме

#define NCIMAGE_COUNT		3000		// максимальное количество обучающих образов
//==================================================================================//

//==================================================================================//
//						        ТИПЫ ДАННЫХ				                            //
//==================================================================================//
// Образ рукописного ввода
struct BNCSIGNIMAGE {
	UINT	m_Num;				// количество точек (координат X/Y/P/T)
	PUINT	m_X;				// указатель на массив X координат образа
	PUINT	m_Y;				// указатель на массив Y координат образа
	PUINT	m_P;				// указатель на массив значений давления
	PUINT	m_T;				// указатель на массив отсчётов времени
	UINT	m_MaxX;				// максимальное значение координаты X
	UINT	m_MaxY;				// максимальное значение координаты Y
	UINT	m_MaxP;				// максимальное значение давления 
};

// Статистические моменты какого-либо распределения
struct BNCMOMENTS {
	FLOAT faver;				// математическое ожидание
	FLOAT fdisp;				// дисперсия				 
	FLOAT fqual;				// качество
	INT  X[NCIMAGE_COUNT];		// меры Хемминга
	POINT p01[NEURON_COUNT];	// в данный массив записываем количество 
								// появлений 0 (p01[i].x) и 1 (p01[i].y) для i-го нейрона
	BYTE tmpKey[210*NEURON_COUNT];//временный массив для ключей 21 образа 
								//(используется при тестировании УДАЛИТЬ после 3-х экспериментов!!!)
};

// Ключ защиты
typedef BYTE	NCKEY[NCKEY_SIZE];
// Целочисленный ключ 
typedef int		NCINTKEY[NCKEY_BITS];
// Таблица весов используемой искусственной нейронной сети (веса + смещения)
typedef float	NCWEIGHTS[NCWEIGHTS_COUNT+NCOFFSET_COUNT];
// Массив коэффициентов Фурье
typedef float	NCFOURIER[NCFOURIER_COUNT];
// Хеш-функция, вычисляемая на основе ключа
typedef BYTE	NCHASH[NCHASH_LEN];
// Имя пользователя (логин)
typedef WCHAR	NCLOGIN[21];
// Данные одной сессии обучения/тестирования
typedef struct sessionInfo_t {
  NCLOGIN userName;   // имя пользователя
  int     keyLen;     // длина ключа
  NCKEY   key;        // ключ
} sessionInfo;

//==================================================================================//

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef BNC32_EXPORTS
#define BNC32_API __declspec(dllexport)
#else
#define BNC32_API __declspec(dllimport)
#endif	//end BNC32_EXPORTS

//==================================================================================//
//							ЭКСПОРТИРУЕМЫЕ  ФУНКЦИИ		                            //
//==================================================================================//

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
									UINT  *group );
//функция с сохранением коэффициентов
BNC32_API HRESULT BncNeuroNetTrain_2 (
                                    const WCHAR path[],
                                    const NCKEY key, 
                                    const NCKEY mixKey,
                                    UINT  count,
                                    const BNCSIGNIMAGE imgs[],
                                    NCWEIGHTS weights,
                                    UINT  *group );



//назначение: получение ключа из нейронной сети
//описание:   преобразование введённого пользователем  
//			  рукописного образа в ключ
//параметры:  img     - [in]  рукописный образ/подпись пользователя
//			  weights - [in]  таблица весовых коэффициентов обученной сети
//			  mixKey  - [in]  перемешивающий ключ
//			  key     - [out] получаемый ключ
//возвращает: количество выходов первого слоя, попавших в указанный интервал
//			  используется для разделения СВОИХ и ЧУЖИХ
BNC32_API INT BncExtractKey (
							 const BNCSIGNIMAGE img,
							 const NCWEIGHTS weights,
							 const NCKEY mixKey,
							 NCKEY key );
//функция с сохранением коэффициентов
BNC32_API INT BncExtractKey_2 (
                             const WCHAR path[],
                             const BNCSIGNIMAGE img,
                             const NCWEIGHTS weights,
                             const NCKEY mixKey,
                             NCKEY key );

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
									NCWEIGHTS weights );

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
									   NCWEIGHTS weights );

//назначение: вычисление коэффициентов Фурье входных образов
//описание:	  используется только на этапе исследований и экспериментов
//параметры:  imgs  - [in] исходные рукописные образы
//			  count - [in] количество рукописных образов
//			  bioParam - [out] вычисленные параметры рукописных образов (коэф-ты Фурье)
BNC32_API void BncGetImgParams (
							   const BNCSIGNIMAGE imgs[],
							   UINT  count, 
							   FLOAT bioParam[] );

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
							 NCKEY key ); 

//назначение: Сравнение ключей и подсчет количества отличающихся битов
//параметры:  first     - [in] первый ключ
//	          second    - [in] второй ключ
//возвращает: количество отличий между ключами (0, если ключи равны)
BNC32_API INT BncCompareKeys ( const NCKEY first, const NCKEY second );

//назначение: получение двоичного ключа
//описание:   преобразование строкового ключа NCKEY 
//			  в двоичный ключ типа INT
//параметры:  key	 - [in]  строка с ключём
//			  keyArr - [out] двоичный ключ (одномерный массив, каждая ячейка
//							 которого является элементом двоичного ключа)
//возвращает: код ошибки
BNC32_API HRESULT BncConvertKey ( const NCKEY key, INT keyArr[] );

//назначение: хеширование входных данных
//параметры:  pbData    - [in]  входные/хешируемые данные
//			  dwDataLen - [in]  размер буфера pbData
//			  hash	    - [out] хеш
//возвращает: код ошибки
BNC32_API HRESULT BncCreateHash ( const BYTE *pbData, DWORD dwDataLen, NCHASH hash );

//назначение: сравнение хешей
//параметры:  first	 - [in] первый хеш
//			  second - [in] второй хеш
//возвращает: TRUE, если хеши равны; FALSE, если хеши не равны
BNC32_API BOOL BncCompareHash( const NCHASH first, const NCHASH second );

//назначение: преобразование ключа в пароль
//параметры:  key	 - [in]  ключ
//			  passw  - [out] пароль в виде строки
//			  as_hex - [in]  флаг вывода в 16-м режиме (в этом случае 
//						     размер буфера должен быть 65 символов)
//возвращает: TRUE, если всё нормально, иначе возвращает FALSE
BNC32_API BOOL BncKeyToPassw (
							 const NCKEY key,
							 TCHAR passw[NC_MAX_PASSW_SIZE],
							 BOOL as_hex );

//назначение: преобразование пароля в ключ
//параметры:  key	 - [in]  ключ
//			  passw	 - [out] пароль в виде строки
//			  as_hex - [in]  флаг, определяющий, что пароль задан в виде 16-ых чисел
//возвращает: TRUE, если всё нормально, иначе возвращает FALSE
BNC32_API BOOL BncPasswToKey ( 
							  NCKEY key,
							  const TCHAR passw[],
							  BOOL as_hex );

//Режимы генерации ключа
#define NC_MODE_DEFAULT	0x0000	//по умолчанию (используется диапазон 1..254)
#define NC_MODE_LATIN	0x0001	//исп. латинские буквы и цифры от 0 до 10
#define NC_MODE_HEX		0x0002	//цифры и буквы, исп. в 16-й с.с.

//назначение: генерация псевдослучайного ключа
//параметры:  key	 - [out] ключ
//			  length - [in] длина ключа (ост. байты обнуляются)
//			  mode	 - [in] набор флагов, определяющих режим генерации
//возвращает: код ошибки
BNC32_API HRESULT BncGenerateKey ( 
								  NCKEY key,
								  INT length = NCKEY_SIZE,
								  UINT mode = NC_MODE_DEFAULT );

//назначение: увеличение размера обучающего ключа до размера контейнера
//параметры:  key	    - [in]  исходный обучающий ключ
//			  pbDestKey - [out] полученный ключ
//			  dwKeyLen	- [in]  длина выходного ключа
//возвращает: код ошибки
BNC32_API HRESULT BncResizeTrainKey ( 
									 NCKEY key,
									 BYTE *pbDestKey,
									 DWORD dwKeyLen );
//==================================================================================//

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_BIONEUROCONT_H
