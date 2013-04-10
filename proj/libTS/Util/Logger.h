/* Logger.h
//
// デバッグ情報をファイルに書き出す
//
//
*/

#ifndef LIBTS_LOGGER_H
#define LIBTS_LOGGER_H


#ifdef RELEASE
#  if defined(WIN32)
#    define TS_DEBUGLOG __noop
#  else
#    define TS_DEBUGLOG(...)  // 0&&
#  endif
#else
#  define TS_DEBUGLOG ts_DebugLog
#endif

#define TS_INFOLOG ts_DebugLogI
#define TS_TESTLOG ts_DebugLogT
#define TS_ALERTLOG ts_DebugLogA
#define TS_WARNINGLOG ts_DebugLogW
#define TS_ERRORLOG ts_DebugLogE
#define TS_FATALLOG ts_DebugLogF

#define TS_SETLOGFILE(x,f) ts_SetDebuglogFile(x,f)
#define TS_SETLOGSTDERR(x) ts_SetDebuglogStderr(x)
#define TS_SETLOGSYSLOG(x) ts_SetDebuglogSyslog(x)
#define TS_DEBUGLOG_ON ts_SetDebuglog(true)
#define TS_DEBUGLOG_OFF ts_SetDebuglog(false)


enum  {
  TSLOG_DEBUG,
  TSLOG_INFO,
  TSLOG_TEST,
  TSLOG_ALERT,
  TSLOG_WARNING,
  TSLOG_ERR,
  TSLOG_FATAL
};

#ifdef __cplusplus
extern "C" {
#endif

  void ts_DebugLog(const char* fmt, ...);
  void ts_DebugLogI(const char* fmt, ...);
  void ts_DebugLogT(const char* fmt, ...);
  void ts_DebugLogA(const char* fmt, ...);
  void ts_DebugLogW(const char* fmt, ...);
  void ts_DebugLogE(const char* fmt, ...);
  void ts_DebugLogF(const char* fmt, ...);

  void ts_SetDebuglogFile(const char* fn, int flushflag); // debugログファイル名
  void ts_SetDebuglogStderr(int);
  void ts_SetDebuglogSyslog(int);
  void ts_SetDebuglog(int);
  void ts_SetDebuglogSocket(int fd, void* to);
  void ts_SetLogPrefix(const char* prefix);
#ifdef __cplusplus
}
#endif




#endif
