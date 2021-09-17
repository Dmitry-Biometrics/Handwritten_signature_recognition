//file:		ncmath.cpp
//descr:	�������������� ������ ���
//			�������� ��������� �����

#include "ncmath.h"

////////////////////////////////////////////////////////////////////////////////
// ������� ���������� �����, ��������� ������� ����� �� �����
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

//����������: ���������� ����������
//��������:   ���������� ���������� ����� ����� �������� ���������
//���������:  count - [in]  ���������� ��������� ������� coefs
//			  coefs - [in]  ������������� ������ (�������� ������ ������� �� ���� �������� �������)
// ���������� ����������� ���������� ���� ��������
float CalcCorr ( unsigned count, const float coefs1[], const float coefs2[] )
{
	float fAver1 = 0.0f;
	float fAver2 = 0.0f;
	float fDisp1 = 0.0f;
	float fDisp2 = 0.0f;

	// ���������� �����������
	for ( UINT i = 0; i < count; i++ ) {
		fAver1 += coefs1[i];
		fAver2 += coefs2[i];
	}
	fAver1 /= count;
	fAver2 /= count;

	// ���������� ���������
	for ( UINT i = 0; i < count; i++ ) {
		fDisp1 += pow(coefs1[i] - fAver1, 2);
		fDisp2 += pow(coefs2[i] - fAver2, 2);
	}
	fDisp1 = sqrt(fDisp1/count);
	fDisp2 = sqrt(fDisp2/count);

	// ���������� ����������
	float fCorrel = 0.0f;
	for ( UINT i = 0; i < count; i++ ) {
		fCorrel += ( (fAver1 - coefs1[i])*(fAver2 - coefs2[i]) ) / 
			( fDisp1 * fDisp2 );
	}
	fCorrel /= count;

	return fCorrel;
}

