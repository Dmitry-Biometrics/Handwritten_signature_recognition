//����:			bnc32.h (bnc - ����������-������������ ���������/��������� �������)
//��������:		����������-������������ ��������� ������������:
//              �������� ��������� ���� - ����+�������=����;
//				���������� ����� �� ���������� - ����+�������=����;
//				������������ ��������� ��������� ����


#include <stdio.h>
#include "stdafx.h"
#include "bnc32.h"

#include "ConversionSFN.h"
#include "NetTraining.h"
#include "NetTesting.h"
#include "NetSimulation.h"
#include "ncmath.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	break;	// ����������� ���������� � ��������
	case DLL_THREAD_ATTACH:		break;	// ����������� � ������
	case DLL_THREAD_DETACH:		break;	// ���������� �� ��������
	case DLL_PROCESS_DETACH:	break;	// ���������� �� ��������
	}	//end switch
    return TRUE;

}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// ��������� ������������ � ����
LONG SaveCoefs( const float coefs[], int count, 
                const WCHAR path[], 
                const CHAR info[], 
                const WCHAR mode[] )
{
  FILE *stream;
  if( _wfopen_s(&stream, path, mode) ) { 
    return ERROR_FILE_NOT_FOUND;
  }      
  //fprintf(stream, info);
  for (int i = 0; i < count; i++) {
    fprintf(stream, "%.3f\n", coefs[i]);
  }
  fprintf(stream, "\n\n");
  fclose(stream);

  return ERROR_SUCCESS;
}
// ������� ������������ �� �����
bool LoadCoefs ( float coefs[], int &count, const WCHAR path[]) {
  FILE *stream;
  if( _wfopen_s(&stream, path, TEXT("r")) ) { 
    return false;
  }
//  CHAR info[MAX_PATH];
  //fscanf( stream, "%s\n", info );   
  int i = 0;
  while ( !feof( stream ) && i < count ) {
    fscanf_s(stream, "%f\n", &coefs[i]);
    i++;
  }
  count = i;
  fclose(stream);

  return true;
}
// ��������� ����������� � ����������� ���������� � �������� � ����
void calcAndWriteMS(float *coefs, int count, const WCHAR path[]) {
  // ������� �������������� (��� ��������, ��������� � �������� �� ����� ����)
  NCFOURIER averArr;
  NCFOURIER dispArr;
  
  // ���������� �������� ���. �������� � �������� �������� �� ������ ����
  fadq ( count, NCFOURIER_COUNT, coefs, averArr, dispArr, NULL );

  // ��������� ������������ � ����
  WCHAR filePath[MAX_PATH];
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("mean.txt"));
  SaveCoefs(averArr,NCFOURIER_COUNT,filePath,"�������������� �������� �������������� ���������� ���������� ������:\n",TEXT("w"));  
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("stdev.txt"));
  SaveCoefs(dispArr,NCFOURIER_COUNT,filePath,"����������� ���������� �������������� ���������� ���������� ������:\n",TEXT("w"));
}
// ���������� ���-�� �������������, ���������� � ��� ����� ���������� ������
void calcAndWriteExtractor(float *coefs, int count, const WCHAR path[]) {
  // ��������� ������������ ����� � ����
  WCHAR filePath[MAX_PATH];
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("params.txt"));
  SaveCoefs(coefs,count,filePath,"�������������� ��������� ���������� ��������� ����������� �������:\n",TEXT("w"));
  if (count != NCFOURIER_COUNT) {
    return;
  }
  // ������� ����������� � ���������� ���������� ������
  NCFOURIER averArr;
  NCFOURIER dispArr;
  int cnt = NCFOURIER_COUNT;
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("mean.txt")); 
  if (!LoadCoefs(averArr,cnt,filePath)) {
    return;
  }
  if (cnt != NCFOURIER_COUNT) {
    return;
  }
  cnt = NCFOURIER_COUNT;
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("stdev.txt"));
  if (!LoadCoefs(dispArr,cnt,filePath)) {
    return;
  }
  if (cnt != NCFOURIER_COUNT) {
    return;
  }
  // ���������� ���-�� ����������, �������� � ��� �����
  int *outArr = new int[NCFOURIER_COUNT];
  cnt = 0;
  for (int i = 0; i < NCFOURIER_COUNT; i++) {
    outArr[i] = ((coefs[i] >= averArr[i]-3.0f*dispArr[i])&&(coefs[i] <= averArr[i]+3.0f*dispArr[i])) ? 1 : 0;
    cnt += outArr[i];
  }
  wcscpy_s ( filePath, path );
  wcscat_s ( filePath, TEXT("3sigma.txt"));
  FILE *stream;
  if( !(_wfopen_s(&stream, filePath, TEXT("w"))) ) { 
    char  buffer[200];   
    sprintf( buffer, "� ��� ����� ������ %d ���������� �� %d\n", cnt, NCFOURIER_COUNT);
    fprintf(stream, buffer);
    for (int i = 0; i < NCFOURIER_COUNT; i++) {
      fprintf(stream, "%d\n", outArr[i]);
    }
    fprintf(stream, "\n");
    fclose(stream);  
  }
  delete [] outArr;

}

