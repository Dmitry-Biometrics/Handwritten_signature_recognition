//файл:			NetTraining.cpp
//описание:		модуль обучения нейронной сети

#include "NetTraining.h"

#include "ncmath.h"
#include "NetSimulation.h"	

//////////////////////////////////////////////////////////////////////////
// Обучение нейрона с заданным качеством
// neuronNum		- [in] номер обучаемого нейрона
// weightsCount		- [in] количество весов данного нейрона
// ConnectionArr	- [in] таблица связей нейронов данного слоя
// imageCount		- [in] количество обучающих образов
// inputArr			- [in] коэффициенты обучающих образов
// tstCount			- [in] количество тестовых образов (все ЧУЖИЕ)
// testCoef			- [in] коэффициенты тестовых образов
// key				- [in] обучающий ключ
// weightsArr		- [in] весовые коэффициенты
// qualLimFrom		- [in]
// qualLimTo		- [in]
void NeuronTrainingA2 (
					   const int neuronNum,
					   int weightsCount, 
					   const short ConnectionArr[],
					   UINT imageCount, float inputArr[],
					   const NCKEY key, 
					   float weightsArr[], 
					   float qualLimFrom, float qualLimTo, float averArr[], float dispArr[], float qualArr[] )
{
	float outQuality = 0.0f;	// выходное качество нейрона
	int   znak;					// знак настраиваемого веса (+/-)
//	int	  cycleCount;			// количество попыток по снижению качества

	// получаем значение ключа для данного нейрона
	int keybit = getbit(key,neuronNum);	

	int weightNum = 0;			// номер веса рассматриваемого нейрона
	int errorCount = 10;		// количество неправильно распознанных образов

	// Пока не переберём все веса или пока не пройдут все обучающие образы
	while ( weightNum < weightsCount && outQuality < qualLimFrom  )
	{
		bool keyOne = (keybit == 1); 
		znak = ( keyOne == (averArr[ConnectionArr[neuronNum*weightsCount+weightNum]] > 0.0f) ) ? 1 : -1 ;

		weightsArr[neuronNum*weightsCount + weightNum] = 
								znak*
								qualArr[ConnectionArr[neuronNum*weightsCount+weightNum]]/ 
								dispArr[ConnectionArr[neuronNum*weightsCount+weightNum]];

		if ( weightNum == 0 ) { // т.е. у нейрона должны быть минимум 2 ненулевых веса
			weightNum++;
			znak = ( keyOne == (averArr[ConnectionArr[neuronNum*weightsCount+weightNum]] > 0.0f) ) ? 1 : -1 ;
			weightsArr[neuronNum*weightsCount + weightNum] = znak*qualArr[ConnectionArr[neuronNum*weightsCount+weightNum]]/dispArr[ConnectionArr[neuronNum*weightsCount+weightNum]];
		}

		// Вычисляем выходное качество нейрона на входных/обучающих образах
		outQuality = NeuronQualityOnTest ( neuronNum, weightsCount, ConnectionArr, imageCount, inputArr, weightsArr );
		weightNum++;
	}

	errorCount = NeuronRecognitionOnTest ( neuronNum, imageCount, keybit, inputArr, weightsArr, weightsCount, ConnectionArr );

	// Если выходное качество нейрона выше заданного и ещё остались
	// незаданные (т.е нулевые) веса, снижаем выходное качество добавлением шума
	if ( weightNum < weightsCount && outQuality > qualLimTo )
	{
		float fixQual = outQuality;
		int iter = 0;
		float *fixWArr = new float[weightsCount]; //наилучшие веса
		memcpy(fixWArr, weightsArr+neuronNum*weightsCount, weightsCount*sizeof(float));
		do 
		{
			for ( int i = weightNum; i < weightsCount; i++) {
				weightsArr[neuronNum*weightsCount + i] = (rand()%1000 - rand()%1000)/1000.f;
			}
			outQuality = NeuronQualityOnTest ( neuronNum, weightsCount, ConnectionArr, imageCount, inputArr, weightsArr );
			errorCount = NeuronRecognitionOnTest ( neuronNum, imageCount, keybit, inputArr, weightsArr, weightsCount, ConnectionArr );
			// фиксируем лучший вариант
			if ( errorCount == 0 && outQuality < fixQual && outQuality > qualLimFrom ) {
				fixQual = outQuality;
				for ( int i = weightNum; i < weightsCount; i++) { fixWArr[i] = weightsArr[neuronNum*weightsCount + i]; }
			}
			if (iter++ > 1000) { for ( int i = weightNum; i < weightsCount; i++) { weightsArr[neuronNum*weightsCount + i] = fixWArr[i]; } break; }
		} while(errorCount != 0 || outQuality < qualLimFrom || outQuality > qualLimTo);
		delete [] fixWArr;
	}


}

