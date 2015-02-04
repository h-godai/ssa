
// ITestForm.h
//
// PerformanceTest�̃��[�U�[�C���^�[�t�F�C�X���`���鉼�z�N���X

#ifndef APP_ITESTFORM_H
#define APP_ITESTFORM_H

#include <string>
#include <boost/shared_ptr.hpp>

#ifdef _MSC_VER
#define ENDL "\r\n" 
#else
#define ENDL "\n"
#endif



namespace app {

  using boost::shared_ptr;
  using std::string;


  class ITestFormEvent {
  public:
    typedef shared_ptr<ITestFormEvent> ptr;

    ITestFormEvent() {}
    virtual ~ITestFormEvent() {}

    // �e�X�g�J�n
    virtual void startTestEvent(int kind, int num) = 0;

    // ���Z�b�g
    virtual void reset() = 0;
    // ���ʕۑ�
    virtual void saveResult(const std::string&) = 0;
  };

  class ITestForm {
  public:
    typedef shared_ptr<ITestForm> ptr;

    ITestForm() {}
    virtual ~ITestForm() {}

    // ITestForm���p�������N���X���擾����
    static ptr getInstance();

    static bool isNull(ITestForm::ptr p) {
      return p.get() == 0;
    }

    virtual void setResultMsg(const std::string& msg)  = 0;
    virtual void setEvent(ITestFormEvent::ptr event) = 0;
    virtual void outputMessage(const string& msg) = 0;
    virtual void setResultMsg(int num, const string& msg, const string& sub) = 0;
    virtual void addTestName(const string& name) = 0;
	virtual void showResultMessages(const string& msg) {}

  };




}

#endif
