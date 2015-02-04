//
//  MasterViewController.h
//  PerformanceTest
//
//  Created by IKEDA KOHEI on 13/02/23.
//  Copyright (c) 2013年 IKEDA KOHEI. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface MasterViewController : UITableViewController

@property (strong, nonatomic) DetailViewController *detailViewController;
//@property (strong, nonatomic) IBOutlet UIButton* button1_;

@end
