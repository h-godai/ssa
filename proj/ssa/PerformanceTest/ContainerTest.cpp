#include "stdafx.h"
#define NO_BOOST_HEAP
#include <functional>
#include <vector>
#include <queue>
#include <deque>
#include <list>
#include <set>
#include <utility>
#include <algorithm>
#include "TestMain.h"
#include "SortTest.h"
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include "libTS/Util/util11.hpp"
#include <ITestBody.h>
#include <boost/range/algorithm.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/set.hpp>
#include <boost/container/flat_set.hpp>
#if !defined(NO_BOOST_HEAP)
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/priority_queue.hpp>
#endif
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_set.hpp>
#include <boost/array.hpp>

#include "std_allocator.hpp"



#ifdef _DEBUG
const int DebugFactor = 1;
#else
const int DebugFactor = 10;
#endif

namespace {

  
  typedef float LargeObjectElementType;
  const size_t LargeObjectElementCount = 512;
  const size_t ElementSize = 2000;

  enum TestCase {
	SO_PUSH_TEST,
	LO_PUSH_TEST,
	SO_EMPLACE_TEST,
	LO_EMPLACE_TEST,
	SO_INSERT_TEST,
	LO_INSERT_TEST,
	SO_MAKE_SORTED_TEST,
	LO_MAKE_SORTED_TEST,
	SO_ENUM_SORTED_TEST,
	LO_ENUM_SORTED_TEST,
	SO_SEARCH_TEST,
	LO_SEARCH_TEST,
	SO_ELEMENT_TEST,
	LO_ELEMENT_TEST,
  };

  struct TestTable {
	TestCase case_;
	size_t loop_;
	int testCount_;
	const char* name_;
	const char* subTitle_[10];
  } testTable[] = {
	{ SO_PUSH_TEST, 300 * DebugFactor, 7
	  ,	    "020 Container Push Test (SmallObject)"
	  , { ""
		  , "using std::vector"
		  , "using boost::container::vector"
		  , "using std::deque"
		  , "using boost::container::deque"
		  , "using boost::fibonacci_heap"
		  , "using std::priority_queue"
		  , "using boost::stable_vector"
	  }
	},
	{ LO_PUSH_TEST, 1 * DebugFactor, 7
	  ,	    "021 Container Push Test (LargeObject)"
	  , { ""
		  , "using std::vector"
		  , "using boost::container::vector"
		  , "using std::deque"
		  , "using boost::container::deque"
		  , "using boost::fibonacci_heap"
		  , "using std::priority_queue"
		  , "using boost::stable_vector"
	  }
	},
	{ SO_EMPLACE_TEST, 1000 * DebugFactor, 8
	  ,	    "022 Container Push vs Emplace Test (SmallObject)"
	  ,{	""
			,"push_back with std::vector"
			,"emplace_back with std::vector"
			,"push_back with boost::vector"
			,"emplace_back with boost::vector"
			,"push_back with std::deque"
			,"emplace_back with std::deque"
			,"push_back with boost::deque"
			,"emplace_back with boost:deque"
	  }
	},
	{ LO_EMPLACE_TEST, 2 * DebugFactor, 8
	  ,	    "023 Container Push vs Emplace Test (LargeObject)"
	  ,{	""
			,"push_back with std::vector"
			,"emplace_back with std::vector"
			,"push_back with boost::vector"
			,"emplace_back with boost::vector"
			,"push_back with std::deque"
			,"emplace_back with std::deque"
			,"push_back with boost::deque"
			,"emplace_back with boost:deque"
	  }
	},
	{ SO_INSERT_TEST, 1000 * DebugFactor, 6
	  ,	    "024 Container Insert Test (SmallObject)"
	  ,{	""
			,"insert to std::vector"
			,"insert to boost::vector"
			,"insert to boost::stable_vector"
			,"insert to std::deque"
			,"insert to boost::deque"
			,"insert to std::list"
	  }
	},
	{ LO_INSERT_TEST, 30 * DebugFactor, 6
	  ,	    "025 Container Insert Test (LargeObject)"
	  ,{	""
			,"insert to std::vector"
			,"insert to boost::vector"
			,"insert to boost::stable_vector"
			,"insert to std::deque"
			,"insert to boost::deque"
			,"insert to std::list"
	  }
	},
	{ SO_MAKE_SORTED_TEST, 200 * DebugFactor, 8
	  ,	    "026 Make Sorted Container Test (SmallObject)"
	  ,{	""
			,"std::vector"
			,"std::deque"
			,"boost::stable_vector"
			,"std::multiset"
			,"boost::multiset"
			,"std::priority_queue"
			,"boost::priority_queue"
			,"boost::fibonacci_heap"
	  }
	},
	{ LO_MAKE_SORTED_TEST, 2 * DebugFactor, 8
	  ,	    "027 Make Sorted Container Test (LargeObject)"
	  ,{	""
			,"std::vector"
			,"std::deque"
			,"boost::stable_vector"
			,"std::multiset"
			,"boost::multiset"
			,"std::priority_queue"
			,"boost::priority_queue"
			,"boost::fibonacci_heap"
	  }
	},
	{ SO_ENUM_SORTED_TEST, 50 * DebugFactor, 8
	  ,	    "028 Enumlate Sorted Container Test (SmallObject)"
	  ,{	""
			,"std::vector"
			,"std::deque"
			,"boost::stable_vector"
			,"std::multiset"
			,"boost::multiset"
			,"std::priority_queue"
			,"boost::priority_queue"
			,"boost::fibonacci_heap"
	  }
	},
	{ LO_ENUM_SORTED_TEST, 2 * DebugFactor, 8
	  ,	    "029 Enumlate Sorted Container Test (LargeObject)"
	  ,{	""
			,"std::vector"
			,"std::deque"
			,"boost::stable_vector"
			,"std::multiset"
			,"boost::multiset"
			,"std::priority_queue"
			,"boost::priority_queue"
			,"make boost::fibonacci_heap"
	  }
	},
	{ SO_SEARCH_TEST,100 * DebugFactor, 6
	  ,	    "030 Search Test (SmallObject)"
	  ,{	""
			,"std::vector"
			,"std::vector (sorted)"
			,"std::multiset"
			,"boost::multiset"
			,"flat_multiset"
			,"unordered_multiset"
	  }
	},
	{ LO_SEARCH_TEST,10 * DebugFactor, 6
	  ,	    "031 Search Test (LargeObject)"
	  ,{	""
			,"std::vector"
			,"std::vector (sorted)"
			,"std::multiset"
			,"boost::multiset"
			,"flat_multiset"
			,"unordered_multiset"
	  }
	},
	{ SO_ELEMENT_TEST,1000 * DebugFactor, 2
	  ,	    "090 Element Test (SmallObject)"
	  ,{	""
			,"construct"
			,"copy"
	  }
	},
	{ LO_ELEMENT_TEST,100 * DebugFactor, 2
	  ,	    "091 Element Test (LargeObject)"
	  ,{	""
			,"construct"
			,"copy"
	  }
	},
  };
  const size_t TestCount = sizeof(testTable) / sizeof(TestTable);
}

