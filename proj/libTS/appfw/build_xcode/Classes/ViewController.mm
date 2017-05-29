// -*-tab-width:4;mode:c++-*-
//  ViewController.m
//  OpenGLES-test2
//
//  Created by IKEDA KOHEI on 11/04/01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#include <AVFoundation/AVFoundation.h>

#import "ViewController.h"
#import "EAGLView.h"

#include "AppMain.h"

// Uniform index.
enum {
    UNIFORM_TRANSLATE,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};


@interface ViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation ViewController

@synthesize animating, context, displayLink;


- (void)viewDidLoad {
  [super viewDidLoad];


  touchArray_ = [[[NSMutableArray array] init]retain];
}

- (void)awakeFromNib
{
  
  EAGLContext *aContext = 0; // using ES1 [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
  if (!aContext) {
	aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
  }
  
  if (!aContext)
	NSLog(@"Failed to create ES context");
  else if (![EAGLContext setCurrentContext:aContext])
	NSLog(@"Failed to set ES context current");
  
  self.context = aContext;
  [aContext release];
  
  [(EAGLView *)self.view setContext:context];
  [(EAGLView *)self.view setFramebuffer];
  
  if ([context API] == kEAGLRenderingAPIOpenGLES2)
	[self loadShaders];
  
  animating = FALSE;
  animationFrameInterval = 1;
  self.displayLink = nil;

  // Documentディレクトリを取得
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); 
  NSString *dirPath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil; 

  // app初期化処理
  ts::appfw::app_info::ptr info(new ts::appfw::app_info);
  info->app_dir_ = [[[NSBundle mainBundle] bundlePath] UTF8String];
  info->res_dir_ = [[[NSBundle mainBundle] resourcePath] UTF8String];
  info->doc_dir_ = [dirPath UTF8String];
  app::appInstance().initialize(info);

  app::appInstance().gr_.set_screen_size(((EAGLView *)self.view).framebufferWidth,
										 ((EAGLView *)self.view).framebufferHeight);


  // soundの登録
  std::vector<std::string> soundfn;
  app::appInstance().get_sound_list(soundfn);
  if (!soundfn.empty()) {
	soundIdList_ = new SystemSoundID[soundfn.size()];
  
	for (int i = 0; i < soundfn.size(); ++i) {
	  NSString* path = [NSString stringWithUTF8String:soundfn[i].c_str()];
	  NSURL* url = [NSURL fileURLWithPath:path];
	  SystemSoundID soundID;
	  OSStatus status;
	  status = AudioServicesCreateSystemSoundID((CFURLRef)url, &soundID);
	  soundIdList_[i] = soundID;
	}
  }
  

  // BGMの登録
  music_array_ = [[NSMutableArray alloc] init];
  std::vector<std::string> musicfn;
  app::appInstance().get_music_list(musicfn);
  if (!musicfn.empty()) {
	for (int i = 0; i < musicfn.size(); ++i) {
      AVAudioPlayer* mBgm;
      NSString* path = [NSString stringWithUTF8String:musicfn[i].c_str()];
      NSURL* url = [NSURL fileURLWithPath:path];
      mBgm = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
      mBgm.numberOfLoops = -1;
      [music_array_ addObject:(id)mBgm];
    }
  }
  
}

// サウンド再生のコールバック
- (void)audio_hook
{
  std::vector<ts::appfw::sounds::audio_param> params;
  app::appInstance().audio_control_hook(params);
  BOOST_FOREACH(ts::appfw::sounds::audio_param& param, params) {
    switch (param.type_) {
    case ts::appfw::sounds::sound_type:
      AudioServicesPlaySystemSound(soundIdList_[param.id_]);
      break;
    case ts::appfw::sounds::music_type:
      {
        AVAudioPlayer* mBgm = (AVAudioPlayer*)[music_array_ objectAtIndex:(NSUInteger)param.id_]; 
		if (param.cmd_ == ts::appfw::sounds::cmd_play) {
		  [mBgm play];
		}
		else {
		  [mBgm stop];
		}
		// mBgm.numberOfLoops = -1;// 無限ループの指定
		// mBgm.numberOfLoops = 0;// １回ループの指定
		// mBgm.numberOfLoops = 1;// ２回ループの指定
      }
	  break;
    }
  }
}


- (void)dealloc
{
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
  
    for (int i = 0; i < soundIdSize_; ++i) {
      AudioServicesDisposeSystemSoundID (soundIdList_[i]);
    }
    delete [] soundIdList_;
    soundIdSize_ = 0;
  
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
      [self startAccelMonitoring];
      animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        [self stopAccelMonitoring];
        animating = FALSE;
    }
}

- (void)drawFrame
{
  [(EAGLView *)self.view setFramebuffer];
  app::appInstance().draw_frame_hook();
  [(EAGLView *)self.view presentFramebuffer];
  [self audio_hook];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Attach vertex shader to program.
    glAttachShader(program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(program, ATTRIB_COLOR, "color");
    
    // Link program.
    if (![self linkProgram:program])
    {
        NSLog(@"Failed to link program: %d", program);
        
        if (vertShader)
        {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }
        
        return FALSE;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_TRANSLATE] = glGetUniformLocation(program, "translate");
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}




- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
  using namespace ts::appfw::in;
//  UITouch *touch = [[event allTouches] anyObject];
  for(UITouch *touch in [touches allObjects]) {
	[touchArray_ addObject:touch];
	CGPoint pos = [touch locationInView:self.view];
	NSUInteger idx = [touchArray_ indexOfObject:touch];
	app::appInstance().notify_event(tap_ctrl(tap_ctrl::touch, pos.x, pos.y), idx);
  }
  [self audio_hook];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
  using namespace ts::appfw::in;
  for(UITouch *touch in [touches allObjects]) {
	NSUInteger idx = [touchArray_ indexOfObject:touch];
	if(idx != NSNotFound) {
	  CGPoint pos = [touch locationInView:self.view];
	  [touchArray_ removeObjectAtIndex:idx];
	  app::appInstance().notify_event(tap_ctrl(tap_ctrl::release, pos.x, pos.y), idx);
	}
  }
  [self audio_hook];
}  


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
  using namespace ts::appfw::in;
  //UITouch *touch = [[event allTouches] anyObject];
  for(UITouch *touch in [touches allObjects]) {
	NSUInteger idx = [touchArray_ indexOfObject:touch];
	CGPoint pos = [touch locationInView:self.view];
	app::appInstance().notify_event(tap_ctrl(tap_ctrl::move, pos.x, pos.y), idx);
  }
  [self audio_hook];
}  

- (void)initSound:(int)count pathlist:(const char**)list {
  soundIdList_ = new SystemSoundID[count];
  
  for (int i = 0; i < count; ++i) {
    NSString* path = [NSString stringWithUTF8String:list[i]];
    NSURL* url;
    //path = [[NSBundle mainBundle] pathForResource:@"b210" ofType: @"wav"];
    url = [NSURL fileURLWithPath:path];
  
    SystemSoundID soundID;
    OSStatus status;
    status = AudioServicesCreateSystemSoundID((CFURLRef)url, &soundID);
    soundIdList_[i] = soundID;
  }
}

  
- (void)playSound:(int)id {
  
 
  
}

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
  app::appInstance().notify_event(acc_ctrl(acceleration.x, acceleration.y, acceleration.z));
}


  
@end
