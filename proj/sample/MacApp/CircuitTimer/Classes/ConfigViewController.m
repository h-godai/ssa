// -*-tab-width:4-*-
//  ConfigViewController.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/09/03.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "ConfigViewController.h"
#import "FileListDialog.h"
#import "DatePickerView.h"

@implementation ConfigViewController

@synthesize popoverController_;

#pragma mark -
#pragma mark View lifecycle

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
	self.title = @"Configuration";
  }
  return self;
}

- (void)viewDidLoad {
  [super viewDidLoad];
  self.popoverController_ = nil;
  
  //バーにボタンを追加
  fileBtnItem_ = [[UIBarButtonItem alloc] 
				   initWithTitle:@"File" 
				   style:UIBarButtonItemStyleBordered
				   target: self
				   action:@selector(onFileBtn:)
							 ]; 
  
  fileMenuItem_ = [[UIBarButtonItem alloc] initWithCustomView:  rightNabView ]; 

  // テーブルビューにヘッダタイトルを追加

  stintTitleViewController_ = 
	[[StintCellViewController alloc] initWithNibName:@"StintCellView" bundle:nil];
  StintCellView* cell = (StintCellView *)stintTitleViewController_.view;
  cell.backgroundView = stintListTitleImage;
  [stintTitleView addSubview: cell];

#if 0
  UIView *bgView = [[UILabel alloc] initWithFrame:CGRectZero];
  cell.backgroundView = bgView;
  bgView.backgroundColor = [UIColor colorWithRed:0.5 green:0.5 blue:0.8 alpha:1.0];
  raceScheduleView.tableHeaderView = controller.view;
#endif


  [self setNaviBtn:YES];

  [MCircuitTimer calcStint];

  [self reloadAllView];
}

- (void)reloadAllView {

  currentRider_ = 0;
  currentStint_ = 0;

  // setup Race Information
  CTRaceInfo rinfo;
  [MCircuitTimer getRaceInfo: &rinfo];
  
  raceName.text = rinfo.raceName;
  raceDate_ = rinfo.raceDate;
  [dateButton setTitle: [NSString stringWithFormat: @"%04d/%02d/%02d %02d:%02d"
								  , rinfo.raceDate.year_
								  , rinfo.raceDate.month_
								  , rinfo.raceDate.day_
								  , rinfo.raceDate.hour_
								  , rinfo.raceDate.min_ ]
			  forState:UIControlStateNormal];

  circuitName.text = rinfo.circuitName;
  courseLength.text = [NSString stringWithFormat: @"%.3lf", rinfo.courseLength];
  minLapTime.text = [NSString stringWithFormat: @"%.1lf", rinfo.minLapTime];
  [raceDistance setTitle: [NSString stringWithFormat: @"%.2lf", rinfo.raceDistance]  forState: UIControlStateNormal];
  raceType.selectedSegmentIndex = rinfo.raceType;
  sightingLaps.text = [NSString stringWithFormat: @"%d", rinfo.sightingLaps];


  // setup Team Information
  CTTeamInfo tinfo;
  [MCircuitTimer getTeamInfo: &tinfo]; 
  teamName.text = tinfo.teamName;
  [numOfRiders setTitle: [NSString stringWithFormat: @"%d", tinfo.numOfRiders] forState: UIControlStateNormal]; // ライダー人数
  pitStopTime.text = [NSString stringWithFormat: @"%.1lf", tinfo.pitStopTime]; // ピットストップの基本時間
  withFuelFullTime.text = [NSString stringWithFormat: @"%.1lf", tinfo.withFuelFullTime];  // 給油時にプラスされる時間
  withWheelsChangeTime.text= [NSString stringWithFormat: @"%.1lf", tinfo.withWheelsChangeTime];
  inLapLossTime.text = [NSString stringWithFormat: @"%.1lf", tinfo.inLapLossTime];    // インラップに要するロスタイム
  outLapLossTime.text = [NSString stringWithFormat: @"%.1lf", tinfo.outLapLossTime];     // アウトラップに要するロスタイム
  scLapTime.text = @"HOGE";
  sightingFuelCost.text = [NSString stringWithFormat: @"%.2lf", tinfo.sightingFuelCost];  // サイティングラップ中の燃費
  pitInTimingValue.text = [NSString stringWithFormat: @"%.2lf", tinfo.pitInTimingValue]; // ピットインタイミング値
  pitInTimingType.selectedSegmentIndex = (int)tinfo.pitInTimingType; // ピットインタイミングの種別
  machineName.text = tinfo.machineName;  // マシン名
  fuelCapacity.text = [NSString stringWithFormat: @"%.2lf", tinfo.fuelCapacity];      // タンク容量

  
  // Uncomment the following line to preserve selection between presentations.
  // self.clearsSelectionOnViewWillAppear = NO;
 
  // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
  // self.navigationItem.rightBarButtonItem = self.editButtonItem;

  [self loadStintData];

  [raceScheduleView selectRowAtIndexPath:[NSIndexPath indexPathForRow:currentStint_ inSection:0]
					animated:NO
					scrollPosition:UITableViewScrollPositionNone];

}