namespace app {

  using ts::MeasureTime;
  using boost::shared_ptr;
  using boost::format;
  using ts::util::push_to_container;
  using ts::util::push_or_insert_container;

  struct SeedObject {
	float x,y,z;
  };

  struct SmallObject {
	float x,y,z,u,v;
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
	int getkey() const { return static_cast<int>(buffer_[2]/10.0f); }
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

  template <typename ElementType>
  class ContainerTest : public ITestBody {
	const TestCase TESTCASE;
	const size_t LOOPS;
	SeedObject srcData_[ElementSize];
	int currentTestNum_;
	std::string output_;
	
	typedef ts::pool::simple_allocator_with_analyzer<16>  BaseAllocator;
	//typedef ts::pool::simple_allocator<16>  BaseAllocator;
	typedef ts::pool::legacy_allocator<ElementType, BaseAllocator> TsAllocator;
	typedef ts::pool::my_allocator<ElementType, BaseAllocator> MyAllocator2;
	typedef std::allocator<ElementType> StdAllocator;
	typedef TsAllocator MyAllocator;
	//typedef StdAllocator MyAllocator;

  public:
	ContainerTest(TestCase tc)
	  : TESTCASE(tc)
	  , LOOPS(testTable[TESTCASE].loop_)
	{
	  createSeed(1, srcData_);
	}
    virtual ~ContainerTest() {
	  outputMessage(ENDL ENDL);
	  outputMessage(output_);
	}

    virtual const char* getTestName() const { return testTable[TESTCASE].name_; }
    virtual const char* getSubTitle(int num) { return testTable[TESTCASE].subTitle_[num]; }
    virtual size_t getTestLoops(int) const { return ElementSize*LOOPS; }


