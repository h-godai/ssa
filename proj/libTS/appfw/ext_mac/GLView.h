// -*-mode:ObjC;tab-width:4-*-
//  GLView.h
//  RiseOut
//
//  Created by IKEDA KOHEI on 11/05/02.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define USE_DEPTH_BUFFER 1
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>


#include "libTS/appfw/ext_mac/ios_control.hpp"

using namespace ts::appfw::gl;
using namespace ts::appfw::mac;

@interface GLView : UIView <UIAccelerometerDelegate> {
    EAGLContext*    context_;
    render_control* render_control_;
    float           timestamp_;
    
    NSMutableArray* touchArray_;
    SystemSoundID*  soundIdList_;
    int             soundIdSize_;
    NSMutableArray* music_array_;
    
    AVAudioPlayer*  currentMusic_;
    bool animating_;
    CADisplayLink*  displayLink_;
    UITouch*  touchnil_;
}

- (void) drawView: (CADisplayLink*) displayLink;
- (void) didRotate: (NSNotification*) notification;
- (void) audio_hook;
- (void) system_command_hook;
- (void) startAccelMonitoring;
- (void) stopAccelMonitoring;
- (void) startAnimation;
- (void) stopAnimation;

@end