- (void)updateRaceInfo {
  CTRaceInfo rinfo;
  rinfo.raceName = raceName.text;
  rinfo.raceDate  = raceDate_;
  rinfo.circuitName  = circuitName.text;
  rinfo.courseLength = [courseLength.text doubleValue];
  rinfo.raceDistance = [raceDistance.titleLabel.text doubleValue];
  rinfo.minLapTime = [minLapTime.text doubleValue];
  rinfo.raceType = raceType.selectedSegmentIndex;
  rinfo.sightingLaps = [sightingLaps.text intValue];
  [MCircuitTimer setRaceInfo: &rinfo];

}

/*
 - (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}
*/


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
  // Override to allow orientations other than the default portrait orientation.
  return YES;
}


#pragma mark -
#pragma mark Table view data source

// TableViewのデリゲート
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return 1 + [MCircuitTimer getNumOfStint];
}

-(void)setCellColor:(StintCellView *)cell row:(int)row selected:(BOOL)aFlag
{
  if (aFlag) {
	cell.backgroundView.backgroundColor = [UIColor colorWithRed:0.6 green:0.6 blue:1.0 alpha:1.0];
  }
  else {
	if (row & 1) {
	  cell.backgroundView.backgroundColor = [UIColor colorWithRed:0.9 green:0.9 blue:0.9 alpha:0.8];
	}
	else {
	  cell.backgroundView.backgroundColor = [UIColor clearColor];
	}
  }
}


