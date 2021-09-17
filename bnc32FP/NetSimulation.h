//����:			NetSimulation.h
//��������:		������, ������������ ������ ��������� ����


#ifndef _NETSIM_H
#define _NETSIM_H

#include "bnc32.h"

extern const short ConnectionArray_Layer_1[];

//�������:		int ImageRecognition (...)
//����������:	�������� ������ �������� ������� �� ��������� ���� 
//��������:		������������� ���� �� 1 ������. ���������� ���������� 
//		        ������������ � ������
//���������:	inputArr    - [in] ������� ������
//				weightsArr  - [in] ���� ��������� ����
//				keyArr		- [in] ����
//				mixerKey	- [in] �������������� ����
//				checkKeyArr - [out]���������� ����
//				����������	- [out]���������� ������������ � ������	
int ImageRecognition (
				float *inputArr, float *weightsArr, 
				int *keyArr, int *mixerKey, int *checkKeyArr );

//�������:		int ImageSimulation (...)
//����������:	�������� ������ �������� ������� �� ��������� ���� 
//��������:		������������� ���� �� 1 ������. �������������� 
//				����������� ������ � ����
//���������:	inputArr    - [in] ������� ������
//				weightsArr  - [in] ���� ��������� ����
//				mixerKey	- [in] �������������� ����
//				checkKeyArr - [out]���������� ����
// ���������� ���������� ������� ������� ����, �������� � ��������� ��������
// ������������ ��� ���������� ����� � �����
int ImageSimulation (
				const float inputArr[], const NCWEIGHTS weights, 
				const NCKEY mixKey, NCKEY checkKey );

// ���������� ���������� ������� "������"
// ���������� ���������� �������, ������������ ��� "����"
int GroupSimulationForCompare (
				int imageCount, float *inputArr, 
				float *weightsArr, int *keyArr, int *mixerKey );

// �������� �������� �������� �� ����� ������� ������� ��� 2 ����
// ���������� ���������� ������������ � ������
// neuronNum	 - [in] ����� ������������� �������
// weightsCount  - [in] ���������� ����� � ������� �������
// ConnectionArr - [in] ������� ������
// imageCount	 - [in] ���������� �������� �������
// keyBit		 - [in] �������� ���������� ����� ��� ������� ������� (0 ��� 1)
// inputArr      - [in] �������� ��������� (���� 1 ��� 2 ����)
// weightsArr	 - [in] ���� �������/������� ����
//				 - [out]���������� ������������ � ������
int NeuronRecognitionOnTest (
				int neuronNum,  int imageCount, 
				const int keyBit, float inputArr[], float weightsArr[],				
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1 );

// �������� �������� �������� �� ����� ������� �������/������� ����
// ���������� �������� �������� �� ������ ������� �� �������� �������
// neuronNum	 - [in] ����� ������������� �������
// weightsCount  - [in] ���������� ����� � ������� �������
// ConnectionArr - [in] ������� ������
// imageCount	 - [in] ���������� �������� �������
// inputArr      - [in] �������� ��������� (���� 1 ��� 2 ����)
// weightsArr	 - [in] ���� �������/������� ���� ����
//				 - [out]�������� ��������
float NeuronQualityOnTest (
				int neuronNum, int weightsCount, 
				const short *ConnectionArr, int imageCount, 
				float *inputArr, float *weightsArr );	

// ���������� �������� �������� ������ �������
// neuronNum	- [in] ����� ������������� �������
// trnCount		- [in] ���������� ������� ������
// trnCoeff     - [in] ������������ ������
// tstCount		- [in] ���������� ������� ������
// tstCoeff		- [in] ������������ ������
// weightsArr	- [in] ���� �������/������� ���� ����
// ���������� �������� �������� ������������� �������: q = |a1-a2|/(d1+d2)
float NeuronTrainQuality (
							UINT neuronNum, 
							UINT trnCount, float trnCoeff[],
							UINT tstCount, float tstCoeff[],
							float weightsArr[],  
							const int weightsCount = NC_L1_WEIGHT,
							const short ConnectionArr[] = ConnectionArray_Layer_1 );	

// ���������� ������� ��������� ������� �� ������ �������� �������
// neuronNum	- [in]  ����� ������������� �������
// imageCount	- [in]  ���������� �������� �������
// inputArr     - [in]  �������� ��������� (���� 1 ��� 2 ����)
// weightsArr	- [in]  ���� �������/������� ���� ����
// outSum		- [out] ������ ��������� ������� �� ���� �������� ��������
// outNeuron	- [out] ������ ���������������� ������� �� ���� ��������
void NeuronSimulation ( 
				const int neuronNum,  
				const int imageCount,
				float inputArr[], 
				const float weightsArr[], 
				float outSum[], 
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1,
				float outNeuron[] = NULL );

