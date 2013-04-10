// unittest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include <stdio.h>
#include "IThread.hpp"

using namespace tsl;


void* proc1(IThread* myThread, void* userPtr) {
  int tick = 0;
  for (;;) {
    if (myThread->wait(1000) == ICondition::WaitSignaled) break;
    printf("%s: tick %d\n", userPtr, tick++);
  }

  return 0;
}


void* mainProc(IThread* myThread, void* userPtr) {

  IThread* thread1 = IThread::createThread(proc1, (void*)"proc1");

  bool finish = false;
  while(finish == false) {
    switch(thread1->getThreadStatus()) {
    case IThread::UNINITIALIZED:
    case IThread::INITIALIZED:
      thread1->run();
      if (thread1->waitForStatusIs(IThread::RUNNING, 1000)) {
	printf("proc1 status is %d\n", thread1->getThreadStatus());
	printf("proc1 is running wait 5sec\n");
	myThread->sleep(5000);
      }
      else {
	printf("waitForStatus faild. now status is %d\n", thread1->getThreadStatus());
      }
      break;
    case IThread::RUNNING:
      printf("send signal to proc1\n");
      thread1->signal();
      if (thread1->waitForStatusIs(IThread::END, 1000)) {
	printf("status is END\n");
      }
      break;
    case IThread::END:
      printf("proc1 finished\n");
      finish = true;
      break;
    default:
      break;
    }
  }
  IThread::deleteThread(thread1);

  return 0;
}


int main(int /*argc*/, char* /*argv*/ [])
{
  IThread* mainThread = IThread::createThread(mainProc, (void*)"main");
  mainThread->run();
  mainThread->join();
  printf("main end\n");
  IThread::deleteThread(mainThread);
  return 0;
}

