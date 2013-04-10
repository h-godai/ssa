/* -*-mode:ObjC;tab-width:4-*- */

#import "MCircuitTimer.h"
#include "CircuitTimer.hpp"

using namespace ts;
using namespace circuit_timer;

namespace {
  inline CircuitTimer& circuitTimer() {
	return CircuitTimer::Instance();
  }

  void convRaceInfo(const RaceInfo& rinfo, CTRaceInfo* raceinfo) {
  // conv RaceInfo to CTRaceInfo
	raceinfo->raceName = [NSString stringWithUTF8String: rinfo.name_.c_str()];
	raceinfo->raceDate = TimeParam(rinfo.date_);
	raceinfo->circuitName = [NSString stringWithUTF8String: rinfo.circuit_.name_.c_str()];
	raceinfo->courseLength = rinfo.circuit_.courseLength_;
	raceinfo->minLapTime = (double)MicroTime(rinfo.circuit_.minimumLapTime_);
	raceinfo->raceDistance = ((double)MicroTime(rinfo.raceDistanceTime_))/3600.0;
	raceinfo->raceType = (CTRaceUnitType)rinfo.raceType_;
	raceinfo->sightingLaps = rinfo.sightingLaps_;        // サイティングラップ数
	raceinfo->scLapTime = rinfo.circuit_.scLapTime_;     // セーフティーカーの平均ラップタイム
	printf("getRaceInfo: %lf %llu\n", raceinfo->raceDistance, rinfo.raceDistanceTime_.mtime_);
  }

}


@implementation MCircuitTimer;


+ (void)startTimer {
	circuitTimer().startTimer();
}

+ (void)stopTimer {
	circuitTimer().stopTimer();

}

+ (NSString*)lapTimer {
  MicroTime tp(circuitTimer().lapTimer());
  NSString* str = [NSString stringWithFormat: @"%u:%02u.%03u"
							, tp.min_only(), tp.sec(), tp.msec()];
  return str;
}
+ (NSString*)pitOut:(BOOL)changeStint  {
  MicroTime tp(circuitTimer().pitOut(changeStint == YES));
  NSString* str = [NSString stringWithFormat: @"%u:%02u.%03u"
							, tp.min_only(), tp.sec(), tp.msec()];
  return str;
}
+ (NSString*)pitStop {
  MicroTime tp(circuitTimer().pitStop());
  NSString* str = [NSString stringWithFormat: @"%u:%02u.%03u"
							, tp.min_only(), tp.sec(), tp.msec()];
  return str;
}

+ (void)missLap {
  circuitTimer().missLap();
}

+ (NSString*)getLapTime {
  MicroTime lap(circuitTimer().getLapTime());
  NSString* str = [NSString stringWithFormat: @"%u:%02u.%03u"
							, lap.min_only(), lap.sec(), lap.msec()];
  return str;
	
}

+ (NSString*)getTotalTime {
  MicroTime tt(circuitTimer().getTotalTime());
  NSString* str = [NSString stringWithFormat: @"%02u:%02u:%02u"
							, tt.hour(), tt.min(), tt.sec()];
  return str;
}

+ (NSString*)getBestLap {
  MicroTime tt(circuitTimer().getRiderInfo().bestLap_);
  NSString* str = [NSString stringWithFormat: @"%u:%02u.%03u"
							, tt.min_only(), tt.sec(), tt.msec()];
  return str;
}

+ (unsigned int)getLapCount {
  return circuitTimer().getLapCount();
}


+ (bool)isStarted {
  return circuitTimer().getStatus() == CircuitTimer::Started;
}
+ (bool)isPitStop {
  return circuitTimer().getStatus() == CircuitTimer::PitStoped;
}

