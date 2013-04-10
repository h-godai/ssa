// -*-tab-width:4-*-
//  DatePickerView.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/07.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "DatePickerView.h"


@implementation DatePickerView
@synthesize popoverController_;
@synthesize view_;

- (id)initWithDate:(int)fields year:(int)year month:(int)month day:(int)day hour:(int)hour min:(int)min sec:(int)sec
{
  id result = [super init];
  numOfFields_ = fields;
  year_ = year;
  month_ = month;
  day_ = day;
  hour_ = hour;
  min_ = min;
  sec_ = sec;
  return result;
}
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
  if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
	// Custom initialization

	numOfFields_ = 3;
	year_ = 2010;
	month_ = 9;
	day_ = 12;
	hour_ = 12;
	min_ = 34;
	sec_ = 56;
	self.view_ = [[UINavigationController alloc] initWithRootViewController: self];
	UIBarButtonItem* btn = [[UIBarButtonItem alloc] 
							initWithBarButtonSystemItem: UIBarButtonSystemItemDone
							//initWithTitle:@"Done" 
							//style:UIBarButtonItemStyleBordered
							target: self
							action:@selector(onOkBtn:)
							]; 
	self.navigationItem.rightBarButtonItem = btn;
	[btn release];
	[view_ release];
  }
  return self;
}



/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
  [super viewDidLoad];
  
  

}

- (void) showCurrent {
  [pickerView selectRow:10 inComponent:0 animated:NO];
  switch (numOfFields_) {
  case 5:
	[pickerView selectRow:min_ inComponent:4 animated:NO];
  case 4:
	[pickerView selectRow:hour_ inComponent:3 animated:NO];
  case 3:
	[pickerView selectRow:day_ - 1 inComponent:2 animated:NO];
  case 2:
	[pickerView selectRow:month_ - 1 inComponent:1 animated:NO];
  case 1:
	[pickerView selectRow:year_ - 2000 inComponent:0 animated:NO];
	break;
  default:
	  break;
  
  }
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Overriden to allow any orientation.
    return YES;
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
	if (self.view_ != nil) {
	  [self.view_ autorelease];
	}
	pickerView = nil;
	self.popoverController_ = nil;
}


- (void)dealloc {
    [super dealloc];
}


// PickerView delegate

- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component {
  switch (component) {
  case 0: // year
  default:
	return 88.0f; // 2000-2037
  case 1: // month
	return 58.0f;
  case 2: // days
	return 58.0f;
  case 3: // hour
	return 58.0f;
  case 4: // min
	return 58.0f;
  case 5: // sec
	return 58.0f;
  }
}

- (CGFloat)getWidth {
  CGFloat w = 40.0f;
  for (int n = numOfFields_-1; n >= 0; --n) {
	w += [self pickerView: pickerView widthForComponent: n];
  }
  return w;
}


- (void) pickerView: (UIPickerView*)pView didSelectRow:(NSInteger) row  inComponent:(NSInteger)component {  
  NSLog(@"row=%d, component=%d", row, component);  

  switch(numOfFields_) {
  case 5:
	min_ =  [pView selectedRowInComponent:4];  
  case 4:
	hour_ = [pView selectedRowInComponent:3];  
  case 3:
	day_ = 1 + [pView selectedRowInComponent:2];  
  case 2:
	month_ = 1 + [pView selectedRowInComponent:1];  
  case 1:
	year_ = 2000 + [pView selectedRowInComponent:0];  
	break;
  default:
	break;
  }
}  

- (NSInteger) numberOfComponentsInPickerView:(UIPickerView *)pickerView {  
  return numOfFields_;  
}  

- (NSInteger) pickerView: (UIPickerView*)pView numberOfRowsInComponent:(NSInteger) component {
  switch (component) {
  case 0: // year
	return 38; // 2000-2037
  case 1: // month
	return 12;
  case 2: // days
	return 31;
  case 3: // hour
	return 24;
  case 4: // min
	return 60;
  case 5: // sec
	return 60;
  default:
	break;
  }
  return 1;
}  

- (NSString*)pickerView: (UIPickerView*) pView titleForRow:(NSInteger) row forComponent:(NSInteger)component {  
  switch (component) {
  case 0: // year
	return [NSString stringWithFormat:@"%d年", row + 2000];  
  case 1: // month
	return [NSString stringWithFormat:@"%d月", row + 1];  
  case 2: // days
	return [NSString stringWithFormat:@"%d日", row + 1];  
  case 3: // hour
	return [NSString stringWithFormat:@"%d時", row];  
  case 4: // min
	return [NSString stringWithFormat:@"%d分", row];  
  case 5: // sec
	return [NSString stringWithFormat:@"%d秒", row];  
  default:
	return @"ERROR";
  }
}  

- (IBAction)onOkBtn:(id)sender {
  //  id <DatePickerViewDelegate> de = popoverController_.delegate;
  [delegate_ onDatePickerOk];

}


@end
