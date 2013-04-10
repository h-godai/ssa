// -*-tab-width:4-*-
//  CircuitTimerAppDelegate.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/08/05.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "CircuitTimerAppDelegate.h"
#import "ConfigViewController.h"
#import "RaceHistoryCellViewController.h"

@implementation CircuitTimerAppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize popoverController;



#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
  // Override point for customization after application launch.
  [window addSubview:navigationController.view];
  [window makeKeyAndVisible];

  [self startIntervalTimer];
  
  NSArray* paths;
  NSString* path;
  paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  if ([paths count] > 0) {
	path = [paths objectAtIndex:0];
	[MCircuitTimer setDocumentPath:path];
	[MCircuitTimer load:nil];
  }
  if ([MCircuitTimer getHistoryCount] == 0) {
	[lapButton_ setTitle:@"START" forState:UIControlStateNormal];
  }
  showStint_ = 0;
  return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
	[timer invalidate];
	[timer release];
	timer = nil;
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
  [navigationController release];
  [window release];
  [super dealloc];
}


- (void)startIntervalTimer {
  timer = [NSTimer scheduledTimerWithTimeInterval:1.0f/15.0f //0.0167f
		   target:self selector:@selector(timerFired:)
		   userInfo:nil repeats:YES];
}

- (void)timerFired:(NSTimer*)timer
{
  if (![MCircuitTimer isImmidiateMode]) {
	currentLapText_.text = [MCircuitTimer getLapTime];
  }
  totalLapText_.text = [MCircuitTimer getTotalTime];
  
  NSDate* now_date = [NSDate date];
  NSDateFormatter* formatter = [[[NSDateFormatter alloc] init] autorelease];
  [formatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
  dateLabel.text = [formatter stringFromDate:now_date];
}

- (IBAction)onPitStopBtn {
  if ([MCircuitTimer isStarted] == YES) {
    latestLapTime.text = [MCircuitTimer pitStop];
    [lapButton_ setTitle:@"Pit-Out" forState:UIControlStateNormal];
    currentLapLabel_.text = @"PitStopTime";
    latestLapLabel.text =[NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount] ];
	[self updateInfoText];
	[self updateHistory];
  }
  [self pitWork: pitStopBtn];
}

- (IBAction)onFinishBtn {
  actsheet = [[[UIActionSheet alloc]
				initWithTitle:@"レースの終了"
				delegate: self 
				cancelButtonTitle: nil // @"キャンセル"
				destructiveButtonTitle: @"レースを終了する" // nil
				otherButtonTitles:
				  //NSLocalizedString(@"押し忘れ処理を記録", nil),
				  NSLocalizedString(@"キャンセル", nil),
				nil]
			   autorelease];
//  [actsheet showInView: navigationController.view];
  [actsheet showFromBarButtonItem: navigationController.navigationBar.topItem.leftBarButtonItem animated: YES];
  

  actionSheetMode = ActionSheet_EndOfRaceAction; 


}
- (IBAction)onLapBtn {
  if ([MCircuitTimer isImmidiateMode]) {
	[MCircuitTimer setCurrentLapTime:currentLapText_.text];
  }
    
  if ([MCircuitTimer isPitStop]) {
    // ピットアウト
	[self onPitOutAction:YES];
	return;
  }
  else if ([MCircuitTimer isStarted] == NO) {
	// スタート
	[MCircuitTimer startTimer];
	[lapButton_ setTitle:@"LAP" forState:UIControlStateNormal];
  }
  else {
	// 通常のラップ
	latestLapTime.text = [MCircuitTimer lapTimer];
	currentLapLabel_.text = [NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount]];
	latestLapLabel.text =[NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount] - 1];
  }
  

  [self updateRealStintInfo];
  [self updateInfoText];
  [self updateHistory];
}

- (void)onPitOutAction:(BOOL)changeStint {
  [lapButton_ setTitle:@"LAP" forState:UIControlStateNormal];
  currentLapLabel_.text = [NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount] + 1];
  latestLapLabel.text = @"PitStopTime";
  latestLapTime.text = [MCircuitTimer pitOut:changeStint];
  showStint_ = [MCircuitTimer getCurrentStint];
  stintSelector_.selectedSegmentIndex = showStint_;

  [self updateRealStintInfo];
  [self updateInfoText];
  [self updateHistory];
}

- (void)updateRealStintInfo {
  [MCircuitTimer getRealStintInfo:showStint_
				rider:stintRiderName_
				best:bestLapText_ 
				aver:lapAverageText_
				stintlap:stintStartLap_
				stinttime:stintTime_
				fulllap:fullLapCountText_
				assumeFuelCost:assumeFuelCost_
				realFuelCost:realFuelCost_
   ];
}