//////////////////////////////////////////////////////////////////////////////////////
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
									UINT  *group )
{
	HRESULT result = ERROR_SUCCESS;
	if (!count || !key || !imgs || !weights){
		return ERROR_BAD_ARGUMENTS;	// ��� ��������� �.�. ������
	}
	if ( count > NCIMAGE_COUNT ){
		return ERROR_BAD_ARGUMENTS;	// ���������� ������� �� �.�. ������ NCIMAGE_COUNT
	}

	FLOAT *coefs;		// ���������� ������ ������������� ����� ���� ��������� �������
//	FLOAT *testCoefs;	// ���������� ������ ������������� ����� ���� ������� �����
	// ��������� ������ ��� ������������ ���� ����� �������
	coefs = new FLOAT[count*NCFOURIER_COUNT];

	// ���������� ������������� ����� ���� ��������� �������
	NcCalcImagesCoef ( count, imgs, coefs );

	// �������� ��������� ����
	NeuroNetTrain ( count, coefs, key, mixKey, weights );

	// ���������� ������ ��������� ������������
	*group = CalculateStabilityGroup ( key, mixKey, weights );

	// ������������ ������
	delete[] coefs;

	return result;
}
//������� � ����������� �������������
BNC32_API HRESULT BncNeuroNetTrain_2 (
                                    const WCHAR path[],
                                    const NCKEY key, 
                                    const NCKEY mixKey,
                                    UINT  count,
                                    const BNCSIGNIMAGE imgs[],
                                    NCWEIGHTS weights,
                                    UINT  *group )
{
  HRESULT result = ERROR_SUCCESS;
  if (!count || !key || !imgs || !weights){
    return ERROR_BAD_ARGUMENTS;	// ��� ��������� �.�. ������
  }
  if ( count > NCIMAGE_COUNT ){
    return ERROR_BAD_ARGUMENTS;	// ���������� ������� �� �.�. ������ NCIMAGE_COUNT
  }

  FLOAT *coefs;		// ���������� ������ ������������� ����� ���� ��������� �������
  //	FLOAT *testCoefs;	// ���������� ������ ������������� ����� ���� ������� �����
  // ��������� ������ ��� ������������ ���� ����� �������
  coefs = new FLOAT[count*NCFOURIER_COUNT];

  // ���������� ������������� ����� ���� ��������� �������
  NcCalcImagesCoef ( count, imgs, coefs );

  // ��������� ����������� � ����������� ���������� � �������� � ����
  calcAndWriteMS(coefs, count, path);

  // �������� ��������� ����
  NeuroNetTrain ( count, coefs, key, mixKey, weights );

  // ���������� ������ ��������� ������������
  *group = CalculateStabilityGroup ( key, mixKey, weights );

  // ������������ ������
  delete[] coefs;

  return result;
}

