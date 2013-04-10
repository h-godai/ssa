// DebugLog.cpp
//
// デバッグ情報を書き出す。

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>
//#define USE_TSSOCKET
#else
#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <syslog.h>
#define USE_TSSOCKET

#else
#ifdef __APPLE__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#endif
#endif
#endif

#include <string.h>
#ifdef USE_TSSOCKET
#include "libTS/PlatformDependent/Socket/TSSocket.h"
#endif

#include "Logger.h"

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

namespace ts {
  const int LogLineMax = 2048;
  static char gLogMessage[LogLineMax] = "\0";
  size_t gLogPrefixSize = 0;

  class DebugLog {
  public:
    char mDebugLogFile[512];
    FILE* mFH;
    bool mFlush;
    bool mStdOut;
    bool mStdErr;
    bool mSyslog;
    bool mDebug;
#ifdef USE_TSSOCKET
    Socket mSock;
    struct sockaddr* mSockAddr;
#endif
    DebugLog();
    ~DebugLog();
  };

  static DebugLog theDebugLog; // DebugLogの実体

  DebugLog::DebugLog()
    : mFH(NULL)
    , mFlush(false)
    , mStdOut(true)
    , mStdErr(false)
    , mSyslog(false)
#if defined(_DEBUG) || defined(DEBUG)
    , mDebug(true)
#else
    , mDebug(false)
#endif
  {
  }

  DebugLog::~DebugLog()
  {
    if (mFH) {
      fclose(mFH);
    }
  }

} // namespace
using namespace ts;
extern "C" {
  void ts_DebugLogVS(int level, va_list* args, const char* fmt);

  // set Log prefix message
  void ts_SetLogPrefix(const char* prefix) {
    strncpy(gLogMessage, prefix, LogLineMax-256);
    gLogPrefixSize = strlen(prefix);
  }

  void ts_DebugLog(const char* fmt, ...) {
    if (!theDebugLog.mDebug) return;
    va_list args;
    va_start( args, fmt );
    ts_DebugLogVS(TSLOG_DEBUG, &args, fmt);
  }

  #define DEBUGLOG_DEF(l, lv) void ts_DebugLog##l(const char* fmt, ...) { \
    va_list args; \
    va_start( args, fmt ); \
    ts_DebugLogVS(lv, &args, fmt); }

  DEBUGLOG_DEF(I, TSLOG_INFO)
  DEBUGLOG_DEF(T, TSLOG_TEST)
  DEBUGLOG_DEF(A, TSLOG_ALERT)
  DEBUGLOG_DEF(W, TSLOG_WARNING)
  DEBUGLOG_DEF(E, TSLOG_ERR)
  DEBUGLOG_DEF(F, TSLOG_FATAL)


  void ts_DebugLogVS(int level, va_list* args, const char* fmt)
  {
    static bool busy = false;
#ifdef WIN32
    int len;
#endif
    char *buffer = gLogMessage;
    const char* levelStr[] = { "DEBUG: ",
                               "INFO: ",
                               "TEST: ",
                               "ALERT: ",
                               "WARNING: ",
                               "ERROR: ",
                               "FATAL: ",
                               "",""};
#ifdef WIN32
    while(busy) Sleep(0);
#endif
    busy = true;

#ifdef WIN32
    len = _vscprintf( fmt, *args ) + 1; // _vscprintf doesn't count terminating '\0'
    if (len + gLogPrefixSize > LogLineMax) {
      strcpy( buffer + gLogPrefixSize, "LOG MESSAGE TOO LONG");
    }
    else {
      vsprintf(buffer + gLogPrefixSize, fmt, *args );
    }
    ::OutputDebugStringA(levelStr[level & 0x7]);
    ::OutputDebugStringA(buffer);
#else
    vsnprintf(buffer + gLogPrefixSize, LogLineMax - gLogPrefixSize, fmt, *args );
    va_end(*args);
#endif

    // ファイルに書き出す
    if (theDebugLog.mFH) {
      fprintf(theDebugLog.mFH, "%d:", level);
      fwrite(buffer, strlen(buffer), 1, theDebugLog.mFH);
      if (theDebugLog.mFlush) {
        fflush(theDebugLog.mFH);
      }
    }
    // stdoutに出す
    if (theDebugLog.mStdOut) { // || level == LOG_ALERT) {
      printf("%s%s", levelStr[level & 0x7], buffer);
    }
    // stderrに出す
    if (theDebugLog.mStdErr) { // || level == LOG_ALERT) {
      fprintf(stderr, "%s%s", levelStr[level & 0x7], buffer);
    }
    // syslogに出す
#ifdef LINUX
    if (theDebugLog.mSyslog) {
      syslog(level, "%s", buffer);
    }
#endif
#ifdef USE_TSSOCKET
    // Socketに出す
    if (theDebugLog.mSock.valid()) {
      if (theDebugLog.mSock.sendTo((const char*)buffer, (int)strlen(buffer), 0, theDebugLog.mSockAddr, sizeof(struct sockaddr)) < 0) {
        // エラーが起こったら停止
        theDebugLog.mSock = static_cast<SOCKET>(-1);
      }
    }
#endif

    busy = false;
  }

  void ts_SetDebuglogStderr(int f) {
    theDebugLog.mStdErr = f != 0;
  }

  void ts_SetDebuglogSyslog(int f) {
    theDebugLog.mSyslog = f != 0;
  }

  void ts_SetDebuglog(int f) {
    theDebugLog.mDebug = f != 0;
  }

  void ts_SetDebuglogFile(const char* fn, int fl) {
    theDebugLog.mFH = fopen(fn, "w");
    theDebugLog.mFlush = fl != 0;
  }

#ifdef USE_TSSOCKET
  void ts_SetDebuglogSocket(int fd, void* to) {
    theDebugLog.mSock.mFD = fd;
    theDebugLog.mSockAddr  = (struct sockaddr*)to;
  }
#else
  void ts_SetDebuglogSocket(int , void* ) {
  }
#endif

} // extern "C"