+ (void)getRealStintInfo:(int)stint
				   rider:(UILabel*)rider
					best:(UILabel*)bestLap
					aver:(UILabel*)lapAverage 
				stintlap:(UILabel*)stintLap
			   stinttime:(UILabel*)stintTime
				 fulllap:(UILabel*)fullLapCount
		  assumeFuelCost:(UILabel*)assumeFuelCost
		realFuelCost:(UILabel*)realFuelCost
{
  const StintInfo& si = circuitTimer().getStint(stint);
  if (!si.valid()) return;
  const RiderInfo& ri = circuitTimer().getRiderInfo(si.rider_);
  if (!ri.valid()) return;
  rider.text = [NSString stringWithUTF8String: ri.name_.c_str()];
  bestLap.text = [NSString stringWithFormat: @"%u:%02u.%03u"
						   , si.bestLapTime_.min_only()
						   , si.bestLapTime_.sec()
						   , si.bestLapTime_.msec()];
  lapAverage.text = [NSString stringWithFormat: @"%u:%02u.%03u"
							  , si.getLapAverage().min_only()
							  , si.getLapAverage().sec()
							  , si.getLapAverage().msec()];
  stintLap.text = [NSString stringWithFormat: @"%u", si.startLap_];
  stintTime.text = [NSString stringWithFormat: @"%s", si.stintTime_.toMS().c_str()];
  fullLapCount.text = [NSString stringWithFormat: @"%u", si.fullLapCount_];
  assumeFuelCost.text = [NSString stringWithFormat: @"%.2lf", si.assumeFuelCost_];
  realFuelCost.text = [NSString stringWithFormat: @"%.2lf", si.realFuelCost_];
  
}

+ (int)getHistoryCount {
  return circuitTimer().getHistoryCount();
}
+ (BOOL)getActionLog:(int)row actionlog: (CTActionLog*)actionlog {
  const ActionLog& act = circuitTimer().getHistory(row);
  actionlog->act = static_cast<CT_ActionType>(act.act_);
  actionlog->stint = act.stint_;
  actionlog->rider = act.rider_;
  actionlog->total = TimeParam(act.total_);
  actionlog->lapCount = act.lapCount_;
  actionlog->lap = TimeParam(act.lap_);
  actionlog->fuelLeft = act.fuelLeft_;
  actionlog->reFuel = act.reFuel_ ? YES : NO;
  actionlog->action = [NSString stringWithUTF8String: ActionLog::GetActionTypeText(act.act_)];
  return YES;
}

+ (NSString*)getRaceName {
  NSString* str = [NSString stringWithUTF8String: circuitTimer().getRaceInfo().name_.c_str()];
  return str;
}
+ (NSString*)getCircuitName {
  NSString* str = [NSString stringWithUTF8String: circuitTimer().getRaceInfo().circuit_.name_.c_str()];
  return str;
}

+ (double)getFuelLeft {
  return circuitTimer().getFuelLeft();
}
+ (double)getCanLapLeft {
  return circuitTimer().getCanLapLeft();
}

+ (void)setCurrentRider:(int)rider {
  circuitTimer().setCurrentRider(rider);
}

+ (int)getNumOfStint {
  return circuitTimer().getNumOfStint();
}

+ (NSString*)getStintText:(int)row {
  static const char* fuelText[] = { "  -   ", " full " };
  static const char* wheelText[] = { "   -   ", " change " };
  const StintInfo& info = circuitTimer().getStint(row);
  if (!info.valid()) {
	return [NSString stringWithFormat: @"---"];
  }
  NSString* rider = [NSString stringWithUTF8String: circuitTimer().getRiderInfo(info.rider_).name_.c_str()];
  while([rider length] < 18) rider = [rider stringByAppendingString: @" "];
  NSString* str1 = [NSString stringWithFormat: @" %d  %02u/%03u  %02d:%02d:%02d(%02d:%02d) "
							 , info.num_+1
							 , info.laps_, info.getTotalLap()
							 , info.totalTime_.hour(), info.totalTime_.min(), info.totalTime_.sec()
							 , info.stintTime_.min_only(), info.stintTime_.sec()
					];
  NSString* str2 = [NSString stringWithFormat: @"%d:%04.1lf %5.1lfKm/l %5.2lfL   %4.uKm  %s  %s  "
							 , info.assumeAverageLapTime_.min_only()
							 , info.assumeAverageLapTime_.secf()
							 , info.assumeFuelCost_
							 , info.fuelLeft_
							 , static_cast<int>(info.tireUsed_)
							 , fuelText[info.fullFuel_ ? 1 : 0]
							 , wheelText[info.changeWheels_ ? 1 : 0]];
  return [[str1 stringByAppendingString:str2] stringByAppendingString:rider];
}