///////////////////////////////////////////////////////////////////////////////
//����������: ��������� ����� �� ��������� ����
//��������:   �������������� ��������� �������������  
//			  ����������� ������ � ����
//���������:  imgs    - [in]  ���������� �����/������� ������������
//			  weights - [out] ������� ������� ������������� ��������� ����
//			  mixKey  - [in]  �������������� ����
//			  key     - [in]  ���������� ����
//����������: ���������� ������� ������� ����, �������� � ��������� ��������
//			  ������������ ��� ���������� ����� � �����
BNC32_API INT BncExtractKey (
							 const BNCSIGNIMAGE img,
							 const NCWEIGHTS weights,
							 const NCKEY mixKey,
							 NCKEY key )
{
	NCFOURIER fourier; 	// ������ � �������������� �����
	INT count;			// ������� ���������� ���������

	// �������������� ��������� � ������������ �����
	NcCalcCoefZero2416 ( img, fourier );

	// ���������� �����
	count = ImageSimulation ( fourier, weights, mixKey, key );

	return count;
}
//������� � ����������� �������������
BNC32_API INT BncExtractKey_2 (
                             const WCHAR path[],
                             const BNCSIGNIMAGE img,
                             const NCWEIGHTS weights,
                             const NCKEY mixKey,
                             NCKEY key )
{
  NCFOURIER fourier; 	// ������ � �������������� �����
  INT count;			// ������� ���������� ���������

  // �������������� ��������� � ������������ �����
  NcCalcCoefZero2416 ( img, fourier );

  // ���������� ���-�� �������������, ���������� � ��� ����� ���������� ������
  calcAndWriteExtractor(fourier, NCFOURIER_COUNT, path);
 
  // ���������� �����
  count = ImageSimulation ( fourier, weights, mixKey, key );

  return count;
}

///////////////////////////////////////////////////////////////////////////////
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
									   NCWEIGHTS weights )
{
	BNCMOMENTS params;	// ��������� ��� ��������
	FLOAT *coefs;		// ���������� ������ ������������ ���� �������� �������
	// ��������� ������ ��� ������������ ���� �������
	coefs = new FLOAT[count*NCFOURIER_COUNT];

	// ���������� �� ��� �����������
	if ( on_TI == TRUE ) {		// ������������ �� �������� �������
		// ���������� ������������� ����� ���� ��������� �������
		NcCalcImagesCoef ( count, imgs, coefs );
		NetTestOnTF ( count, key, mixKey, weights, coefs, params );
	} else {					// ������������ �� ����� ����
		NetTestOnWN(count, key, mixKey, weights, params);
	}
	 
	// ������������ ������
	delete [] coefs;

	return params;
}

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
										NCWEIGHTS weights )
{
	BNCMOMENTS params;	// ��������� ��� ��������

	NetTestOnTF ( count, key, mixKey, weights, imgsCoef, params );

	return params;
}

//////////////////////////////////////////////////////////////////////////
//����������: ���������� ������������� ����� ������� �������
//��������:	  ������������ ������ �� ����� ������������ � �������������
//���������:  imgs  - [in] �������� ���������� ������
//			  count - [in] ���������� ���������� �������
//			  bioParam - [out] ����������� ��������� ���������� ������� (����-�� �����)
BNC32_API void BncGetImgParams (
								const BNCSIGNIMAGE imgs[],
								UINT  count, 
								FLOAT bioParam[] )
{
	NcCalcImagesCoef ( count, imgs, bioParam );
}
//////////////////////////////////////////////////////////////////////////
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
							  NCKEY key )
{
	ImageSimulation ( coef, weights, mixKey, key );
}


///////////////////////////////////////////////////////////////////////////////
//����������: ��������� ������ � ������� ���������� ������������ �����
//���������:  first     - [in] ������ ����
//	          second    - [in] ������ ����
//����������: ���������� ������� ����� ������� (0, ���� ����� �����)
BNC32_API INT BncCompareKeys ( const NCKEY first, const NCKEY second )
{
	return getbitdiffs(first,second,NCKEY_BITS);
}
///////////////////////////////////////////////////////////////////////////////
//����������: ��������� ��������� �����
//��������:   �������������� ���������� ����� NCKEY 
//			  � �������� ���� ���� INT
//���������:  
//  key	   - [in]  ������ � ������
//	keyArr - [out] �������� ���� (���������� ������, ������ ������
//							   �������� �������� ��������� ��������� �����)
//����������: ��� ������
BNC32_API HRESULT BncConvertKey ( const NCKEY key, INT keyArr[] )
{
	if ( !key )	return ERROR_BAD_ARGUMENTS;	

  //������
  /*for ( int i = 0; i < NEURON_COUNT; i++ ) {
  keyArr[i] = getbit(key,i);
  }*/
  //��� Hex �������
  int k = 0;
  int j = 0;
  for ( int i = 0; i < NEURON_COUNT; i++ ) {
    k = i / 8;
    j = i % 8;
    keyArr[k*8+7-j] = getbit(key,i);
  }
		
	return ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
//����������: ����������� ������� ������
//���������:  pbData    - [in]  �������/���������� ������
//			  dwDataLen - [in]  ������ ������ pbData
//			  hash	    - [out] ���
//����������: ��� ������
BNC32_API HRESULT BncCreateHash ( const BYTE *pbData, DWORD dwDataLen, NCHASH hash )
{
	if (!pbData || !hash){
		return ERROR_BAD_ARGUMENTS;
	}

	TCHAR		szName[]  = TEXT("bnc32.dll");
	HRESULT		result	  = -1;	
	HCRYPTPROV	hProv	  = NULL;
	HCRYPTHASH	hHash	  = NULL;
	DWORD		dwHashLen = sizeof(NCHASH);

	// �������� ������������� ����������
	if (!CryptAcquireContext(&hProv,szName,NULL,PROV_RSA_FULL,NULL)){
		if (GetLastError() != NTE_BAD_KEYSET)
			goto exit;
		// �������� ������ ����������
		if (!CryptAcquireContext(&hProv,szName,NULL,PROV_RSA_FULL,CRYPT_NEWKEYSET))
			goto exit;
	}
	// �������� ������� ����
	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)){
		goto exit;
	}
	// ���������� ���-�������
	if (!CryptHashData(hHash, pbData, dwDataLen, 0)){
		goto exit;
	}

	ZeroMemory(hash,sizeof(NCHASH));
	// ������ ������� ����	
	if(!CryptGetHashParam( hHash, HP_HASHVAL, NULL, &dwHashLen, 0)){
		goto exit;
	}
	// ������ ����
	if(!CryptGetHashParam( hHash, HP_HASHVAL, hash, &dwHashLen, 0)){
		goto exit;
	}
	result = ERROR_SUCCESS;
