//
//  StintCellView.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/13.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "StintCellView.h"


@implementation StintCellView
@synthesize stint_;
@synthesize laps_;
@synthesize totalLaps_;
@synthesize totalTime_;
@synthesize stintTime_;
@synthesize fuel_;
@synthesize fuelLeft_;
@synthesize tire_;
@synthesize rider_;


- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    if ((self = [super initWithStyle:style reuseIdentifier:reuseIdentifier])) {
        // Initialization code
    }
    return self;
}


- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}


- (void)dealloc {
  [stint_ release];
  [laps_ release];
  [totalLaps_ release];
  [totalTime_ release];
  [stintTime_ release];
  [fuel_ release];
  [fuelLeft_ release];
  [tire_ release];
  [rider_ release];

  [super dealloc];
}


@end
