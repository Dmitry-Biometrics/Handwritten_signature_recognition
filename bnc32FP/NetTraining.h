//файл:			NetTraining.h
//описание:		модуль обучения нейронной сети

#ifndef _NETTRAIN_H
#define _NETTRAIN_H

#include "bnc32.h"

//назначение: Функция обучения нейронной сети
//описание:   при обучении сети используется новый подход(рассматривается 1 вход и 
//			  выходное качество, вых. кач. каждого нейрона д.б. не менее 3)
//параметры:  
//	trCount		- [in]  количество обучающих образов
//	trainCoefs	- [in]  одномерный массив коэффициентов Фурье всех обучающих образов 
//	key			- [in]  обучающий ключ
//	mixKey		- [in]  перемешивающий ключ
//	weights		- [out] веса обученной сети
void NeuroNetTrain ( 
					UINT trCount, float trainCoefs[], 
					const NCKEY key, const NCKEY mixKey, 
					NCWEIGHTS weights );

#endif