// PerformanceTestDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "PerformanceTestApp.h"
#include "PerformanceTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// PerformanceTestDlg ダイアログ




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


// PerformanceTestDlg メッセージ ハンドラ

BOOL PerformanceTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

    m_console.SetWindowText(m_outputText);

    for (PerformanceTestForm::nameListIterator i = m_performanceTestForm->getTestNames().begin();
      i != m_performanceTestForm->getTestNames().end(); ++i) {
      m_kindComboBox.AddString(i->c_str());
    }
    m_kindComboBox.SetCurSel(0);


	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void PerformanceTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
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

