#ifndef VECTORTEST_H
#define VECTORTEST_H

#include <vector>
#include <string>
#include <ITestBody.h>


namespace app {

  using std::vector;
  using std::string;

  class VectorTest : public ITestBody {
  public:
    virtual ~VectorTest() {}

    virtual const char* getTestName() const { return "001 vector_performance_test"; }
    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "iterator for() loop";
        case 2: return "for_each algorithm loop";
        case 3: return "primitive int[] and *++p loop";
        case 4: return "BOOST_FOREACH loop";
        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return 4; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum) {
      case 1: return test1(sequence);
      case 2: return test2(sequence);
      case 3: return test3(sequence);
      case 4: return test4(sequence);
      default:
        return 0.0;
      }
    }
  private:
    double test1(int);
    double test2(int);
    double test3(int);
    double test4(int);
  };

} // namespace


#endif