+ (int)getRiders {
  return circuitTimer().getRiders();
}
+ (void)setRiders:(int)riders {
  circuitTimer().setRiders(riders);
}

+ (int)getCurrentRider {
  return circuitTimer().getCurrentRider();
}
+ (int)getCurrentStint {
  return circuitTimer().getCurrentStint();
}

+ (void)updateRiderInfo:(int)rider 
			  riderName:(NSString*)name
	   assumeLapTimeMin:(NSString*)min
	   assumeLapTimeSec:(NSString*)sec
		 assumeFuelCost:(NSString*)fuel 
{
  RiderInfo& info = circuitTimer().riderInfo(rider);
  info.name_ = [name UTF8String];
  int msec = static_cast<int>([sec floatValue] * 1000);
  TimeParam lt(0, [min intValue], msec / 1000, (msec % 1000) * 1000);
  info.averageLapTime_ = (micro_time_t)lt;
  info.fuelCost_ = [fuel doubleValue];

}

+ (void)getSettingRiderInfo:(int)rider 
				  riderName:(UITextField*)name
		   assumeLapTimeMin:(UITextField*)min
		   assumeLapTimeSec:(UITextField*)sec
			 assumeFuelCost:(UITextField*)fuel 
{
  RiderInfo& info = circuitTimer().riderInfo(rider);
  name.text = [NSString stringWithUTF8String: info.name_.c_str()];
  min.text = [NSString stringWithFormat: @"hoge"];
  sec.text = [NSString stringWithFormat: @"%d:%u.%03u"
					   , info.averageLapTime_.min_only()
					   , info.averageLapTime_.sec()
					   , info.averageLapTime_.msec()];
  fuel.text = [NSString stringWithFormat: @"%.3lf", info.fuelCost_];

}

+ (int)updateStint:(int)stint
			 rider:(int)rider
			  laps:(int)laps
		  fuelFlag:(BOOL)fuel
   wheelChangeFlag:(BOOL)wheel
		  fuelCost:(double)fuelCost
		averageLap:(NSString*)average
{
  return circuitTimer().updateStint(stint
									   , rider
									   , laps
									   , fuel==YES
									   , wheel==YES
									   , fuelCost
									   , MicroTime([average UTF8String]));

}


+ (void)getStintInfo:(int)stint
		   stintInfo:(CTStintInfo*)result
{
  const StintInfo& info = circuitTimer().getStint(stint);
  result->riderNum = info.rider_;
  result->laps = info.laps_;
  result->fuelFlag = info.fullFuel_ ? YES : NO;
  result->wheelFlag = info.changeWheels_ ? YES : NO;
  result->fuelCost = info.assumeFuelCost_;
  result->riderName = [NSString stringWithUTF8String: circuitTimer().getRiderInfo(info.rider_).name_.c_str()];
  result->assumeAverage = TimeParam(info.assumeAverageLapTime_);
  result->totalLaps = info.getTotalLap();
  result->totalTime = TimeParam(info.totalTime_);
  result->stintTime = TimeParam(info.stintTime_);
  result->fuelLeft = info.fuelLeft_;
}

+ (void)setStintInfo:(int)stint
			   rider:(int)rider
		   riderName:(NSString*)riderName
				laps:(int)laps
			fuelFlag:(BOOL)fuelFlag
		   wheelFlag:(BOOL)wheelChange
			fuelCost:(double)fuelCost
			 average:(NSString*)average
{
  StintInfo& sinfo = circuitTimer().getStint(stint);
  sinfo.rider_ = rider;
  circuitTimer().riderInfo(rider).name_ = [riderName UTF8String];
  //sinfo.laps_ = laps;
  sinfo.fullFuel_ = fuelFlag == YES;
  sinfo.changeWheels_ = wheelChange == YES;
  sinfo.assumeFuelCost_ = fuelCost;
  sinfo.assumeAverageLapTime_ = [average UTF8String];
}


