#include <fstream>
#include "../unittest.hpp"
#include "../ustring.hpp"
#include "../util.hpp"


using namespace ts;

int main(int ac, char* av[]) {
  
  ustring hoho;
  mstring hehe;
  hoho.unittest(0);
  hehe.unittest(0);

  std::string log;
  bool testResult = ts::Unittest::Run("libTS::Util");
  log = ts::Unittest::Instance().getLog();

  std::ofstream ofs("unittest.log");
  ofs << log << std::endl;

  return testResult ? 0 : -1;
}