- (void)updateInfoText {
  int stint = [MCircuitTimer getCurrentStint];
  if ([MCircuitTimer isPitStop]) {
    msgText_.text = [NSString stringWithFormat: @"Lap-%u PitBox", [MCircuitTimer getLapCount]];
  }
  else {
	msgText_.text = [NSString stringWithFormat: @"Lap-%u running", [MCircuitTimer getLapCount]+1];
  }

  fuelLeftText_.text = [NSString stringWithFormat:@"%.2lf", [MCircuitTimer getFuelLeft]];
  canLapLeftText_.text = [NSString stringWithFormat:@"%.1lf", [MCircuitTimer getCanLapLeft]];

  ridersLapText_.text = [MCircuitTimer getRiderLapLeft];
  leftLapText_.text = [MCircuitTimer getStintLapLeft];
  stintLabel_.text = [NSString stringWithFormat: @"STINT-%d", [MCircuitTimer getCurrentStint] + 1];
  currentRiderName_.text = [MCircuitTimer getRiderName:[MCircuitTimer getRider:stint]];
  nextRiderName_.text = [MCircuitTimer getRiderName:[MCircuitTimer getNextRider:stint]];


}

- (void)updateHistory {
  [historyList reloadData];
  int lines = [MCircuitTimer getHistoryCount];
  if (lines > 0) {
	NSIndexPath* indexPath = [NSIndexPath indexPathForRow: lines-1
												inSection: 0];
	[ historyList scrollToRowAtIndexPath: indexPath 
		atScrollPosition: UITableViewScrollPositionBottom
		animated: YES];
  }
}

// TableViewのデリゲート
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
  return [MCircuitTimer getHistoryCount];
}

- (UITableViewCell *)tableView:(UITableView *)tableView
	 cellForRowAtIndexPath:(NSIndexPath *)indexPath {

  static NSString *identifier = @"RaceHistoryCellView";
  RaceHistoryCellView *cell = (RaceHistoryCellView *)[tableView dequeueReusableCellWithIdentifier:identifier];
  if (cell == nil) {
	RaceHistoryCellViewController *controller = 
	  [[RaceHistoryCellViewController alloc] initWithNibName:identifier
											 bundle:nil];
	cell = (RaceHistoryCellView *)controller.view;
	[controller release];
	
	//UIView *bgView = [[UILabel alloc] initWithFrame:CGRectZero];
	//cell.backgroundView = bgView;				// ここでbgViewがretainされる
	//[bgView release];							// autoreleaseではないので、自分でrelease
  }

  CTActionLog act;
  [MCircuitTimer getActionLog: indexPath.row actionlog:&act];

  [cell.stint_ setText: [NSString stringWithFormat: @"%d", act.stint+1]];
  [cell.rider_ setText: [MCircuitTimer getRiderName: act.rider]];
  [cell.time_  setText: [NSString stringWithFormat: @"%02d:%02d:%02d"
								  , act.total.hour_
								  , act.total.min_
								  , act.total.sec_]];
  [cell.laptime_  setText: [NSString stringWithFormat: @"%d'%02d.%03d"
								  , act.lap.min_
								  , act.lap.sec_
								  , act.lap.usec_/1000]];
  [cell.laps_ setText: [NSString stringWithFormat: @"%d", act.lapCount]];
  [cell.fuelleft_ setText: [NSString stringWithFormat: @"%.2lf", act.fuelLeft]];
	   
#if 0
  static NSString *CellIdentifier = @"Cell";
  
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
  if (cell == nil) {
    cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
  }
	
  // Set up the cell...
  cell.textLabel.text = [MCircuitTimer getHistoryText: indexPath.row];
	  // [NSString stringWithFormat: @"%d:hoge", indexPath.row];
  cell.textLabel.font = [UIFont fontWithName:@"Courier" size:18]; //[UIFont fontWithName: @"Monaco" size:18];
  //(CGFloat)fontSize
	//[UIFont systemFontOfSize:18];
#endif
  return cell;
}

- (IBAction)configBtn {
#if 1
  ConfigViewController* configView = [[ConfigViewController alloc] initWithNibName:@"ConfigViewController"
											 bundle: nil];
  [self.navigationController pushViewController:configView animated:YES];
  [configView release];
  //  [configView retain];
#else
  PitWorkDialog* content = [[PitWorkDialog alloc] init];
  [self.navigationController pushViewController:content animated:YES];
#endif

}

// デバイスの回転に対応するかどうか
- (BOOL)shouldAutorotateToInterfaceOrientation:
(UIInterfaceOrientation)interfaceOrientation
{
  return YES;
}

- (IBAction)onSelectStint {
  showStint_ = stintSelector_.selectedSegmentIndex;
  [self updateRealStintInfo];
}

