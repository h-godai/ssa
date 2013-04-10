/* -*-mode:ObjC;tab-width:4-*-
 *  MCircuitTimer.h
 *  MCircuitTimer
 *
 *  Objective-C CircuitTimerクラスのラッパー
 *  Created by IKEDA KOHEI on 10/08/06.
 *  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
 *
 */

#import <UIKit/UIKit.h>

#include "libTS/Util/TimeParam.h";

typedef enum {
  RaceUnit_Laps,  // レースは規定周回数で終わる
  RaceUnit_Hour,  // レースは規定時間で終わる
  RaceUnit_Km,    // レースは規定距離で終わる
} CTRaceUnitType;

typedef enum {
  WhenFuelEmptyLaps,  // 燃料がゼロになるまでの周回数
  RunningTimeMinits   // 連続走行時間
} CTPitInTimingType;

typedef struct CTRaceInfo_ {
  NSString* raceName;
  TS_TimeParam raceDate;
  NSString* circuitName;
  double courseLength;
  double minLapTime;
  double raceDistance;
  int sightingLaps;         // サイティングラップ数
  CTRaceUnitType raceType;
  double scLapTime;         // セーフティーカーの平均ラップタイム
} CTRaceInfo;

typedef struct CTTeamInfo_ {
  NSString* teamName;      // チーム名
  int numOfRiders;         // ライダー人数
  double pitStopTime;         // ピットストップの基本時間
  double withFuelFullTime;    // 給油時にプラスされる時間
  double withWheelsChangeTime; // タイヤ交換時にプラスされる時間
  double inLapLossTime;     // インラップに要するロスタイム
  double outLapLossTime;    // アウトラップに要するロスタイム
  double sightingFuelCost;  // サイティングラップ中の燃費
  double pitInTimingValue; // ピットインタイミング値
  CTPitInTimingType pitInTimingType; // ピットインタイミングの種別
  NSString* machineName;    // マシン名
  double fuelCapacity;      // タンク容量
} CTTeamInfo;

typedef struct CTStintInfo_ {
  int riderNum;
  NSString* riderName;
  int laps;
  int totalLaps;
  BOOL fuelFlag;
  BOOL wheelFlag;
  double fuelCost;
  TS_TimeParam assumeAverage;
  TS_TimeParam totalTime;
  TS_TimeParam stintTime;
  double fuelLeft;
} CTStintInfo;

typedef enum {
  CT_NoAction = 0,
  CT_StartAction,
  CT_StopAction,
  CT_LapAction,
  CT_MissLapAction,
  CT_InLapAction, // inLap
  CT_NewStintAction,
  CT_OutLapAction,
  CT_IllegalPitWorkAction, // 緊急ピットイン、ライダー交代なし
  CT_NumOfActionType
} CT_ActionType;

typedef struct CTActionLog_ {
  CT_ActionType act;
  NSString* action;
  int stint;
  int rider;
  TS_TimeParam total;
  TS_TimeParam lap;
  int lapCount;
  double fuelLeft;
  double reFuel; // 給油量
  BOOL real; // 実データの場合はtrue
} CTActionLog;


// サイティングラップの情報
typedef struct CTSightingLapInfo_ {
  double length;
  double usedFuel;
  double fuelLeft;
  int laps;
} CTSightingLapInfo;



@interface MCircuitTimer : NSObject {

}


+ (void)startTimer;
+ (void)stopTimer;
+ (NSString*)pitStop;
+ (NSString*)lapTimer;
+ (void)missLap;
+ (NSString*)pitOut:(BOOL)changSetint;
+ (NSString*)getLapTime;
+ (NSString*)getTotalTime;
+ (NSString*)getBestLap;
+ (unsigned int)getLapCount;
+ (bool)isStarted;
+ (bool)isPitStop;
+ (void)getRealStintInfo:(int)stint
				   rider:(UILabel*)rider
					best:(UILabel*)bestLap 
					aver:(UILabel*)lapAverage
				stintlap:(UILabel*)stintLap
			   stinttime:(UILabel*)stintTime
				 fulllap:(UILabel*)fullLapCount
		  assumeFuelCost:(UILabel*)assumeFuelCost
			realFuelCost:(UILabel*)realFuelCost;

