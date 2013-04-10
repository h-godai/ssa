// -*-mode:ObjC;tab-width:4-*-
//
//  FileListDialog.h
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/01.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCircuitTimer.h"

@interface FileListDialog : UITableViewController {
  NSMutableArray* dirArray_;
  int selected_;
  UIPopoverController* popoverController_;
}

@property  (nonatomic, retain) UIPopoverController* popoverController_;

- (NSString*)getFileName: (int)index;

@end
