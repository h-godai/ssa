#include "stdafx.h"
#define BOOST_CONTAINER_VECTOR_ALLOC_STATS
#include <functional>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <set>
#include <utility>
#include <algorithm>
#include <fstream>
#include "TestMain.h"
#include "SortTest.h"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include "libTS/Util/util11.hpp"
#include <ITestBody.h>
#include <boost/range/algorithm.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/container/allocator.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/set.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_set.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>

#include "std_allocator.hpp"



#ifdef _DEBUG
const int DebugFactor = 1;
#else
const int DebugFactor = 10;
#endif

namespace {

  
  typedef float LargeObjectElementType;
  const size_t LargeObjectElementCount = 512;
  const size_t ElementSize = 1200;
  const size_t NumContainers = 32;

  enum TestCase {
	  SO_ALLOCATOR2_TEST,
	  LO_ALLOCATOR2_TEST,
	  SO_ALLOCATOR2_UNITTIME,
	  LO_ALLOCATOR2_UNITTIME,
	  SO_ALLOCATOR2_INSERT,
	  LO_ALLOCATOR2_INSERT,
  };

  struct TestTable {
    TestCase case_;
	size_t loop_;
	int testCount_;
	const char* name_;
	const char* subTitle_[10];
  } testTable[] = {
	  { SO_ALLOCATOR2_TEST, 500 * DebugFactor, 6
	  , "301 Allocator2 Test (SmallObject)"
	  , { ""
	  , "std::allocator vector push_back performance"
	  , "oreore allocator vector push_back performance"
	  , "boost::vector  push_back performance"
	  , "oreore allocator boost::vector  push_back performance"
	  , "boost::allocator1 boost::vector push_back performance"
	  , "boost::allocator2 boost::vector push_back performance"
	  }},
	  { LO_ALLOCATOR2_TEST, 20 * DebugFactor, 6
	  , "302 Allocator2 Test (LargeObject)"
	  , { ""
	  , "std::allocator vector push_back performance"
	  , "oreore allocator vector push_back performance"
	  , "boost::vector  push_back performance"
	  , "oreore allocator boost::vector  push_back performance"
	  , "boost::allocator1 boost::vector push_back performance"
	  , "boost::allocator2 boost::vector push_back performance"
	  }},
	  { SO_ALLOCATOR2_UNITTIME, 100 * DebugFactor, 6
	  , "303 Allocator2 UnitTime Test (SmallObject)"
	  , { ""
	  , "std::allocator vector push_back unit time"
	  , "oreore allocator vector push_back unit time"
	  , "boost::vector  push_back unit time"
	  , "oreore allocator boost::vector  push_back unit time"
	  , "boost::allocator1 boost::vector push_back unit time"
	  , "boost::allocator2 boost::vector push_back unit time"
	  } },
	  { LO_ALLOCATOR2_UNITTIME, 10 * DebugFactor, 6
	  , "304 Allocator2 UnitTime Test (LargeObject)"
	  , { ""
	  , "std::allocator vector push_back unit time"
	  , "oreore allocator vector push_back unit time"
	  , "boost::vector  push_back unit time"
	  , "oreore allocator boost::vector  push_back unit time"
	  , "boost::allocator1 boost::vector push_back unit time"
	  , "boost::allocator2 boost::vector push_back unit time"
	  } },
	  { SO_ALLOCATOR2_INSERT, 20 * DebugFactor, 6
	  , "305 Allocator2 Insert Test (SmallObject)"
	  , { ""
	  , "std::allocator vector insert performance"
	  , "oreore allocator vector insert performance"
	  , "boost::vector  insert performance"
	  , "oreore allocator boost::vector  insert performance"
	  , "boost::allocator1 boost::vector insert performance"
	  , "boost::allocator2 boost::vector insert performance"
	  } },
	  { LO_ALLOCATOR2_INSERT, 1 * DebugFactor, 6
	  , "306 Allocator2 Insert Test (LargeObject)"
	  , { ""
	  , "std::allocator vector insert performance"
	  , "oreore allocator vector insert performance"
	  , "boost::vector  insert performance"
	  , "oreore allocator boost::vector  insert performance"
	  , "boost::allocator1 boost::vector insert performance"
	  , "boost::allocator2 boost::vector insert performance"
	  } },
  };
  const size_t TestCount = sizeof(testTable) / sizeof(TestTable);


  struct SeedObject {
    float x, y, z;
  };