// ������������� ������� ��� ������� ���� ���� �� ������ ������� 
// ���������� �������� ����
// weightsCount  - [in] ���������� ����� 1 ������� (�.� 1 ��� 2 ����)
// ConnectionArr - [in] ������� ������ 1 ��� 2 ����
// imageCount	 - [in] ����� ���������� �������
// inputArr      - [in] ������������ ���� �������
// weightsArr	 - [in] ���� 1 ��� 2 ���� ����
// outputArr	 - [out]������ ����������
void GroupLayerSimulation (
				int imageCount, float inputArr[], 
				float weightsArr[], float outputArr[],
				const int weightsCount = NC_L1_WEIGHT,
				const short ConnectionArr[] = ConnectionArray_Layer_1 );

// ������������� ������� ��� ������� ���� ���� �� ������ ������� 
// ���������� ����������� ��������� ������ �� ������ �������
// bUseShift	 - [in] ������������ �������� ������� ��������� ������� (TRUE) ��� ���
// imageCount	 - [in] ����� ���������� �������
// inputArr      - [in] ������������ ���� �������
// weightsArr	 - [in] ���� 1 ��� 2 ���� ����
// outputArr	 - [out]����������� ��������� ������ ��� ���� ��������
// weightsCount  - [in] ���������� ����� 1 ������� (�.� 1 ��� 2 ����)
// ConnectionArr - [in] ������� ������ 1 ��� 2 ����
void LayerSimulationForProb (
							 bool bUseShift,
							 int imageCount, float inputArr[], 
							 const float weightsArr[], float outputArr[],
							 const int weightsCount = NC_L1_WEIGHT,
							 const short ConnectionArr[] = ConnectionArray_Layer_1 );

//�������:		void NetRecognition (...)
//����������:	�������� ������ ������� �������� �� ��������� ����������� ���� 
//��������:		������������� ���� �� �������� �������. 
//				���������� ������ � ����������� ������������ � ������
//���������:	imageCount - [in] ���������� �������
//			    inputArr   - [in] ������� ������
//				weightsArr - [in] ���� ����
//				keyArr	   - [in] ����
//				mixerKey   - [in] �������������� ����
//				xemmingArr - [out]���������� ������������ � ������	
void NetRecognition (
				UINT imageCount, const float inputArr[],
				const NCWEIGHTS weightsArr, 
				const NCKEY key, const NCKEY mixKey, 
				int xemmingArr[] );
// p01[] - [out] � ������ ������ ���������� ���������� ��������� 0 (p01[i].x)
//				 � 1 (p01[i].y) ��� i-�� ������� (p01[i].x + p01[i].y = imageCount)
// tmpKeys[] - [out] ������ ��� ������ 21 ��������� ������ (21 �.�. � ���\����2006 � ����� ����� ����������� 21 ������)
void NetRecognition_P (
					 UINT imageCount, float inputArr[],
					 const NCWEIGHTS weightsArr, 
					 const NCKEY key, const NCKEY mixKey, 
					 int xemmingArr[],
					 POINT p01[], BYTE tmpKeys[] );

// �������� �������� �������� �� ����������� ����
// ������������� ������� ���� ���� �� ������ �������� ��������
// imageCount	- [in] ���������� ������������ �������
// inputArr		- [in] ������������ ���� �������
// weightsArr	- [in] ���� ����������� ����
// key			- [in] ��������� ����
//				- [out]���������� ������������ � ������
int NetRecognitionOnTest (
						  UINT imageCount, float inputArr[], 
						  const NCWEIGHTS weightsArr, 
						  const NCKEY key );	

// ���������� ����������
void CalcHistCorr ( POINT p01[], UINT imageCount, float inputArr[], 
				    const float weightsArr[] );

// ���������� ���������� ������� �������� (������ � ������)
void CalcNetCorr ( UINT imageCount, float inputArr[], const float weightsArr[]);

// ���������� ������� ���������� ��� ��������
// count		- [in] ���������� �������� � ������ ��������
// numArr		- [in] ������ � �������� ��������
// imageCount	- [in] ���������� �������� �������
// inputArr		- [in] ������������ ���� �������
// weightsArr	- [in] ���� ����������� ����
// ���������� ������� ���������� ��� ������� ��������
float MeanColCorr ( UINT count, int numArr[], 
				    UINT imageCount, float inputArr[], 
					const float weightsArr[] );

#endif