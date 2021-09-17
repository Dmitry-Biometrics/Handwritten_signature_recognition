//файл:			NetSimulation.h
//описание:		модуль, моделирующий работу нейронной сети


#ifndef _NETSIM_H
#define _NETSIM_H

#include "bnc32.h"

extern const short ConnectionArray_Layer_1[];

//функция:		int ImageRecognition (...)
//назначение:	Проверка одного входного вектора на обученной сети 
//описание:		Моделирование сети на 1 образе. Возвращает количество 
//		        несовпадений с ключом
//параметры:	inputArr    - [in] входной вектор
//				weightsArr  - [in] веса нейронной сети
//				keyArr		- [in] ключ
//				mixerKey	- [in] перемешивающий ключ
//				checkKeyArr - [out]полученный ключ
//				возврашает	- [out]количество несовпадений с ключом	
int ImageRecognition (
				float *inputArr, float *weightsArr, 
				int *keyArr, int *mixerKey, int *checkKeyArr );

//функция:		int ImageSimulation (...)
//назначение:	Проверка одного входного вектора на обученной сети 
//описание:		Моделирование сети на 1 образе. Преобразование 
//				рукописного пароля в ключ
//параметры:	inputArr    - [in] входной вектор
//				weightsArr  - [in] веса нейронной сети
//				mixerKey	- [in] перемешивающий ключ
//				checkKeyArr - [out]полученный ключ
// возвращает количество выходов первого слоя, попавших в указанный интервал
// используется для разделения СВОИХ и ЧУЖИХ
int ImageSimulation (
				const float inputArr[], const NCWEIGHTS weights, 
				const NCKEY mixKey, NCKEY checkKey );

// Вычисление количества образов "Своего"
// возвращает количество образов, распознанных как "Свой"
int GroupSimulationForCompare (
				int imageCount, float *inputArr, 
				float *weightsArr, int *keyArr, int *mixerKey );

// Проверка тестовых примеров на одном нейроне первого или 2 слоя
// Возвращает количество несовпадений с ключом
// neuronNum	 - [in] номер моделируемого нейрона
// weightsCount  - [in] количество весов у данного нейрона
// ConnectionArr - [in] таблица связей
// imageCount	 - [in] количество тестовых образов
// keyBit		 - [in] значение обучающего ключа для данного нейрона (0 или 1)
// inputArr      - [in] тестовое множество (вход 1 или 2 слоя)
// weightsArr	 - [in] веса первого/второго слоя
//				 - [out]количество несовпадений с ключом
int NeuronRecognitionOnTest (
				int neuronNum,  int imageCount, 
				const int keyBit, float inputArr[], float weightsArr[],				
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1 );

// Проверка тестовых примеров на одном нейроне первого/второго слоя
// Возвращает выходное качество на данном нейроне на тестовых образах
// neuronNum	 - [in] номер моделируемого нейрона
// weightsCount  - [in] количество весов у данного нейрона
// ConnectionArr - [in] таблица связей
// imageCount	 - [in] количество тестовых образов
// inputArr      - [in] тестовое множество (вход 1 или 2 слоя)
// weightsArr	 - [in] веса первого/второго слоя сети
//				 - [out]выходное качество
float NeuronQualityOnTest (
				int neuronNum, int weightsCount, 
				const short *ConnectionArr, int imageCount, 
				float *inputArr, float *weightsArr );	

// Вычисление качества обучения одного нейрона
// neuronNum	- [in] номер моделируемого нейрона
// trnCount		- [in] количество образов Своего
// trnCoeff     - [in] коэффициенты Своего
// tstCount		- [in] количество образов Чужого
// tstCoeff		- [in] коэффициенты Чужого
// weightsArr	- [in] веса первого/второго слоя сети
// Возвращает качество обучения моделируемого нейрона: q = |a1-a2|/(d1+d2)
float NeuronTrainQuality (
							UINT neuronNum, 
							UINT trnCount, float trnCoeff[],
							UINT tstCount, float tstCoeff[],
							float weightsArr[],  
							const int weightsCount = NC_L1_WEIGHT,
							const short ConnectionArr[] = ConnectionArray_Layer_1 );	

