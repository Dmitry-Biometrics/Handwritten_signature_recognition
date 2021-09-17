//	���� ��� �������	���� ��� �������	���� ��� �������	���� ��� �������

//����:		bnc32.h (bnc - ����������-������������ ���������/��������� �������)
//��������: ���������� �������������� ���������.
//			����������-������������ ��������� ������������:
//          �������� ��������� ���� - ����+�������=����;
//			���������� ����� �� ���������� - ����+�������=����;
//			������������ ��������� ��������� ����



//	���� ��� �������	���� ��� �������	���� ��� �������	���� ��� �������

#ifndef _BNC32_H
#define _BNC32_H

#include <windows.h>
#include <wincrypt.h>
#include <stdlib.h>

//==================================================================================//
//						�������� ������������ ��������		                        //
//==================================================================================//
#define NCMAXCOORD_QUAN		5000		// ������������ ���������� ���������
#define NCSCALE_SIZE		  512			// ����� ��������������� ������ ���������
#define NCFOURIER_COUNT		416			// ���������� ������������� �����

#define NEURON_COUNT		256					// ���������� ��������
#define NCKEY_BITS			NEURON_COUNT		// ����� ����� � �����
#define NCKEY_SIZE			(NCKEY_BITS/8)		//����� ����� � ������

#define NC_L1_WEIGHT		24					// ���. ������ ������� 1-�� ����
#define NC_L2_WEIGHT		0					// ���. ������ ������� 2-�� ����
#define NC_L1_COUNT			(NC_L1_WEIGHT*NEURON_COUNT)	// ���. ������ 1�� ����
#define NC_L2_COUNT			(NC_L2_WEIGHT*NEURON_COUNT)	// ���. ������ 2�� ����
#define NCWEIGHTS_COUNT		(NC_L1_COUNT + NC_L2_COUNT)	// ������ ������� �����
#define NCOFFSET_COUNT		NEURON_COUNT		// ������ ������� �������� 
//(�������� ��������� ������� ��������� ������������ ���� ��� �������� ��������� ����)

// ��������� �����
#define NCTRAIN_MODE		0		// ����� ��������	
#define NCTEST_MODE			1		// ����� ������������
// ����� ������ ���� ������������
#define NCTEST_MODE_TIWN	  0		// ������� ����� ������������ �� �������� ������� ��� �� ����� ����
#define NCTEST_MODE_MORFING 1		// ������������ � �������������� �������� (�������� ������ � ����������)

#define NCHASH_MD5_BITS		128
#define NCHASH_MD5_SIZE		(NCHASH_MD5_BITS/8)
#define NCHASH_LEN			  NCHASH_MD5_SIZE		//����� ��������� � ���� (����� ����)
#define NC_MAX_PASSW_SIZE	(NCKEY_SIZE*2+1)	//����. ����� ������ ������ � 16-���� �����

#define NCIMAGE_COUNT		3000		// ������������ ���������� ��������� �������
//==================================================================================//

//==================================================================================//
//						        ���� ������				                            //
//==================================================================================//
// ����� ����������� �����
struct BNCSIGNIMAGE {
	UINT	m_Num;				// ���������� ����� (��������� X/Y/P/T)
	PUINT	m_X;				// ��������� �� ������ X ��������� ������
	PUINT	m_Y;				// ��������� �� ������ Y ��������� ������
	PUINT	m_P;				// ��������� �� ������ �������� ��������
	PUINT	m_T;				// ��������� �� ������ �������� �������
	UINT	m_MaxX;				// ������������ �������� ���������� X
	UINT	m_MaxY;				// ������������ �������� ���������� Y
	UINT	m_MaxP;				// ������������ �������� �������� 
};

// �������������� ������� ������-���� �������������
struct BNCMOMENTS {
	FLOAT faver;				// �������������� ��������
	FLOAT fdisp;				// ���������				 
	FLOAT fqual;				// ��������
	INT  X[NCIMAGE_COUNT];		// ���� ��������
	POINT p01[NEURON_COUNT];	// � ������ ������ ���������� ���������� 
								// ��������� 0 (p01[i].x) � 1 (p01[i].y) ��� i-�� �������
	BYTE tmpKey[210*NEURON_COUNT];//��������� ������ ��� ������ 21 ������ 
								//(������������ ��� ������������ ������� ����� 3-� �������������!!!)
};

