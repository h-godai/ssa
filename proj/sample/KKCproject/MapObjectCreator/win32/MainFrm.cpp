// MainFrm.cpp : MainFrame �N���X�̎���
//

#include "stdafx.h"
#include "MapObjectCreatorWinApp.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace app;

namespace {
   // AppStatus���ω������Ƃ��ɃR�[���o�b�N�����C�x���g�n���h��
  class AppStatusEventMainFrame : public IAppStatusEventHandler {
    MainFrame* m_mainFrame;
  public:
    typedef shared_ptr<AppStatusEventMainFrame> ptr;
    AppStatusEventMainFrame(MainFrame* mainFrame) : m_mainFrame(mainFrame) {}
    virtual ~AppStatusEventMainFrame() {}

    // StatusText���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
    virtual void onSetStatusText(const string& str) {
      m_mainFrame->setStatusBarText(str);
    }
  };
}


// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_EN_CHANGE(IDC_EDIT_CAMERA_POS, &MainFrame::OnEnChangeEditCameraPos)
    ON_BN_CLICKED(IDC_BUTTON_CAMERA__BANK, &MainFrame::OnBnClickedButtonCamera)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // �X�e�[�^�X ���C�� �C���W�P�[�^
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// MainFrame �R���X�g���N�V����/�f�X�g���N�V����

MainFrame::MainFrame()
: m_statusText(5)
{
	// TODO: �����o�������R�[�h�������ɒǉ����Ă��������B
}

MainFrame::~MainFrame()
{
  AppStatusManager::RemoveEventHandler(this);
}


int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// �t���[���̃N���C�A���g�̈�S�̂��߂�r���[���쐬���܂��B
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("�r���[ �E�B���h�E���쐬�ł��܂���ł����B\n");
		return -1;
	}
#if 0
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("�c�[�� �o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;      // �쐬�ł��܂���ł����B
	}
#endif
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("�_�C�A���O�o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;		// �쐬�ł��܂���ł����B
	}
    m_wndDlgBar.initialize();

    if (!m_wndEditTexMenuBar.Create(this, EditTextureMenuBar::IDD, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("�_�C�A���O�o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;		// �쐬�ł��܂���ł����B
	}
    m_wndEditTexMenuBar.initialize();

    if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndDlgBar),// ) //||
		!m_wndReBar.AddBar(&m_wndEditTexMenuBar) ) //||
		//!m_wndReBar.AddBar(&m_wndToolBar)) // ||
	{
		TRACE0("rebar �̍쐬�Ɏ��s���܂����B\n");
		return -1;      // �쐬�ł��܂���ł����B
	}
    m_wndEditTexMenuBar.ShowWindow(SW_HIDE);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("�X�e�[�^�X �o�[�̍쐬�Ɏ��s���܂����B\n");
		return -1;      // �쐬�ł��܂���ł����B
	}

	// TODO: �c�[�� �q���g��\�����Ȃ��ꍇ�́A�폜���Ă��������B
	//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |CBRS_TOOLTIPS | CBRS_FLYBY);

//    AppStatusManager::AddEventHandler(IAppStatusEventHandler::ptr(new AppStatusEventMainFrame(this)));
    AppStatusManager::AddEventHandler(this);
	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
    cs.x  = theApp.GetProfileInt(REG_MAINWINDOW, REG_LEFT, 0);
    cs.y  = theApp.GetProfileInt(REG_MAINWINDOW, REG_TOP,  0);
    cs.cx = theApp.GetProfileInt(REG_MAINWINDOW, REG_WIDTH, 1024);
    cs.cy = theApp.GetProfileInt(REG_MAINWINDOW, REG_HEIGHT, 768);
	return TRUE;
}


// MainFrame �f�f

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// MainFrame ���b�Z�[�W �n���h��

void MainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// �r���[ �E�B���h�E�Ƀt�H�[�J�X��^���܂��B
	m_wndView.SetFocus();
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// �r���[�ɍŏ��ɃR�}���h����������@���^���܂��B
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ȊO�̏ꍇ�́A����̏������s���܂��B
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



BOOL MainFrame::OnEraseBkgnd(CDC* /*pDC*/)
{
  // TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

  return TRUE; //CFrameWnd::OnEraseBkgnd(pDC);
}