  struct SmallObject {
    float x, y, z, u, v;
    SmallObject() {};
    SmallObject(const SeedObject& s) : x(s.x), y(s.y), z(s.z) {};
    SmallObject(const SmallObject& a)
      : x(a.x), y(a.y), z(a.z), u(a.u), v(a.v)
    {}
    float getx() const { return x; }
    bool operator < (const SmallObject& a) const {
      return z < a.z;
    }
    bool operator == (const SmallObject& a) const {
      return x == a.x && y == a.y && z == a.z;
    }
  };
  struct LargeObject {
    boost::array<LargeObjectElementType, LargeObjectElementCount> buffer_;
    LargeObject() : buffer_() {}
    LargeObject(const SeedObject& s) : buffer_() {
      buffer_[0] = static_cast<LargeObjectElementType>(s.x);
      buffer_[1] = static_cast<LargeObjectElementType>(s.y);
      buffer_[2] = static_cast<LargeObjectElementType>(s.z);
    }
    LargeObject(const LargeObject& src)
      : buffer_(src.buffer_)
    {}


    LargeObject(const LargeObject&& src) {
      buffer_ = std::move(src.buffer_);
    }
    LargeObject& operator = (const LargeObject&& src) {
      buffer_ = std::move(src.buffer_);
      return *this;
    }


    LargeObject& operator = (const LargeObject& src) {
      buffer_ = src.buffer_;
      return *this;
    }
    float getx() const { return buffer_[0]; }
    bool operator < (const LargeObject& a) const {
      return getkey() < a.getkey();
    }
    bool operator == (const LargeObject& a) const {
      return buffer_[0] == a.buffer_[0]
        && buffer_[1] == a.buffer_[1]
        && buffer_[2] == a.buffer_[2];
    }
    int getkey() const { return static_cast<int>(buffer_[2] / 10.0f); }
  };

  // ハッシュの計算!
  std::size_t hash_value(const SmallObject& p) {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.z);
    return seed;
  }
  std::size_t hash_value(const LargeObject& p) {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.getkey());
    return seed;
  }
}

namespace app {

  using ts::MeasureTime;
  using boost::format;
  using ts::util::push_to_container;
  using ts::util::push_or_insert_container;



  template <typename ElementType>
  class Allocator2Test : public ITestBody {
	const TestCase testCase_;
	const size_t LOOPS;
	SeedObject srcData_[ElementSize];
	int currentTestNum_;
	std::string output_;
	std::array<int64_t, ElementSize> unittimes_;
	std::ofstream ofs_;

	typedef ts::pool::simple_allocator_with_analyzer<16>  BaseAllocator;
	typedef ts::pool::legacy_allocator<ElementType, BaseAllocator> TsAllocator;
	typedef ts::pool::my_allocator<ElementType, BaseAllocator> MyAllocator2;
	typedef std::allocator<ElementType> StdAllocator;
	typedef TsAllocator MyAllocator;

	struct StlContainerAnalayzer {
		template <typename T> void begin(T& dest) {}
		template <typename T> void end(T& cont) {}
		std::string dump() { return ""; }
        std::string result() { return ""; }
		uint32_t numTests_ = 1;
	};
	struct BoostContainerAnalayzer {
		uint32_t numTests_ = 1;
		uint32_t num_expand_fwd_ = 0;
		uint32_t num_expand_bwd_ = 0;
		uint32_t num_shrink_ = 0;
		uint32_t num_alloc_ = 0;
		template <typename T>
		void begin(T& dest) {
#ifdef BOOST_CONTAINER_VECTOR_ALLOC_STATS
			dest.num_expand_fwd = 0;
			dest.num_expand_bwd = 0;
			dest.num_shrink = 0;
			dest.num_alloc = 0;
#endif
		}
		template <typename T>
		void end(T& dest) {
#ifdef BOOST_CONTAINER_VECTOR_ALLOC_STATS
			num_expand_fwd_ += dest.num_expand_fwd;
			num_expand_bwd_ += dest.num_expand_bwd;
			num_shrink_ += dest.num_shrink;
			num_alloc_ += dest.num_alloc;
#endif
		};
		std::string dump() {
			return (boost::format("BoostContainerAnalayzer: num_expand_fwd:%d, num_expand_bwd:%d, num_shrink:%d, num_alloc:%d" ENDL)
				% (num_expand_fwd_ / numTests_)
				% (num_expand_bwd_ / numTests_)
				% (num_shrink_ / numTests_)
				% (num_alloc_ / numTests_)).str();
		}
        std::string result() {
          return (boost::format("%d,%d,%d,%d")
            % (num_expand_fwd_ / numTests_)
            % (num_expand_bwd_ / numTests_)
            % (num_shrink_ / numTests_)
            % (num_alloc_ / numTests_)).str();
          }
      };

