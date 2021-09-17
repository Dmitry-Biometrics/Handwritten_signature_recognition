// �������������� ������ 

#ifndef _COMPUT_H
#define _COMPUT_H

#include "../NCTestProj/bnc32.h"

//����������: ���������� ��������������� ��������, ��������� � �������� 
//			  ��� ������ ����� �����
//��������:   ���������� ���.��������, ��������� � �������� ��� ��������
//���������:  count - [in]  ���������� ��������� ������� coefs
//			  coefs - [in]  ���������� ������ �������� ��� ��������
//			  params - [out] ����������� �����������, ��������� � ��������
void fadq ( unsigned count, const int coefs[], BNCMOMENTS &params );

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
void fadq ( unsigned count, unsigned step, const float coefs[], float aver[], float disp[], float qual[] );

// ��������� ����������� �������� �������
// ���������� ����������� ��������
int fminElem ( unsigned count, const int coefs[] );
float fminElem ( unsigned count, const float coefs[] );

// ��������� ������������ �������� �������
// ���������� ������������ ��������
int fmaxElem ( unsigned count, const int coefs[] );

// ��������� �������������� �������� �������
// ���������� ������� ��������
float faverElem( unsigned count, const float coefs[] );

// ��������� ���������� "�������" �������, �.�. ���-�� ��� == 0
// ���������� ���-�� ��� �������� ������ ����
int fzeroXem ( unsigned count, const int coefs[] );

// ��������� ��������� � ������ �������
// fqual - [in] ����������� ��� ������������� �������� ��� ��������
// ������������ �������� � ������� ���������������� ��������� �������:
// ��������� ������� � ������ ������� ����� 10 � ������� � 
float Qual2Stab ( float fqual );

// ������� ���������� �����, ��������� ������� ����� �� �����
int roundUp ( float x );

// ������� ���������� ��������, ����� �� ����������� ����
void DelegateFun();

#endif