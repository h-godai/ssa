//
//  RootViewControl.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/09/03.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "RootViewControl.h"


@implementation RootViewControl


- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

- (void)dealloc {
    [super dealloc];
}
// デバイスの回転に対応するかどうか
- (BOOL)shouldAutorotateToInterfaceOrientation:
(UIInterfaceOrientation)interfaceOrientation
{
  return YES;
}



@end
