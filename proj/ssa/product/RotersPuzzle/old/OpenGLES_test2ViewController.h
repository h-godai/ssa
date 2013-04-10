//
//  OpenGLES_test2ViewController.h
//  OpenGLES-test2
//
//  Created by IKEDA KOHEI on 11/04/01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

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

@interface OpenGLES_test2ViewController : UIViewController
{
    EAGLContext *context;
    GLuint program;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
  NSData* m_imageData;
  NSMutableArray*		touchArray_;

}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (TextureDescription)LoadImagePot:(NSString*)file;


@end
