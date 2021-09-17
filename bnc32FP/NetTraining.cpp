//����:			NetTraining.cpp
//��������:		������ �������� ��������� ����

#include "NetTraining.h"

#include "ncmath.h"
#include "NetSimulation.h"	

//////////////////////////////////////////////////////////////////////////
// �������� ������� � �������� ���������
// neuronNum		- [in] ����� ���������� �������
// weightsCount		- [in] ���������� ����� ������� �������
// ConnectionArr	- [in] ������� ������ �������� ������� ����
// imageCount		- [in] ���������� ��������� �������
// inputArr			- [in] ������������ ��������� �������
// tstCount			- [in] ���������� �������� ������� (��� �����)
// testCoef			- [in] ������������ �������� �������
// key				- [in] ��������� ����
// weightsArr		- [in] ������� ������������
// qualLimFrom		- [in]
// qualLimTo		- [in]
void NeuronTrainingA2 (
					   const int neuronNum,
					   int weightsCount, 
					   const short ConnectionArr[],
					   UINT imageCount, float inputArr[],
					   const NCKEY key, 
					   float weightsArr[], 
					   float qualLimFrom, float qualLimTo, float averArr[], float dispArr[], float qualArr[] )
{
	float outQuality = 0.0f;	// �������� �������� �������
	int   znak;					// ���� �������������� ���� (+/-)
//	int	  cycleCount;			// ���������� ������� �� �������� ��������

	// �������� �������� ����� ��� ������� �������
	int keybit = getbit(key,neuronNum);	

	int weightNum = 0;			// ����� ���� ���������������� �������
	int errorCount = 10;		// ���������� ����������� ������������ �������

	// ���� �� �������� ��� ���� ��� ���� �� ������� ��� ��������� ������
	while ( weightNum < weightsCount && outQuality < qualLimFrom  )
	{
		bool keyOne = (keybit == 1); 
		znak = ( keyOne == (averArr[ConnectionArr[neuronNum*weightsCount+weightNum]] > 0.0f) ) ? 1 : -1 ;

		weightsArr[neuronNum*weightsCount + weightNum] = 
								znak*
								qualArr[ConnectionArr[neuronNum*weightsCount+weightNum]]/ 
								dispArr[ConnectionArr[neuronNum*weightsCount+weightNum]];

		if ( weightNum == 0 ) { // �.�. � ������� ������ ���� ������� 2 ��������� ����
			weightNum++;
			znak = ( keyOne == (averArr[ConnectionArr[neuronNum*weightsCount+weightNum]] > 0.0f) ) ? 1 : -1 ;
			weightsArr[neuronNum*weightsCount + weightNum] = znak*qualArr[ConnectionArr[neuronNum*weightsCount+weightNum]]/dispArr[ConnectionArr[neuronNum*weightsCount+weightNum]];
		}

		// ��������� �������� �������� ������� �� �������/��������� �������
		outQuality = NeuronQualityOnTest ( neuronNum, weightsCount, ConnectionArr, imageCount, inputArr, weightsArr );
		weightNum++;
	}

	errorCount = NeuronRecognitionOnTest ( neuronNum, imageCount, keybit, inputArr, weightsArr, weightsCount, ConnectionArr );

	// ���� �������� �������� ������� ���� ��������� � ��� ��������
	// ���������� (�.� �������) ����, ������� �������� �������� ����������� ����
	if ( weightNum < weightsCount && outQuality > qualLimTo )
	{
		float fixQual = outQuality;
		int iter = 0;
		float *fixWArr = new float[weightsCount]; //��������� ����
		memcpy(fixWArr, weightsArr+neuronNum*weightsCount, weightsCount*sizeof(float));
		do 
		{
			for ( int i = weightNum; i < weightsCount; i++) {
				weightsArr[neuronNum*weightsCount + i] = (rand()%1000 - rand()%1000)/1000.f;
			}
			outQuality = NeuronQualityOnTest ( neuronNum, weightsCount, ConnectionArr, imageCount, inputArr, weightsArr );
			errorCount = NeuronRecognitionOnTest ( neuronNum, imageCount, keybit, inputArr, weightsArr, weightsCount, ConnectionArr );
			// ��������� ������ �������
			if ( errorCount == 0 && outQuality < fixQual && outQuality > qualLimFrom ) {
				fixQual = outQuality;
				for ( int i = weightNum; i < weightsCount; i++) { fixWArr[i] = weightsArr[neuronNum*weightsCount + i]; }
			}
			if (iter++ > 1000) { for ( int i = weightNum; i < weightsCount; i++) { weightsArr[neuronNum*weightsCount + i] = fixWArr[i]; } break; }
		} while(errorCount != 0 || outQuality < qualLimFrom || outQuality > qualLimTo);
		delete [] fixWArr;
	}


}

