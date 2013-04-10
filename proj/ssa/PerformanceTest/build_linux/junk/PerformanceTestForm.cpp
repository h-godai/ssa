#include "StdAfx.h"
#include "PerformanceTestApp.h"
#include "PerformanceTestDlg.h"
#include "PerformanceTestForm.h"




PerformanceTestForm::PerformanceTestForm(PerformanceTestDlg& parent)
: m_parent(parent)
{
}

void PerformanceTestForm::outputMessage(const std::string& msg) {
  // メッセージを追加
  m_parent.m_outputText += msg.c_str();

  // Windowが生成されていたらメッセージを送って再描画する
  if (::IsWindow(m_parent.m_hWnd)) {
    m_parent.m_console.SetWindowText(m_parent.m_outputText);
    int len = m_parent.m_outputText.GetLength();
    m_parent.m_console.SetSel(len, len);
    m_parent.m_console.ReplaceSel("", FALSE);


    m_parent.RedrawWindow();
  }

}

void PerformanceTestForm::setResultMsg(int num, const std::string& msg) {
  switch(num) {
  case 1:  m_parent.m_result1 = msg.c_str(); break;
  case 2:  m_parent.m_result2 = msg.c_str(); break;
  case 3:  m_parent.m_result3 = msg.c_str(); break;
  case 4:  m_parent.m_result4 = msg.c_str(); break;
  default:
    break;
  }
  if (::IsWindow(m_parent.m_hWnd)) {
    m_parent.UpdateData(FALSE);
  }

}