  public:
    Allocator2Test(TestCase tc)
	  : testCase_(tc)
	  , LOOPS(testTable[testCase_].loop_)
	{
	  createSeed(1, srcData_);
	}
    virtual ~Allocator2Test() {
	  outputMessage(ENDL ENDL);
	  outputMessage(output_);
	}

    const char* getTestName() const override { return testTable[testCase_].name_; }
    const char* getSubTitle(int num) override { return testTable[testCase_].subTitle_[num]; }
    size_t getTestLoops(int) const override { return ElementSize*LOOPS; }


    // テストプログラムの個数を返す!
    int getTestCount() const override { return testTable[testCase_].testCount_; }

	double hoge(int, size_t) { return 0; }

    void outputUnitTime(std::ostream& ofs_, int testNum, size_t elements) {
      // last sequence
      if (testNum == 1) {
        ofs_ << "count,";
        for (size_t n = 1; n <= elements; ++n) {
          ofs_ << n << ",";
        }
        ofs_ << std::endl;
      }
      //                 1000000000000
      double toNanoSec = 1000000000000.0 / MeasureTime::getCPUCounter();
      toNanoSec /= (TestMain::TestSequenceCount_ * LOOPS);

      ofs_ << testTable[testCase_].subTitle_[testNum] << ",";
      for (size_t n = 0; n < elements; ++n) {
        ofs_ << unittimes_[n]*toNanoSec << ",";
      }
      ofs_ << std::endl;
    }

    // テストの実行!
    double doTestImpl(int testNum, int sequence) override {
	  currentTestNum_ = testNum;
	  const size_t bufferSize = 1024*1024*256; // 256M
	  BaseAllocator alc(bufferSize);
	  TsAllocator::defaultAllocator_ = &alc;
	  double ret = 0;

	  BoostContainerAnalayzer bca;
	  StlContainerAnalayzer sca;

	  switch (testCase_) {
	  case SO_ALLOCATOR2_TEST: // 301
	  case LO_ALLOCATOR2_TEST:
	  { // 302		
		std::function<double(int, size_t)> tb[] = {
          [&](int sequence, size_t loops) { return test_push_back<std::vector<ElementType> >(sequence, loops, sca); },
          [&](int sequence, size_t loops) { return test_push_back<std::vector<ElementType, MyAllocator> >(sequence, loops, sca); },
          [&](int sequence, size_t loops) { return test_push_back<boost::container::vector<ElementType>>(sequence, loops, bca); },
          [&](int sequence, size_t loops) { return test_push_back<boost::container::vector<ElementType, MyAllocator> >(sequence, loops, bca); },
          [&](int sequence, size_t loops) { return test_push_back<boost::container::vector<ElementType, boost::container::allocator<ElementType, 1>>>(sequence, loops, bca); },
          [&](int sequence, size_t loops) { return test_push_back<boost::container::vector<ElementType, boost::container::allocator<ElementType, 2>>>(sequence, loops, bca); }
		};
		ret = tb[testNum - 1](sequence, LOOPS);

	  }
		  break;
	  case SO_ALLOCATOR2_UNITTIME: // 303
	  case LO_ALLOCATOR2_UNITTIME: // 304
	  {
		if (sequence == 1) {
			std::memset(&unittimes_[0], 0, sizeof(unittimes_));
			if (testNum == 1) {
				ofs_.open(std::string(testTable[testCase_].name_) + ".csv");
			}
		}

		std::function<double(int, size_t)> tb[] = {
			[&](int sequence, size_t loops) { return push_back_unittime<std::vector<ElementType> >(sequence, loops, sca); },
			[&](int sequence, size_t loops) { return push_back_unittime<std::vector<ElementType, MyAllocator> >(sequence, loops, sca); },
			[&](int sequence, size_t loops) { return push_back_unittime<boost::container::vector<ElementType>>(sequence, loops, bca); },
			[&](int sequence, size_t loops) { return push_back_unittime<boost::container::vector<ElementType, MyAllocator> >(sequence, loops, bca); },
			[&](int sequence, size_t loops) { return push_back_unittime<boost::container::vector<ElementType, boost::container::allocator<ElementType, 1>>>(sequence, loops, bca); },
			[&](int sequence, size_t loops) {
				return push_back_unittime<boost::container::vector<ElementType, boost::container::allocator<ElementType, 2>>>(sequence, loops, bca); },
		};
		ret = tb[testNum - 1](sequence, LOOPS);
		if (sequence == TestMain::TestSequenceCount_) {
          outputUnitTime(ofs_, testNum, ElementSize);
		}

	  }
		  break;
	  case SO_ALLOCATOR2_INSERT: // 305
	  case LO_ALLOCATOR2_INSERT: // 306
	  {
			if (sequence == 1) {
				std::memset(&unittimes_[0], 0, sizeof(unittimes_));
				if (testNum == 1) {
					ofs_.open(std::string(testTable[testCase_].name_) + ".csv");
				}
			}

			std::function<double(int, size_t)> tb[] = {
				[&](int sequence, size_t loops) { return test_insert<std::vector<ElementType> >(sequence, loops, sca); },
				[&](int sequence, size_t loops) { return test_insert<std::vector<ElementType, MyAllocator> >(sequence, loops, sca); },
				[&](int sequence, size_t loops) { return test_insert<boost::container::vector<ElementType>>(sequence, loops, bca); },
				[&](int sequence, size_t loops) { return test_insert<boost::container::vector<ElementType, MyAllocator> >(sequence, loops, bca); },
				[&](int sequence, size_t loops) { return test_insert<boost::container::vector<ElementType, boost::container::allocator<ElementType, 1>>>(sequence, loops, bca); },
				[&](int sequence, size_t loops) { return test_insert<boost::container::vector<ElementType, boost::container::allocator<ElementType, 2>>>(sequence, loops, bca); },
			};
			ret = tb[testNum - 1](sequence, LOOPS);
            if (sequence == TestMain::TestSequenceCount_) {
              outputUnitTime(ofs_, testNum, NumContainers);
            }
      }
		  break;
	  default:
		break;
	  }

	  if (sequence == 1) {
		if (alc.total_alloc_bytes_) {
		  outputMessage(str(format("allocator: memory leak! %u" ENDL) % alc.total_alloc_bytes_));
		}
	  }
	  return ret;
    }

