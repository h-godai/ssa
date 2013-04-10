// -*-mode:ObjC;tab-width:4-*-
//  CircuitTimerAppDelegate.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/08/05.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCircuitTimer.h"
#import "PitWorkDialog.h"

typedef enum {
  ActionSheet_NonAction,
  ActionSheet_DeleteAction,
  ActionSheet_MissLapAction,
  ActionSheet_EndOfRaceAction
} ActionSheetMode;


@interface CircuitTimerAppDelegate : NSObject <UIApplicationDelegate
  , UIActionSheetDelegate
  , UIPopoverControllerDelegate
  , UINavigationControllerDelegate
  > 
{
  UIWindow *window;
  UINavigationController *navigationController;
  IBOutlet UITextField* currentLapText_;
  IBOutlet UILabel* currentLapLabel_;
  IBOutlet UILabel* msgText_;
  IBOutlet UILabel* totalLapText_;
  IBOutlet UILabel* latestLapTime;
  IBOutlet UILabel* dateLabel;
  IBOutlet UILabel* latestLapLabel;
  IBOutlet UILabel* bestLapText_;
  IBOutlet UILabel* lapAverageText_;
  IBOutlet UILabel* stintStartLap_;
  IBOutlet UILabel* stintTime_;
  IBOutlet UILabel* fullLapCountText_;
  IBOutlet UIButton* lapButton_;
  IBOutlet UILabel* fuelLeftText_;
  IBOutlet UILabel* assumeFuelCost_;
  IBOutlet UILabel* realFuelCost_;
  IBOutlet UILabel* canLapLeftText_;
  IBOutlet UILabel* ridersLapText_;
  IBOutlet UILabel* leftLapText_;
  IBOutlet UILabel* bestLapInfoText_;
  IBOutlet UISegmentedControl* stintSelector_;
  IBOutlet UISwitch* immidateInputSw_;
  IBOutlet UITextField* fuelInText_;
  IBOutlet UILabel* currentRiderName_;
  IBOutlet UILabel* nextRiderName_;
  IBOutlet UILabel* stintRiderName_;
  IBOutlet UILabel* stintLabel_;
  IBOutlet UIButton* pitStopBtn;
  IBOutlet UIButton* pitWorkBtn;
  NSTimer* timer;
  IBOutlet UITableView* historyList;
  // MyTableView* myTableView;
  
  UIActionSheet *actsheet;
  ActionSheetMode actionSheetMode;
  int showStint_;
  
//  PitWorkViewController* pitWorkView;
  UIPopoverController* popoverController;

}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;
@property (nonatomic, retain) UIPopoverController* popoverController;

- (IBAction)onFuelBtn;
- (IBAction)onPitStopBtn;
- (IBAction)onFinishBtn;
- (IBAction)onLapBtn;
- (IBAction)configBtn;
- (IBAction)onSelectStint;
- (IBAction)onDeleteBtn;
- (IBAction)onMissLapBtn;
- (IBAction)onSelectImmidiateInput;
- (IBAction)onDidEndEditLapTime: (id)sender;
- (IBAction)onPitWorkBtn;

- (void)updateHistory;
- (void)startIntervalTimer;
- (void)timerFired:(NSTimer*)timer;
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section;
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath;
- (void)updateInfoText;
- (void)updateRealStintInfo;
- (void)onPitOutAction:(BOOL)changeStint;
- (void)pitWork: (UIButton*)btn;

@end