// Обучение первого слоя сети
// алгоритм обучения:
//	0. для всех нейронов первого слоя
//	1. все веса делаем равными 0 (обнуляем)
//	2. для всех весов
//  3. определяем знак веса по входному мат.ожиданию и по ключу
//	4. задаём значение веса по формуле W = Qвх/Dвх, где Qвх - входное качество (качество
//	   подаваемого на данный вход нейрона коэффициента), Dвх - входная дисперсия
//	5. вычисляем выходное качество нейрона
//	6. Если выходное качество > setQualLimFrom (т.е. заданного в алгоритме м.б. 4,5,6..),
//     переходим к шагу 7, оставшиеся веса не учитываем, они остаются равны 0.
//	   Иначе переходим на шаг 2 и выполняем все оперрации 2-4 для следующего веса
//	   Затем переходим к шагу 5 и 6 и так до тех пор пока мы не достигнем нужного 
//	   качества на выходе нейрона или пока не переберём все веса.
//	7. Если выходное качество больше заданного (4,5,6,...), сжимаем его методом
//	   добавления случайного шума: оставшиеся равными нулю веса заменяем на случайный
//     шум и вычисляем выходное качество нейрона. Добавляем шум до тех пор, пока
//     качество не попадёт в заданный интервал. 
//	8. Переходим к следующему нейрону
//параметры:	weightsCount  - [in]  количество весов одного нейрона
//				ConnectionArr - [in]  таблица связей данного слоя
//				imageCount    - [in]  количество обучающих образов
//				keyArr		  - [in]  ключ на котором будет обучаться сеть
//				inputArr	  - [in]  вектор входа
//			    weightsArr	  - [out] веса обученной сети
//				averageArr	  - [in]  математическое ожидание входных коэффициентов
//				dispersionArr - [in]  дисперсия входных коэффициентов
//				qualityArr    - [in]  качество входных коэффициентов
void LayerTrainingA2 (
					 int weightsCount, 
					 const short ConnectionArr[],
					 const NCKEY key, 
					 UINT imageCount, float inputArr[], 
					 float weightsArr[] )
{
	srand(::GetTickCount());

	// Обнуляем таблицу весовых коэффициентов
	ZeroMemory(weightsArr, NEURON_COUNT*weightsCount*sizeof(float));
	
	// Входные характеристики (мат ожидание, дисперсия и качество на входе сети)
	float *averArr = new NCFOURIER;
	float *dispArr = new NCFOURIER;
	float *qualArr = new NCFOURIER;

	// Вычисление входного мат. ожидания и входного качества на первом слое
	fadq ( imageCount, NCFOURIER_COUNT, inputArr, averArr, dispArr, qualArr );

	// Для всех нейронов первого слоя
	for ( int neuronNum = 0; neuronNum < NEURON_COUNT; neuronNum++ )
	{
		float qualLimFrom = 6.0f;	// нижняя и верхняя границы
		float qualLimTo   = 7.0f;	// выходного качества нейрона

		NeuronTrainingA2 ( neuronNum, weightsCount, ConnectionArr, 
						   imageCount, inputArr, key, weightsArr, qualLimFrom, qualLimTo, averArr, dispArr, qualArr);

	} // для всех нейронов

	delete [] averArr;
	delete [] dispArr;
	delete [] qualArr;
}

//////////////////////////////////////////////////////////////////////////
// Корректировка весов сети с учетом выходной дисперсии или выходного мат.ожидания
// weightsCount  - [in]     количество весов одного нейрона
// correctiveArr - [in]     корректирующий массив
// weightsArr    - [in/out] вектор весов
void CalculateModifiedWeights ( int weightsCount, float *correctiveArr, float *weightsArr )
{
	for ( int iCount = 0; iCount < NEURON_COUNT; iCount++ ) {
		for ( int jCount = 0; jCount < weightsCount; jCount++ ) {
			weightsArr[iCount*weightsCount+jCount] /= fabs(correctiveArr[iCount]); 
		}
	}
}


