//
//  StintCellView.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/13.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface StintCellView : UITableViewCell {
  IBOutlet UIView* baseView_;
  IBOutlet UILabel* stint_;
  IBOutlet UILabel* laps_;
  IBOutlet UILabel* totalLaps_;
  IBOutlet UILabel* totalTime_;
  IBOutlet UILabel* stintTime_;
  IBOutlet UILabel* fuel_;
  IBOutlet UILabel* fuelLeft_;
  IBOutlet UILabel* tire_;
  IBOutlet UILabel* rider_;
  

}

@property (nonatomic, retain) IBOutlet UILabel* stint_;
@property (nonatomic, retain) IBOutlet UILabel* laps_;
@property (nonatomic, retain) IBOutlet UILabel* totalLaps_;
@property (nonatomic, retain) IBOutlet UILabel* totalTime_;
@property (nonatomic, retain) IBOutlet UILabel* stintTime_;
@property (nonatomic, retain) IBOutlet UILabel* fuel_;
@property (nonatomic, retain) IBOutlet UILabel* fuelLeft_;
@property (nonatomic, retain) IBOutlet UILabel* tire_;
@property (nonatomic, retain) IBOutlet UILabel* rider_;

@end
