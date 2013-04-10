// -*-mode:ObjC-*-
//  RotersPuzzleViewController.h
//  RotersPuzzle
//
//  Created by IKEDA KOHEI on 11/04/15.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include <AudioToolbox/AudioToolbox.h>

typedef struct {
  int x;
  int y;
} ivec2;

typedef struct TextureDescription_ {
  //TextureFormat Format;
  int BitsPerComponent;
  ivec2 Size;
  int MipCount;
  ivec2 OriginalSize;
} TextureDescription;

@interface RotersPuzzleViewController : UIViewController <UIAccelerometerDelegate>
{
  EAGLContext *context;
  GLuint program;
    
  BOOL animating;
  NSInteger animationFrameInterval;
  CADisplayLink *displayLink;
  NSData* m_imageData;
  NSMutableArray*		touchArray_;
  SystemSoundID* soundIdList_;
  int soundIdSize_;
  NSMutableArray *music_array_;

}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (void)audio_hook;
- (void)initSound:(int)count pathlist:(const char**)list;
- (void)startAccelMonitoring;
- (void)stopAccelMonitoring;
//- (void)accelermeter:(UIAccelerometer*)accelerometerdidAccelerate:(UIAcceleration*)acceleration;

@end
