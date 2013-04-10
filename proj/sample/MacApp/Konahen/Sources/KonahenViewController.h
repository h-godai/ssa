//
//  KonahenViewController.h
//  Konahen
//
//  Created by nishi on 11/09/21.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface KonahenViewController : UIViewController
{
	EAGLContext *context;
	GLuint program;
    
	BOOL animating;
	NSInteger animationFrameInterval;
	CADisplayLink *displayLink;

	float scale;
	NSMutableArray *array;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;

@end
