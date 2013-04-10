#ifndef IncTest_H
#define IncTest_H


#include <vector>
#include <string>
#include <ITestBody.h>


namespace app {

  using std::vector;
  using std::string;

  class IncTest1 : public ITestBody {
  public:
    virtual ~IncTest1() {}

    virtual const char* getTestName() const { return "incliment operator test1 (map<>::iterator)"; }
    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "map::iterator++";
        case 2: return "++map::iterator";
        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return 2; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test1(sequence);
      case 2: return test2(sequence);
      default:
        return 0.0;
      }
    }
  private:
    void createSrc(int seed);
    double test1(int);
    double test2(int);
  };

  class IncTest2 : public ITestBody {
  public:
    virtual ~IncTest2() {}

    virtual const char* getTestName() const { return "incliment operator test2 (vector<>::iterator)"; }
    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "vector::iterator++";
        case 2: return "++vector::iterator";
        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return 2; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test1(sequence);
      case 2: return test2(sequence);
      default:
        return 0.0;
      }
    }

  private:
    double test1(int);
    double test2(int);
  };

} // namespace


#endif