- (IBAction)onDeleteBtn {
  actsheet = [[[UIActionSheet alloc]
			//initWithTitle: nil
			initWithTitle:@"最後の記録を消去しますか？"
			delegate: self 
			//cancelButtonTitle: NSLocalizedString(@"Cancel", nil) 
 		    cancelButtonTitle: nil // @"キャンセル"
			destructiveButtonTitle: @"消去する" // nil
			otherButtonTitles:
			NSLocalizedString(@"キャンセル", nil),
			// NSLocalizedString(@"Google", nil),  
			   nil]
           autorelease];
  [actsheet showInView: navigationController.view];
  actionSheetMode = ActionSheet_DeleteAction; 
}

- (IBAction)onMissLapBtn {
  actsheet = [[[UIActionSheet alloc]
				initWithTitle:@"Lapボタンの押し忘れ処理"
				delegate: self 
				cancelButtonTitle: nil // @"キャンセル"
				destructiveButtonTitle: nil //@"消去する" // nil
				otherButtonTitles:
				  NSLocalizedString(@"押し忘れ処理を記録", nil),
				NSLocalizedString(@"キャンセル", nil),
				nil]
			   autorelease];
  [actsheet showInView: navigationController.view];
  actionSheetMode = ActionSheet_MissLapAction; 
}

- (void)actionSheet:(UIActionSheet *)sheet
didDismissWithButtonIndex:(NSInteger)index
{
  if( index == [sheet cancelButtonIndex]) {
	// Do Nothing
  }
  else if( index == [sheet destructiveButtonIndex] ) {
	// Show Alert...
	switch (actionSheetMode) {
	case ActionSheet_DeleteAction:
	  [MCircuitTimer deleteLastHistory];
	  [self updateHistory];
	  break;
	case ActionSheet_EndOfRaceAction:
	  [MCircuitTimer stopTimer];
	  [lapButton_ setTitle:@"Continue" forState:UIControlStateNormal];
	  [self updateHistory];
	  [MCircuitTimer save:NO];
	  break;
	default:
	  break;
	}
  }
  else {
	switch (actionSheetMode) {
	case ActionSheet_MissLapAction:
	  if (index == 0) {
		printf("MissLap");
		[MCircuitTimer missLap];
		currentLapLabel_.text = [NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount] + 1];
		latestLapLabel.text =[NSString stringWithFormat: @"Lap-%u", [MCircuitTimer getLapCount] ];
		[self updateRealStintInfo];
		[self updateInfoText];
		[self updateHistory];
	  }
	  break;
	case ActionSheet_EndOfRaceAction:
	default:
	  break;
	}
	// ...
  }
}  

- (IBAction)onFuelBtn {
  [MCircuitTimer fuelIn:fuelInText_.text];
  [self updateHistory];
}

- (IBAction)onSelectImmidiateInput {
  [MCircuitTimer setImmidiateMode: immidateInputSw_.on];
}

- (IBAction)onDidEndEditLapTime: (id)sender {
  [currentLapText_ resignFirstResponder];
  [MCircuitTimer setCurrentLapTime:currentLapText_.text];

}

- (IBAction)onPitWorkBtn {
  [self pitWork: pitWorkBtn];
}

- (void)pitWork: (UIButton*)btn {
	PitWorkDialog* content = [[PitWorkDialog alloc] init];
	if ([MCircuitTimer isPitStop]) {
	  content.pitWorkDialogMode = JustNowPitWorkd;
	}
	else {
	  content.pitWorkDialogMode = EditLatestPitWork;
	}
	UIPopoverController* aPopover = [[UIPopoverController alloc] initWithContentViewController:content];
	content.popoverController = aPopover;
	aPopover.delegate = self;
	

	//	aPopover.popoverContentSize = CGSizeMake(320.0f, 480.0f); //PitWorkDialog内部で設定
	[content release];
	// 後から使用するために、Popoverをカスタムプロパティに格納する
	self.popoverController = aPopover;
	[aPopover release];
	/*
	  [self->popoverController presentPopoverFromBarButtonItem:sender
	  permittedArrowDirections:UIPopoverArrowDirectionAny
	  animated:YES];
	*/
	[popoverController presentPopoverFromRect: btn.frame // CGRectMake(100.0f, 512.0f, 10.0f, 10.0f)
					   inView: self.navigationController.visibleViewController.view
					   permittedArrowDirections: UIPopoverArrowDirectionUp // UIPopoverArrowDirectionAny
					   animated: YES];

}


- (void)navigationController:(UINavigationController *)navigationController 
	   didShowViewController:(UIViewController *)viewController 
					animated:(BOOL)animated
{
  NSLog(@"didShowViewController %@\n", viewController.title);
  //if ([viewController.title compare:@"Main"] == NSOrderedSame) {
  //[self updateRealStintInfo];
  //[self updateInfoText];
  //[self updateHistory];
  //}
}
- (void)navigationController:(UINavigationController *)navigationController
	  willShowViewController:(UIViewController *)viewController 
					animated:(BOOL)animated
{
  NSLog(@"willShowViewController %@\n", viewController.title);
  
  
}


@end