exit:
	if ( hHash )	CryptDestroyHash(hHash);
	if ( hProv )	CryptReleaseContext(hProv,0);

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//����������: ��������� �����
//���������:  first	 - [in] ������ ���
//			  second - [in] ������ ���
//����������: TRUE, ���� ���� �����; FALSE, ���� ���� �� �����
BNC32_API BOOL BncCompareHash( const NCHASH first, const NCHASH second) {
	return (0 == getbitdiffs ( first, second, NCHASH_LEN*sizeof(BYTE)));
}

///////////////////////////////////////////////////////////////////////////////
//����������: �������������� ����� � ������
//���������:  key	 - [in]  ����
//			  passw	 - [out] ������ � ���� ������
//			  as_hex - [in]  ���� ������ � 16-� ������ (� ���� ������ 
//					 	     ������ ������ ������ ���� 65 ��������)
//����������: TRUE, ���� �� ���������, ����� ���������� FALSE
BNC32_API BOOL BncKeyToPassw ( 
							  const NCKEY key,
							  TCHAR passw[NCKEY_SIZE+1],
							  BOOL as_hex
							 )
{
	if (!key || !passw)	return FALSE;

	if ( as_hex ) {
		// ����������������� ����� ������� ������
		TCHAR hex[] = TEXT("0123456789ABCDEF");
		for ( INT i = 0; i < NCKEY_SIZE; i++ ) {
			/*passw[2*i]	= hex[key[i] & 0x0F];
			passw[2*i+1]= hex[(key[i]>>4) & 0x0F];*/
      passw[2*i+1]	= hex[key[i] & 0x0F];
      passw[2*i]= hex[(key[i]>>4) & 0x0F];
		}
		passw[2*NCKEY_SIZE] = NULL;
	} else {
		//����� ��������-��������� ������
		//(������� ������ ���� ������ �����. ����������� � ������ ��������� ������)
		for ( INT i = 0; i < NCKEY_SIZE; i++ ){
			if ( key[i] >= 128 ) 
				passw[i] = (WORD)(key[i] + 1024 - 128); // ��������� ��� ���. ����
			else
				passw[i] = (WORD)key[i];				// ��������� ��� ��������
		}
		passw[NCKEY_SIZE] = NULL;
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//����������: �������������� ������ � ����
//���������:  key	 - [in]  ����
//			  passw	 - [out] ������ � ���� ������
//			  as_hex - [in]  ����, ������������, ��� ������ ����� � ���� 16-�� �����
//����������: TRUE, ���� �� ���������, ����� ���������� FALSE
BNC32_API BOOL BncPasswToKey (
							  NCKEY key,
							  const TCHAR passw[],
							  BOOL as_hex
							 )
{
	if (!key || !passw)	return FALSE;
	
	ZeroMemory(key,sizeof(NCKEY));
	
	if (as_hex){
		// ����������������� ����� ������� ������
		BYTE val;
		for (INT i=0; i<2*NCKEY_SIZE; i++) {
			 if (passw[i] >= TEXT('0') && passw[i] <= TEXT('9'))
				 val = (BYTE)(passw[i] - TEXT('0'));
			 else if (passw[i] >= TEXT('A') && passw[i] <= TEXT('F'))
				 val = (BYTE)(passw[i] - TEXT('A') + 10);
			 else if (passw[i] >= TEXT('a') && passw[i] <= TEXT('f'))
				 val = (BYTE)(passw[i] - TEXT('a') + 10);
			 else return FALSE;

			if (i & 0x01)	key[i>>1] |= (val<<4);
			else			key[i>>1] = val;
		}
	} else {
		// ����� ��������-��������� ������
		// (������� ������ ������������� ���������� �������������)
		for (INT i=0; i<NCKEY_SIZE; i++){
			if (passw[i] == 0) return TRUE;
			if (passw[i]>=128) key[i] = (BYTE)(passw[i] - 1024 + 128);
			else 			   key[i] = (BYTE)(passw[i]);
		}
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//����������: ��������� ���������������� �����
//���������:  key	 - [out] ����
//			  length - [in] ����� ����� (���. ����� ����������)
//			  mode	 - [in] ����� ������, ������������ ����� ���������
//����������: ��� ������
BNC32_API HRESULT BncGenerateKey ( 
								  NCKEY key,
								  int length,
								  unsigned mode								  
								 )
{
	if (!key || length>NCKEY_SIZE){
		return ERROR_BAD_ARGUMENTS;
	}

	ZeroMemory(key,sizeof(NCKEY));
	srand(GetTickCount());

	unsigned page_mode = mode & (NC_MODE_LATIN | NC_MODE_HEX);
	const char latin_table[]= "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char hex_table[]	= "0123456789ABCDEF";

	for (int index=0; index<length; index++) 
	{
		switch (page_mode) {
		//-------------------------------------------------------------------//
		//���� �� ��������� ���� � ����
		//-------------------------------------------------------------------//
		case NC_MODE_LATIN:		
			key[index] = latin_table[rand()%62];
			break;
		//-------------------------------------------------------------------//
		//���� �� ����� � 16-� �.�.
		//-------------------------------------------------------------------//
		case NC_MODE_HEX:		
			key[index] = hex_table[rand()%16];
			break;
		//-------------------------------------------------------------------//
		//���� �� ����� [1..254]
		//-------------------------------------------------------------------//
		case NC_MODE_DEFAULT:	
			{
				while (key[index]==0 || key[index]==0xff)
					key[index] = rand() % 256;
			}
			break;
		}
	}
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//����������: ���������� ������� ���������� ����� �� ������� ����������
//���������:  key	    - [in]  �������� ��������� ����
//			  pbDestKey - [out] ���������� ����
//			  dwKeyLen	- [in]  ����� ��������� �����
//����������: ��� ������
BNC32_API HRESULT BncResizeTrainKey ( 
									 NCKEY key,
									 BYTE *pbDestKey,
									 DWORD dwKeyLen )
{
	if (!key || !pbDestKey || !dwKeyLen){
		return ERROR_BAD_ARGUMENTS;
	}
	
	ZeroMemory(pbDestKey, dwKeyLen);

	// ������ ����������� ���� ����������� ���� �� ���������� ����� �����
	NCHASH hash;		// ���
	DWORD dwLen;		// ����� ����������� ����� �����
	UINT   iStep;		// ���: ���������� ������������ ���������
	dwLen = NCKEY_SIZE;
	memcpy(pbDestKey, key, dwLen); // ���������� ���� ���� � ������ �������� �����
	iStep = NCHASH_LEN;			   // ���������� ���������� ���� ������	
	while ( dwLen < dwKeyLen )	   // ���� ��������� ���� �� ������ ������ ����������
	{
		BncCreateHash ( pbDestKey, dwLen, hash ); // ��������� ��� ����������� �����
		memcpy(pbDestKey+dwLen, hash, iStep);	  // ���������� ��� � �����
		dwLen += iStep;							  // ���������� ����� �����
		if ( dwKeyLen - dwLen < iStep )			  // ����� �� ���� �� ������� �����
			iStep = dwKeyLen - dwLen;
	}

	return ERROR_SUCCESS;
}
