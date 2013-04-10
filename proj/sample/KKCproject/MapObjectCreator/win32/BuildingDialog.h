#pragma once
#include "afxcmn.h"
#ifndef STDAFX_H
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#endif


// BuildingDialog �_�C�A���O

class BuildingDialog : public CDialog
{
	DECLARE_DYNAMIC(BuildingDialog)

public:
	BuildingDialog(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~BuildingDialog();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG_BULDING };

    virtual BOOL OnInitDialog();

private:
  CRect m_windowRect;
  bool m_initialized;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonAddBuilding1();
  afx_msg void OnBnClickedButtonAddBuilding2();
  afx_msg void OnBnClickedButtonAddBuilding3();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnMove(int x, int y);
  CEdit m_editProperty;
  afx_msg void OnBnClickedButtonSaveBuilding();
};
