// ITestBody.h
#pragma once

#include <boost/shared_ptr.hpp>
#include "ITestForm.h"

namespace app {
  using boost::shared_ptr;

  class ITestBody {
  public:
    typedef shared_ptr<ITestBody> ptr;

    virtual ~ITestBody() {}

    // �e�X�g��
    virtual const char* getTestName() const = 0;

    // �e�X�g�v���O�����̌���Ԃ�
    virtual int getTestCount() const = 0;

    // �e�X�g�̃T�u�^�C�g���𓾂�
    virtual const char* getSubTitle(int) {
      return "no title";
    }
    
    // �e�X�g�̎��s
    double doTest(int testNum, int sequence, ITestForm::ptr testForm) {
      testForm_ = testForm;
      return doTestImpl(testNum, sequence);
    }
    // 1�e�X�g������̃��[�v����Ԃ�
    virtual size_t getTestLoops(int) const { return 1; }

  private:
    // �e�X�g���s�̎���
    virtual double doTestImpl(int testNum, int sequence) = 0;

    // �p���N���X�p�̃T�[�r�X���[�`��
  protected:
    // ���b�Z�[�W�o��
    void outputMessage(const string& msg) {
      if (testForm_) {
        testForm_->outputMessage(msg);
      }
    }

    ITestForm::ptr testForm_;
  

  };



} // namespace



