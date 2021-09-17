// �������������� ������ 

#include "computation.h"
#include <math.h>

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
	if (!coefs || !aver || !disp || !qual || !count) return;

	CalcAverage    ( count, step, coefs, aver );		// ���������� ���. ��������
	CalcDispersion ( count, step, coefs, aver, disp );	// ���������� ���������
	CalcQuality    ( step, aver, disp, qual );			// ���������� ��������
}

// ��������� ����������� �������� �������
// ���������� ����������� ��������
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

// ��������� ������������ �������� �������
// ���������� ������������ ��������
int fmaxElem ( unsigned count, const int coefs[] )
{
	int fmax = 0;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( fmax < coefs[i] ) fmax = coefs[i];
	}
	return fmax;
}

// ��������� �������������� �������� �������
// ���������� ������� ��������
float faverElem( unsigned count, const float coefs[] )
{
	float faver = 0.0f;
	for ( unsigned i = 0; i < count; i++ ) {
		faver += coefs[i];
	}
	faver /= count;
	return faver;
}

// ��������� ���������� "�������" �������, �.�. ���-�� ��� == 0
// ���������� ���-�� ��� �������� ������ ����
int fzeroXem ( unsigned count, const int coefs[] )
{
	int zeroCount = 0;
	for ( unsigned i = 0; i < count; i++ ) {
		if ( coefs[i] == 0 ) zeroCount++;
	}
	return zeroCount;
}

// ��������� ��������� � ������ �������
// fqual - [in] ����������� ��� ������������� �������� ��� ��������
// ������������ �������� � ������� ���������������� ��������� �������:
// ��������� ������� � ������ ������� ����� 10 � ������� � 
float Qual2Stab ( float fqual )
{
	return 0.87f+0.221f*pow(fqual,2);		// ������� �� (������ ��� ���)
}

// ������� ���������� �����, ��������� ������� ����� �� �����
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

// ������� ���������� ��������, ����� �� ����������� ����
void DelegateFun()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}