// Обучение первого слоя нейронной сети
// imageCount	- [in] количество обучающих образов
// coeffArr		- [in] коэффициенты обучающих образов
// keyArr	    - [in] обучающий ключ
// weightsArr   - [in/out] веса первого слоя (на входе - необученные веса, 
//						  на выходе - веса обученной сети
void FirstLayerTraining ( int imageCount, float coeffArr[], 
						  const NCKEY key, float weightsArr[] )
{
	// Массив под выходы всех сумматоров первого/второго слоя
	float *netSumArr = new float [NEURON_COUNT*imageCount];

	// НОВЫЙ АЛГОРИТМ ОБУЧЕНИЯ ПЕРВОГО СЛОЯ
	LayerTrainingA2 ( NC_L1_WEIGHT, ConnectionArray_Layer_1, key, 
					  imageCount, coeffArr, weightsArr );

	// Вычисление выходов сумматоров всех нейронов первого слоя
	GroupLayerSimulation ( imageCount, coeffArr, weightsArr, netSumArr );

	// Выходные характеристики (мат ожидание, дисперсия и качество
	// на входе первого слоя или всей сети)
	float *outAverArr = new float [NEURON_COUNT];
	float *outDispArr = new float [NEURON_COUNT];
	float *outQualArr = new float [NEURON_COUNT];

	// Вычисление выходных характеристик первого слоя
	fadq ( imageCount, NEURON_COUNT, netSumArr, outAverArr, outDispArr, outQualArr );

	// Корректировка весов первого слоя сети с учетом выходной дисперсии
	CalculateModifiedWeights ( NC_L1_WEIGHT, outDispArr, weightsArr );

	// Вычисление выходов всех нейронов первого слоя
	GroupLayerSimulation ( imageCount, coeffArr, weightsArr, netSumArr );

	// Вычисление выходных характеристик первого слоя
	fadq ( imageCount, NEURON_COUNT, netSumArr, outAverArr, outDispArr, outQualArr );

	// Корректировка весов первого слоя с учетом выходного мат ожидания (по модулю)
	CalculateModifiedWeights ( NC_L1_WEIGHT, outAverArr, weightsArr );

	delete [] outAverArr;
	delete [] outDispArr;
	delete [] outQualArr;
	delete [] netSumArr;

}

// Перемешиваем таблицу весов по определённому правилу
// описание: при перемешивании весов нейронной сети используем следующий алгоритм:
//				Сравниваем попарно все элементы обучающего (keyArr) и 
//				перемешивающего ключа (mixerKey). Если keyArr[i] == mixerKey[i],
//				все веса i-го нейрона оставляем без изменения, если же 
//				keyArr[i] != mixerKey[i], умножаем все васа нейрона i на -1
// keyArr     - [in]	обучающий ключ
// mixerKey   - [in]	перемешивающий ключ
// weightsArr - [in/out] веса обученной сети, на выходе получаем перемешанные веса
void ShufflePermutation ( const NCKEY key, const NCKEY mixKey, float weightsArr[] )
{
	for ( int i = 1; i < NEURON_COUNT; i++ )  {
		// получаем знак весов нейрона i, для этого 
		// сравниваем обучающий и перемешивающий ключи в позиции i-1,
		// и если они отличаются, то меняем знак всех весов нейрона i
		// на противоположный
		int znak = -2*(getbit(key,i-1)^getbit(mixKey,i-1))+1;
		
		for ( int j = 0; j < NC_L1_WEIGHT; j++ ) {
			weightsArr[i*NC_L1_WEIGHT+j] *= znak;
		}
	}
}

// Главная функция обучения однослойной нейронной сети
void NeuroNetTrain ( 
					UINT trCount, float trainCoefs[], 
					const NCKEY key, const NCKEY mixKey, 
					NCWEIGHTS weights )
{
//-----------------------------------------------------------------------------------//
//						ОБУЧЕНИЕ ПЕРВОГО СЛОЯ НЕЙРОННОЙ СЕТИ	                     //
	float firstWeightsArr[NC_L1_COUNT];	 // веса первого слоя сети
	FirstLayerTraining(trCount, trainCoefs, key, firstWeightsArr);
//					КОНЕЦ ОБУЧЕНИЯ ПЕРВОГО СЛОЯ НЕЙРОННОЙ СЕТИ	                     //
//-----------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------//
//									МЕШАЛКА						                     //
//	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	 //
//	Перемешиваем таблицу весов по определённому правилу								 //
//	ShufflePermutation(key, mixKey, firstWeightsArr);								 //
//	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	СЕТЬ БЕЗ МЕШАЛКИ	 //
//								КОНЕЦ МЕШАЛКИ					                     //
//-----------------------------------------------------------------------------------//
	// Записываем новые веса обученной сети
	memcpy ( weights, firstWeightsArr, NC_L1_COUNT*sizeof(float) );
}

