// -*-tab-width:4-*-
//  PitWorkDialog.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/09/26.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "PitWorkDialog.h"
#import "CircuitTimerAppDelegate.h"

@implementation PitWorkDialog

@synthesize pitWorkDialogMode;
@synthesize pickerType;
@synthesize fuelActionType;
@synthesize tireActionType;
@synthesize riderActionType;
@synthesize popoverController;
@synthesize pitStopLap;


/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
  [super viewDidLoad];

  switch (pitWorkDialogMode) {
  case JustNowPitWorkd:    // ピットストップ中
	currentStint_ = [MCircuitTimer getCurrentStint];
	pitStopLap = [MCircuitTimer getLapCount];
	break;
  case EditLatestPitWork:  // 前回のピットワークを編集
	currentStint_ = [MCircuitTimer getCurrentStint] - 1;
    [pitOutBtn setTitle:@"更新" forState:UIControlStateNormal];
	break;
  case EditNextPitWork:     // 次回のピットワークを編集
	currentStint_ = [MCircuitTimer getCurrentStint];
    [pitOutBtn setTitle:@"更新" forState:UIControlStateNormal];
	break;
  }
  
  CTStintInfo sinfo;
  [MCircuitTimer getStintInfo:currentStint_ stintInfo: &sinfo];

  currentRider_ = sinfo.riderNum;

  fuelActionType = sinfo.fuelFlag ? FuelAction_Full : FuelAction_None;
  tireActionType = sinfo.wheelFlag ? TireAction_Change : TireAction_None;
  riderActionType = RiderAction_Change;

  fuelAction.selectedSegmentIndex = fuelActionType;
  tireAction.selectedSegmentIndex = tireActionType;
  riderAction.selectedSegmentIndex = riderActionType;

  if (riderActionType == RiderAction_Change) {
	// 次のスティントのライダーを得る
	currentRider_ = [MCircuitTimer getNextRider: currentStint_];
  }
  
  [riderBtn setTitle: [MCircuitTimer getRiderName:currentRider_]
			forState: UIControlStateNormal];
  
  [self updateView];
}

- (void)updateView {
  float y = 0.0f;
  float tireY, riderY, pitoutY;
  if (fuelActionType == FuelAction_None) {
	// 給油が無いので給油量の行を隠す
	y = fuelAction.frame.origin.y + fuelAction.frame.size.height + 20.0f;
	fuelBtn.hidden = YES;
  }
  else {
	y = fuelBtn.frame.origin.y + fuelBtn.frame.size.height + 20.0f;
	fuelBtn.hidden = NO;
  }
  tireY = y;
  y += tireAction.frame.size.height + 20.0f;

  if (tireActionType != TireAction_None) {
	// タイヤ交換あり
	y += 50.0f;
  }
  riderY = y;
  y += riderBtn.frame.size.height + 20.0f;
  if (riderActionType != RiderAction_None) {
	y += 70.0f;
  }
  pitoutY = y;
  
  
  y += pitOutBtn.frame.size.height + 10.0f;


  self.contentSizeForViewInPopover = CGSizeMake(self.contentSizeForViewInPopover.width, y);

  tireView.frame = CGRectMake(tireView.frame.origin.x, tireY,
							  tireView.frame.size.width,tireView.frame.size.height);
  riderView.frame = CGRectMake(riderView.frame.origin.x, riderY,
							   riderView.frame.size.width, riderView.frame.size.height);
  pitOutBtn.frame = CGRectMake(pitOutBtn.frame.origin.x, pitoutY,
							   pitOutBtn.frame.size.width, pitOutBtn.frame.size.height);
  
  
//  self.view.frame = CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y,
//							   self.view.frame.size.width, y);
  
	
 
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)onFuelBtn {
//  self.pickerView = [[UIPickerView alloc] initWithFrame: CGRectMake(0.0f,0.0f,320.0f,216.0f)];
//  [pitWorkDialogView addSubview: self.pickerView];
//  [self.pickerView reloadAllComponents];
  if (pickerView.hidden == YES) {
	pickerType = FuelPickerSlected;
	CGRect r = fuelBtn.frame;
	pickerView.frame = CGRectMake(r.origin.x, r.origin.y+(r.size.height/2)-pickerView.frame.size.height/2
								  , r.size.width, pickerView.frame.size.height);
    pickerView.hidden = NO;
    okBtn.hidden = NO;
  }
  else {
	[self hideFuelPicker];
  }
}