+ (int)getHistoryCount;
+ (BOOL)getActionLog:(int)row actionlog: (CTActionLog*)actionlog;
+ (NSString*)getRaceName;

+ (NSString*)getCircuitName;
+ (double)getFuelLeft;
+ (double)getCanLapLeft;
+ (void)setCurrentRider:(int)rider;
+ (int)getCurrentRider;
+ (int)getCurrentStint;

+ (int)getNumOfStint;
+ (NSString*)getStintText:(int)row;
+ (int)getRiders;
+ (void)setRiders:(int)riders;
+ (void)updateRiderInfo:(int)rider 
			  riderName:(NSString*)name
	   assumeLapTimeMin:(NSString*)min
	   assumeLapTimeSec:(NSString*)sec
		 assumeFuelCost:(NSString*)fuel;

+ (void)getSettingRiderInfo:(int)rider 
			  riderName:(UITextField*)name
	   assumeLapTimeMin:(UITextField*)min
	   assumeLapTimeSec:(UITextField*)sec
		 assumeFuelCost:(UITextField*)fuel;

+ (int)updateStint:(int)stint
			 rider:(int)rider
			  laps:(int)laps
		  fuelFlag:(BOOL)fuel
   wheelChangeFlag:(BOOL)wheel
		  fuelCost:(double)fuelCost
		averageLap:(NSString*)average;

+ (void)getStintInfo:(int)stint
		   stintInfo:(CTStintInfo*)result;
#if 0
+ (void)getStintInfo:(int)stint
			 p_rider:(int*)rider
		   riderName:(NSString**)riderName
				laps:(int*)laps
		  p_fuelFlag:(BOOL*)fuelFlag
		 p_wheelFlag:(BOOL*)wheelFlag
			fuelCost:(NSString**)fuelCost
			 average:(NSString**)average;
#endif
+ (void)setStintInfo:(int)stint
			   rider:(int)rider
		   riderName:(NSString*)riderName
				laps:(int)laps
			fuelFlag:(BOOL)fuelFlag
		   wheelFlag:(BOOL)wheelChange
			fuelCost:(double)fuelCost
			 average:(NSString*)average;


+ (void)deleteLastHistory;  
+ (void)setDocumentPath:(NSString*)path;
+ (void)load:(NSString*)path;
+ (void)save:(BOOL)latest;
+ (void)fuelIn:(NSString*)fuel;
+ (void)setImmidiateMode:(BOOL)on;
+ (BOOL)isImmidiateMode;
+ (void)setCurrentLapTime:(NSString*)laptime;
+ (void)calcStint;
+ (void)addStintLap:(int)stint value:(int)value;
+ (void)setStintRider:(int)stint rider:(int)rider;
+ (void)addStintAverageLap:(int)stint value:(double)value;
+ (void)addStintFuelCost:(int)stint value:(double)value;
+ (void)resetStint;

+ (void)getRaceInfo:(CTRaceInfo*)raceinfo;
+ (void)setRaceInfo:(CTRaceInfo*)raceinfo;
+ (void)getTeamInfo:(CTTeamInfo*)teaminfo;
+ (void)setTeamInfo:(CTTeamInfo*)teaminfo;
+ (NSString*)getStintLapLeft;
+ (NSString*)getRiderLapLeft;
+ (NSString*)getRiderName:(int)rider;
+ (int)getNextRider:(int)stint;
+ (int)getRider:(int)stint;
+ (void)refuel:(double)value lap:(int)lap;
+ (int)loadRaceInfo:(CTRaceInfo*)raceinfo filename:(NSString*)filename;
+ (void)getSightingLapInfo:(CTSightingLapInfo*)info;
@end


