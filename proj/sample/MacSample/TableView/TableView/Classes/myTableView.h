//
//  myTableView.h
//
//  Created by nishi on 10/08/25.
//  Copyright 2010 TARGET ENTERTAINMENT INC.. All rights reserved.
//

#import <UIKit/UIKit.h>
// #import <Foundation/Foundation.h>

@interface myTableView : UITableView<UITableViewDelegate, UITableViewDataSource>
{
	NSIndexPath *selected;
}

@property (nonatomic, retain) NSIndexPath *selected;

@end