    // テストプログラムの個数を返す!
    virtual int getTestCount() const { return testTable[TESTCASE].testCount_; }

	double hoge(int, size_t) { return 0; }
    // テストの実行!
    virtual double doTestImpl(int testNum, int sequence) {
	  currentTestNum_ = testNum;
	  const size_t bufferSize = 1024*1024*256; // 256M
	  BaseAllocator alc(bufferSize);
	  TsAllocator::defaultAllocator_ = &alc;
	  double ret = 0;

	  typedef double (ContainerTest::*testfunc)(int, size_t);
	  typedef double (ContainerTest::*testfunc3)(int, size_t, int);


	  switch (TESTCASE) {
	  case SO_PUSH_TEST: // 020
	  case LO_PUSH_TEST: // 021
		{
		  static testfunc const tb[] = {
			&ContainerTest::test_std_vector_push_back<ElementType>,
			&ContainerTest::test_boost_vector_push_back<ElementType>,
			&ContainerTest::test_std_deque_push_back<ElementType>,
			&ContainerTest::test_boost_deque_push_back<ElementType>, 
#if !defined(NO_BOOST_HEAP)
			&ContainerTest::test_fibonacci_push<ElementType>,
			&ContainerTest::test_priority_queue<ElementType>,
#endif
            &ContainerTest::test_push_back<boost::container::stable_vector<ElementType,MyAllocator2> >
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS);

		}
		break;
	  case SO_EMPLACE_TEST:
	  case LO_EMPLACE_TEST:
		{
		  static testfunc const tb[] = {
			&ContainerTest::test_std_vector_push_back<ElementType>,
			&ContainerTest::test_std_vector_emplace<ElementType>,
			&ContainerTest::test_boost_vector_push_back<ElementType>,
			&ContainerTest::test_boost_vector_emplace<ElementType>,
			&ContainerTest::test_std_deque_push_back<ElementType>,
			&ContainerTest::test_std_deque_emplace<ElementType>,
			&ContainerTest::test_boost_deque_push_back<ElementType>,
			&ContainerTest::test_boost_deque_emplace<ElementType>,
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS);
		}
		break;
	  case SO_INSERT_TEST: // 024
	  case LO_INSERT_TEST: // 025
		{
		  static testfunc const tb[] = {
			&ContainerTest::test_insert_at<std::vector<ElementType,MyAllocator>>,
			&ContainerTest::test_insert_at<boost::container::vector<ElementType,MyAllocator2>>,
			&ContainerTest::test_insert_at<boost::container::stable_vector<ElementType,MyAllocator2>>,
			&ContainerTest::test_insert_at<std::deque<ElementType,MyAllocator>>,
			&ContainerTest::test_insert_at<boost::container::deque<ElementType,MyAllocator>>,
			&ContainerTest::test_insert_at<std::list<ElementType,MyAllocator>>,
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS);
		}
		break;
	  case SO_MAKE_SORTED_TEST:
	  case LO_MAKE_SORTED_TEST:
		{
		  static testfunc3 const tb[] = {
			&ContainerTest::test_make_sorted2<std::vector<ElementType,MyAllocator>>,
			&ContainerTest::test_make_sorted2<std::deque<ElementType,MyAllocator>>,
			&ContainerTest::test_make_sorted2<boost::container::stable_vector<ElementType,MyAllocator2>>,
			&ContainerTest::test_make_sorted<std::multiset<ElementType,std::less<ElementType>, MyAllocator> >,
			&ContainerTest::test_make_sorted<boost::container::multiset<ElementType,std::less<ElementType>, MyAllocator2> >,
			&ContainerTest::test_make_sorted3<std::priority_queue<ElementType, std::vector<ElementType, MyAllocator > > >,
			//&ContainerTest::test_make_sorted3<std::priority_queue<ElementType, boost::container::deque<ElementType, MyAllocator > > >,
#if !defined(NO_BOOST_HEAP)
			&ContainerTest::test_make_sorted3<boost::heap::priority_queue<ElementType, boost::heap::allocator<MyAllocator> > >,
			&ContainerTest::test_make_sorted<boost::heap::fibonacci_heap<ElementType, boost::heap::allocator<MyAllocator> > >,
#endif
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS, 0);
		}
		break;
	  case SO_ENUM_SORTED_TEST:
	  case LO_ENUM_SORTED_TEST:
		{
		  static testfunc3 const tb[] = {
			&ContainerTest::test_make_sorted2<std::vector<ElementType,MyAllocator>>,
			&ContainerTest::test_make_sorted2<std::deque<ElementType,MyAllocator>>,
			&ContainerTest::test_make_sorted2<boost::container::stable_vector<ElementType,MyAllocator2>>,
			&ContainerTest::test_make_sorted<std::multiset<ElementType,std::less<ElementType>, MyAllocator> >,
			&ContainerTest::test_make_sorted<boost::container::multiset<ElementType,std::less<ElementType>, MyAllocator2> >,
			&ContainerTest::test_make_sorted3<std::priority_queue<ElementType, std::vector<ElementType, MyAllocator > > >,
			//&ContainerTest::test_make_sorted3<std::priority_queue<ElementType, boost::container::deque<ElementType, MyAllocator > > >,
#if !defined(NO_BOOST_HEAP)
            &ContainerTest::test_make_sorted3<boost::heap::priority_queue<ElementType, boost::heap::allocator<MyAllocator> > >,
			&ContainerTest::test_make_sorted<boost::heap::fibonacci_heap<ElementType, boost::heap::allocator<MyAllocator> > >,
#endif
          };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS, 1);
		}
		break;
	  case SO_SEARCH_TEST:
	  case LO_SEARCH_TEST:
		{
		  static testfunc const tb[] = {
			&ContainerTest::test_search2<std::vector<ElementType,MyAllocator>>,
			&ContainerTest::test_search3<std::vector<ElementType,MyAllocator>>,
			&ContainerTest::test_search<std::multiset<ElementType,std::less<ElementType>, MyAllocator> >,
			&ContainerTest::test_search<boost::container::multiset<ElementType,std::less<ElementType>, MyAllocator2> >,
			&ContainerTest::test_search<boost::container::flat_multiset<ElementType,std::less<ElementType>, MyAllocator2> >,
			&ContainerTest::test_search<boost::unordered_multiset<ElementType,boost::hash<ElementType>, std::equal_to<ElementType>, MyAllocator2> >,
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS);
		}
		break;
				
	  case SO_ELEMENT_TEST:
	  case LO_ELEMENT_TEST:
		{
		  static testfunc const tb[] = {
			&ContainerTest::constructor_test<ElementType>,
			&ContainerTest::copy_test<ElementType>,
		  };
		  ret = (this->*tb[testNum-1])(sequence, LOOPS);
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
	template <typename CT>
	double test_search(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
	  typedef typename CT::iterator iterator;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }

	  std::vector<VT> src;
	  CT dest;
	  src.reserve(ElementSize);
	  for (size_t c= 0; c < ElementSize; ++c) {
		src.emplace_back(srcData_[c]);
		push_or_insert_container(dest, srcData_[c]);
	  }
	  
      time.start();
	  float sum = 0.0f;
      for (size_t l = 0; l < loops; ++l) {
		for (size_t c= 0; c < ElementSize; ++c) {
		  iterator found = dest.find(src[c]);
		  sum += found->getx();
		}
	  }
      time.stop();
	  if (sequence == 1) {
		outputMessage(str(format("sum=%f" ENDL) % sum));
		dumpMemoryAnalyze();
	  }
      return time.getTotalTime();
	}

	// vectorなどfindメソッドが無いコンテナ用
	template <typename CT>
	double test_search2(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
	  typedef typename CT::iterator iterator;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }

	  std::vector<VT> src;
	  CT dest;
	  src.reserve(ElementSize);
	  for (size_t c= 0; c < ElementSize; ++c) {
		src.emplace_back(srcData_[c]);
		dest.emplace_back(srcData_[c]);
	  }
	  
      time.start();
	  float sum = 0.0f;
      for (size_t l = 0; l < loops; ++l) {
		for (size_t c= 0; c < ElementSize; ++c) {
		  iterator found = boost::find(dest, src[c]);
		  sum += found->getx();
		}
	  }
      time.stop();
	  if (sequence == 1) {
		outputMessage(str(format("sum=%f" ENDL) % sum));
		dumpMemoryAnalyze();
	  }
      return time.getTotalTime();
	}
	// vectorなどfindメソッドが無いコンテナ用(ソート済み)
	template <typename CT>
	double test_search3(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
	  typedef typename CT::iterator iterator;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }

