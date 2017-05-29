// -*-mode:ObjC;tab-width:4-*-
//
//  GLView.mm
//  RiseOut
//
//  Created by IKEDA KOHEI on 11/05/02.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define USE_DEPTH_BUFFER 1
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "GLView.h"
#import "mach/mach_time.h"
#import <OpenGLES/ES2/gl.h>
#include "libTS/appfw/drawframe_app.hpp"

using namespace ts;
using namespace ts::appfw;

@implementation GLView

+ (Class) layerClass {
    return [CAEAGLLayer class];
}


- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        animating_ = FALSE;
        
        // iPhone4以降の高解像度ディスプレイに対応
        self.contentScaleFactor = [UIScreen mainScreen].scale;
        
        
        // Initialize Application Framework 
        
        // Documentディレクトリを取得                                                                      
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *dirPath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
        
        ts::appfw::app_info::ptr info(new ts::appfw::app_info);
        info->app_dir_ = [[[NSBundle mainBundle] bundlePath] UTF8String];
        info->res_dir_ = [[[NSBundle mainBundle] resourcePath] UTF8String];
        info->doc_dir_ = [dirPath UTF8String];
        NSLog(@"DocumentDir: %@\n", dirPath);
        app_framework::app_instance().initialize(info);
        touchArray_ = [[[NSMutableArray array] init]retain];
        touchnil_ = [[UITouch alloc] init];
        
        // soundの登録                                                                                     
        std::vector<std::string> soundfn;
        app_framework::app_instance().get_sound_list(soundfn);
        if (!soundfn.empty()) {
            soundIdList_ = new SystemSoundID[soundfn.size()];
            
            for (int i = 0; i < soundfn.size(); ++i) {
                NSString* path = [NSString stringWithUTF8String:soundfn[i].c_str()];
                NSURL* url = [NSURL fileURLWithPath:path];
                SystemSoundID soundID;
                OSStatus status;
                status = AudioServicesCreateSystemSoundID((CFURLRef)url, &soundID);
                soundIdList_[i] = soundID;
                //AudioServicesPlaySystemSound(soundID);
                //printf("#%d %s\n", soundID, [path UTF8String]);
            }
        }
        
        
        // BGMの登録                                                                                       
        music_array_ = [[NSMutableArray alloc] init];
        std::vector<std::string> musicfn;
        app_framework::app_instance().get_music_list(musicfn);
        if (!musicfn.empty()) {
            for (int i = 0; i < musicfn.size(); ++i) {
                AVAudioPlayer* mBgm;
                NSString* path = [NSString stringWithUTF8String:musicfn[i].c_str()];
                NSURL* url = [NSURL fileURLWithPath:path];
                mBgm = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
                if (mBgm == nil) {
                    NSLog(@"bgm load error %@ as %@", path, [url absoluteString]);
                    [music_array_ addObject: (id)@"error"];
                }
                else {
                    mBgm.numberOfLoops = -1;
                    [music_array_ addObject:(id)mBgm];
                }
            }
        }
        
        
        
        
        // Graphics Initialization code.
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
        eaglLayer.opaque = YES;
        context_ = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
        if (!context_ || ![EAGLContext setCurrentContext: context_]) {
            [self release];
            return nil;
        }
        // initialize fremework context
        render_control_ = render_control_factory::create();
        
        [context_ renderbufferStorage: GL_RENDERBUFFER_OES
                         fromDrawable: eaglLayer];
        
        render_control_->initialize(CGRectGetWidth(frame)
                                    , CGRectGetHeight(frame) 
                                    , self.contentScaleFactor);
        
        //[self drawView: nil];
        timestamp_ = CACurrentMediaTime();
#if 0
        CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget: self
                                                                 selector: @selector(drawView:)];
        [displayLink addToRunLoop: [NSRunLoop currentRunLoop]
                          forMode: NSDefaultRunLoopMode];
        
#endif
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        
        [[NSNotificationCenter defaultCenter]
         addObserver: self
         selector: @selector(didRotate:)
         name: UIDeviceOrientationDidChangeNotification
         object:nil];
        
        // 加速度センサの起動
        [self startAccelMonitoring];
        
        // マルチタッチの許可
        [self setMultipleTouchEnabled:YES];
        
        
        
    }
    return self;
}

- (void) didRotate:(NSNotification *)notification {
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    //UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    render_control_->on_rotate((render_control::device_orientation)orientation);
    //[self drawView: nil];
}