// ������ ����������� ������������� ������������ ��������� ������� � �������� 
// �� ������� �������� ����������� ���� (� ����������� 10 ���������: 0 - 0,1, ... 0,9 - 1)
// probArr	- [in]	������������� ������������ ��������� (i-� ������� - ����������� 
//					��������� ������ �� i-� �������)
// hist		- [out]	� i-� ������ ������������ ����� ������/�������� i-�� ������� 
void PlotHist ( float probArr[], int hist[] )
{
	// ���������� ������� �������� � ����� ������ ������
//	int hist[NEURON_COUNT];	// � i-� ������ ������������ ����� ������/�������� i-�� ������� 
	int colC0 = 0, colC1 = 0, colC2 = 0, colC3 = 0, colC4 = 0,	// �������� ���������� 
		colC5 = 0, colC6 = 0, colC7 = 0, colC8 = 0, colC9 = 0;	// ��������� � ��������
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
// ��������� ��������� � ������ �������
float Quality2Stab ( float fqual )
{
	return 0.3151f*pow(fqual,1.8686f);
}

////////////////////////////////////////////////////////////////////////////////
//������ ���� � �������� �������
//���������: source	- [in] ��������� �� ������� ������
//			 index	- [in] �������� � ����� �� ������ ������
//���������: �������� ����
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
//��������� ���� � �������� �������
//���������: source - [in] ��������� �� ������� ������
//			 index	- [in] �������� � ����� �� ������ ������
//			 bit	- [in] ��������������� �������� ���� (1 ��� 0)
void setbit(void* source, int index, int bit)
{
	int shift = index % 8;
	char *p    = (char*)source + (index >> 3);
	
	char mask  = 1<<shift;
	char value = (char)bit << shift;
	*p = (*p & ~mask) | value;
}

//������� ���������� ������������ �����
unsigned getbitdiffs(const void *src1, const void *src2, unsigned count)
{
	unsigned counter=0;
	for (unsigned i=0; i<count;i++){
		counter += (getbit(src1,i) != getbit(src2,i));
	}
	return counter;
}

////////////////////////////////////////////////////////////////////////////////
// ���������� ��������������� ��������
// count - [in]  ���������� ������� � ������� coefs
// step  - [in]  ���������� ������������� � ����� �������
// coefs - [in]  ���������� ������ ��������������� ���������� �������������
// aver  - [out] �������������� ��������
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
// ���������� ���������
// disp - [out] ���������
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
// ���������� ��������
// count - ���������� ��������� � ������� aver � disp 
void CalcQuality ( unsigned count, float aver[], float disp[], float qual[] )
{
	for( unsigned i = 0; i < count; i++ ) {
		qual[i] = fabs(aver[i])/disp[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
//���������� ��������������� ��������, ��������� � �������� ��� ������ �������. �����
//����������: ���������� ��������������� ��������, ��������� � �������� 
//			  ��� ������ �������. �����
//��������:   ���������� �������� ������������� �������/�������� ������ ����
//			  ��� count �������� � 1 ���������� (� ����� ������� 1 �������)
//���������:  count - [in]  ���������� ��������� ������� coefs
//			  coefs - [in]  ���������� ������ ��������� �� ������� ����������� ����������� � ��������
//			  params - [out] ����������� �����������, ��������� � ��������
void fadq ( unsigned count, const float coefs[], BNCMOMENTS & params )
{
	if (!coefs) return;

	float averArr[2], dispArr[2], qualArr[2];	

	CalcAverage    (count, 1, coefs, averArr);
	CalcDispersion (count, 1, coefs, averArr, dispArr);
	CalcQuality	   (1, averArr, dispArr, qualArr);
	
	params.faver = averArr[0];	// �����������
	params.fdisp = dispArr[0];	// ���������
	params.fqual = qualArr[0];	// ��������
}

//����������: ���������� ��������������� ��������, ��������� � �������� 
//			  ��� ������ ����� �����
//��������:   ���������� ���.��������, ��������� � �������� ��� ��������
//���������:  count - [in]  ���������� ��������� ������� coefs
//			  coefs - [in]  ���������� ������ �������� ��� ��������
//			  params - [out] ����������� �����������, ��������� � ��������
void fadq ( unsigned count, const int coefs[], BNCMOMENTS &params )
{
	if (!coefs) return;
	unsigned i;
	// ���������� ���. ��������
	params.faver = 0.0f;
	for ( i = 0; i < count; i++ ) {
		params.faver += coefs[i];
	}
	params.faver /= count;
	// ���������� ���������
	params.fdisp = 0.0f;
	for ( i = 0; i < count; i++ ) {
		params.fdisp += pow(coefs[i] - params.faver, 2);
	}
	params.fdisp /= count;
	params.fdisp = sqrt(params.fdisp);
	// ���������� ��������
	params.fqual = fabs(params.faver)/params.fdisp;
}

//����������: ���������� ���.��������, ��������� � �������� �� �����/������
//��������:	  ���������� ��������������� ��������, ��������� � �������� ��� 
//			  ���������� ������� ������������ �����, ������������� 
//			  ��������������� � ������� coefs.
//			  ���������� ������������� i-�� �������� ���������� �� ������ i-�
//			  ��������� �������
//����������:
//			������� �������� average, disp, quality ������ ���� �� ������ shift
//���������:  count - [in]  ���������� ������� (���������� �������)
//			  step  - [in]  �������� �� ������ ��������� ������� (������ �������) (���������� ������������� � ����� ������)
//			  coefs - [in]  ������ ��������� (������� ������������ ������ count � step)
//			  aver  - [out] ���. �������� \
//			  disp  - [out] ���������	  -- ������� ������������
//			  qual  - [out] ��������	  /
void fadq ( unsigned count, unsigned step, const float coefs[], float aver[], float disp[], float qual[] )
{
	if (!coefs || !aver || !disp || !count) return;

	CalcAverage    ( count, step, coefs, aver );		// ���������� ���. ��������
	CalcDispersion ( count, step, coefs, aver, disp );	// ���������� ���������
  if (!qual) return;
	CalcQuality    ( step, aver, disp, qual );			// ���������� ��������
}

//////////////////////////////////////////////////////////////////////////
// ���������� �������� �������� ��������� �������� �������
// count - [in] ���������� ��������� ������� �
// A	 - [in] ������������� ���������
float mean ( unsigned count, float A[] )
{
	float aver = 0.0f;
	for ( unsigned i = 0; i < count; i++ ) {
		aver += A[i];
	}
	aver /= count;
	return aver;
}

