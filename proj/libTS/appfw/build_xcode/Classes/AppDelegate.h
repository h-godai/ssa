//
//  RotersPuzzleAppDelegate.h
//  RotersPuzzle
//
//  Created by IKEDA KOHEI on 11/04/15.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RotersPuzzleViewController;

@interface RotersPuzzleAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RotersPuzzleViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet RotersPuzzleViewController *viewController;

@end

