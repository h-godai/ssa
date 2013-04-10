//
//  StintCellViewController.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/13.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "StintCellView.h"

@interface StintCellViewController : UIViewController {

  IBOutlet StintCellView* cell_;
}

@property (nonatomic, retain) StintCellView *cell_;



@end
