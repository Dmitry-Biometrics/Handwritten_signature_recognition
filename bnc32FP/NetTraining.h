//����:			NetTraining.h
//��������:		������ �������� ��������� ����

#ifndef _NETTRAIN_H
#define _NETTRAIN_H

#include "bnc32.h"

//����������: ������� �������� ��������� ����
//��������:   ��� �������� ���� ������������ ����� ������(��������������� 1 ���� � 
//			  �������� ��������, ���. ���. ������� ������� �.�. �� ����� 3)
//���������:  
//	trCount		- [in]  ���������� ��������� �������
//	trainCoefs	- [in]  ���������� ������ ������������� ����� ���� ��������� ������� 
//	key			- [in]  ��������� ����
//	mixKey		- [in]  �������������� ����
//	weights		- [out] ���� ��������� ����
void NeuroNetTrain ( 
					UINT trCount, float trainCoefs[], 
					const NCKEY key, const NCKEY mixKey, 
					NCWEIGHTS weights );

#endif