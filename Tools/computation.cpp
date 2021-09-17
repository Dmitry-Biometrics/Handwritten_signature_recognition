// Математический модуль 

#include "computation.h"
#include <math.h>

//назначение: Вычисление математического ожидания, дисперсии и качества 
//			  для набора целых чисел
//описание:   Вычисление мат.ожидания, дисперсии и качества мер Хемминга
//параметры:  count - [in]  количество элементов массива coefs
//			  coefs - [in]  одномерный массив значений мер Хемминга
//			  params - [out] вычисленные матожидание, дисперсия и качество
void fadq ( unsigned count, const int coefs[], BNCMOMENTS &params )
{
	if (!coefs) return;
	unsigned i;
	// Вычисление мат. ожидания
	params.faver = 0.0f;
	for ( i = 0; i < count; i++ ) {
		params.faver += coefs[i];
	}
	params.faver /= count;
	// Вычисление дисперсии
	params.fdisp = 0.0f;
	for ( i = 0; i < count; i++ ) {
		params.fdisp += pow(coefs[i] - params.faver, 2);
	}
	params.fdisp /= count;
	params.fdisp = sqrt(params.fdisp);
	// Вычисление качества
	params.fqual = fabs(params.faver)/params.fdisp;
}

////////////////////////////////////////////////////////////////////////////////
// Вычисление математического ожидания
// count - [in]  количество выборок в массиве coefs
// step  - [in]  количество коэффициентов в одной выборке
// coefs - [in]  одномерный массив последовательно записанных коэффициентов
// aver  - [out] математическое ожидание
void CalcAverage ( unsigned count, unsigned step, const float coefs[], float aver[] )
{
	for ( unsigned i = 0; i < step; i++ ) {
		aver[i] = 0.0f;
		for ( unsigned j = 0; j < count; j++ ) {
			aver[i] += coefs[i + j*step];
		}
		aver[i] /= count;
	}
}

// Вычисление дисперсии
// disp - [out] дисперсия
void CalcDispersion (
					 unsigned count, unsigned step, const float coefs[], 
					 float aver[], float disp[] )
{
	for ( unsigned i = 0; i < step; i++ ) {
		disp[i] = 0.0f;
		for ( unsigned j = 0; j < count; j++ ) {
			disp[i] += 
				pow(coefs[i + j*step] - aver[i], 2);
		}
		disp[i] /= count;
		disp[i] = sqrt(disp[i]);
	}
}
// Вычисление качества
// count - количество элементов в выборке aver и disp 
void CalcQuality ( unsigned count, float aver[], float disp[], float qual[] )
{
	for( unsigned i = 0; i < count; i++ ) {
		qual[i] = fabs(aver[i])/disp[i];
	}
}
//назначение: Вычисление мат.ожидания, дисперсии и качества на входе/выходе
//описание:	  Вычисление математического ожидания, дисперсии и качества для 
//			  нескольких выборок вещественных чисел, расположенных 
//			  последовательно в массиве coefs.
//			  Вычисление характеристик i-го элемента проводится на основе i-х
//			  элементов выборок
//примечание:
//			размеры массивов average, disp, quality должен быть не меньше shift
//параметры:  count - [in]  количество выборок (количество образов)
//			  step  - [in]  смещение до каждой следующей выборки (размер выборки) (количество коэффициентов в одном образе)
//			  coefs - [in]  массив элементов (входные коэффициенты размер count Х step)
//			  aver  - [out] мат. ожидание \
//			  disp  - [out] дисперсия	  -- каждого коэффициента
//			  qual  - [out] качество	  /
void fadq ( unsigned count, unsigned step, const float coefs[], float aver[], float disp[], float qual[] )
{
	if (!coefs || !aver || !disp || !qual || !count) return;

	CalcAverage    ( count, step, coefs, aver );		// вычисление мат. ожидания
	CalcDispersion ( count, step, coefs, aver, disp );	// вычисление дисперсии
	CalcQuality    ( step, aver, disp, qual );			// вычисление качества
}

// Вычисляем минимальное значение массива
// возвращает минимальное значение
int fminElem ( unsigned count, const int coefs[] )
{
	int fmin = coefs[0]*100;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( fmin > coefs[i] ) fmin = coefs[i];
	}
	return fmin;
}

float fminElem ( unsigned count, const float coefs[] )
{
	float fmin = coefs[0]*100.0f;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( fmin > coefs[i] ) fmin = coefs[i];
	}
	return fmin;
}

// Вычисляем максимальное значение массива
// возвращает максимальное значение
int fmaxElem ( unsigned count, const int coefs[] )
{
	int fmax = 0;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( fmax < coefs[i] ) fmax = coefs[i];
	}
	return fmax;
}

// Вычисляем математическое ожидание массива
// возвращает среднее значение
float faverElem( unsigned count, const float coefs[] )
{
	float faver = 0.0f;
	for ( unsigned i = 0; i < count; i++ ) {
		faver += coefs[i];
	}
	faver /= count;
	return faver;
}

// Вычисляем количество "взломов" системы, т.е. кол-во мер == 0
// возвращает кол-во мер Хемминга равных нулю
int fzeroXem ( unsigned count, const int coefs[] )
{
	int zeroCount = 0;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( coefs[i] == 0 ) zeroCount++;
	}
	return zeroCount;
}

// Вычисляем стойкость к атакам подбора
// fqual - [in] вычисленное при моделировании качество мер Хемминга
// Возвращаемое значение Х следует интерпретировать следующим образом:
// стойкость системы к атакам подбора равна 10 в степени Х 
float Qual2Stab ( float fqual )
{
	return 0.87f+0.221f*pow(fqual,2);		// формула АИ (точнее чем моя)
}

// Функция округления чисел, округляет дробные числа до целых
int roundUp ( float x )
{
	float y;

	if ( x != 0 ) {
		y = x/fabs(x);
		y *=floor(fabs(x) + 0.5f);
	}
	else
		y = x; 

	return (int)y;
};

// Передаём управление виндовсу, чтобы он перерисовал окно
void DelegateFun()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}