/* -*-tab-width:4-*-
 *  CircuitInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_CIRCUITINFO_HPP
#define CIRCUIT_TIMER_CIRCUITINFO_HPP

#include <string>

#include "libTS/Util/serialize_policy.hpp";
#include "libTS/Util/TimeParam.hpp";

namespace circuit_timer {
  using namespace ts;

  struct CircuitInfo {
	double courseLength_; //
	MicroTime minimumLapTime_; //
	std::string name_;
	MicroTime scLapTime_;         // セーフティーカーの平均ラップタイム
	CircuitInfo()
	  : courseLength_(3.73750)
	  , minimumLapTime_(10*1000000)
	  , name_("スポーツランドSUGO本コース")
	  , scLapTime_(MicroTime(3*60.0))
	{}
	
	void serialize(FILE* f) const {
	  write(f, courseLength_); //
	  write(f, minimumLapTime_); //
	  write(f, name_);
	}
	void deserialize(FILE* f) {
	  read(f, courseLength_); //
	  read(f, minimumLapTime_); //
	  read(f, name_);
	}
	
  };
  
  
}

#endif
