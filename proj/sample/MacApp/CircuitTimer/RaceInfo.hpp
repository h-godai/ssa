/* -*-tab-width:4-*-
 *  RaceInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_RACEINFO_HPP
#define CIRCUIT_TIMER_RACEINFO_HPP
#include <sys/time.h>

#include "libTS/Util/serialize_policy.hpp"
#include "libTS/Util/TimeParam.hpp"

#include "CircuitInfo.hpp"

namespace circuit_timer {
  using namespace ts;

	enum RaceUnitType {
	  RaceUnit_Laps,  // レースは規定周回数で終わる
	  RaceUnit_Hour,  // レースは規定時間で終わる
	  RaceUnit_Km,    // レースは規定距離で終わる
	} ;

	struct RaceInfo {
	  time_t date_;
	  uint32_t raceDistanceLap_; // 周回数(規定がない場合は０)
	  MicroTime raceDistanceTime_; // レース時間(規定がない場合は０)
	  MicroTime pitStopMinTime_; // ピットストップ時の最低停止時間
	  MicroTime fullFuelMinTime_; // 給油時の最低停止時間
	  MicroTime XchangeWheelsTime_; // not used
	  uint32_t maxRiders_;
	  uint32_t sightingLaps_;
	  double sightingFuelCost_; // サイティングラップの燃費
	  MicroTime XoutLapLossTime_;
	  MicroTime XinLapLossTime_;
	  double XpitinTimingLaps_;
	  std::string name_;
	  CircuitInfo circuit_;
	  RaceUnitType raceType_;
	  RaceInfo()
		: date_(time(NULL))
		, raceDistanceLap_(0)
		, raceDistanceTime_(6*3600*1000000ULL)
		, pitStopMinTime_(0*1000000ULL)
		, fullFuelMinTime_(0*1000000ULL)
		, XchangeWheelsTime_(40*1000000ULL)
		, maxRiders_(3)
		, sightingLaps_(1)
		, sightingFuelCost_(11.0)
		, XoutLapLossTime_(20*1000000ULL)
		, XinLapLossTime_(20*1000000ULL)
		, XpitinTimingLaps_(4.0)
		, name_("SUGO OPEN6時間耐久レース")
		, raceType_(RaceUnit_Hour)
	  {}

	  void serialize(FILE* f) const {
		write(f, date_);
		write(f, raceDistanceLap_); // 周回数(規定がない場合は０)
		write(f, raceDistanceTime_); // レース時間(規定がない場合は０)
		write(f, pitStopMinTime_);
		write(f, fullFuelMinTime_);
		write(f, XchangeWheelsTime_);
		write(f, maxRiders_);
		write(f, sightingLaps_);
		write(f, sightingFuelCost_); // サイティングラップの燃費
		write(f, XoutLapLossTime_);
		write(f, XinLapLossTime_);
		write(f, XpitinTimingLaps_);
		write(f, name_);
		// raceType_(RaceUnit_Hour)
		circuit_.serialize(f);
	  }
	  void deserialize(FILE* f) {
		read(f, date_);
		read(f, raceDistanceLap_); // 周回数(規定がない場合は０)
		read(f, raceDistanceTime_); // レース時間(規定がない場合は０)
		read(f, pitStopMinTime_);
		read(f, fullFuelMinTime_);
		read(f, XchangeWheelsTime_);
		read(f, maxRiders_);
		read(f, sightingLaps_);
		read(f, sightingFuelCost_); // サイティングラップの燃費
		read(f, XoutLapLossTime_);
		read(f, XinLapLossTime_);
		read(f, XpitinTimingLaps_);
		read(f, name_);
		// raceType_(RaceUnit_Hour)
		circuit_.deserialize(f);
	  }

	  bool isFinishRace(uint32_t lap, MicroTime total) {
		switch (raceType_) {
		case RaceUnit_Hour:
		  return total > this->raceDistanceTime_;
		  break;
		case RaceUnit_Laps:
		  return lap > this->raceDistanceLap_;
		  break;
		case RaceUnit_Km:
		  return lap > this->raceDistanceLap_;
		  break;
		default:
		  return true;
		}
	  }

	};
  
}

#endif