- (void) drawView: (CADisplayLink*) displayLink {
    
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - timestamp_;
        timestamp_ = displayLink.timestamp;
        render_control_->update_animation(elapsedSeconds);
        
        NSDate *startTime = [NSDate date];
        render_control_->render(timestamp_);
        [context_ presentRenderbuffer:GL_RENDERBUFFER];
        NSTimeInterval elapsedTime = [startTime timeIntervalSinceNow];
        float fps = 1.0f / -elapsedTime;
        if (fps > 100.0f) fps = 60.0f; // 時々おかしな値になる
        app_framework::app_instance().set_fps(fps, -elapsedTime);
    }
    [self audio_hook];
    [self system_command_hook];
}


/*
 // Only override drawRect: if you perform custom drawing.
 // An empty implementation adversely affects performance during animation.
 - (void)drawRect:(CGRect)rect {
 // Drawing code.
 }
 */

- (void)dealloc {
    if ([EAGLContext currentContext] == context_) {
        [EAGLContext setCurrentContext: nil];
    }
    [context_ release];
    
    // サウンドバッファの解放
    for (int i = 0; i < soundIdSize_; ++i) {
        AudioServicesDisposeSystemSoundID (soundIdList_[i]);
    }
    delete [] soundIdList_;
    soundIdSize_ = 0;
    
    [super dealloc];
}


// タッチ入力のハンドリング

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    using namespace ts::appfw::in;
    for (UITouch *touch in [touches allObjects]) {
        NSUInteger idx = [touchArray_ indexOfObject:touch];
        if(idx == NSNotFound) {
            idx = [touchArray_ indexOfObject:touchnil_];
            if(idx != NSNotFound) {
                [touchArray_ replaceObjectAtIndex:idx withObject:touch];
            }
            else {
                [touchArray_ addObject:touch];
                idx = [touchArray_ indexOfObject:touch];
            }
        }
        CGPoint pos = [touch locationInView:self];
        ts::math::pos2_f p(pos.x, pos.y);
        app_framework::app_instance().tap_device_position_correct(p);
        app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::touch, p), idx);
    }
    //[self audio_hook];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    using namespace ts::appfw::in;
    for(UITouch *touch in [touches allObjects]) {
        NSUInteger idx = [touchArray_ indexOfObject:touch];
        if(idx != NSNotFound) {
            CGPoint pos = [touch locationInView:self];
            ts::math::pos2_f p(pos.x, pos.y);
            app_framework::app_instance().tap_device_position_correct(p);
            app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::release, p), idx);
        }
    }
    
    for(UITouch *touch in [touches allObjects]) {
        NSUInteger idx = [touchArray_ indexOfObject:touch];
        if(idx != NSNotFound) {
            [touchArray_ replaceObjectAtIndex:idx withObject:touchnil_];
        }
    }
    //[self audio_hook];
}  


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    using namespace ts::appfw::in;
    //UITouch *touch = [[event allTouches] anyObject];
    for(UITouch *touch in [touches allObjects]) {
        NSUInteger idx = [touchArray_ indexOfObject:touch];
        CGPoint pos = [touch locationInView:self];
        ts::math::pos2_f p(pos.x, pos.y);
        app_framework::app_instance().tap_device_position_correct(p);
        app_framework::app_instance().notify_event(tap_ctrl(tap_ctrl::move, p), idx);
    }
    //[self audio_hook];
}  

// システムコマンド実行のコールバック
- (void)system_command_hook
{
    std::vector<ts::appfw::system_command> cmds;
    app_framework::app_instance().system_command_hook(cmds);
    BOOST_FOREACH(ts::appfw::system_command& cmd, cmds) {
        if (cmd.cmd_ == "openurl") {
            NSString *urlString = [NSString stringWithUTF8String: cmd.param_.c_str()];
            NSURL *url = [NSURL URLWithString:urlString];
            [[UIApplication sharedApplication] openURL:url];
        }
    }
    
}

