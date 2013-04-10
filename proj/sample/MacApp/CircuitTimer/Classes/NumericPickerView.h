// -*-mode:ObjC;tab-width:4-*-
//  NumericPickerView.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/18.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol NumericPickerViewDelegate
- (void) onNumericPickerOk:(id)userid;       // DoneもしくはOKボタンが押された
- (void) onNumericPickerClosed:(id)userid;   // Popoverが閉じた
@end



@interface NumericPickerView
  : UIViewController <UIPickerViewDelegate
  					, UIPickerViewDataSource
					, UIPopoverControllerDelegate
					 >
{
  UIPopoverController* popoverController_;

  UIPickerView* pickerView_;
  UINavigationController* view_;
@public
  id <NumericPickerViewDelegate> delegate_;
  id userid_;
  float width_;
  int minValue_;
  int maxValue_;
  float value_;
  int numOfUnderPoint_; // 小数点以下の桁数
  int c0num_;
}

@property  (nonatomic, retain) UIPopoverController* popoverController_;
@property  (nonatomic, retain) UINavigationController* view_;
@property  (readonly) float width_;
@property int minValue_;
@property int maxValue_;
@property float value_;
@property int numOfUnderPoint_; // 小数点以下の桁数


- (id)initWithRect:(id<NumericPickerViewDelegate>)delegate
			 rootview:(UIView*)rootview
				arrow:(CGRect)frame
			   userid:(id)userid
		   underPoint:(int)underPoint;
- (void)onDoneBtn:(id)sender;

@end
