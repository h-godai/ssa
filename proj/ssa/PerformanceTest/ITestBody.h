// ITestBody.h -*-c++-*-
#pragma once

#include <string.h>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "ITestForm.h"

namespace app {
  using boost::shared_ptr;
  using std::string;

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

	bool operator < (const ITestBody& b) const {
	  return string(getTestName()) < string(b.getTestName());
	}
	
	const string& getAdditionalTitles() const { return additionalTitles_; }
	string getAdditionalResult(size_t n) const { 
	  if (n < additionalResults_.size()) return additionalResults_[n];
	  return "";
	}

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

	void setAdditionalTitles(const string& s) { additionalTitles_ = s; }
	void addAdditionalResults(const string& s)  { additionalResults_.push_back(s); }


    ITestForm::ptr testForm_;
	std::string additionalTitles_;
	std::vector<std::string> additionalResults_;
  

  };

  inline bool operator < (const ITestBody::ptr& a, const ITestBody::ptr& b) {
	return *a < *b;
  }

} // namespace