// �������� ������� ���� ����
// �������� ��������:
//	0. ��� ���� �������� ������� ����
//	1. ��� ���� ������ ������� 0 (��������)
//	2. ��� ���� �����
//  3. ���������� ���� ���� �� �������� ���.�������� � �� �����
//	4. ����� �������� ���� �� ������� W = Q��/D��, ��� Q�� - ������� �������� (��������
//	   ����������� �� ������ ���� ������� ������������), D�� - ������� ���������
//	5. ��������� �������� �������� �������
//	6. ���� �������� �������� > setQualLimFrom (�.�. ��������� � ��������� �.�. 4,5,6..),
//     ��������� � ���� 7, ���������� ���� �� ���������, ��� �������� ����� 0.
//	   ����� ��������� �� ��� 2 � ��������� ��� ��������� 2-4 ��� ���������� ����
//	   ����� ��������� � ���� 5 � 6 � ��� �� ��� ��� ���� �� �� ��������� ������� 
//	   �������� �� ������ ������� ��� ���� �� �������� ��� ����.
//	7. ���� �������� �������� ������ ��������� (4,5,6,...), ������� ��� �������
//	   ���������� ���������� ����: ���������� ������� ���� ���� �������� �� ���������
//     ��� � ��������� �������� �������� �������. ��������� ��� �� ��� ���, ����
//     �������� �� ������ � �������� ��������. 
//	8. ��������� � ���������� �������
//���������:	weightsCount  - [in]  ���������� ����� ������ �������
//				ConnectionArr - [in]  ������� ������ ������� ����
//				imageCount    - [in]  ���������� ��������� �������
//				keyArr		  - [in]  ���� �� ������� ����� ��������� ����
//				inputArr	  - [in]  ������ �����
//			    weightsArr	  - [out] ���� ��������� ����
//				averageArr	  - [in]  �������������� �������� ������� �������������
//				dispersionArr - [in]  ��������� ������� �������������
//				qualityArr    - [in]  �������� ������� �������������
void LayerTrainingA2 (
					 int weightsCount, 
					 const short ConnectionArr[],
					 const NCKEY key, 
					 UINT imageCount, float inputArr[], 
					 float weightsArr[] )
{
	srand(::GetTickCount());

	// �������� ������� ������� �������������
	ZeroMemory(weightsArr, NEURON_COUNT*weightsCount*sizeof(float));
	
	// ������� �������������� (��� ��������, ��������� � �������� �� ����� ����)
	float *averArr = new NCFOURIER;
	float *dispArr = new NCFOURIER;
	float *qualArr = new NCFOURIER;

	// ���������� �������� ���. �������� � �������� �������� �� ������ ����
	fadq ( imageCount, NCFOURIER_COUNT, inputArr, averArr, dispArr, qualArr );

	// ��� ���� �������� ������� ����
	for ( int neuronNum = 0; neuronNum < NEURON_COUNT; neuronNum++ )
	{
		float qualLimFrom = 6.0f;	// ������ � ������� �������
		float qualLimTo   = 7.0f;	// ��������� �������� �������

		NeuronTrainingA2 ( neuronNum, weightsCount, ConnectionArr, 
						   imageCount, inputArr, key, weightsArr, qualLimFrom, qualLimTo, averArr, dispArr, qualArr);

	} // ��� ���� ��������

	delete [] averArr;
	delete [] dispArr;
	delete [] qualArr;
}

//////////////////////////////////////////////////////////////////////////
// ������������� ����� ���� � ������ �������� ��������� ��� ��������� ���.��������
// weightsCount  - [in]     ���������� ����� ������ �������
// correctiveArr - [in]     �������������� ������
// weightsArr    - [in/out] ������ �����
void CalculateModifiedWeights ( int weightsCount, float *correctiveArr, float *weightsArr )
{
	for ( int iCount = 0; iCount < NEURON_COUNT; iCount++ ) {
		for ( int jCount = 0; jCount < weightsCount; jCount++ ) {
			weightsArr[iCount*weightsCount+jCount] /= fabs(correctiveArr[iCount]); 
		}
	}
}


