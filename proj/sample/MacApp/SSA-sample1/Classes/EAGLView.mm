//
//  EAGLView.mm
//  Last Gladiators 9.7
//
//  Created by 平松 邦仁 on 09/11/10.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "EAGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

#include <sys/sysctl.h>

#include "IGLcore.hpp"
//#include "pinball.hpp"
//#include "lgr.hpp"
#include "mainroot.hpp"

#define kFilteringFactor			0.1f

@implementation EAGLView

@synthesize animating;
@dynamic animationFrameInterval;

mainroot* g_app_core;

// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
//		renderer = [[ES2Renderer alloc] init];
//		if (!renderer)
		{
			renderer = [[ES1Renderer alloc] init];
			if (!renderer)
			{
				[self release];
				return nil;
			}
		}
#if 1
		gl::create_glcore();
		gl::IGLcore* igl = gl::get_glcore();
		if(igl) {
#ifndef NDEBUG
			printf("application glcore create!\n");
#endif
			int argc = 0;
			igl->initialize(argc, NULL);
			// カレントの解像度は、アプリケーションの初期化で設定するので、ここでは、仮の値を代入
			igl->setup(-1, -1, "LGR.MMX application", true);

			NSString* fullpath = [[NSBundle mainBundle] pathForResource:@"Logo" ofType:@"png"];
#ifndef NDEBUG			
			printf("(0)Full path: '%s'\n",[fullpath cStringUsingEncoding:NSUTF8StringEncoding]);
#endif
			NSRange rng = [fullpath rangeOfString:(NSString*)@"Logo"];
			NSString* path = [fullpath substringToIndex:rng.location];
#ifndef NDEBUG
			printf("(1)path: '%s'\n",[path cStringUsingEncoding:NSUTF8StringEncoding]);
#endif
			fio::fpathman& fph = igl->at_fpathman();
			fph.initialize((const char *)[path cStringUsingEncoding:NSUTF8StringEncoding]);
		}
	
		g_app_core = new mainroot;
		g_app_core->initialize();
#endif
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;
		displayLink = nil;
		animationTimer = nil;
		
		self.multipleTouchEnabled = YES;
		
		[UIApplication sharedApplication].idleTimerDisabled = YES;
		
		accelerometer = [UIAccelerometer sharedAccelerometer];
		accelerometer.updateInterval = 1.0f / 30.0f;
		accelerometer.delegate = self;		
		
//		m_aplayer = [[AudioQueuePlayer alloc] init];
		m_bgm_a = -1;
		m_bgm_b = -1;
		m_bgm_loop = 0;

		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending) {
			displayLinkSupported = TRUE;
		}
		
    }
	
    return self;
}

- (void) drawView:(id)sender
{
	// オーディオ（BGM）のリクエスト
//	int a, b;
#if 0
	if(pinball::GetTrackCD(a, b)) {
		[m_aplayer play: a int:b];
	}
	[m_aplayer service];
#endif
    [renderer render_begin];
#if 1
	gl::IGLcore* igl = gl::get_glcore();
	igl->service();
	g_app_core->service();
#endif
	[renderer render_end];
}

- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is two on a display that refreshes 60 times a second. The default
	// frame interval setting of one will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if(frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if(animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if(displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.

			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		} else {
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		}
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating)
	{
		if(displayLinkSupported)
		{
			[displayLink invalidate];
			displayLink = nil;
		}
		else
		{
			[animationTimer invalidate];
			animationTimer = nil;
		}
		
		animating = FALSE;
	}
}

- (void) dealloc
{
	[UIApplication sharedApplication].idleTimerDisabled = NO;
	delete g_app_core;
	g_app_core = 0;
	gl::destroy_glcore();	

#if 0
	[m_aplayer release];
#endif
    [renderer release];
	
    [super dealloc];
}


// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	gl::IGLcore* Igl = gl::get_glcore();
	if(Igl) {
		gldev& dev = Igl->at_device();
		UITouch* touch;
	/*if ( [touches count] == 1 )
	 {
	 touch = [[event touchesForView:self] anyObject];
	 [delegate settouch:touch touchevent:1 touchview:self];
	 }
	 else{*/
		NSEnumerator *touchenm = [touches objectEnumerator];
		while ( (touch = [touchenm nextObject]) )
		{
			CGPoint location = [touch locationInView: self];
			dev.set_touch_event(began, location.x, location.y);
		}
	}
}

	
// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	gl::IGLcore* Igl = gl::get_glcore();
	if(Igl) {
		gldev& dev = Igl->at_device();
		UITouch* touch = [[event touchesForView:self] anyObject];
		CGPoint location = [touch locationInView: self];
		dev.set_touch_event(moved, location.x, location.y);
	}
}


// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	gl::IGLcore* Igl = gl::get_glcore();
	if(Igl) {
		gldev& dev = Igl->at_device();
		UITouch *touch;
	/*if ( [touches count] == 1 )
	 {
	 touch = [[event touchesForView:self] anyObject];
	 [delegate settouch:touch touchevent:3 touchview:self];
	 }
	 else{*/
		NSEnumerator *touchenm = [touches objectEnumerator];
		while ( (touch = [touchenm nextObject]) )
		{
			CGPoint location = [touch locationInView: self];
			dev.set_touch_event(ended, location.x, location.y);
		}
	}
}


// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	// If appropriate, add code necessary to save the state of the application.
	// This application is not saving state.
}

	
//-----------------------------------------------------------------//
/*!
	@brief	加速度センサー応答
 */
//-----------------------------------------------------------------//
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
	gl::IGLcore* Igl = gl::get_glcore();
	if(Igl) {
		gldev& dev = Igl->at_device();
		
		m_accel[0] = acceleration.x * kFilteringFactor + m_accel[0] * (1.0f - kFilteringFactor);
		m_accel[1] = acceleration.y * kFilteringFactor + m_accel[1] * (1.0f - kFilteringFactor);
		m_accel[2] = acceleration.z * kFilteringFactor + m_accel[2] * (1.0f - kFilteringFactor);

		dev.set_accel_event(m_accel[0], m_accel[1], m_accel[2]);
	}
}

@end
