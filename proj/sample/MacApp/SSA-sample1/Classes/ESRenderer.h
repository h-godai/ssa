//
//  ESRenderer.h
//  iFonts
//
//  Created by 平松 邦仁 on 09/11/10.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void) render_begin;
- (void) render_end;
- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer;

@end