	  std::vector<VT> src;
	  CT dest;
	  src.reserve(ElementSize);
	  for (size_t c= 0; c < ElementSize; ++c) {
		src.emplace_back(srcData_[c]);
		dest.emplace_back(srcData_[c]);
	  }
	  boost::sort(dest);
	  
      time.start();
	  float sum = 0.0f;
      for (size_t l = 0; l < loops; ++l) {
		for (size_t c= 0; c < ElementSize; ++c) {
		  iterator found = boost::lower_bound(dest, src[c]);
		  sum += found->getx();
		}
	  }
      time.stop();
	  if (sequence == 1) {
		outputMessage(str(format("sum=%f" ENDL) % sum));
		dumpMemoryAnalyze();
	  }
      return time.getTotalTime();
	}

	
	template <typename VT>
	double constructor_test(int sequence, size_t loops) {
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  void* buf = malloc(sizeof(VT));
      time.start();
	  float sum = 0.0f;
      for (size_t l = 0; l < loops; ++l) {
		for (size_t c= 0; c < ElementSize; ++c) {
		  VT* p = new(buf) VT(srcData_[c]);
		  sum += p->getx();
		}
	  }
      time.stop();
	  free(buf);
	  if (sequence == 1) {
		outputMessage(str(format("sum=%f" ENDL) % sum));
	  }
      return time.getTotalTime();
	}
	template <typename VT>
	double copy_test(int sequence, size_t loops) {
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  VT dest;
	  static VT src[ElementSize];
	  for (size_t c = 0; c < ElementSize; ++c) { src[c] = srcData_[0]; }
	  time.start();
	  float sum = 0.0f;
      for (size_t l = 0; l < loops; ++l) {
		for (size_t c= 0; c < ElementSize; ++c) {
		  dest = src[c];
		  sum += dest.getx();
		}
	  }
      time.stop();
	  if (sequence == 1) {
		outputMessage(str(format("sum=%f" ENDL) % sum));
	  }
      return time.getTotalTime();
	}

	
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
	
