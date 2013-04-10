// PerformanceTestDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "PerformanceTestApp.h"
#include "PerformanceTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// PerformanceTestDlg �_�C�A���O




PerformanceTestDlg::PerformanceTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PerformanceTestDlg::IDD, pParent)
    , m_result1(_T(""))
    , m_result2(_T(""))
    , m_result3(_T(""))
	, m_result4(_T(""))
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_performanceTestForm = shared_ptr<PerformanceTestForm>(new PerformanceTestForm(*this));
}

void PerformanceTestDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_RESULT1, m_result1);
  DDX_Text(pDX, IDC_EDIT_RESULT2, m_result2);
  DDX_Text(pDX, IDC_EDIT_RESULT3, m_result3);
  DDX_Text(pDX, IDC_EDIT_RESULT4, m_result4);
  DDX_Control(pDX, IDC_EDIT1, m_console);
  DDX_Control(pDX, IDC_COMBO_KIND, m_kindComboBox);
}

BEGIN_MESSAGE_MAP(PerformanceTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &PerformanceTestDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &PerformanceTestDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &PerformanceTestDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &PerformanceTestDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// PerformanceTestDlg ���b�Z�[�W �n���h��

BOOL PerformanceTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

    m_console.SetWindowText(m_outputText);

    for (PerformanceTestForm::nameListIterator i = m_performanceTestForm->getTestNames().begin();
      i != m_performanceTestForm->getTestNames().end(); ++i) {
      m_kindComboBox.AddString(i->c_str());
    }
    m_kindComboBox.SetCurSel(0);


	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void PerformanceTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void PerformanceTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR PerformanceTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void PerformanceTestDlg::OnBnClickedButton1()
{
  m_performanceTestForm->startTest(m_kindComboBox.GetCurSel(), 1);
}

void PerformanceTestDlg::OnBnClickedButton2()
{
  m_performanceTestForm->startTest(m_kindComboBox.GetCurSel(), 2);
}


void PerformanceTestDlg::OnBnClickedButton3()
{
  m_performanceTestForm->startTest(m_kindComboBox.GetCurSel(), 3);
}

void PerformanceTestDlg::OnBnClickedButton4()
{
  m_performanceTestForm->startTest(m_kindComboBox.GetCurSel(), 4);
}