// ���� ������
typedef BYTE	NCKEY[NCKEY_SIZE];
// ������������� ���� 
typedef int		NCINTKEY[NCKEY_BITS];
// ������� ����� ������������ ������������� ��������� ���� (���� + ��������)
typedef float	NCWEIGHTS[NCWEIGHTS_COUNT+NCOFFSET_COUNT];
// ������ ������������� �����
typedef float	NCFOURIER[NCFOURIER_COUNT];
// ���-�������, ����������� �� ������ �����
typedef BYTE	NCHASH[NCHASH_LEN];
// ��� ������������ (�����)
typedef WCHAR	NCLOGIN[21];
// ������ ����� ������ ��������/������������
typedef struct sessionInfo_t {
  NCLOGIN userName;   // ��� ������������
  int     keyLen;     // ����� �����
  NCKEY   key;        // ����
} sessionInfo;

//==================================================================================//

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef BNC32_EXPORTS
#define BNC32_API __declspec(dllexport)
#else
#define BNC32_API __declspec(dllimport)
#endif	//end BNC32_EXPORTS

//==================================================================================//
//							��������������  �������		                            //
//==================================================================================//

//����������: ������� �������� ��������� ����
//��������:   �������������� �������������� ���������� ������� ������������ � 
//			  ���������� ����� � ������� ������� ������������� ��������� ����
//���������:  key		- [in]  ��������� ����
//			  mixKey	- [in]  �������������� ����
//			  imgs		- [in]  ��������� ������
//			  count		- [in]  ���������� ��������� �������
//			  weights	- [out] ������� ������� �������������
//			  group		- [out] ����� ������ ������������ ������������
BNC32_API HRESULT BncNeuroNetTrain (
									const NCKEY key, 
									const NCKEY mixKey,
									UINT  count,
									const BNCSIGNIMAGE imgs[],
									NCWEIGHTS weights,
									UINT  *group );
//������� � ����������� �������������
BNC32_API HRESULT BncNeuroNetTrain_2 (
                                    const WCHAR path[],
                                    const NCKEY key, 
                                    const NCKEY mixKey,
                                    UINT  count,
                                    const BNCSIGNIMAGE imgs[],
                                    NCWEIGHTS weights,
                                    UINT  *group );



//����������: ��������� ����� �� ��������� ����
//��������:   �������������� ��������� �������������  
//			  ����������� ������ � ����
//���������:  img     - [in]  ���������� �����/������� ������������
//			  weights - [in]  ������� ������� ������������� ��������� ����
//			  mixKey  - [in]  �������������� ����
//			  key     - [out] ���������� ����
//����������: ���������� ������� ������� ����, �������� � ��������� ��������
//			  ������������ ��� ���������� ����� � �����
BNC32_API INT BncExtractKey (
							 const BNCSIGNIMAGE img,
							 const NCWEIGHTS weights,
							 const NCKEY mixKey,
							 NCKEY key );
//������� � ����������� �������������
BNC32_API INT BncExtractKey_2 (
                             const WCHAR path[],
                             const BNCSIGNIMAGE img,
                             const NCWEIGHTS weights,
                             const NCKEY mixKey,
                             NCKEY key );

//����������: ���������� �������� ��� �������� �� �������� �������
//��������:   ������������� ���� �� �������� ������� � ����������
//			  �����������, ��������� � �������� ��� ��������
//���������:  on_TI   - [in]  TRUE - ������������ �� �������� �������, 
//							  FALSE - ������������ �� ����� ����
//			  key     - [in]  ��������� ����
//			  mixKey  - [in]  �������������� ����
//			  imgs    - [in]  �������� ������
//			  count	  - [in]  ���������� �������� �������
//			  weights - [in] ������� ������� �������������
BNC32_API BNCMOMENTS BncGetXemMoments (
									const BOOL on_TI,
									const NCKEY key, 
									const NCKEY mixKey,
									const BNCSIGNIMAGE imgs[],
									UINT  count,
									NCWEIGHTS weights );

//����������: ���������� �������� ��� �������� ��� ������������� �����
//��������:   ������������� ���� ������������ ���������� ������� � ����������
//			  �����������, ��������� � �������� ��� ��������
//���������:  
//			  key      - [in]  ��������� ����
//			  mixKey   - [in]  �������������� ����
//			  imgsCoef - [in]  ����������� ���� ������� � ����� ������� �������
//			  count	   - [in]  ���������� �������� �������
//			  weights  - [in]  ������� ������� �������������
BNC32_API BNCMOMENTS BncCoefsToXemMoments (
									   const NCKEY key, 
									   const NCKEY mixKey,
									   const FLOAT imgsCoef[],
									   UINT  count,
									   NCWEIGHTS weights );