	template <typename CT>
	double test_make_sorted(int sequence, size_t loops, int ef) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
	  size_t elements = ElementSize;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (elements*loops) % (elements) % loops % sizeof(VT)));
      }
	  float sum = 0.0f;
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		if (ef) { time.pause(); }
		CT dest;
		for (size_t n = 0; n < elements; ++n) {
		  push_or_insert_container(dest, VT(srcData_[n]));
		}
		if (ef) {
		  time.restart();
		  ts::util::foreach_precedence_ordered(dest,[&](const VT& v) {
			  sum += v.getx();
			});
		}

		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
	  if (sequence == 10) {
		outputMessage(str(format("sum= %f" ENDL) % sum));
	  }
      return time.getTotalTime();
	}

	template <typename CT>
	double test_make_sorted3(int sequence, size_t loops, int ef) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
	  size_t elements = ElementSize;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (elements*loops) % (elements) % loops % sizeof(VT)));
      }
	  float sum = 0.0f;
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		if (ef) { time.pause(); }
		CT dest;
		for (size_t n = 0; n < elements; ++n) {
		  push_or_insert_container(dest, VT(srcData_[n]));
		}
		if (ef) {
		  time.restart();
		  while (!dest.empty()) { sum += dest.top().getx(); dest.pop(); }
		}

		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
	  if (sequence == 10) {
		outputMessage(str(format("sum= %f" ENDL) % sum));
	  }
      return time.getTotalTime();
	}

	
	template <typename CT>
	double test_make_sorted2(int sequence, size_t loops, int ef) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
	  size_t elements = ElementSize;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (elements*loops) % (elements) % loops % sizeof(VT)));
      }
	  float sum = 0.0f;
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		if (ef) { time.pause(); }
		CT dest;
		for (size_t n = 0; n < elements; ++n) {
		  //printf("%f\n", srcData_[n].x);
		  dest.emplace_back(srcData_[n]);
		}
		boost::sort(dest);
		if (ef) {
		  time.restart();
		  std::for_each(dest.begin(), dest.end(), [&](const VT& v) {
			  sum += v.getx();
			});
		}
		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
	  if (sequence == 10) {
		outputMessage(str(format("sum= %f" ENDL) % sum));
	  }
      return time.getTotalTime();
	}

	template <typename CT>
	double test_insert_at(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
	  typedef typename CT::iterator CT_iterator;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  CT dest;
	  for (size_t n = 0; n < ElementSize/2; ++n) {
		push_to_container(dest, VT(srcData_[n]));
	  }
	  srand(1); // set seed 1
	  
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		time.pause(); // イテレータ生成の時間はカウントしない!
		size_t at = (rand() % (ElementSize/2)); //dest.size());
		CT_iterator it = dest.begin();
		for (size_t c = 0; c < at; ++c, ++it); // itを進める!
		time.restart();
		dest.insert(it, VT(srcData_[at]));
	  }
      time.stop();
	  if (sequence == 1) dumpMemoryAnalyze();
      return time.getTotalTime();
	}



	
	template <typename CT>
	double test_push_back(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		CT dest;
		for (size_t n = 0; n < ElementSize; ++n) {
		  //dest.push_back(VT(srcData_[n]));
		  push_to_container(dest, VT(srcData_[n]));
		}
		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
      return time.getTotalTime();
	}


	template <typename CT>
	double test_push(int sequence, size_t loops) {
	  typedef typename CT::value_type VT;
      MeasureTime time;
      if (sequence == 1) {
        outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(VT)));
      }
	  
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		CT dest;
		for (size_t n = 0; n < ElementSize; ++n) {
		  push_to_container(dest, VT(srcData_[n]));
		  //dest.push(VT(srcData_[n]));
		}
		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
      return time.getTotalTime();
	}

	template <typename CT>
	double test_emplace_back(int sequence, size_t loops) {
      MeasureTime time;
      if (sequence == 1) {
		outputMessage(str(format("%d samples (%u Elements x %u Loop) ContainerSize:%u" ENDL) % (ElementSize*loops) % (ElementSize*1) % loops % sizeof(typename CT::value_type)));
      }
      time.start();
      for (size_t l = 0; l < loops; ++l) {
		CT dest;
		for (size_t n = 0; n < ElementSize; ++n) {
		  dest.emplace_back(srcData_[n]);
		}
		if (sequence == 1 && l == 0) {
		  time.pause();
		  dumpMemoryAnalyze();
		  time.restart();
		}
	  }
      time.stop();
      return time.getTotalTime();
	}
	

	template <typename OT>
	double test_std_vector_push_back(int sequence, size_t loops) {
	  return test_push_back<std::vector<OT, MyAllocator> >(sequence, loops);
	}
	template <typename OT>
	double test_boost_vector_push_back(int sequence, size_t loops) {
	  return test_push_back<boost::container::vector<OT,MyAllocator > >(sequence, loops);
	}
	template <typename OT>
	double test_std_deque_push_back(int sequence, size_t loops) {
	  return test_push_back<std::deque<OT,MyAllocator> >(sequence, loops);
	}
	template <typename OT>
	double test_boost_deque_push_back(int sequence, size_t loops) {
	  return test_push_back<boost::container::deque<OT,MyAllocator> >(sequence, loops);
	}
	
