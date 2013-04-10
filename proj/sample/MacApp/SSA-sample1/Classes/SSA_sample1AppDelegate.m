//
//  SSA_sample1AppDelegate.m
//  SSA-sample1
//
//  Created by IKEDA KOHEI on 10/05/11.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "SSA_sample1AppDelegate.h"
#import "EAGLView.h"

@implementation SSA_sample1AppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)dealloc
{
    [window release];
    [glView release];

    [super dealloc];
}

@end
