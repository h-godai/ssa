/* -*-tab-width:4-*-
 *  TeamInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_TEAMINFO_HPP
#define CIRCUIT_TIMER_TEAMINFO_HPP

#include "libTS/Util/serialize_policy.hpp"
#include "libTS/Util/TimeParam.hpp"

#include "MachineInfo.hpp"

namespace circuit_timer {
  using namespace ts;

	enum PitInTimingType {
	  WhenFuelEmptyLaps,  // 燃料がゼロになるまでの周回数
	  RunningTimeMinits   // 連続走行時間
	};

	struct TeamInfo {
	  std::string name_;      // チーム名
	  uint32_t numOfRiders_;         // ライダー人数
	  MicroTime pitStopTime_;         // ピットストップの基本時間
	  MicroTime withFullFuelTime_;    // 給油時にプラスされる時間
	  MicroTime withChangeWheelsTime_; // タイヤ交換時にプラスされる時間
	  MicroTime inLapLossTime_;     // インラップに要するロスタイム
	  MicroTime outLapLossTime_;    // アウトラップに要するロスタイム
	  double sightingFuelCost_;  // サイティングラップ中の燃費
	  double pitInTimingValue_; // ピットインタイミング値
	  PitInTimingType pitInTimingType_; // ピットインタイミングの種別
	  MachineInfo machineInfo_;

	  TeamInfo() 
		: name_("スーパーツインズ&BLUE-EYES")
		, numOfRiders_(3)
		, pitStopTime_((10.0))
		, withFullFuelTime_((60.0))
		, withChangeWheelsTime_((40.0))
		, inLapLossTime_((30.0))
		, outLapLossTime_((30.0))
		, sightingFuelCost_(10.0)
		, pitInTimingValue_(3.0)
		, pitInTimingType_(WhenFuelEmptyLaps)
	  {
	  }

	  void serialize(FILE* f) const { 
		machineInfo_.serialize(f);
	  }
	  void deserialize(FILE* f) {
		machineInfo_.deserialize(f);
	  }

	  bool isPitInTiming(double canLaps) {
		switch (pitInTimingType_) {
		case WhenFuelEmptyLaps:  // 燃料がゼロになるまでの周回数
		  if (canLaps < pitInTimingValue_ + 1.0) return true;
		  break;
		default:
		case RunningTimeMinits:   // 連続走行時間
		  break;
		}
		return false;
	  }
	};

  
}

#endif
