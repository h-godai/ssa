// PerformanceTest.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��
#include "TestMain.h"
using app::TestMain;

// PerformanceTestApp:
// ���̃N���X�̎����ɂ��ẮAPerformanceTest.cpp ���Q�Ƃ��Ă��������B
//

class PerformanceTestApp : public CWinApp
{
public:
  PerformanceTestApp();

// �I�[�o�[���C�h
public:
  virtual BOOL InitInstance();

// ����

  DECLARE_MESSAGE_MAP()


};

extern PerformanceTestApp theApp;