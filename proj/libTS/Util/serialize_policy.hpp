// -*-tab-width:4-*-
//
// serialize policy

#ifndef TS_SERIALIZE_POLICY
#define TS_SERIALIZE_POLICY

#include "../Common/StdTypes.h"
#include "TimeParam.hpp"

namespace ts {




  // serialize method
  inline void write(FILE* f, uint32_t d) { fprintf(f, "%u\n", d); }
  inline void write(FILE* f, time_t d) { fprintf(f, "%ld\n", static_cast<long>(d)); }
  inline void write(FILE* f, int32_t d) { fprintf(f, "%d\n", d); }
  inline void write(FILE* f, double d) { fprintf(f, "%lf\n", d); }
  inline void write(FILE* f, size_t d) { fprintf(f, "%u\n", static_cast<uint32_t>(d)); }
  inline void write(FILE* f, bool d) { fprintf(f, "%d\n", static_cast<int32_t>(d)); }
  inline void write(FILE* f, micro_time_t d) { fprintf(f, "%llu\n", d); }
  inline void write(FILE* f, const MicroTime& d) { fprintf(f, "%llu\n", d.mtime_); }
  inline void write(FILE* f, const std::string& d) { fprintf(f, "%s\n", d.c_str()); }

  inline void read(FILE* f, uint32_t& d) { fscanf(f, "%u\n", &d); }
  inline void read(FILE* f, time_t& d) { fscanf(f, "%ld\n", static_cast<long*>(&d)); }
  inline void read(FILE* f, int32_t& d) { fscanf(f, "%d\n", &d); }
  inline void read(FILE* f, double& d) { fscanf(f, "%lf\n", &d); }
  inline void read(FILE* f, size_t& d) { uint32_t s=0; fscanf(f, "%u\n", &s); d = s; }
  inline void read(FILE* f, bool& d) { uint32_t b=0; fscanf(f, "%d\n", &b); d = b != 0; }
  inline void read(FILE* f, micro_time_t& d) { fscanf(f, "%llu\n", &d); }
  inline void read(FILE* f, MicroTime& d) { fscanf(f, "%llu\n", &d.mtime_); }
  inline void read(FILE* f, std::string& d) { char buf[4096]; fgets(buf, 4096, f); d = buf; }


}


#endif