void MainFrame::OnSize(UINT nType, int cx, int cy)
{
  CFrameWnd::OnSize(nType, cx, cy);


  this->GetWindowRect(m_windowRect);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_LEFT, m_windowRect.left);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_TOP, m_windowRect.top);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_WIDTH, m_windowRect.Width());
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_HEIGHT, m_windowRect.Height());
}

void MainFrame::OnMove(int x, int y)
{
  CFrameWnd::OnMove(x, y);

  this->GetWindowRect(m_windowRect);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_LEFT, m_windowRect.left);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_TOP, m_windowRect.top);
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_WIDTH, m_windowRect.Width());
  theApp.WriteProfileInt(REG_MAINWINDOW, REG_HEIGHT, m_windowRect.Height());
}

void MainFrame::OnEnChangeEditCameraPos()
{
  // TODO:  ���ꂪ RICHEDIT �R���g���[���̏ꍇ�A
  // �܂��ACFrameWnd::OnInitDialog() �֐����I�[�o�[���C�h���āAOR ��Ԃ� ENM_CHANGE
  // �t���O���}�X�N�ɓ���āACRichEditCtrl().SetEventMask() ���Ăяo���Ȃ�����A
  // �R���g���[���́A���̒ʒm�𑗐M���܂���B

  // TODO:  �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����Ă��������B
}


void MainFrame::setCameraPosString(const string& eye, const string& target, const string& fovy) {
  CEdit* editEye    = static_cast<CEdit*>(m_wndDlgBar.GetDlgItem(IDC_EDIT_CAMERA_POS));
  CEdit* editTarget = static_cast<CEdit*>(m_wndDlgBar.GetDlgItem(IDC_EDIT_CAMERA_TARGET));
  CEdit* editFovy   = static_cast<CEdit*>(m_wndDlgBar.GetDlgItem(IDC_EDIT_CAMERA_FOVY));
  if (editEye != 0)    editEye->SetWindowText(eye.c_str());
  if (editTarget != 0) editTarget->SetWindowText(target.c_str());
  if (editFovy != 0)   editFovy->SetWindowText(fovy.c_str());
}


void MainFrame::OnBnClickedButtonCamera()
{
  // TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
}

void MainFrame::setStatusBarText(const std::string& text) {
  if (text.length() < 2 || text[1] != ':' || text[0] < '0' || text[0] > '4') {
    // ���������̂�tesxt0
    m_statusText[0] = text;
  }
  else {
    int n = text[0] - '0';
    if (static_cast<int>(m_statusText.size()) <= n) m_statusText.resize(n+1);
    m_statusText[n] = text.substr(2);
  }
  CString msg;
  msg.Format("%s  %s  %s  %s  %s", 
    m_statusText[1].c_str(),
    m_statusText[2].c_str(),
    m_statusText[3].c_str(),
    m_statusText[4].c_str(),
    m_statusText[0].c_str());

  
  m_wndStatusBar.SetWindowText(msg);
}

void MainFrame::setCutBuidingText(const std::string& text) {
  CButton* btn = static_cast<CButton*>(m_wndDlgBar.GetDlgItem(IDC_BUTTON_CUT_BULDING));
  if (btn != 0) {
    btn->SetWindowText(text.c_str());
  }
}

// �C�x���g�n���h��


// StatusText���ύX���ꂽ�Ƃ��ɌĂ΂��C�x���g
void MainFrame::onSetStatusText(const string& str) {
  setStatusBarText(str);
}



void MainFrame::onSetAppMode(AppMode mode) {
  if (!::IsWindow(m_wndEditTexMenuBar.m_hWnd)  ||
      !::IsWindow(m_wndDlgBar.m_hWnd)  ||
      !::IsWindow(m_wndReBar.m_hWnd))
      return;

  switch(mode) {
  case APP_MAP_VIEW_MODE:
  case APP_EDIT_BUILDING_MODE:
  case APP_CALC_CAMERAPOS_MODE:
    m_wndEditTexMenuBar.ShowWindow(SW_HIDE);
    m_wndDlgBar.ShowWindow(SW_SHOW);
    m_wndReBar.CalcFixedLayout(0,0);
    break;
  case APP_EDIT_TEXTURE_MODE:
    m_wndDlgBar.ShowWindow(SW_HIDE);
    m_wndEditTexMenuBar.ShowWindow(SW_SHOW);
    m_wndReBar.CalcFixedLayout(0,0);
    break;
  default:
    break;
  }
}

