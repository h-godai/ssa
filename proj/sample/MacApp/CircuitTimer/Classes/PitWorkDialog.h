// -*-Mode:ObjC;tab-width:4-*-
//  PitWorkDialog.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/09/26.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCircuitTimer.h"


// PitWorkDialogの動作モード
typedef enum {
  JustNowPitWorkd,    // ピットストップ中
  EditLatestPitWork,  // 前回のピットワークを編集
  EditNextPitWork     // 次回のピットワークを編集
} PitWorkDialogMode;

// Pickerの状態
typedef enum {
  FuelPickerSlected,
  RiderPickerSelected
} PickerSelectedType;

// 給油アクション
typedef enum {
  FuelAction_None,
  FuelAction_Full,
  FuelAction_NotFull
} FuelActionType;

// タイヤ交換アクション
typedef enum {
  TireAction_None,
  TireAction_Change
} TireActionType;

// ライダー交代アクション
typedef enum {
  RiderAction_None,
  RiderAction_Change
} RiderActionType;


@interface PitWorkDialog : UIViewController<UIPickerViewDelegate
										  , UIPickerViewDataSource
										   >
{
  IBOutlet UIPickerView* pickerView;
  IBOutlet UIButton* fuelBtn;
  IBOutlet UIButton* riderBtn;
  IBOutlet UIButton* okBtn;
  IBOutlet UIButton* okBtn2;
  IBOutlet UIButton* pitOutBtn;
  IBOutlet UISegmentedControl* fuelAction;
  IBOutlet UISegmentedControl* tireAction;
  IBOutlet UISegmentedControl* riderAction;
  IBOutlet UIView* tireView;
  IBOutlet UIView* riderView;

  PitWorkDialogMode pitWorkDialogMode;
  PickerSelectedType pickerType;
  FuelActionType fuelActionType;
  TireActionType tireActionType;
  RiderActionType riderActionType;


  UIPopoverController* popoverController;
  int currentStint_;
  int currentRider_;
  int pitStopLap;
}

@property (readwrite)  PitWorkDialogMode pitWorkDialogMode;
@property (readwrite)  PickerSelectedType pickerType;
@property (readwrite)  FuelActionType fuelActionType;
@property (readwrite)  TireActionType tireActionType;
@property (readwrite)  RiderActionType riderActionType;
@property (readwrite)  int pitStopLap;

@property  (nonatomic, retain) UIPopoverController* popoverController;

- (IBAction)onFuelBtn;
- (IBAction)onRiderBtn;
- (IBAction)onOkBtn;
- (IBAction)onOkBtn2;
- (IBAction)onPitOutBtn;

- (IBAction)onSelectFuelAction;
- (IBAction)onSelectTireAction;
- (IBAction)onSelectRiderAction;


- (void)hideFuelPicker;
- (void)updateView;
@end