// Вычисление выходов заданного нейрона на группе тестовых образов
// neuronNum	- [in]  номер моделируемого нейрона
// imageCount	- [in]  количество тестовых образов
// inputArr     - [in]  тестовое множество (вход 1 или 2 слоя)
// weightsArr	- [in]  веса первого/второго слоя сети
// outSum		- [out] выходы сумматора нейрона на всех тестовых примерах
// outNeuron	- [out] выходы рассматриваемого нейрона на всех примерах
void NeuronSimulation ( 
				const int neuronNum,  
				const int imageCount,
				float inputArr[], 
				const float weightsArr[], 
				float outSum[], 
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1,
				float outNeuron[] = NULL );

// Моделирование первого или второго слоя сети на группе образов 
// Вычисление выходных сумм
// weightsCount  - [in] количество весов 1 нейрона (т.е 1 или 2 слой)
// ConnectionArr - [in] таблица связей 1 или 2 слоя
// imageCount	 - [in] общее количество образов
// inputArr      - [in] коэффициенты всех образов
// weightsArr	 - [in] веса 1 или 2 слоя сети
// outputArr	 - [out]выходы сумматоров
void GroupLayerSimulation (
				int imageCount, float inputArr[], 
				float weightsArr[], float outputArr[],
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1 );

// Моделирование первого или второго слоя сети на группе образов 
// Вычисление вероятности появления нолика на выходе нейрона
// bUseShift	 - [in] использовать смещение функции активации нейрона (TRUE) или нет
// imageCount	 - [in] общее количество образов
// inputArr      - [in] коэффициенты всех образов
// weightsArr	 - [in] веса 1 или 2 слоя сети
// outputArr	 - [out]вероятность появления нолика для всех нейронов
// weightsCount  - [in] количество весов 1 нейрона (т.е 1 или 2 слой)
// ConnectionArr - [in] таблица связей 1 или 2 слоя
void LayerSimulationForProb (
							 bool bUseShift,
							 int imageCount, float inputArr[], 
							 const float weightsArr[], float outputArr[],
							 const int weightsCount = NC_L1_WEIGHT,
							 const short ConnectionArr[] = ConnectionArray_Layer_1 );

//функция:		void NetRecognition (...)
//назначение:	Проверка группы входных векторов на обученной двухслойной сети 
//описание:		Моделирование сети на тестовых образах. 
//				Возвращает массив с количеством несовпадений с ключом
//параметры:	imageCount - [in] количество образов
//			    inputArr   - [in] входной вектор
//				weightsArr - [in] веса сети
//				keyArr	   - [in] ключ
//				mixerKey   - [in] перемешивающий ключ
//				xemmingArr - [out]количество несовпадений с ключом	
void NetRecognition (
				UINT imageCount, const float inputArr[],
				const NCWEIGHTS weightsArr, 
				const NCKEY key, const NCKEY mixKey, 
				int xemmingArr[] );
// p01[] - [out] в данный массив записываем количество появлений 0 (p01[i].x)
//				 и 1 (p01[i].y) для i-го нейрона (p01[i].x + p01[i].y = imageCount)
// tmpKeys[] - [out] массив для ключей 21 тестового образа (21 т.к. в БТБ\База2006 в одном файле максимально 21 образа)
void NetRecognition_P (
					 UINT imageCount, float inputArr[],
					 const NCWEIGHTS weightsArr, 
					 const NCKEY key, const NCKEY mixKey, 
					 int xemmingArr[],
					 POINT p01[], BYTE tmpKeys[] );

// Проверка тестовых примеров на однослойной сети
// Моделирование первого слоя сети на группе тестовых примеров
// imageCount	- [in] количество моделируемых образов
// inputArr		- [in] коэффициенты всех образов
// weightsArr	- [in] веса однослойной сети
// key			- [in] обучающий ключ
//				- [out]количество несовпадений с ключем
int NetRecognitionOnTest (
						  UINT imageCount, float inputArr[], 
						  const NCWEIGHTS weightsArr, 
						  const NCKEY key );	

// Вычисление корреляции
void CalcHistCorr ( POINT p01[], UINT imageCount, float inputArr[], 
				    const float weightsArr[] );

// Вычисление корреляции выходов нейронов (каждый с каждым)
void CalcNetCorr ( UINT imageCount, float inputArr[], const float weightsArr[]);

// Вычисление средней корреляции для столбика
// count		- [in] количество нейронов в данном столбике
// numArr		- [in] вектор с номерами нейронов
// imageCount	- [in] количество тестовых образов
// inputArr		- [in] коэффициенты всех образов
// weightsArr	- [in] веса однослойной сети
// возвращает среднюю корреляцию для данного столбика
float MeanColCorr ( UINT count, int numArr[], 
				    UINT imageCount, float inputArr[], 
					const float weightsArr[] );

#endif