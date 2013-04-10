// -*-tab-width:4;mode:c++-*-
//  OpenGLES_test2ViewController.m
//  OpenGLES-test2
//
//  Created by IKEDA KOHEI on 11/04/01.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "OpenGLES_test2ViewController.h"
#import "EAGLView.h"

#include "BoostTest.h"
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

@interface OpenGLES_test2ViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

@implementation OpenGLES_test2ViewController

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
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
  [(EAGLView *)self.view setFramebuffer];
  app::appInstance().begin_frame_callback();
  [(EAGLView *)self.view presentFramebuffer];
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


- (TextureDescription)LoadImagePot:(NSString*)file
{
  NSString* basePath = file; // [NSString stringWithUTF8String:file.c_str()];
  NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
  NSString* fullPath = 
  [resourcePath stringByAppendingPathComponent:basePath];
  UIImage* uiImage = [UIImage imageWithContentsOfFile:fullPath];
  
  TextureDescription description;
  description.OriginalSize.x = CGImageGetWidth(uiImage.CGImage);
  description.OriginalSize.y = CGImageGetHeight(uiImage.CGImage);
  description.Size.x = 512; //NextPot(description.OriginalSize.x);
  description.Size.y = 512; // NextPot(description.OriginalSize.y);
  description.BitsPerComponent = 8;
  //description.Format = TextureFormatRgba;
  
  int bpp = description.BitsPerComponent / 2;
  int byteCount = description.Size.x * description.Size.y * bpp;
  unsigned char* data = (unsigned char*) calloc(byteCount, 1);
  
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGBitmapInfo bitmapInfo = 
  kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
  CGContextRef context2 = CGBitmapContextCreate(data,
											   description.Size.x,
											   description.Size.y,
											   description.BitsPerComponent,
											   bpp * description.Size.x,
											   colorSpace,
											   bitmapInfo);
  CGColorSpaceRelease(colorSpace);
  CGRect rect = CGRectMake(0, 0, description.Size.x, description.Size.y);
  CGContextDrawImage(context2, rect, uiImage.CGImage);
  CGContextRelease(context2);
  
  m_imageData = [NSData dataWithBytesNoCopy:data length:byteCount freeWhenDone:YES];
  return description;
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
  
  
  
}  

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
  using namespace ts::appfw::in;
  //UITouch *touch = [[event allTouches] anyObject];
  for(UITouch *touch in [touches allObjects]) {
	NSUInteger idx = [touchArray_ indexOfObject:touch];
	CGPoint pos = [touch locationInView:self.view];
	app::appInstance().notify_event(tap_ctrl(tap_ctrl::move, pos.x, pos.y), idx);
  }
}  

  
#if 0  
- (void)drawFrame
{
  
  [(EAGLView *)self.view setFramebuffer];
    
 
  // Replace the implementation of this method to do your own custom drawing.
  static const GLfloat squareVertices[] = {
        -0.5f, -0.33f,
        0.5f, -0.33f,
        -0.5f,  0.33f,
        0.5f,  0.33f,
  };
  static const GLfloat squareTexCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
  };
	
    
  static const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
  };
    
  static float transY = 0.0f;

  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  static bool first = TRUE;
  static GLuint textureid = -1;
  if (first) {
	first = FALSE;
	[self LoadImagePot:@"godai.png"];
	  
	  glGenTextures(1, &textureid); 
	  glBindTexture(GL_TEXTURE_2D, textureid); 
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, [m_imageData bytes]); 
	  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, textureid, 0); 
	}
	else {
	  glBindTexture(GL_TEXTURE_2D, textureid); 
	}
    
    
    if ([context API] == kEAGLRenderingAPIOpenGLES2)
    {
        // Use shader program.
        glUseProgram(program);
        
        // Update uniform value.
        glUniform1f(uniforms[UNIFORM_TRANSLATE], (GLfloat)transY);
        transY += 0.075f;	
        
        // Update attribute values.
        glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);
        glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, 0, squareColors);
        glEnableVertexAttribArray(ATTRIB_COLOR);
        
        // Validate program before drawing. This is a good check, but only really necessary in a debug build.
        // DEBUG macro must be defined in your debug configurations if that's not already the case.
#if defined(DEBUG)
        if (![self validateProgram:program])
        {
            NSLog(@"Failed to validate program: %d", program);
            return;
        }
#endif
    }
    else
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, (GLfloat)(sinf(transY)/2.0f), 0.0f);
        transY += 0.075f;
		glEnable(GL_TEXTURE_2D);
        
        glVertexPointer(2, GL_FLOAT, 0, squareVertices);
        glEnableClientState(GL_VERTEX_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	  
        //glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
 	    //glEnableClientState(GL_COLOR_ARRAY);
    }
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	[(EAGLView *)self.view presentFramebuffer];
}
#endif


  
@end
