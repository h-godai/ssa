
#import <QuartzCore/QuartzCore.h>
#import "KonahenViewController.h"
#import "EAGLView.h"
#import <tr1/memory>
#import <string>
#import "co_defines.hpp"
#import "co_mouse.hpp"
#import "nn_app.hpp"

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
	NUM_ATTRIBUTES
};

@interface KonahenViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end


static void mainFin()
{
	ngs::App::instance().close();											// FTGLでシングルトンを使っている為の措置
}


@implementation KonahenViewController

@synthesize animating, context, displayLink;

- (void)awakeFromNib
{
	EAGLContext *aContext = nil;

	
//	aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	if (!aContext)
	{
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

	if([UIView instancesRespondToSelector:@selector(contentScaleFactor)])
	{
		scale = [UIScreen mainScreen].scale;
		self.view.contentScaleFactor = scale;
		// Retina対応
	}
	else
	{
		scale = 1.0;
	}

	CGRect bounds = self.view.bounds;
	std::string path = [[[NSBundle mainBundle] resourcePath] UTF8String];
	ngs::App::instance().initialize(bounds.size.width * scale, bounds.size.height * scale, path);
	ngs::App::instance().reset();
	ngs::Mouse::instance();
	ngs::App::instance().resize(bounds.size.width * scale, bounds.size.height * scale);
	atexit(mainFin);
	array = [[NSMutableArray alloc] init];
}

- (void)dealloc
{
	if (program)
	{
		glDeleteProgram(program);
		program = 0;
	}
	[array release];
    
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


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;
}

-(void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation duration:(NSTimeInterval)duration
{
	CGRect bounds = self.view.bounds;
	ngs::App::instance().resize(bounds.size.width * scale, bounds.size.height * scale);
}

#if 0
-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
}
#endif


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	int count = [array count];
	for(UITouch *touch in [touches allObjects])
	{
		[array addObject:touch];
	}

	if([array count] == 1)
	{
		CGPoint pos = [[array objectAtIndex:0] locationInView:self.view];
		ngs::Mouse::instance().cursorCallBack(pos.x, pos.y);
		ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_LEFT, ngs::MOUSE_PUSH);
	}
	else
	if([array count] == 2)
	{
		for(int i = 0; i < 2; ++i)
		{
			CGPoint pos = [[array objectAtIndex:i] locationInView:self.view];
			ngs::Mouse::instance().cursorCallBack(i, pos.x, pos.y);
		}
#if 1
		if(count == 1) ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_LEFT, ngs::MOUSE_PULL);
		ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_MULTI_TOUCH, ngs::MOUSE_PUSH);
#else
		enum ngs::MOUSE_BUTTON btn = count ? ngs::MOUSE_MULTI_TOUCH : static_cast<enum ngs::MOUSE_BUTTON>(ngs::MOUSE_MULTI_TOUCH | ngs::MOUSE_LEFT);
		ngs::Mouse::instance().buttonCallBack(btn, ngs::MOUSE_PUSH);
#endif
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for(UITouch *touch in [touches allObjects])
	{
		NSUInteger idx = [array indexOfObject:touch];
		if(idx < 2)
		{
			CGPoint pos = [[array objectAtIndex:idx] locationInView:self.view];
			ngs::Mouse::instance().cursorCallBack(idx, pos.x, pos.y);
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	int count = [array count];
	for(UITouch *touch in [touches allObjects])
	{
		[array removeObject:touch];
	}

	if([array count] == 1)
	{
		ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_MULTI_TOUCH, ngs::MOUSE_PULL);
		ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_LEFT, ngs::MOUSE_PUSH);
	}
	else
	if([array count] == 0)
	{
		if(count) ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_MULTI_TOUCH, ngs::MOUSE_PULL);
		ngs::Mouse::instance().buttonCallBack(ngs::MOUSE_LEFT, ngs::MOUSE_PULL);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesEnded:touches withEvent:event];
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
		CADisplayLink *aDisplayLink;
		if([UIScreen instancesRespondToSelector:@selector(displayLinkWithTarget:selector:)])
		{
			aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
		}
		else
		{
			aDisplayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawFrame)];
		}

		// CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
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
	ngs::Mouse::instance().update();
	ngs::App::instance().step();
	
	[(EAGLView *)self.view setFramebuffer];
    
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	if ([context API] == kEAGLRenderingAPIOpenGLES2)
	{
#if 0
		// Use shader program.
		glUseProgram(program);

		// Update uniform value.
		glUniform1f(uniforms[UNIFORM_TRANSLATE], (GLfloat)transY);
        
		// Update attribute values.
		glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
		glEnableVertexAttribArray(ATTRIB_VERTEX);
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, 1, 0, squareColors);
		glEnableVertexAttribArray(ATTRIB_COLOR);
        
		// Validate program before drawing. This is a good check, but only really necessary in a debug build.
		// _DEBUG macro must be defined in your debug configurations if that's not already the case.
#if defined(_DEBUG)
		if (![self validateProgram:program])
		{
			NSLog(@"Failed to validate program: %d", program);
			return;
		}
#endif
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#endif
	}
	else
	{
		ngs::App::instance().draw();
	}
    
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
    
#if defined(_DEBUG)
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
    
#if defined(_DEBUG)
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

@end
