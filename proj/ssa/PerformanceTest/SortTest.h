#ifndef SORTTEST_H
#define SORTTEST_H


#include <vector>
#include <string>
#include <ITestBody.h>


namespace app {

#ifdef _DEBUG
  static const size_t ELEMENT_SIZE = 50000;
#else
  static const size_t ELEMENT_SIZE = 1000000;
#endif

  using std::vector;
  using std::string;

  class SortTest : public ITestBody {

  public:
    SortTest();
    virtual ~SortTest() {}

    virtual const char* getTestName() const { return "Quick Sort performance test"; }
    virtual size_t getTestLoops(int) const { return 1; }

    virtual const char* getSubTitle(int num) {
      switch(num) {
        case 1: return "qsort()";
        case 2: return "STL sort algorithm";
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

} // namespace


#endif
