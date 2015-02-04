// AllocatorTest
#include "stdafx.h"
#include <stdio.h>
#undef new
#undef delete
#undef malloc
#undef free


#include <vector>
#include <string>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include "libTS/PlatformDependent/Timer/MeasureTime.hpp"
#include "libTS/Util/pool/pool_storage.hpp"

#include <ITestBody.h>
#include "TestMain.h"
#include "libTS/Util/allocator.hpp"


namespace {

  enum TestCase {
    NEW_DELETE_AS_BOOST_POOL_TEST,
    TS_POOL_TEST
  };

//  const TestCase TESTCASE = NEW_DELETE_AS_BOOST_POOL_TEST; //TS_POOL_TEST;
//  const TestCase TESTCASE = TS_POOL_TEST;

  const char* TestTitle[] = {
    "003 boost pool performance test"
  , "004 TS pool_storage test"
  };

//  typedef double SmallObject;
  static const size_t NumPtrs = 8192;
  struct SmallObject {
    double value_;
    SmallObject() : value_(0.0) {
      //if (value_ != 0.0) throw std::bad_alloc("hoge");
	  if (value_ != 0.0) throw std::bad_alloc();
    }
    virtual ~SmallObject() {}
  };


//  const bool NoFreeTest = false; //true;
  const bool NoFreeTest = true;

  struct no_mutex {
    static void lock() {}
    static void unlock() {}
  };
  struct new_delete_allocator {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static char* malloc(size_type sz) {
      return new char[sz];
    }
    static void free(char* const ptr) {
      delete [] ptr;
    }
    static void* allocate(size_type n) {
	return new uint8_t[n];
    }
    static void deallocate(void* ptr) {
	delete [] reinterpret_cast<uint8_t*>(ptr);
    }
  };

  struct tag {};

}
namespace app {

  using std::vector;
  using std::string;
  using ts::MeasureTime;
  using boost::format;
  using boost::str;

  class AllocatorTest : public ITestBody {
	const TestCase TESTCASE;
	const size_t LOOPS;
  public:
	AllocatorTest(TestCase tc)
	  : TESTCASE(tc)
#ifdef _DEBUG
	  , LOOPS(TESTCASE == TS_POOL_TEST ? 500 : 30)
#else
	  , LOOPS(TESTCASE == TS_POOL_TEST ? 3000 : 1000)
#endif
	{

	}
    virtual ~AllocatorTest() {}

    virtual size_t getTestLoops(int) const { return LOOPS*NumPtrs; }

    virtual const char* getTestName() const { return TestTitle[TESTCASE]; }
    virtual const char* getSubTitle(int num) {
      switch(num + (TESTCASE*10)) {
        case 1: return "standerd new/delete";
        case 2: return "TS simple allocator";
        case 3: return "boost pool";
        case 4: return "TS pool";

        case 11: return "boost pool";
        case 12: return "boost object_pool";
        case 13: return "boost pool ordered";
        case 14: return "boost singleton_pool";
        case 15: return "TS pool";
        case 16: return "TS simple allocator";
        default: return "no test";
      }
    };

    // テストプログラムの個数を返す
    virtual int getTestCount() const { return TESTCASE == 0 ? 4 : 6; }