- (IBAction)onRiderBtn {
  if (pickerView.hidden == YES) {
	pickerType = RiderPickerSelected;
	CGRect r = riderBtn.frame;
	r.origin.y += riderView.frame.origin.y;
	pickerView.frame = CGRectMake(r.origin.x, r.origin.y+(r.size.height/2)-pickerView.frame.size.height/2
								  , r.size.width, pickerView.frame.size.height);
    pickerView.hidden = NO;
    okBtn2.hidden = NO;
  }
  else {
    [self hideFuelPicker];
  }
}

- (void)onOkBtn {
  [self hideFuelPicker];
}

- (void)hideFuelPicker {
  if (pickerView.hidden == NO) {
    pickerView.hidden = YES;
    okBtn.hidden = YES;
    okBtn2.hidden = YES;
  }
}

- (IBAction)onOkBtn2 {
  [self hideFuelPicker];
}
- (IBAction)onPitOutBtn {
  if (pitWorkDialogMode == JustNowPitWorkd) {
	[(CircuitTimerAppDelegate*)popoverController.delegate
							   onPitOutAction:riderActionType == RiderAction_Change];

	pitWorkDialogMode = EditLatestPitWork;
    [pitOutBtn setTitle:@"Done" forState:UIControlStateNormal];
	[self hideFuelPicker];
	[self updateView];
  }
  else {
	[self hideFuelPicker];
	[MCircuitTimer refuel: [fuelBtn.titleLabel.text doubleValue] lap: pitStopLap];
	[popoverController dismissPopoverAnimated:YES];
	[popoverController release];
  }

}

- (IBAction)onSelectFuelAction {
  fuelActionType = fuelAction.selectedSegmentIndex;
  [self hideFuelPicker];
  [self updateView];
}
- (IBAction)onSelectTireAction {
  tireActionType = tireAction.selectedSegmentIndex;
  [self hideFuelPicker];
  [self updateView];
}
- (IBAction)onSelectRiderAction {
  riderActionType = riderAction.selectedSegmentIndex;
  [self hideFuelPicker];
  [self updateView];
}




- (void) pickerView: (UIPickerView*)pView didSelectRow:(NSInteger) row  inComponent:(NSInteger)component {  
  NSLog(@"row=%d, component=%d", row, component);  
  
  if (pickerType == RiderPickerSelected) {
	int row1 = [pView selectedRowInComponent:0];  
	[riderBtn setTitle:	[MCircuitTimer getRiderName: row1]
			  forState: UIControlStateNormal];

  }
  else {
	int row1 = [pView selectedRowInComponent:0];  
	int row2 = [pView selectedRowInComponent:1];  
  
	//     NSLog(@"selected %d, %d", row1, row2);
	[fuelBtn setTitle: [NSString stringWithFormat: @"%.1lf", (double)row1 + (row2 / 10.0)]
			 forState:UIControlStateNormal];
  }
}  

- (NSInteger) numberOfComponentsInPickerView:(UIPickerView *)pickerView {  
  if (pickerType == RiderPickerSelected) return 1;
  return 2;  
}  

- (NSInteger) pickerView: (UIPickerView*)pView numberOfRowsInComponent:(NSInteger) component {
  if (pickerType == RiderPickerSelected) {
	return [MCircuitTimer getRiders];
  }
  if (component == 0) {
	CTTeamInfo ti;
	[MCircuitTimer getTeamInfo: &ti];
	return (int)(ti.fuelCapacity+1.0);
  }
  else {
	return 10;
  }
}  

- (NSString*)pickerView: (UIPickerView*) pView titleForRow:(NSInteger) row forComponent:(NSInteger)component {  
  if (pickerType == RiderPickerSelected) {
	return [MCircuitTimer getRiderName: row];
  }
  else {
	if (component == 0) {
	  return [NSString stringWithFormat:@"%2d", row];
	}
	else {
	  return [NSString stringWithFormat:@".%d", row];  
	}
  }

}  


- (void)didReceiveMemoryWarning {
  // Releases the view if it doesn't have a superview.
  [super didReceiveMemoryWarning];
  
  // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
  [super viewDidUnload];
  // Release any retained subviews of the main view.
  // e.g. self.myOutlet = nil;
}


- (void)dealloc {
  [super dealloc];
}




@end
