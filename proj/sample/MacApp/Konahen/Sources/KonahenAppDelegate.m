
#import "KonahenAppDelegate.h"
#import "KonahenViewController.h"

@implementation KonahenAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	[UIApplication sharedApplication].idleTimerDisabled = YES;

	[window addSubview:viewController.view];
	[window makeKeyAndVisible];

	// self.window.rootViewController = self.viewController;
	return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application
{
	// アクティブから非アクティブになる際、起動される。ロックとかSMSとか着信とかバックグラウンドに行く時とか
	[self.viewController stopAnimation];
	[UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// アプリケーションがアクティブになる時、呼ばれる
	[self.viewController startAnimation];
	[UIApplication sharedApplication].idleTimerDisabled = YES;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// アプリケーションが終了する時、呼ばれる
	[self.viewController stopAnimation];
	[UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// バックグラウンドに行く時、呼ばれる。データ保存や不要なタイマーを無効にしておいてね
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// バックグランドからアクティブになる時、呼ばれる
}


- (void)dealloc
{
	[UIApplication sharedApplication].idleTimerDisabled = NO;

	[viewController release];
	[window release];
    
	[super dealloc];
}

@end