    // テストの実行
    virtual double doTestImpl(int testNum, int sequence) {
      switch (testNum + (TESTCASE*10)) {
      case 1: return test_new_delete(sequence);
      case 2: return test_simpleallocator(sequence);
      case 3: return test_pool(sequence);
      case 4: return test_tspool(sequence);

      case 11: return test_pool(sequence);
      case 12: return test_object_pool(sequence);
      case 13: return test_ordered(sequence);
      case 14: return test_singleton(sequence);
      case 15: return test_tspool(sequence);
      case 16: return test_simpleallocator(sequence);
      default:
        return 0.0;
      }
    }
  private:
    double test_new_delete(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      

      time.start();
#ifdef WIN32
      for (size_t l = 0; l < LOOPS/10; ++l) {
#else
      for (size_t l = 0; l < LOOPS; ++l) {
#endif
        for (size_t i = 0; i < NumPtrs; ++i) {
          ptrs[i] = new SmallObject;
		  if (ptrs[i] == 0) throw std::bad_alloc();
        }
        TestMain::tick(ptrs); // 最適化でptrsが消滅しないように
	    for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
	      delete ptrs[i];
	    }
        TestMain::tick(ptrs); // 最適化でptrsが消滅しないように
      }
      time.stop();
#ifdef WIN32
      return time.getTotalTime()*10.0;
#else
      return time.getTotalTime();
#endif

    }
    double test_pool(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      boost::pool<> poo(sizeof(SmallObject));

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          void* ptr = poo.malloc();
		  if (ptr == 0) throw std::bad_alloc();
          ptrs[i] = new (ptr) SmallObject();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo.purge_memory();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            ptrs[i]->~SmallObject();
            poo.free(ptrs[i]);
          }
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
      }
      time.stop();
      return time.getTotalTime();
    }
    double test_ordered(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      boost::pool<> poo(sizeof(SmallObject));

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          void* ptr = poo.ordered_malloc();
		  if (ptr == 0) throw std::bad_alloc();
          ptrs[i] = new (ptr) SmallObject();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo.purge_memory();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            ptrs[i]->~SmallObject();
            poo.ordered_free(ptrs[i]);
          }
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
      }
      time.stop();
      return time.getTotalTime();
    }
    double test_object_pool(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      typedef boost::object_pool<SmallObject> pool_t;
      pool_t poo;

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          ptrs[i] = poo.construct();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo.~pool_t();
          new (&poo) pool_t();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            poo.destroy(ptrs[i]);
          }
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
      }
      time.stop();
      return time.getTotalTime();
    }
    double test_tspool(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      typedef ts::pool::pool_storage<sizeof(SmallObject)
	                       , new_delete_allocator // typename Allocator
	                       , 16 // size_type Alignment
	                       , 1024 //size_type NextSize
	                       , 102400 //size_type MaxSize
	                      > pool_t;
      pool_t poo;

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          void* ptr =poo.malloc();
          ptrs[i] = (SmallObject*)ptr; //  new (ptr) SmallObject();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo.purge_memory();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            ptrs[i]->~SmallObject();
            poo.free(ptrs[i]);
          }
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
      }
      time.stop();
      return time.getTotalTime();
    }
    double test_simpleallocator(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      const size_t buffersize = 65536*8;
      char* buffer = new char[buffersize];
      typedef ts::pool::simple_allocator<8> pool_t;
      pool_t poo(buffer, buffersize);

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          void* ptr =poo.malloc(sizeof(SmallObject));
		  if (ptr == 0) throw std::bad_alloc();
          ptrs[i] = new (ptr) SmallObject();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo.purge_memory();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            ptrs[i]->~SmallObject();
            poo.free(ptrs[i]);
          }
          TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        }
      }
      time.stop();
      delete [] buffer;
      return time.getTotalTime();
    }
    double test_singleton(int times) {
      SmallObject* ptrs[NumPtrs];
      MeasureTime time;
      if (times == 1) {
        outputMessage(str(format("%d loops\r\n") % (NumPtrs*LOOPS)));
      }
      typedef boost::singleton_pool<tag, sizeof(SmallObject), new_delete_allocator, no_mutex> poo;
//      boost::object_pool<SmallObject> poo;

      time.start();
      for (size_t l = 0; l < LOOPS; ++l) {
        for (size_t i = 0; i < NumPtrs; ++i) {
          void* ptr =poo::ordered_malloc();
		  if (ptr == 0) throw std::bad_alloc();
          ptrs[i] = new (ptr) SmallObject();
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
        if (NoFreeTest) {
          poo::purge_memory();
        }
        else {
          for (size_t i = NumPtrs-1; i < NumPtrs; --i) {
            ptrs[i]->~SmallObject();
            poo::ordered_free(ptrs[i]);
          }
        }
        TestMain::tick(ptrs); // 最適化でsumが消滅しないように
      }
      time.stop();
      return time.getTotalTime();
    }
  };



	struct AllocatorTestInitializer {
  	AllocatorTestInitializer() {
    	TestMain::addTestBody(shared_ptr<ITestBody>(new AllocatorTest(NEW_DELETE_AS_BOOST_POOL_TEST)));
    	TestMain::addTestBody(shared_ptr<ITestBody>(new AllocatorTest(TS_POOL_TEST)));
	  }
	} allocatorTestInitializer;


} // namespace