+ (void)deleteLastHistory {
  circuitTimer().deleteLastHistory();
}
+ (void)setDocumentPath:(NSString*)path {
  circuitTimer().setDocumentPath([path UTF8String]);
}

+ (void)load:(NSString*)path {
  if (path == nil) {
	circuitTimer().load();
  }
  else {
	circuitTimer().load([path UTF8String]);
  }
  circuitTimer().calcStint();
}

+ (void)save:(BOOL)latest {
  circuitTimer().save(latest==YES);
}

+ (void)fuelIn:(NSString*)fuel {
  circuitTimer().fuelIn([fuel doubleValue]);
}

+ (void)setImmidiateMode:(BOOL)on {
  circuitTimer().setImmidateMode(on == YES);
}

+ (BOOL)isImmidiateMode {
  return circuitTimer().isImmidiateMode() ? YES : NO;
}

+ (void)setCurrentLapTime:(NSString*)laptime {
  circuitTimer().setCurrentLapTime(MicroTime([laptime UTF8String]));
}

+ (void)calcStint {
  circuitTimer().calcStint();
}

+ (void)addStintLap:(int)stint value:(int)value {
  circuitTimer().getStint(stint).laps_ += value;
  circuitTimer().getStint(stint).autoLapNum_ = false;
}

+ (void)setStintRider:(int)stint rider:(int)rider {
  StintInfo& stinfo = circuitTimer().getStint(stint);
  if (stinfo.rider_ != rider) {
	// ライダー変更時はライダーの基本データをコピーする
	const RiderInfo& rinfo = circuitTimer().getRiderInfo(rider);
	stinfo.assumeAverageLapTime_ = rinfo.averageLapTime_;
	stinfo.assumeFuelCost_ = rinfo.fuelCost_;
  }
  stinfo.rider_ = rider;
}

+ (void)addStintAverageLap:(int)stint value:(double)value {
  int v = static_cast<int>(value*1000000.0);
  circuitTimer().getStint(stint).assumeAverageLapTime_ += v;
  circuitTimer().riderInfo(circuitTimer().getStint(stint).rider_).averageLapTime_ =
	circuitTimer().getStint(stint).averageLapTime_;
}
+ (void)addStintFuelCost:(int)stint value:(double)value {
  circuitTimer().getStint(stint).assumeFuelCost_ += value;
  circuitTimer().riderInfo(circuitTimer().getStint(stint).rider_).fuelCost_ =
	circuitTimer().getStint(stint).assumeFuelCost_;
}

+ (void)resetStint {
  circuitTimer().clearStint();
}

+ (void)getRaceInfo:(CTRaceInfo*)raceinfo {
  const RaceInfo& rinfo = circuitTimer().getRaceInfo();
  convRaceInfo(rinfo, raceinfo);
}

+ (void)setRaceInfo:(CTRaceInfo*)raceinfo {
  RaceInfo& rinfo = circuitTimer().raceInfo();
  rinfo.name_ = [raceinfo->raceName UTF8String];
  rinfo.date_ = TimeParam(raceinfo->raceDate);
  rinfo.circuit_.name_ = [raceinfo->circuitName UTF8String];
  rinfo.circuit_.courseLength_ = raceinfo->courseLength;
  rinfo.circuit_.minimumLapTime_ = raceinfo->minLapTime;
  rinfo.raceDistanceTime_ = MicroTime(raceinfo->raceDistance * 3600.0);
  rinfo.raceType_ = static_cast<circuit_timer::RaceUnitType>(raceinfo->raceType);
  rinfo.sightingLaps_ = raceinfo->sightingLaps;
  rinfo.circuit_.scLapTime_ = raceinfo->scLapTime;
  printf("%lf %lf %llu\n",raceinfo->raceDistance, (double)rinfo.raceDistanceTime_, rinfo.raceDistanceTime_.mtime_);
}