// �������� ������� ���� ��������� ����
// imageCount	- [in] ���������� ��������� �������
// coeffArr		- [in] ������������ ��������� �������
// keyArr	    - [in] ��������� ����
// weightsArr   - [in/out] ���� ������� ���� (�� ����� - ����������� ����, 
//						  �� ������ - ���� ��������� ����
void FirstLayerTraining ( int imageCount, float coeffArr[], 
						  const NCKEY key, float weightsArr[] )
{
	// ������ ��� ������ ���� ���������� �������/������� ����
	float *netSumArr = new float [NEURON_COUNT*imageCount];

	// ����� �������� �������� ������� ����
	LayerTrainingA2 ( NC_L1_WEIGHT, ConnectionArray_Layer_1, key, 
					  imageCount, coeffArr, weightsArr );

	// ���������� ������� ���������� ���� �������� ������� ����
	GroupLayerSimulation ( imageCount, coeffArr, weightsArr, netSumArr );

	// �������� �������������� (��� ��������, ��������� � ��������
	// �� ����� ������� ���� ��� ���� ����)
	float *outAverArr = new float [NEURON_COUNT];
	float *outDispArr = new float [NEURON_COUNT];
	float *outQualArr = new float [NEURON_COUNT];

	// ���������� �������� ������������� ������� ����
	fadq ( imageCount, NEURON_COUNT, netSumArr, outAverArr, outDispArr, outQualArr );

	// ������������� ����� ������� ���� ���� � ������ �������� ���������
	CalculateModifiedWeights ( NC_L1_WEIGHT, outDispArr, weightsArr );

	// ���������� ������� ���� �������� ������� ����
	GroupLayerSimulation ( imageCount, coeffArr, weightsArr, netSumArr );

	// ���������� �������� ������������� ������� ����
	fadq ( imageCount, NEURON_COUNT, netSumArr, outAverArr, outDispArr, outQualArr );

	// ������������� ����� ������� ���� � ������ ��������� ��� �������� (�� ������)
	CalculateModifiedWeights ( NC_L1_WEIGHT, outAverArr, weightsArr );

	delete [] outAverArr;
	delete [] outDispArr;
	delete [] outQualArr;
	delete [] netSumArr;

}

// ������������ ������� ����� �� ������������ �������
// ��������: ��� ������������� ����� ��������� ���� ���������� ��������� ��������:
//				���������� ������� ��� �������� ���������� (keyArr) � 
//				��������������� ����� (mixerKey). ���� keyArr[i] == mixerKey[i],
//				��� ���� i-�� ������� ��������� ��� ���������, ���� �� 
//				keyArr[i] != mixerKey[i], �������� ��� ���� ������� i �� -1
// keyArr     - [in]	��������� ����
// mixerKey   - [in]	�������������� ����
// weightsArr - [in/out] ���� ��������� ����, �� ������ �������� ������������ ����
void ShufflePermutation ( const NCKEY key, const NCKEY mixKey, float weightsArr[] )
{
	for ( int i = 1; i < NEURON_COUNT; i++ )  {
		// �������� ���� ����� ������� i, ��� ����� 
		// ���������� ��������� � �������������� ����� � ������� i-1,
		// � ���� ��� ����������, �� ������ ���� ���� ����� ������� i
		// �� ���������������
		int znak = -2*(getbit(key,i-1)^getbit(mixKey,i-1))+1;
		
		for ( int j = 0; j < NC_L1_WEIGHT; j++ ) {
			weightsArr[i*NC_L1_WEIGHT+j] *= znak;
		}
	}
}

// ������� ������� �������� ����������� ��������� ����
void NeuroNetTrain ( 
					UINT trCount, float trainCoefs[], 
					const NCKEY key, const NCKEY mixKey, 
					NCWEIGHTS weights )
{
//-----------------------------------------------------------------------------------//
//						�������� ������� ���� ��������� ����	                     //
	float firstWeightsArr[NC_L1_COUNT];	 // ���� ������� ���� ����
	FirstLayerTraining(trCount, trainCoefs, key, firstWeightsArr);
//					����� �������� ������� ���� ��������� ����	                     //
//-----------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------//
//									�������						                     //
//	���� ��� �������	���� ��� �������	���� ��� �������	���� ��� �������	 //
//	������������ ������� ����� �� ������������ �������								 //
//	ShufflePermutation(key, mixKey, firstWeightsArr);								 //
//	���� ��� �������	���� ��� �������	���� ��� �������	���� ��� �������	 //
//								����� �������					                     //
//-----------------------------------------------------------------------------------//
	// ���������� ����� ���� ��������� ����
	memcpy ( weights, firstWeightsArr, NC_L1_COUNT*sizeof(float) );
}