- (UITableViewCell *)tableView:(UITableView *)tableView
		 cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  
  static NSString *identifier = @"StintCellView";
  StintCellView *cell = (StintCellView *)[tableView dequeueReusableCellWithIdentifier:identifier];
  if (cell == nil) {
	StintCellViewController *controller = [[StintCellViewController alloc] initWithNibName:identifier
																					bundle:nil];
	cell = (StintCellView *)controller.view;
	[controller release];

	UIView *bgView = [[UILabel alloc] initWithFrame:CGRectZero];
	cell.backgroundView = bgView;				// ここでbgViewがretainされる
	[bgView release];							// autoreleaseではないので、自分でrelease

  }

  if (indexPath.row == 0) {
	CTSightingLapInfo sinfo;
	[MCircuitTimer getSightingLapInfo: &sinfo];
	[cell.stint_ setText: @""];
	[cell.laps_  setText: [NSString stringWithFormat: @"%d", sinfo.laps]];
	[cell.totalLaps_ setText: @"-"];
	[cell.totalTime_ setText: @"--:--:--"];
	[cell.stintTime_ setText: @"--:--"];
	[cell.rider_ setText: @"SightingLap"];
	[cell.fuelLeft_ setText: [NSString stringWithFormat: @"%.2lfL", sinfo.fuelLeft]];
	cell.backgroundView.backgroundColor = 
	  [UIColor colorWithRed:0.6 green:0.6 blue:0.6 alpha:1.0];
	[cell.fuel_ setText: @""];
	[cell.tire_ setText: @""];
  }
  else {
	int stint = indexPath.row-1;
	CTStintInfo sinfo;
	[MCircuitTimer getStintInfo:stint stintInfo: &sinfo];
	[cell.stint_ setText: [NSString stringWithFormat: @"%d", stint+1]];
	[cell.laps_  setText: [NSString stringWithFormat: @"%d", sinfo.laps]];
	[cell.totalLaps_ setText: [NSString stringWithFormat: @"%d", sinfo.totalLaps]];
	[cell.totalTime_ setText: [NSString stringWithFormat: @"%02d:%02d:%02d"
									  , sinfo.totalTime.hour_
									  , sinfo.totalTime.min_
									  , sinfo.totalTime.sec_]];
	[cell.stintTime_ setText: [NSString stringWithFormat: @"%02d:%02d"
									  , sinfo.stintTime.hour_*60+ sinfo.stintTime.min_
									  , sinfo.stintTime.sec_]];
	[cell.rider_ setText: sinfo.riderName];
	[cell.fuelLeft_ setText: [NSString stringWithFormat: @"%.2lfL", sinfo.fuelLeft]];
	[cell.fuel_ setText: sinfo.fuelFlag ? @"full" : @""];
	[cell.tire_ setText: sinfo.wheelFlag ? @"Change" : @""];
	[self setCellColor:cell row:stint selected:(stint == currentStint_)];
  }
  
  return cell;
}

- (NSInteger)numberOfSections {
  return 1; // セクションは1個とします
} 


// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}