+ (void)getTeamInfo:(CTTeamInfo*)teaminfo {
  const TeamInfo& tinfo = circuitTimer().getTeamInfo();
  teaminfo->teamName= [NSString stringWithUTF8String: tinfo.name_.c_str()];      // チーム名
  teaminfo->numOfRiders = tinfo.numOfRiders_;         // ライダー人数
  teaminfo->pitStopTime = tinfo.pitStopTime_; // ピットストップの基本時間
  teaminfo->withFuelFullTime = tinfo.withFullFuelTime_;    // 給油時にプラスされる時間
  teaminfo->withWheelsChangeTime = tinfo.withChangeWheelsTime_; // タイヤ交換時にプラスされる時間
  teaminfo->inLapLossTime = tinfo.inLapLossTime_;    // インラップに要するロスタイム
  teaminfo->outLapLossTime = tinfo.outLapLossTime_;     // アウトラップに要するロスタイム
  teaminfo->sightingFuelCost = tinfo.sightingFuelCost_;  // サイティングラップ中の燃費
  teaminfo->pitInTimingValue = tinfo.pitInTimingValue_; // ピットインタイミング値
  teaminfo->pitInTimingType = (CTPitInTimingType)tinfo.pitInTimingType_; // ピットインタイミングの種別
  teaminfo->machineName = [NSString stringWithUTF8String: tinfo.machineInfo_.name_.c_str()];    // マシン名
  teaminfo->fuelCapacity = tinfo.machineInfo_.fuelCapacity_;      // タンク容量
}
+ (void)setTeamInfo:(CTTeamInfo*)teaminfo {
  TeamInfo& tinfo = circuitTimer().teamInfo();
  tinfo.name_ = [teaminfo->teamName UTF8String];
  tinfo.numOfRiders_ = teaminfo->numOfRiders;
  tinfo.pitStopTime_ = teaminfo->pitStopTime;
  tinfo.withFullFuelTime_ = teaminfo->withFuelFullTime;
  tinfo.withChangeWheelsTime_ = teaminfo->withWheelsChangeTime;
  tinfo.inLapLossTime_ = teaminfo->inLapLossTime;
  tinfo.outLapLossTime_ = teaminfo->outLapLossTime;
  tinfo.sightingFuelCost_ = teaminfo->sightingFuelCost;
  tinfo.pitInTimingValue_ = teaminfo->pitInTimingValue;
  tinfo.pitInTimingType_ = static_cast<circuit_timer::PitInTimingType>(teaminfo->pitInTimingType);
  tinfo.machineInfo_.name_ = [teaminfo->machineName UTF8String];
  tinfo.machineInfo_.fuelCapacity_ = teaminfo->fuelCapacity;
}

+ (NSString*)getStintLapLeft {
  return [NSString stringWithFormat: @"%d"
				   , circuitTimer().getStintLaps() - circuitTimer().getCurrentStintLap()];
}

+ (NSString*)getRiderLapLeft {
  return [NSString stringWithFormat: @"%d/%d"
				   , circuitTimer().getCurrentStintLap()
				   , circuitTimer().getStintLaps()];
}

+ (NSString*)getRiderName:(int)rider {
  const RiderInfo& rinfo = circuitTimer().getRiderInfo(rider);
  return [NSString stringWithUTF8String: rinfo.name_.c_str()];
}

+ (int)getRider:(int)stint {
  return circuitTimer().getStint(stint).rider_;
}


+ (int)getNextRider:(int)stint {
  int rider = circuitTimer().getStint(stint+1).rider_;
  if (rider == -1) {
	rider = (circuitTimer().getStint(stint).rider_ + 1) % circuitTimer().getRiders();
  }
  return rider;
}

+ (void)refuel:(double)value lap:(int)lap {
  circuitTimer().reFuel(value, lap);
}

+ (int)loadRaceInfo:(CTRaceInfo*)raceinfo filename:(NSString*)filename {
  CircuitTimer ct;
  if (ct.load([filename UTF8String])) {;
	const RaceInfo& rinfo = ct.getRaceInfo();
	convRaceInfo(rinfo, raceinfo);
	return ct.getHistoryCount();
  }
  return -1;
}

+ (void)getSightingLapInfo:(CTSightingLapInfo*)info {
  const SightingLapInfo& s = circuitTimer().getSightingLapInfo();
  info->length = s.length_;
  info->usedFuel = s.usedFuel_;
  info->fuelLeft = s.fuelLeft_;
  info->laps = s.laps_;
}

@end
