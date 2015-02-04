//
//  DetailViewController.h
//  PerformanceTest
//
//  Created by IKEDA KOHEI on 13/02/23.
//  Copyright (c) 2013年 IKEDA KOHEI. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DetailViewController : UIViewController <UISplitViewControllerDelegate>

@property (strong, nonatomic) id detailItem;

@property (weak, nonatomic) IBOutlet UILabel *detailDescriptionLabel;
@property (weak, nonatomic) IBOutlet UITextView *textArea_;
@end
