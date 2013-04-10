/* -*-tab-width:4-*-
 *  RiderInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_RIDERINFO_HPP
#define CIRCUIT_TIMER_RIDERINFO_HPP

#include "libTS/Util/serialize_policy.hpp";
#include "libTS/Util/TimeParam.hpp";

namespace circuit_timer {
  using namespace ts;

  struct RiderInfo {
	uint32_t lapCount_; // ラップ数
	uint32_t outLapCount_; // アウトラップ数
	uint32_t inLapCount_;  //インラップ数
	MicroTime bestLap_;
	MicroTime averageLapTime_;
	double fuelCost_;
	std::string name_;
	int32_t riderId_;

	RiderInfo(const char* name="", uint32_t lt = 100, double fc=0.0)
	  : lapCount_(0)
	  , outLapCount_(0)
	  , inLapCount_(0)
	  , bestLap_(0)
	  , averageLapTime_(static_cast<micro_time_t>(lt*1000000ULL)) // 1分40秒
	  , fuelCost_(fc)
	  , name_(name)
	  , riderId_(0)
	{}
	void serialize(FILE* f) const {
	  std::string sig("RiderInfo 7");
	  write(f, sig);
	  //write(f, riderId_);
	  write(f, lapCount_); // ラップ数
	  write(f, outLapCount_); // アウトラップ数
	  write(f, inLapCount_);  //インラップ数
	  write(f, bestLap_);
	  write(f, averageLapTime_);
	  write(f, fuelCost_);
	  write(f, name_);
	}
	void deserialize(FILE* f) {
	  std::string sig;
	  read(f, sig);
	  //read(f, riderId_);
	  read(f, lapCount_); // ラップ数
	  read(f, outLapCount_); // アウトラップ数
	  read(f, inLapCount_);  //インラップ数
	  read(f, bestLap_);
	  read(f, averageLapTime_);
	  read(f, fuelCost_);
	  read(f, name_);
	}

	void setInvalid() { riderId_ = -1; }
	bool valid() const { return riderId_ != -1; }
	
  };

} // namespace

#endif //CIRCUIT_TIMER_RIDERINFO_HPP
