//
//  ES2Renderer.h
//	LGR.MMX
//
//  Created by 平松 邦仁 on 09/11/10.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface ES2Renderer : NSObject <ESRenderer>
{
@private
	EAGLContext *context;
	
	// The pixel dimensions of the CAEAGLLayer
	GLint backingWidth;
	GLint backingHeight;
	
	// The OpenGL names for the framebuffer and renderbuffer used to render to this view
	GLuint defaultFramebuffer, colorRenderbuffer;
	
	GLuint program;
}

- (void) render_begin;
- (void) render_end;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

@end

