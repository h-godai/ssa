// -*-mode:ObjC;tab-width:4-*-
//  ConfigViewController.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/09/03.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCircuitTimer.h"
#import "FileListDialog.h"
#import "DatePickerView.h"
#import "StintCellView.h"
#import "StintCellViewController.h"
#import "NumericPickerView.h";

@interface ConfigViewController : UIViewController<UITableViewDelegate
											 	 , UITableViewDataSource
												 , UIPopoverControllerDelegate
												 , DatePickerViewDelegate
												 , NumericPickerViewDelegate
												 > 

{
  // RaceInformation
  IBOutlet UITextField* raceName;
  IBOutlet UIButton*    dateButton;
  IBOutlet UITextField* circuitName;
  IBOutlet UITextField* courseLength;
  IBOutlet UITextField* minLapTime;
  IBOutlet UIButton* raceDistance;
  IBOutlet UISegmentedControl* raceType;

  // TeamInformation
  IBOutlet UITextField* teamName;
  IBOutlet UIButton*     numOfRiders;

  IBOutlet UITextField* pitStopTime;
  IBOutlet UITextField* withFuelFullTime;
  IBOutlet UITextField* withWheelsChangeTime;
  IBOutlet UITextField* inLapLossTime;
  IBOutlet UITextField* outLapLossTime;
  IBOutlet UITextField* scLapTime;
  IBOutlet UITextField* sightingLaps;
  IBOutlet UITextField* sightingFuelCost;
  IBOutlet UITextField* pitInTimingValue;
  IBOutlet UISegmentedControl* pitInTimingType;

  IBOutlet UITextField* machineName;
  IBOutlet UITextField* fuelCapacity;
  
  IBOutlet UISegmentedControl* riderSelect;  
  IBOutlet UITextField* riderName;
  IBOutlet UITextField* assumeAverageLapTimeMin;
  IBOutlet UITextField* assumeAverageLapTimeSec;
  IBOutlet UITextField* assumeFuelCost;

  IBOutlet UILabel*     stintTitle;
  IBOutlet UITextField* stintLaps;
  IBOutlet UISwitch*    fullFuelSw;
  IBOutlet UISwitch*    wheelChangeSw;
  IBOutlet UISegmentedControl* editEnableSelect;  
  IBOutlet UISegmentedControl* lapCountIncDec;  
  IBOutlet UISegmentedControl* fuelCostIncDec;  
  IBOutlet UISegmentedControl* averageLapIncDec;  

  IBOutlet UITableView* raceScheduleView;
  IBOutlet UIView* rightNabView;

  IBOutlet UITableViewController* raceScheduleViewController;
  IBOutlet UIImageView* stintListTitleImage;
  IBOutlet UIView* stintTitleView;

  StintCellViewController *stintTitleViewController_;

  int currentRider_;
  int currentStint_;
  UIPopoverController* popoverController_;
  UIBarButtonItem* fileBtnItem_;
  UIBarButtonItem* fileMenuItem_;
  FileListDialog* fileListDialog_;
  DatePickerView* datePickerView_;
  NumericPickerView* numericPickerView_;
  TS_TimeParam raceDate_;
}

@property  (nonatomic, retain) UIPopoverController* popoverController_;
- (void)reloadAllView;
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;
- (NSInteger)numberOfSections;

- (IBAction)onPlusRiderBtn;
- (IBAction)onMinusRiderBtn;
- (IBAction)onAddStintBtn;
- (IBAction)onUpdateStintBtn;
- (IBAction)onSelectRider;
- (IBAction)onEditEndRiderName;
- (IBAction)onSelectEditEnable;
- (IBAction)onResetBtn;
- (IBAction)onLapCountIncDec;
- (IBAction)onFuelCostIncDec;
- (IBAction)onAverageLapIncDec;
- (IBAction)onEditEndRaceInfo;
- (IBAction)onEditEndTeamInfo;
- (IBAction)onEditEndStintInfo;
- (IBAction)onFileCancelBtn;
- (IBAction)onFileDeleteBtn;
- (IBAction)onFileAddNewBtn;
- (IBAction)onDateBtn;
- (IBAction)onNumOfRiders;
- (IBAction)onRaceDistance;



										
- (void)updateRiderInfo;
- (void)loadStintData;

- (void)updateRaceInfo;
- (void)updateTeamInfo;
- (void)onFileBtn:(id)sender;
- (void)onSelectFile:(NSString*)filename;

- (void)setNaviBtn:(BOOL)filebtn;
- (void)setCellColor:(StintCellView *)cell row:(int)row selected:(BOOL)aFlag;
@end