#if !defined(NO_BOOST_HEAP)
    template <typename OT>
	double test_priority_queue(int sequence, size_t loops) {
	  return test_push<std::priority_queue<OT,std::vector<OT,MyAllocator> > >(sequence, loops);
	}

	template <typename OT>
	double test_fibonacci_push(int sequence, size_t loops) {
	  return test_push<boost::heap::fibonacci_heap<OT, boost::heap::allocator<MyAllocator> > >(sequence, loops);
	}
#endif
	template <typename OT>
	double test_std_vector_emplace(int sequence, size_t loops) {
	  return test_emplace_back<std::vector<OT,MyAllocator> >(sequence, loops);
	}
	template <typename OT>
	double test_boost_vector_emplace(int sequence, size_t loops) {
	  return test_emplace_back<boost::container::vector<OT,MyAllocator> >(sequence, loops);
	}
	template <typename OT>
	double test_std_deque_emplace(int sequence, size_t loops) {
	  return test_emplace_back<std::deque<OT,MyAllocator> >(sequence, loops);
	}
	template <typename OT>
	double test_boost_deque_emplace(int sequence, size_t loops) {
	  return test_emplace_back<boost::container::deque<OT,MyAllocator> >(sequence, loops);
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
  using boost::shared_ptr;
  struct ContainerTestInitializer {
  	ContainerTestInitializer() {
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_PUSH_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_PUSH_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_EMPLACE_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_EMPLACE_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_INSERT_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_INSERT_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_MAKE_SORTED_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_MAKE_SORTED_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_ENUM_SORTED_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_ENUM_SORTED_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_SEARCH_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_SEARCH_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<SmallObject>(SO_ELEMENT_TEST)));
	  TestMain::addTestBody(shared_ptr<ITestBody>(new ContainerTest<LargeObject>(LO_ELEMENT_TEST)));
	}
  } allocatorTestInitializer;

} // namespace


















