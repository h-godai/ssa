/* -*-tab-width:4-*-
 *  MachineInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_MACHINEINFO_HPP
#define CIRCUIT_TIMER_MACHINEINFO_HPP

#include <string>

#include "libTS/Util/serialize_policy.hpp";
#include "libTS/Util/TimeParam.hpp";

namespace circuit_timer {
  using namespace ts;

	struct MachineInfo {
	  double fuelCapacity_; // タンク容量
	  std::string name_;
	  MachineInfo()
		: fuelCapacity_(18.0)
		, name_("VTR1000SP-2")
	  {}
	  void serialize(FILE* f) const { 
		write(f, fuelCapacity_);
		write(f, name_);
	  }
	  void deserialize(FILE* f) {
		read(f, fuelCapacity_);
		read(f, name_);
	  }

	};
  
  
}

#endif
