//файл:			NetTesting.h
//описание:		модуль, осуществляющий тестирование нейронной сети

#ifndef _NETTEST_H
#define _NETTEST_H

#include "bnc32.h"

// Протестировать обученную сеть на белом шуме
// count   - [in] количество тестовых образов
// key     - [in] обучающий ключ
// mixKey  - [in] перемешивающий ключ
// weights - [in] вектор весов
// params - [out] матожидание, дисперсия и качество мер Хемминга
void NetTestOnWN (  UINT count, const NCKEY key, const NCKEY mixKey,
					const NCWEIGHTS weights, BNCMOMENTS &params );

// Протестировать обученную сеть на тестовом множестве
// count   - [in] количество тестовых образов
// key     - [in] обучающий ключ
// mixKey  - [in] перемешивающий ключ
// weights - [in] вектор весов
// coefs   - [in] вектор входных коэффициентов (imageCount*NCFOURIER_COUNT)
// params - [out] матожидание, дисперсия и качество мер Хемминга
void NetTestOnTF (  UINT count, const NCKEY key, const NCKEY mixKey,
					const NCWEIGHTS weights, const float coefs[], 
					BNCMOMENTS &params );

// Определить группу стойкости пользователя
// key     - [in] обучающий ключ
// mixKey  - [in] перемешивающий ключ
// weights - [in] вектор весов
//         - [out]номер группы к которой относится пользователь
int CalculateStabilityGroup ( const NCKEY key, const NCKEY mixKey, 
							  const NCWEIGHTS weights );

#endif