/*
- (void)tableView:(UITableView *)tableView 
commitEditingStyle:(UITableViewCellEditingStyle)editingStyle 
forRowAtIndexPath:(NSIndexPath *)indexPath 
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
	  [MCircuitTimer deleteStint:indexPath.row];
	  [raceScheduleView reloadData];

    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  // indexPath.row でタップされた要素の番号がとれる。
  [tableView deselectRowAtIndexPath:indexPath animated:YES];
  if (indexPath.row > 0) {
	currentStint_ = indexPath.row - 1;
	stintTitle.text = [NSString stringWithFormat: @"Stint-%d", currentStint_ + 1];
	[self loadStintData];
	[raceScheduleView reloadData];
  }
}


/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

- (IBAction)onPlusRiderBtn {
  if ([MCircuitTimer isStarted]) return;
  int riders = [MCircuitTimer getRiders] + 1;
  [MCircuitTimer setRiders: riders];
//  numOfRiders.text = [NSString stringWithFormat: @"%d", riders];
  NSString* title = [NSString stringWithFormat: @"Rider-%d", riders];
  [riderSelect insertSegmentWithTitle:title atIndex:riders animated:YES];
}

- (IBAction)onMinusRiderBtn {
  if ([MCircuitTimer isStarted]) return;
  int riders = [MCircuitTimer getRiders];
  if (riders > 1) {
	--riders;
	[MCircuitTimer setRiders: riders];
//	numOfRiders.text = [NSString stringWithFormat: @"%d", riders];
	[riderSelect removeSegmentAtIndex:riders animated:YES];
  }
}

- (IBAction)onAddStintBtn {
  currentStint_ = [MCircuitTimer updateStint: [MCircuitTimer getNumOfStint]
								 rider:currentRider_
								 laps: [stintLaps.text intValue]
								 fuelFlag: fullFuelSw.on
								 wheelChangeFlag: wheelChangeSw.on
								 fuelCost: [assumeFuelCost.text doubleValue]
								 averageLap: assumeAverageLapTimeSec.text 
				   ];
  [raceScheduleView reloadData];

  //  [raceScheduleView selectRowAtIndexPath: [NSIndexPath indexPathForRow:currentStint_ inSection:0]
  //   animated:NO
  //   scrollPosition:UITableViewScrollPositionNone];
}
- (IBAction)onUpdateStintBtn {
}



- (IBAction)onSelectRider {
  
  currentRider_ = [riderSelect selectedSegmentIndex];
  [MCircuitTimer setStintRider:currentStint_ rider:currentRider_];

  [self loadStintData];
  [MCircuitTimer calcStint];
  [raceScheduleView reloadData];
}

- (IBAction)onEditEndRiderName {
  printf("ライダー情報更新\n");
  [self updateRiderInfo];
}

- (IBAction)onSelectEditEnable {
}
- (IBAction)onResetBtn {
  [MCircuitTimer resetStint];
  [self loadStintData];
  [raceScheduleView reloadData];
}

// Stint LAP +-
- (IBAction)onLapCountIncDec {
  printf("- (IBAction)onLapCountIncDec {\n");
  if ([lapCountIncDec selectedSegmentIndex] != 0) {
	[MCircuitTimer addStintLap:currentStint_ value:1];
  }
  else {
	[MCircuitTimer addStintLap:currentStint_ value:-1];
  }
  [MCircuitTimer calcStint];
  [raceScheduleView reloadData];
}
- (IBAction)onAverageLapIncDec {
  if ([averageLapIncDec selectedSegmentIndex] != 0) {
	[MCircuitTimer addStintAverageLap:currentStint_ value:0.1];
  }
  else {
	[MCircuitTimer addStintAverageLap:currentStint_ value:-0.1];
  }
  [MCircuitTimer calcStint];
  [self loadStintData];
  [raceScheduleView reloadData];
}


- (IBAction)onFuelCostIncDec {
  if ([fuelCostIncDec selectedSegmentIndex] != 0) {
	[MCircuitTimer addStintFuelCost:currentStint_ value:0.1];
  }
  else {
	[MCircuitTimer addStintFuelCost:currentStint_ value:-0.1];
  }
  [MCircuitTimer calcStint];
  [self loadStintData];
  [raceScheduleView reloadData];
}



- (void)loadStintData {
  CTStintInfo sinfo;
  [MCircuitTimer getStintInfo:currentStint_ stintInfo: &sinfo];

  riderName.text = sinfo.riderName;
  assumeFuelCost.text = [NSString stringWithFormat: @"%.1lf",sinfo.fuelCost];
  assumeAverageLapTimeSec.text = [NSString stringWithFormat: @"%d:%d.%02d"
										   , sinfo.assumeAverage.min_
										   , sinfo.assumeAverage.sec_
										   , sinfo.assumeAverage.usec_ / 10000];
  stintLaps.text = [NSString stringWithFormat: @"%d", sinfo.laps];
  [fullFuelSw setOn: (BOOL)sinfo.fuelFlag animated:YES];
  [wheelChangeSw setOn: (BOOL)sinfo.wheelFlag animated:YES];
  riderSelect.selectedSegmentIndex = sinfo.riderNum;  
}

- (void)updateRiderInfo {

  [MCircuitTimer updateRiderInfo:currentRider_
					   riderName:riderName.text
				 assumeLapTimeMin:assumeAverageLapTimeMin.text 
				 assumeLapTimeSec:assumeAverageLapTimeSec.text 
				 assumeFuelCost:assumeFuelCost.text];
  

}


- (IBAction)onEditEndRaceInfo {
  printf("onEditEndRaceInfo\n");
  [self updateRaceInfo];

  [MCircuitTimer calcStint];
  [self loadStintData];
  [raceScheduleView reloadData];
}

- (void)updateTeamInfo {
  CTTeamInfo tinfo;

  tinfo.teamName = [NSString stringWithString: teamName.text];
  tinfo.numOfRiders = [numOfRiders.titleLabel.text intValue];
  tinfo.pitStopTime = [pitStopTime.text doubleValue];
  tinfo.withFuelFullTime = [withFuelFullTime.text doubleValue];
  tinfo.withWheelsChangeTime = [withWheelsChangeTime.text doubleValue];
  tinfo.inLapLossTime = [inLapLossTime.text doubleValue];
  tinfo.outLapLossTime = [outLapLossTime.text doubleValue];
  //scLapTime.text = @"HOGE";
  //sightingLaps.text = @"1";
  tinfo.sightingFuelCost = [sightingFuelCost.text doubleValue];
  tinfo.pitInTimingValue = [pitInTimingValue.text doubleValue];
  tinfo.pitInTimingType = (CTPitInTimingType)pitInTimingType.selectedSegmentIndex;
  tinfo.machineName = [NSString stringWithString: machineName.text];
  tinfo.fuelCapacity = [fuelCapacity.text doubleValue];
  
  [MCircuitTimer setTeamInfo: &tinfo]; 
}

- (IBAction)onEditEndTeamInfo {
  [self updateTeamInfo];
  [MCircuitTimer calcStint];
  [self loadStintData];
  [raceScheduleView reloadData];

}

- (IBAction)onEditEndStintInfo {

  [MCircuitTimer setStintInfo:currentStint_
				rider: riderSelect.selectedSegmentIndex
				riderName: riderName.text
				laps: [stintLaps.text intValue]
				fuelFlag: fullFuelSw.on
				wheelFlag: wheelChangeSw.on
				fuelCost: [assumeFuelCost.text doubleValue]
				average: assumeAverageLapTimeSec.text
   ];

  [MCircuitTimer calcStint];
  [raceScheduleView reloadData];
}



#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
  if (popoverController_ != nil)  [popoverController_ autorelease];
  if (fileMenuItem_ != nil) [fileMenuItem_ autorelease];  
  if (fileBtnItem_ != nil) [fileBtnItem_ autorelease];  
  if (stintTitleViewController_ != nil) [stintTitleViewController_ autorelease];
  stintTitleViewController_ = nil;
}

- (void)viewWillDisappear:(BOOL)animated {
  [self updateRaceInfo];
  [self updateTeamInfo];
  [MCircuitTimer save:YES];
}



- (void)dealloc {
  [super dealloc];
}


- (void)onFileBtn:(id)sender {
  if (self.popoverController_ != nil) {
	// cancel
	[self onFileCancelBtn];
	return; // 
  }
  fileListDialog_ = [[FileListDialog alloc] init];
  UIPopoverController* aPopover = [[UIPopoverController alloc] initWithContentViewController:fileListDialog_];
  fileListDialog_.popoverController_ = aPopover;
  aPopover.delegate = self;
  
  aPopover.popoverContentSize = CGSizeMake(480.0f, 720.0f);
  [fileListDialog_ release];
  // 後から使用するために、Popoverをカスタムプロパティに格納する
  self.popoverController_ = aPopover;
  [aPopover release];


  [self.popoverController_ presentPopoverFromBarButtonItem:sender
					  permittedArrowDirections:UIPopoverArrowDirectionAny
					  animated:YES];
  /*
  [popoverController_ presentPopoverFromRect: self.navigationItem.rightBarButtonItem.frame // sender.frame // CGRectMake(100.0f, 512.0f, 10.0f, 10.0f)
					 inView: self.view // self.navigationController.visibleViewController.view
					 permittedArrowDirections: UIPopoverArrowDirectionAny
					 animated: YES];
  */

  [self setNaviBtn:NO];


  
}

