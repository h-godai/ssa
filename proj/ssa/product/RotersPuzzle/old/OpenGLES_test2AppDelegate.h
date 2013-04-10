//
//  OpenGLES_test2AppDelegate.h
//  OpenGLES-test2
//
//  Created by IKEDA KOHEI on 11/04/01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class OpenGLES_test2ViewController;

@interface OpenGLES_test2AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    OpenGLES_test2ViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet OpenGLES_test2ViewController *viewController;

@end