  private:
	void dumpMemoryAnalyze() {
	  BaseAllocator& alc = *TsAllocator::defaultAllocator_;
	  outputMessage(str(format("allocator: allocated:%ubytes alloc_peak:%ubytes alloc count:%d free count:%d" ENDL) 
						% alc.total_alloc_bytes_
						% alc.peak_alloc_bytes_
						% alc.total_alloc_count_
						% alc.total_free_count_
						));
	  setAdditionalTitles("num,allocated(Kb),peak_allocated(Kb),alloc count, free count");
	  addAdditionalResults(str(format("%d,%.3f,%.3f,%d,%d") 
							   % currentTestNum_
							   % (alc.total_alloc_bytes_ / 1024.0f)
							   % (alc.peak_alloc_bytes_ / 1024.0f)
							   % alc.total_alloc_count_
							   % alc.total_free_count_
							   ));
	  output_ += str(format("allocator#%d: allocated:%ubytes alloc_peak:%ubytes alloc count:%d free count:%d" ENDL) 
					 % currentTestNum_
					 % alc.total_alloc_bytes_
					 % alc.peak_alloc_bytes_
					 % alc.total_alloc_count_
					 % alc.total_free_count_
					 );
	}
	


	template <typename CT, typename Analyzer>
	double push_back_unittime(int sequence, size_t loops, Analyzer& analyzer) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  
      time.start();
	  analyzer.numTests_ = loops;
	  for (size_t l = 0; l < loops; ++l) {
		  CT dest;
		  analyzer.begin(dest);

		  int64_t* utp = &unittimes_[0];
		  int64_t lastnow = time.getCPUCounter();
		  for (size_t n = 0; n < ElementSize; ++n) {
			  dest.emplace_back(VT(srcData_[n]));
			  int64_t now = time.getCPUCounter();
			  *utp += now - lastnow;
			  lastnow = now;
			  ++utp;
		  }
		  analyzer.end(dest);
	  }
      outputMessage(analyzer.dump());
      if (sequence == 1) {
        setAdditionalTitles("num_expand_fwd,num_expand_bwd,num_shrink,num_alloc");
        addAdditionalResults(analyzer.result());
      }

