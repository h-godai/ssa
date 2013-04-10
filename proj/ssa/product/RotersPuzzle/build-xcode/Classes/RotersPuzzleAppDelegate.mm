//
//  RotersPuzzleAppDelegate.m
//  RotersPuzzle
//
//  Created by IKEDA KOHEI on 11/04/15.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RotersPuzzleAppDelegate.h"
//#import "RotersPuzzleViewController.h"

@implementation RotersPuzzleAppDelegate

//@synthesize window;
//@synthesize viewController;


#pragma mark -
#pragma mark Application lifecycle

  
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
  
  // Override point for customization after application launch.
  CGRect scrreenBounds = [[UIScreen mainScreen] bounds];
  window = [[UIWindow alloc] initWithFrame: scrreenBounds];
  viewController = [[GLView alloc] initWithFrame: scrreenBounds];
  [window addSubview: viewController];
  [window makeKeyAndVisible];
  
//    self.window.rootViewController = self.viewController;
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
  [viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
  [viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
  [viewController stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
