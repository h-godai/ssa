
#ifndef TS_SHARED_MEMORY_CTRL_LINUX
#define TS_SHARED_MEMORY_CTRL_LINUX

#include <map>
#include <vector>
#include <boost/foreach.hpp>
#include <algorithm>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


namespace ts { 
namespace shm {
  using std::vector;
  using std::map;
  using std::remove;

  // linux用の定義
  template <>
  struct SharedMemoryCtrl<ts::PLATFORM_LINUX> {
    uint32_t keyBase_;
    static const uint32_t DefaultKeyBase = 0x40545340; // @TS@
    static const int Perm = 0600; // permission
    static map<uint32_t, int> keyMap_;
    static vector<int> createList_;
    SharedMemoryCtrl() : keyBase_(DefaultKeyBase) {}
    
    static SharedMemoryCtrl& Instance() {
      static SharedMemoryCtrl ins;
      return ins;
    }

    static void SetKeyBase(uint32_t kb) { Instance().keyBase_ = kb; }
    static uint32_t GetKeyBase() { return Instance().keyBase_; }
    static key_t MakeKey(uint32_t key) { return static_cast<key_t>(GetKeyBase() + key); }
    static void Release() {
      BOOST_FOREACH(int a, createList_) {
	::shmctl(a, IPC_RMID, 0);
	//printf("Release shm %d\n", a);
      }
      createList_.clear();
    }

    // 共有メモリ作成して、バッファのポインタを返す
    // @throw SharedMemoryException
    static void* Create(uint32_t key, size_t size) {

      int result = ::shmget(MakeKey(key), size, IPC_CREAT|IPC_EXCL|Perm);
	
      if (result != -1) {
	if (createList_.empty()) {
	  ::atexit(Release); // プロセス終了時に破棄するように指示
	}
	createList_.push_back(result); // Createした共有メモリを覚えておく
	keyMap_[key] = result;
	return ::shmat(result, 0, 0);
      }
      else {
	std::string msg("SharedMemoryCtrl:Create:");
	msg += ::strerror(errno);
	Destroy(key); // とりあえず破棄
	throw SharedMemoryException(msg.c_str());
      }
    }

    // 既存の共有メモリをアタッチして、バッファのポインタを返す
    // @throw SharedMemoryException
    static void * Attach(uint32_t key) {
      int result = ::shmget(MakeKey(key), 0, Perm);
      if (result != -1) {
	keyMap_[key] = result;
	return ::shmat(result, 0, 0);
      }
      else {
	std::string msg("SharedMemoryCtrl:Attach:");
	msg += ::strerror(errno);
	throw SharedMemoryException(msg.c_str());
      }
    }
    // 共有メモリを破棄する
    static void Destroy(uint32_t key) {
      int id = ::shmget(MakeKey(key), 0, Perm);
      ::shmctl(id, IPC_RMID, 0);
      keyMap_.erase(key);
      // idの要素を削除
      createList_.erase(remove(createList_.begin(), createList_.end(), id), createList_.end());
    }  
  };

  map<uint32_t, int> SharedMemoryCtrl<PLATFORM_LINUX>::keyMap_;
  vector<int> SharedMemoryCtrl<PLATFORM_LINUX>::createList_;

}} // namespace
#endif

  
