/* -*-tab-width:4-*-
 *  ActionLog.hpp
 *  CircuitTimer
 *
 *  Created by IKEDA KOHEI on 10/09/22.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#ifndef CIRCUIT_TIMER_ACTIONLOG_HPP
#define CIRCUIT_TIMER_ACTIONLOG_HPP

#include "libTS/Util/serialize_policy.hpp";
#include "libTS/Util/TimeParam.hpp";

namespace circuit_timer {
  using namespace ts;

  enum ActionType {
	NoAction = 0,
	StartAction,
	StopAction,
	LapAction,
	MissLapAction,
	InLapAction, // inLap
	NewStintAction,
	OutLapAction,
	IllegalPitWorkAction, // 緊急ピットイン、ライダー交代なし
	NumOfActionType
  };

  struct ActionLog {
	ActionType act_;
	MicroTime ts_;
	uint32_t stint_;
	uint32_t rider_;
	MicroTime total_;
	MicroTime lap_;
	uint32_t lapCount_;
	double fuelLeft_;
	double reFuel_; // 給油量
	bool real_; // 実データの場合はtrue
	ActionLog(ActionType act = NoAction
			  , bool real = true
			  , MicroTime ts = 0
			  , uint32_t stint=0
			  , uint32_t rider = 0
			  , double fuel = 0.0
			  , MicroTime total = 0
			  , MicroTime lap = 0
			  , uint32_t lapcount = 0
			  )
	  : act_(act)
	  , real_(real)
	  , ts_(ts)
	  , stint_(stint)
	  , rider_(rider)
	  , fuelLeft_(fuel)
	  , reFuel_(0.0)
	  , total_(total), lap_(lap), lapCount_(lapcount)
		
	{}
	void serialize(FILE* f) const {
	  write(f, act_);
	  write(f, ts_);
	  write(f, stint_);
	  write(f, rider_);
	  write(f, total_);
	  write(f, lap_);
	  write(f, lapCount_);
	  write(f, fuelLeft_);
	  write(f, real_);
	  //write(f, reFuel_);
	}
	void deserialize(FILE* f) {
	  read(f, (uint32_t&)act_);
	  read(f, ts_);
	  read(f, stint_);
	  read(f, rider_);
	  read(f, total_);
	  read(f, lap_);
	  read(f, lapCount_);
	  read(f, fuelLeft_);
	  read(f, real_);
	  //read(f, reFuel_);
	}
	
	static const char* GetActionTypeText(ActionType at) {
	  static const char* name[NumOfActionType] = {
		"NoAction",
		"Start  ",
		"Stop   ",
		"Passed ",
		"MissLap",
		"InLap  ",
		"PitStop",
		"OutLap ",
		"Illegal",
	  };
	  return name[at];
	}

	bool isPitStoped() const { return act_ == NewStintAction || act_ == IllegalPitWorkAction; }
	bool isRunning() const { return act_ == LapAction 
	                             || act_ == MissLapAction
	                             || act_ == InLapAction
                                 || act_ == OutLapAction; }
	
  };
  
} // namespace

#endif // ifndef CIRCUIT_TIMER_ACTIONLOG_HPP