	  // sub no operation time
	  int64_t lastnow = time.getCPUCounter();
	  int64_t sum = 0;
	  for (size_t n = 0; n < ElementSize*loops; ++n) {
		  int64_t now = time.getCPUCounter();
		  sum += now - lastnow;
		  lastnow = now;
	  }
	  int64_t average = sum / ElementSize ;
	  outputMessage(str(format("No operation average time:%lld" ENDL) % average));
	  int64_t err = 0;
	  for (size_t n = 0; n < ElementSize; ++n) {
		  unittimes_[n] -= average - err;
		  if (unittimes_[n] < 1) {
			  err += 1 - unittimes_[n];
			  unittimes_[n] = 1;
		  }
	  }
	  time.stop();
      return time.getTotalTime();
	}

	template <typename CT, typename Analyzer>
    double test_push_back(int sequence, size_t loops, Analyzer& analyzer) {
		typedef typename CT::value_type VT;
		MeasureTime time;
		if (sequence == 1) {
			outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize * 1) % loops % sizeof(VT)));
		}

		time.start();
        analyzer.numTests_ = loops;
		for (size_t l = 0; l < loops; ++l) {
			CT dest;
            analyzer.begin(dest);
            for (size_t n = 0; n < ElementSize; ++n) {
				dest.emplace_back(srcData_[n]);
			}
            analyzer.end(dest);
            if (sequence == 1 && l == 0) {
				time.pause();
				//dumpMemoryAnalyze();
				time.restart();
			}
		}
		time.stop();
        outputMessage(analyzer.dump());
        if (sequence == 1) {
          setAdditionalTitles("num_expand_fwd,num_expand_bwd,num_shrink,num_alloc");
          addAdditionalResults(analyzer.result());
        }

		return time.getTotalTime();
	}

	template <typename CT, typename Analyzer>
	double test_insert(int sequence, size_t loops, Analyzer& analyzer) {
		typedef typename CT::value_type VT;
		MeasureTime time;
		if (sequence == 1) {
			outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize * 1) % loops % sizeof(VT)));
		}

		///const size_t NumContainers = 32;
        const size_t Elements = 64;

		time.start();
		time.pause();
		analyzer.numTests_ = loops;
		for (size_t l = 0; l < loops; ++l) {
          CT dest[NumContainers];
            for (auto& v : dest) {
              v.reserve(Elements * 10);
              v.resize(Elements);
              v.shrink_to_fit();
              analyzer.begin(v);
            }
            for (size_t n = 0; n < NumContainers; ++n) {
              size_t nn = (n * 2 + (n > NumContainers / 2)) % NumContainers;
              nn = n;
              auto at = dest[nn].begin() + (n * Elements / NumContainers);
              int64_t now = time.restart();
              dest[nn].insert(at, 1, srcData_[0]);
              time.pause();
              unittimes_[n] += time.lastTime() - now;
            }
			for (auto& v : dest) {
				analyzer.end(v);
			}
			if (sequence == 1 && l == 0) {
				//dumpMemoryAnalyze();
			}
		}
		time.stop();
		outputMessage(analyzer.dump());
        if (sequence == 1) {
          setAdditionalTitles("num_expand_fwd,num_expand_bwd,num_shrink,num_alloc");
          addAdditionalResults(analyzer.result());
        }
		return time.getTotalTime();
	}


	template <typename T>
	void createSeed(int seed, T& container) {
      srand(seed);
      BOOST_FOREACH(auto& e, container) {
        e.x = (1.0f + rand());
        e.y = (1.0f + rand());
        e.z = (1.0f + rand());
      }
    }

  };
} // app

namespace {
  using namespace app;
  struct ContainerTestInitializer {
  	ContainerTestInitializer() {
		TestMain::addTestBody(boost::make_shared<Allocator2Test<SmallObject>>(SO_ALLOCATOR2_TEST));      // 301
		TestMain::addTestBody(boost::make_shared<Allocator2Test<LargeObject>>(LO_ALLOCATOR2_TEST));      // 302
		TestMain::addTestBody(boost::make_shared<Allocator2Test<SmallObject>>(SO_ALLOCATOR2_UNITTIME));  // 303
		TestMain::addTestBody(boost::make_shared<Allocator2Test<LargeObject>>(LO_ALLOCATOR2_UNITTIME));  // 304
		TestMain::addTestBody(boost::make_shared<Allocator2Test<SmallObject>>(SO_ALLOCATOR2_INSERT));  // 305
		TestMain::addTestBody(boost::make_shared<Allocator2Test<LargeObject>>(LO_ALLOCATOR2_INSERT));  // 306
	}
  } allocatorTestInitializer;

} // namespace


















