    //
//  NumericPickerView.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/18.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "NumericPickerView.h"


@implementation NumericPickerView
@synthesize popoverController_;
@synthesize view_;
@synthesize minValue_;
@synthesize maxValue_;
@synthesize numOfUnderPoint_; // 小数点以下の桁数

static const float colWidth = 36.0f;

// setter/getter
- (float)width_ {
  c0num_ = 1;
  if (maxValue_ > 99.9f) c0num_ = 3;
  else if (maxValue_ > 9.9f) c0num_ = 2;
  return 26.0f + c0num_ * colWidth + numOfUnderPoint_ * colWidth;
}

- (float)value_ {
  return value_;
}

- (void)setValue_:(float)val {
  int upt = (int)(val * 100)%100;
  value_ = val;
  [pickerView_ selectRow: (int)(value_  - minValue_) inComponent:0 animated:NO];
  [pickerView_ selectRow: upt/10 inComponent:1 animated:NO];
  [pickerView_ selectRow: upt%10 inComponent:2 animated:NO];
}

- (id)initWithRect:(id<NumericPickerViewDelegate>)delegate 
	     rootview:(UIView*)rootview
		arrow:(CGRect)frame
	       userid:(id)userid
  	   underPoint:(int)underPoint
{
  if ((self = [super init])) {
    userid_ = userid;
    numOfUnderPoint_ = underPoint;
    UIPopoverController* aPopover = 
      [[UIPopoverController alloc]
	initWithContentViewController: self.view_];
    self.popoverController_ = aPopover;
    [aPopover release];
    delegate_ = delegate;
    aPopover.delegate = self;
  
    aPopover.popoverContentSize = CGSizeMake(self.width_, 262.0f); //PitWorkDialog内部で設定
    [aPopover presentPopoverFromRect: frame
	      inView: rootview
	      permittedArrowDirections: UIPopoverArrowDirectionAny
	      animated: YES];
  }
  return self;
}


 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
      minValue_ = 1.0f;
      maxValue_ = 8.0f;
      value_ = 3.12f;
      numOfUnderPoint_ = 2;
      self.view_ = [[UINavigationController alloc] initWithRootViewController: self];
      UIBarButtonItem* btn = [[UIBarButtonItem alloc] 
			       initWithBarButtonSystemItem: UIBarButtonSystemItemDone
			       target: self
			       action:@selector(onDoneBtn:)
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
  pickerView_ = [[UIPickerView alloc] initWithFrame: CGRectMake(0.0f,0.0f,320.0f,320.0f)];
  pickerView_.delegate = self;
  pickerView_.showsSelectionIndicator = YES;
  self.view = pickerView_;
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
    pickerView_ = nil;
    self.popoverController_ = nil;
}


- (void)dealloc {
    [super dealloc];
}


//　ポップビューの外側がタップされて消えた場合
- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController 
{
  [popoverController_ autorelease];
  popoverController_ = nil;
  return YES;
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
  [delegate_ onNumericPickerClosed: userid_];   // Popoverが閉じた

}




// PickerView delegate

- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component {
  if (component == 0) {
    return c0num_ * colWidth;
  }
  else {
    return colWidth;
  }
}

- (void) pickerView: (UIPickerView*)pView didSelectRow:(NSInteger) row  inComponent:(NSInteger)component {  

  value_ = (int)(minValue_ + [pView selectedRowInComponent:0]);

  switch (numOfUnderPoint_) {
  case 3:
    value_ += [pView selectedRowInComponent:3] / 1000.0f;
  case 2:
    value_ += [pView selectedRowInComponent:2] / 100.0f;
  case 1:
    value_ += [pView selectedRowInComponent:1] / 10.0f;
    break;
  }
}  

- (NSInteger) numberOfComponentsInPickerView:(UIPickerView *)pickerView {  
  return numOfUnderPoint_ + 1;
}  

- (NSInteger) pickerView: (UIPickerView*)pView numberOfRowsInComponent:(NSInteger) component {
  if (component == 0) {
    return (int)(maxValue_ - minValue_ + 0.9f);
  }
  else {
    return 10;
  }
}  

- (NSString*)pickerView: (UIPickerView*) pView titleForRow:(NSInteger) row forComponent:(NSInteger)component {  
  if (component == 0) {
    if (numOfUnderPoint_ == 0) 
      return [NSString stringWithFormat: @"%d", (int)(row + minValue_)];
    else
      return [NSString stringWithFormat: @"%d.", (int)(row + minValue_)];
  }
  else {
    return [NSString stringWithFormat: @"%d", row];
  }
}  


- (void)onDoneBtn:(id)sender {
  [popoverController_ dismissPopoverAnimated:YES];
  [popoverController_ autorelease];
  popoverController_ = nil;
  [delegate_ onNumericPickerOk: userid_];
}
@end
