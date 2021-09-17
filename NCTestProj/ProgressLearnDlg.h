//����������: ����������� �������� �� ����� ��������, �������� ��������� ����, 
//            ����������� ����������� ��������

#pragma once
#include "bnc32.h"

// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	// imgsCnt	- ���������� ��������� �������
	// imgs		  - ���������� ��������� �������
	// sesInf   - ������ ������ ��������
	// mxKey    - �������������� ����
	// wghtsArr - ����
	CProgressDlg(int imgsCnt, BNCSIGNIMAGE imgs[], sessionInfo *sesInf, NCKEY *mxKey, NCWEIGHTS *wghtsArr, CWnd* pParent = NULL);
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROGRESS_DLG };

private:
	// �������, ��������� ��������� ���� � ��������� ��������������� ��������� ���������
	void ProgressNeuroNetTrain(int iStability);
	
	int          mode;					// ��������� �� ������ ������ ����/�����
	UINT         imgCount;		  // ���������� ��������� �������
	BNCSIGNIMAGE *images;		    // ��������� ���������� ������
	sessionInfo  *si;			      // ������ ������ ��������
	NCKEY        *mixKey;	  	  // �������������� ����
	NCWEIGHTS    *weights;	    // ������ ����� ����
	UINT         stabGroup;			// ����� ������ ������������ � ������� ��������� ������������
	

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	bool linear_garm;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void    OnPaint();
	afx_msg void    OnClose();
	afx_msg void    OnLinear();

};