// サウンド再生のコールバック
- (void)audio_hook
{
    std::vector<ts::appfw::sounds::audio_param> params;
    app_framework::app_instance().audio_control_hook(params);
    BOOST_FOREACH(ts::appfw::sounds::audio_param& param, params) {
        switch (param.type_) {
            case ts::appfw::sounds::sound_type:
                AudioServicesPlaySystemSound(soundIdList_[param.id_]);
                break;
            case ts::appfw::sounds::music_type:
            {
                currentMusic_ = (AVAudioPlayer*)[music_array_ objectAtIndex:(NSUInteger)param.id_]; 
                if ((NSString*)currentMusic_ == @"error") {
                    currentMusic_ = nil;
                    break;
                }
                switch (param.cmd_) {
                    case ts::appfw::sounds::cmd_play:
                        currentMusic_.numberOfLoops = param.repeat_ == 0 ? -1 : param.repeat_-1; // 無限ループの指定
                        currentMusic_.currentTime = 0.0f;
                        currentMusic_.volume = 1.0f;
                        [currentMusic_ play];
                        break;
                    case ts::appfw::sounds::cmd_mute:
                        currentMusic_.volume = 0.001f;
                        [currentMusic_ play];
                        break;
                    case ts::appfw::sounds::cmd_pause:
                        [currentMusic_ pause];
                        break;
                    case ts::appfw::sounds::cmd_resume:
                        [currentMusic_ play];
                        break;
                    case ts::appfw::sounds::cmd_stop:
                    default:
                        currentMusic_.volume = 0.0f;
                        [currentMusic_ stop];
                        currentMusic_ = nil;
                }
                // mBgm.numberOfLoops = -1;// 無限ループの指定
                // mBgm.numberOfLoops = 0;// １回ループの指定
                // mBgm.numberOfLoops = 1;// ２回ループの指定
            }
                break;
        }
    }
    if (currentMusic_ != nil) {
        app_framework::app_instance().set_current_music_time(currentMusic_.currentTime);
    }
}


// 加速度センサー
- (void)startAccelMonitoring {
    UIAccelerometer* accelerometer = [UIAccelerometer sharedAccelerometer];
    accelerometer.updateInterval = 10.0f / 60.0f;
    accelerometer.delegate = self;
}
- (void)stopAccelMonitoring {
    UIAccelerometer* accelerometer = [UIAccelerometer sharedAccelerometer];
    accelerometer.updateInterval = 600.0f;
    accelerometer.delegate = nil;
}

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration{
    using namespace ts::appfw::in;
    ts::math::vec2_f v(acceleration.x, acceleration.y);
    app_framework::app_instance().vector_correct(v);
    app_framework::app_instance().notify_event(acc_ctrl(v.x_, v.y_, acceleration.z));
}


// 
- (void)startAnimation
{
    if (!animating_)
    {
        displayLink_ = [CADisplayLink displayLinkWithTarget: self selector: @selector(drawView:)];
        [displayLink_ addToRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
        
        
        //CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawView)];
        //[aDisplayLink setFrameInterval:animationFrameInterval];
        //[aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        //self.displayLink = aDisplayLink;
        
        [self startAccelMonitoring];
        animating_ = TRUE;
        render_control_->enable_animation(true);
    }
}

- (void)stopAnimation
{
    if (animating_)
    {
        [displayLink_ invalidate];
        displayLink_ = nil;
        [self stopAccelMonitoring];
        animating_ = FALSE;
        render_control_->enable_animation(false);
    }
}
#if 0

//キーダウンを受け取る
-(void) keyDown:(NSEvent *)theEvent
{
    NSString *keys = [theEvent charactersIgnoringModifiers];
    
    if (keys && [keys length] > 0) {
        unichar c = [keys characterAtIndex:0];
        if (c == NSUpArrowFunctionKey) { //上矢印キー
            //処理
            NSLog(@"up key\n");
        }
        else if (c == NSDownArrowFunctionKey) { //下矢印キー
            //処理
            NSLog(@"down key\n");
        }
        else if (c == NSLeftArrowFunctionKey) { //左矢印キー
            //処理
            NSLog(@"left key\n");
        }
        else if (c == NSRightArrowFunctionKey) { //右矢印キー
            //処理
            NSLog(@"right key\n");
        }
        else if (c == NSEnterCharacter) { //enterキー
            //処理
        }
        else if (c == NSCarriageReturnCharacter) { //returnキー
            //処理
        }
        else if (c == NSDeleteCharacter) { //deleteキー
            //処理
        }
    }
    
    NSString *chars = [theEvent characters];
    if ( chars && [chars length] > 0 ) {
        if ( [chars isEqualToString:@"0"] ) { //0キー(任意の文字キー）
            //処理
        }
    }
}
#endif
@end