//����������: ���������� ������������� ����� ������� �������
//��������:	  ������������ ������ �� ����� ������������ � �������������
//���������:  imgs  - [in] �������� ���������� ������
//			  count - [in] ���������� ���������� �������
//			  bioParam - [out] ����������� ��������� ���������� ������� (����-�� �����)
BNC32_API void BncGetImgParams (
							   const BNCSIGNIMAGE imgs[],
							   UINT  count, 
							   FLOAT bioParam[] );

//����������: �������������� ������������� ����� � ����
//��������:	  ������������ ������ �� ����� ������������ � �������������
//���������:  coef    - [in]  ������������ ����� ������ ������
//			  weights - [in]  ������� ������� ������������� ��������� ����
//			  mixKey  - [in]  �������������� ����
//			  key     - [out] ���������� ����
BNC32_API void BncCoefsToKey (
							 const NCFOURIER coef,
							 const NCWEIGHTS weights,
							 const NCKEY mixKey,
							 NCKEY key ); 

//����������: ��������� ������ � ������� ���������� ������������ �����
//���������:  first     - [in] ������ ����
//	          second    - [in] ������ ����
//����������: ���������� ������� ����� ������� (0, ���� ����� �����)
BNC32_API INT BncCompareKeys ( const NCKEY first, const NCKEY second );

//����������: ��������� ��������� �����
//��������:   �������������� ���������� ����� NCKEY 
//			  � �������� ���� ���� INT
//���������:  key	 - [in]  ������ � ������
//			  keyArr - [out] �������� ���� (���������� ������, ������ ������
//							 �������� �������� ��������� ��������� �����)
//����������: ��� ������
BNC32_API HRESULT BncConvertKey ( const NCKEY key, INT keyArr[] );

//����������: ����������� ������� ������
//���������:  pbData    - [in]  �������/���������� ������
//			  dwDataLen - [in]  ������ ������ pbData
//			  hash	    - [out] ���
//����������: ��� ������
BNC32_API HRESULT BncCreateHash ( const BYTE *pbData, DWORD dwDataLen, NCHASH hash );

//����������: ��������� �����
//���������:  first	 - [in] ������ ���
//			  second - [in] ������ ���
//����������: TRUE, ���� ���� �����; FALSE, ���� ���� �� �����
BNC32_API BOOL BncCompareHash( const NCHASH first, const NCHASH second );

//����������: �������������� ����� � ������
//���������:  key	 - [in]  ����
//			  passw  - [out] ������ � ���� ������
//			  as_hex - [in]  ���� ������ � 16-� ������ (� ���� ������ 
//						     ������ ������ ������ ���� 65 ��������)
//����������: TRUE, ���� �� ���������, ����� ���������� FALSE
BNC32_API BOOL BncKeyToPassw (
							 const NCKEY key,
							 TCHAR passw[NC_MAX_PASSW_SIZE],
							 BOOL as_hex );

//����������: �������������� ������ � ����
//���������:  key	 - [in]  ����
//			  passw	 - [out] ������ � ���� ������
//			  as_hex - [in]  ����, ������������, ��� ������ ����� � ���� 16-�� �����
//����������: TRUE, ���� �� ���������, ����� ���������� FALSE
BNC32_API BOOL BncPasswToKey ( 
							  NCKEY key,
							  const TCHAR passw[],
							  BOOL as_hex );

//������ ��������� �����
#define NC_MODE_DEFAULT	0x0000	//�� ��������� (������������ �������� 1..254)
#define NC_MODE_LATIN	0x0001	//���. ��������� ����� � ����� �� 0 �� 10
#define NC_MODE_HEX		0x0002	//����� � �����, ���. � 16-� �.�.

//����������: ��������� ���������������� �����
//���������:  key	 - [out] ����
//			  length - [in] ����� ����� (���. ����� ����������)
//			  mode	 - [in] ����� ������, ������������ ����� ���������
//����������: ��� ������
BNC32_API HRESULT BncGenerateKey ( 
								  NCKEY key,
								  INT length = NCKEY_SIZE,
								  UINT mode = NC_MODE_DEFAULT );

//����������: ���������� ������� ���������� ����� �� ������� ����������
//���������:  key	    - [in]  �������� ��������� ����
//			  pbDestKey - [out] ���������� ����
//			  dwKeyLen	- [in]  ����� ��������� �����
//����������: ��� ������
BNC32_API HRESULT BncResizeTrainKey ( 
									 NCKEY key,
									 BYTE *pbDestKey,
									 DWORD dwKeyLen );
//==================================================================================//

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_BIONEUROCONT_H
