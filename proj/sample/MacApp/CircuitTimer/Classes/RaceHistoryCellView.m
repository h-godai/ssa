//
//  RaceHistoryCellView.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/15.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "RaceHistoryCellView.h"


@implementation RaceHistoryCellView

@synthesize stint_;
@synthesize time_;
@synthesize rider_;
@synthesize laps_;
@synthesize status_;
@synthesize laptime_;
@synthesize fuelleft_;
@synthesize fuelcost_;


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
    [super dealloc];
}


@end
