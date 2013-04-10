/* -*-tab-width:4-*-
 *  StintInfo.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_STINTINFO_HPP
#define CIRCUIT_TIMER_STINTINFO_HPP

#include <string>

#include "libTS/Util/serialize_policy.hpp";
#include "libTS/Util/TimeParam.hpp";

namespace circuit_timer {
  using namespace ts;
  
  enum StintStatus {
	StrategyStint, // 作戦段階
	RunningStint,  // 走行中
	ResultStint,   // 走行結果
  };
  
  struct StintInfo {
	int32_t num_;
	int32_t rider_;
	int32_t laps_;
	int32_t startLap_;
	bool fullFuel_;
	bool changeWheels_;
	double XfuelCost_;
	double fuelLeft_;
	double tireUsed_; // タイや使用Km
	MicroTime stintTime_;
	MicroTime totalTime_;
	MicroTime averageLapTime_;

	MicroTime bestLapTime_;
	MicroTime fullLapTime_;
	uint32_t fullLapCount_;
	MicroTime outLapTime_;
	MicroTime inLapTime_;
	MicroTime assumeAverageLapTime_;
	double assumeFuelCost_; // 設定燃費
	double realFuelCost_; //実燃費
	bool autoLapNum_; // laps_を自動計算する
	StintStatus status_;
	
	  StintInfo(int32_t num=0
				, int32_t rider=0
				, int32_t laps=0
				, int32_t startLap=0
				, bool fuel=false
				, bool wheel=false
				, double fc=0.0
				, MicroTime at = 0
				)
		: num_(num)
		, rider_(rider)
		, laps_(laps)
		, startLap_(startLap)
		, fullFuel_(fuel)
		, changeWheels_(wheel)
		, XfuelCost_(fc)
		, fuelLeft_(0.0)
		, tireUsed_(0.0)
		, stintTime_(0)
		, totalTime_(0)
		, averageLapTime_(at)
		, bestLapTime_(0)
		, fullLapTime_(0)
		, fullLapCount_(0)
		, outLapTime_(0)
		, inLapTime_(0)
		, assumeAverageLapTime_(at)
		, assumeFuelCost_(fc)
		, realFuelCost_(0)
		, autoLapNum_(true)
		, status_(StrategyStint)
	  {
		printf("StintInfo() stint=%d rider=%d\n", num_, rider_);
	  }
	  void serialize(FILE* f) const {
		std::string signature("StintInfo 20");
		write(f, signature);
		write(f, num_);
		write(f, rider_);
		write(f, laps_);
		write(f, startLap_);
		write(f, fullFuel_);
		write(f, changeWheels_);
		write(f, XfuelCost_);
		write(f, fuelLeft_);
		write(f, tireUsed_);
		write(f, stintTime_);
		write(f, totalTime_);
		write(f, averageLapTime_);
		write(f, bestLapTime_);
		write(f, fullLapTime_);
		write(f, fullLapCount_);
		write(f, outLapTime_);
		write(f, inLapTime_);
		write(f, assumeAverageLapTime_);
		write(f, assumeFuelCost_); // 設定燃費
		write(f, realFuelCost_); //実燃費
		//write(f, autoLapNum_);
		//write(f, (int)status_);

	  }
	  void deserialize(FILE* f) {
		std::string signature;
		read(f, signature);
		read(f, num_);
		read(f, rider_);
		read(f, laps_);
		read(f, startLap_);
		read(f, fullFuel_);
		read(f, changeWheels_);
		read(f, XfuelCost_);
		read(f, fuelLeft_);
		read(f, tireUsed_);
		read(f, stintTime_);
		read(f, totalTime_);
		read(f, averageLapTime_);
		read(f, bestLapTime_);
		read(f, fullLapTime_);
		read(f, fullLapCount_);
		read(f, outLapTime_);
		read(f, inLapTime_);
		read(f, assumeAverageLapTime_);
		read(f, assumeFuelCost_); // 設定燃費
		read(f, realFuelCost_); //実燃費
		//read(f, autoLapNum_);
		//read(f, (int)status_);
	  }
	  
	  MicroTime getLapAverage() const {
		if (fullLapCount_ > 0) {
		  return fullLapTime_.mtime_ / fullLapCount_;
		}
		else {
		  return 0;
		}
	  }

	  int32_t getStartLap() const {
		return startLap_;
	  }
	  int32_t getTotalLap() const {
		return startLap_ + laps_ - 1;
	  }

	  void setInvalid() {
		num_ = -1;
		rider_ = -1;
		laps_ = -1;
	  }
	  
	  bool valid() const { return num_ != -1; }
	  
	};

  struct SightingLapInfo {
	double length_;
	double usedFuel_;
	double fuelLeft_;
	int laps_;
	SightingLapInfo()
	  : length_(0.0)
	  , usedFuel_(0.0)
	  , fuelLeft_(0.0)
	  , laps_(0)
	{}
  };
  
}

#endif
