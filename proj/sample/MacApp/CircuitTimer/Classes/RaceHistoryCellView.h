//
//  RaceHistoryCellView.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/15.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface RaceHistoryCellView : UITableViewCell {

  IBOutlet UILabel* stint_;
  IBOutlet UILabel* time_;
  IBOutlet UILabel* rider_;
  IBOutlet UILabel* laps_;
  IBOutlet UILabel* status_;
  IBOutlet UILabel* laptime_;
  IBOutlet UILabel* fuelleft_;
  IBOutlet UILabel* fuelcost_;

}

@property (nonatomic, retain)   IBOutlet UILabel* stint_;
@property (nonatomic, retain)   IBOutlet UILabel* time_;
@property (nonatomic, retain)   IBOutlet UILabel* rider_;
@property (nonatomic, retain)   IBOutlet UILabel* laps_;
@property (nonatomic, retain)   IBOutlet UILabel* status_;
@property (nonatomic, retain)   IBOutlet UILabel* laptime_;
@property (nonatomic, retain)   IBOutlet UILabel* fuelleft_;
@property (nonatomic, retain)   IBOutlet UILabel* fuelcost_;

@end
