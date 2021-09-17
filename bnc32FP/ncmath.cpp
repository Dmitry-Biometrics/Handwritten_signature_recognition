//file:		ncmath.cpp
//descr:	Математический модуль для
//			рассчета нейронных сетей

#include "ncmath.h"

////////////////////////////////////////////////////////////////////////////////
// Функция округления чисел, округляет дробные числа до целых
float roundUp ( float x )
{
	float y;

	if ( x != 0 ) {
		y = x/fabs(x);
		y *=floor(fabs(x) + 0.5f);
	}
	else
		y = x; 

	return y;
};

//назначение: Вычисление корреляции
//описание:   Вычисление корреляции между двумя входными векторами
//параметры:  count - [in]  количество элементов массива coefs
//			  coefs - [in]  анализируемый массив (например выходы нейрона на всех тестовых образах)
// возвращает вычисленную корреляцию двух векторов
float CalcCorr ( unsigned count, const float coefs1[], const float coefs2[] )
{
	float fAver1 = 0.0f;
	float fAver2 = 0.0f;
	float fDisp1 = 0.0f;
	float fDisp2 = 0.0f;

	// Вычисление матожидания
	for ( UINT i = 0; i < count; i++ ) {
		fAver1 += coefs1[i];
		fAver2 += coefs2[i];
	}
	fAver1 /= count;
	fAver2 /= count;

	// Вычисление дисперсии
	for ( UINT i = 0; i < count; i++ ) {
		fDisp1 += pow(coefs1[i] - fAver1, 2);
		fDisp2 += pow(coefs2[i] - fAver2, 2);
	}
	fDisp1 = sqrt(fDisp1/count);
	fDisp2 = sqrt(fDisp2/count);

	// Вычисление корреляции
	float fCorrel = 0.0f;
	for ( UINT i = 0; i < count; i++ ) {
		fCorrel += ( (fAver1 - coefs1[i])*(fAver2 - coefs2[i]) ) / 
			( fDisp1 * fDisp2 );
	}
	fCorrel /= count;

	return fCorrel;
}

// Строим гистограмму распределения вероятностей появления ноликов и единичек 
// на выходах нейронов однослойной сети (в гистограмме 10 столбиков: 0 - 0,1, ... 0,9 - 1)
// probArr	- [in]	распределение вероятностей появления (i-й элемент - вероятность 
//					появления нолика на i-м нейроне)
// hist		- [out]	в i-ю ячейку записывается номер группы/столбика i-го нейрона 
void PlotHist ( float probArr[], int hist[] )
{
	// Определяем сколько нейронов в какую группу попало
//	int hist[NEURON_COUNT];	// в i-ю ячейку записывается номер группы/столбика i-го нейрона 
	int colC0 = 0, colC1 = 0, colC2 = 0, colC3 = 0, colC4 = 0,	// счётчики количества 
		colC5 = 0, colC6 = 0, colC7 = 0, colC8 = 0, colC9 = 0;	// элементов в столбике
	for ( int i = 0; i < NEURON_COUNT; i++ ) 
	{
		if ( probArr[i] <= 0.1f ) { hist[i] = 0; colC0++; continue; }	// 0.0 - 0.1
		if ( probArr[i] <= 0.2f ) { hist[i] = 1; colC1++; continue; }	// 0.1 - 0.2
		if ( probArr[i] <= 0.3f ) { hist[i] = 2; colC2++; continue; }	// 0.2 - 0.3
		if ( probArr[i] <= 0.4f ) { hist[i] = 3; colC3++; continue; }	// 0.3 - 0.4
		if ( probArr[i] <= 0.5f ) { hist[i] = 4; colC4++; continue; }	// 0.4 - 0.5
		if ( probArr[i] <= 0.6f ) { hist[i] = 5; colC5++; continue; }	// 0.5 - 0.6
		if ( probArr[i] <= 0.7f ) { hist[i] = 6; colC6++; continue; }	// 0.6 - 0.7
		if ( probArr[i] <= 0.8f ) { hist[i] = 7; colC7++; continue; }	// 0.7 - 0.8
		if ( probArr[i] <= 0.9f ) { hist[i] = 8; colC8++; continue; }	// 0.8 - 0.9
		if ( probArr[i] <= 1.0f ) { hist[i] = 9; colC9++; continue; }	// 0.9 - 1.0
	}

}

////////////////////////////////////////////////////////////////////////////////
// Вычисляем стойкость к атакам подбора
float Quality2Stab ( float fqual )
{
	return 0.3151f*pow(fqual,1.8686f);
}

////////////////////////////////////////////////////////////////////////////////
//Чтение бита с заданным номером
//параметры: source	- [in] Указатель на область памяти
//			 index	- [in] Смещение в битах от начала памяти
//результат: Значение бита
int getbit(const void* source, int index)
{
	int shift = index % 8;
	char *p    = (char*)source + (index >> 3);
	char v = *p;
	char x = v >> shift; 
	x &= 1;
	return x;
}

////////////////////////////////////////////////////////////////////////////////
//Установка бита с заданным номером
//параметры: source - [in] Указатель на область памяти
//			 index	- [in] Смещение в битах от начала памяти
//			 bit	- [in] Устанавливаемое значение бита (1 или 0)
void setbit(void* source, int index, int bit)
{
	int shift = index % 8;
	char *p    = (char*)source + (index >> 3);
	
	char mask  = 1<<shift;
	char value = (char)bit << shift;
	*p = (*p & ~mask) | value;
}

//Подсчет количества отличающихся битов
unsigned getbitdiffs(const void *src1, const void *src2, unsigned count)
{
	unsigned counter=0;
	for (unsigned i=0; i<count;i++){
		counter += (getbit(src1,i) != getbit(src2,i));
	}
	return counter;
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

///////////////////////////////////////////////////////////////////////////////
//Вычисление математического ожидания, дисперсии и качества для набора веществ. чисел
//назначение: Вычисление математического ожидания, дисперсии и качества 
//			  для набора веществ. чисел
//описание:   Вычисление числовых характеристик входных/выходных данных сети
//			  для count векторов с 1 параметром (в одном векторе 1 элемент)
//параметры:  count - [in]  количество элементов массива coefs
//			  coefs - [in]  одномерный массив элементов на котором вычисляется матожидание и качество
//			  params - [out] вычисленные матожидание, дисперсия и качество
void fadq ( unsigned count, const float coefs[], BNCMOMENTS & params )
{
	if (!coefs) return;

	float averArr[2], dispArr[2], qualArr[2];	

	CalcAverage    (count, 1, coefs, averArr);
	CalcDispersion (count, 1, coefs, averArr, dispArr);
	CalcQuality	   (1, averArr, dispArr, qualArr);
	
	params.faver = averArr[0];	// матожидание
	params.fdisp = dispArr[0];	// дисперсия
	params.fqual = qualArr[0];	// качество
}

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
	if (!coefs || !aver || !disp || !count) return;

	CalcAverage    ( count, step, coefs, aver );		// вычисление мат. ожидания
	CalcDispersion ( count, step, coefs, aver, disp );	// вычисление дисперсии
  if (!qual) return;
	CalcQuality    ( step, aver, disp, qual );			// вычисление качества
}

//////////////////////////////////////////////////////////////////////////
// Вычисление среднего значения элементов входного массива
// count - [in] количество элементов массива А
// A	 - [in] анализируемое множество
float mean ( unsigned count, float A[] )
{
	float aver = 0.0f;
	for ( unsigned i = 0; i < count; i++ ) {
		aver += A[i];
	}
	aver /= count;
	return aver;
}