- (IBAction)onFileCancelBtn {
  [popoverController_ dismissPopoverAnimated:YES];
  [popoverController_ autorelease];
  popoverController_ = nil;
  [self setNaviBtn:YES];
}
- (IBAction)onFileDeleteBtn {
  [fileListDialog_ setEditing: !fileListDialog_.editing animated:YES];
  
}
- (IBAction)onFileAddNewBtn {
}


- (void)onSelectFile:(NSString*)filename {
  [popoverController_ dismissPopoverAnimated:YES];
  [popoverController_ autorelease];
  popoverController_ = nil;

  [MCircuitTimer load: filename];
  [self setNaviBtn:YES];
  [self reloadAllView];
}

//　ポップビューの外側がタップされて消えた場合
- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController 
{
  [popoverController_ autorelease];
  popoverController_ = nil;
  [self setNaviBtn:YES];
  return YES;
}

- (void)setNaviBtn:(BOOL)filebtn {
  if (filebtn == YES) {
	self.navigationItem.rightBarButtonItem = fileBtnItem_;
  }
  else {
	self.navigationItem.rightBarButtonItem = fileMenuItem_;
  }
}
- (IBAction)onDateBtn {
  if (self.popoverController_ != nil) {
	// cancel
	[self onFileCancelBtn];
	return; // 
  }
  datePickerView_ = [[DatePickerView alloc] initWithDate:5
											year:raceDate_.year_
											month:raceDate_.month_
											day:raceDate_.day_
											hour:raceDate_.hour_
											min:raceDate_.min_
											sec:0];

  UIPopoverController* aPopover = [[UIPopoverController alloc] initWithContentViewController: datePickerView_.view_];
  datePickerView_.popoverController_ = aPopover;
  aPopover.delegate = self;
  datePickerView_->delegate_ = self;
  
  aPopover.popoverContentSize = CGSizeMake([datePickerView_ getWidth], 262.0f); //PitWorkDialog内部で設定
  // 後から使用するために、Popoverをカスタムプロパティに格納す
  self.popoverController_ = aPopover;
  [aPopover release];
  [datePickerView_ showCurrent];

  /*

  [self.popoverController_ presentPopoverFromBarButtonItem:sender
					  permittedArrowDirections:UIPopoverArrowDirectionAny
					  animated:YES];
   */

  [popoverController_ presentPopoverFromRect: dateButton.frame // sender.frame // CGRectMake(100.0f, 512.0f, 10.0f, 10.0f)
					  inView: self.view // self.navigationController.visibleViewController.view
					  permittedArrowDirections: UIPopoverArrowDirectionAny
					  animated: YES];


}

