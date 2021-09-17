//����:			NetTesting.h
//��������:		������, �������������� ������������ ��������� ����

#ifndef _NETTEST_H
#define _NETTEST_H

#include "bnc32.h"

// �������������� ��������� ���� �� ����� ����
// count   - [in] ���������� �������� �������
// key     - [in] ��������� ����
// mixKey  - [in] �������������� ����
// weights - [in] ������ �����
// params - [out] �����������, ��������� � �������� ��� ��������
void NetTestOnWN (  UINT count, const NCKEY key, const NCKEY mixKey,
					const NCWEIGHTS weights, BNCMOMENTS &params );

// �������������� ��������� ���� �� �������� ���������
// count   - [in] ���������� �������� �������
// key     - [in] ��������� ����
// mixKey  - [in] �������������� ����
// weights - [in] ������ �����
// coefs   - [in] ������ ������� ������������� (imageCount*NCFOURIER_COUNT)
// params - [out] �����������, ��������� � �������� ��� ��������
void NetTestOnTF (  UINT count, const NCKEY key, const NCKEY mixKey,
					const NCWEIGHTS weights, const float coefs[], 
					BNCMOMENTS &params );

// ���������� ������ ��������� ������������
// key     - [in] ��������� ����
// mixKey  - [in] �������������� ����
// weights - [in] ������ �����
//         - [out]����� ������ � ������� ��������� ������������
int CalculateStabilityGroup ( const NCKEY key, const NCKEY mixKey, 
							  const NCWEIGHTS weights );

#endif