// -*-mode:ObjC;tab-width:4-*-
//  DatePickerView.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/07.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol DatePickerViewDelegate
- (void) onDatePickerOk;
@end


@interface DatePickerView : UIViewController <UIPickerViewDelegate, UIPickerViewDataSource>
//@interface DatePickerView : UINavigationController <UIPickerViewDelegate, UIPickerViewDataSource>
{
  UINavigationController* view_;
  IBOutlet UIPickerView* pickerView;
  UIPopoverController* popoverController_;
@public
  id <DatePickerViewDelegate> delegate_;
  int numOfFields_;
  int year_;
  int month_;
  int day_;
  int hour_;
  int min_;
  int sec_;
}

@property  (nonatomic, retain) UIPopoverController* popoverController_;
@property  (nonatomic, retain) UINavigationController* view_;

- (id)initWithDate:(int)fields year:(int)year month:(int)month day:(int)day hour:(int)hour min:(int)min sec:(int)sec;
- (void)onOkBtn:(id)sender;
- (CGFloat)getWidth;
- (void) showCurrent;

@end