- (void) onDatePickerOk {
  [popoverController_ dismissPopoverAnimated:YES];
  [popoverController_ autorelease];
  popoverController_ = nil;
  
  [dateButton setTitle: [NSString stringWithFormat: @"%04d/%02d/%02d %02d:%02d"
								  , datePickerView_->year_
								  , datePickerView_->month_
								  , datePickerView_->day_ 
								  , datePickerView_->hour_
			                      , datePickerView_->min_ ]
			  forState: UIControlStateNormal];

  raceDate_.year_ = datePickerView_->year_;
  raceDate_.month_ = datePickerView_->month_;
  raceDate_.day_ = datePickerView_->day_;
  raceDate_.hour_ = datePickerView_->hour_;
  raceDate_.min_ = datePickerView_->min_;
  [datePickerView_ release];
}

- (IBAction)onNumOfRiders {
  numericPickerView_ = [[NumericPickerView alloc] initWithRect: self 
												  rootview: self.view
												  arrow:numOfRiders.frame
												  userid: numOfRiders
												  underPoint: 0
						];
  numericPickerView_.value_ = [numOfRiders.titleLabel.text intValue];
}

- (IBAction)onRaceDistance {
  numericPickerView_ = [[NumericPickerView alloc] initWithRect: self 
												  rootview: self.view
												  arrow:raceDistance.frame
												  userid: raceDistance
												  underPoint: 1];
  numericPickerView_.value_ = [raceDistance.titleLabel.text doubleValue];
}


- (void) onNumericPickerOk:(id)userid {

}
- (void) onNumericPickerClosed:(id)userid {
  numericPickerView_ = nil;
}
@end

