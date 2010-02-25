//
//  CorelyzerGLView.h
//  Cocoa Corelyzer
//
//  Created by Julian on 9/2/09.
//  Copyright 2009 Julian Yu-Chung Chen. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#import "LODRender/scenegraph_c.h"
#import "SAGE/sail.h"

@interface CorelyzerGLView : NSOpenGLView {
	int canvasId;

	float *scenePos;
	NSPoint  prePos;
	NSSize	 size;
	
	BOOL doSAGEStream;	
}

@property BOOL doSAGEStream;

- (void) mouseDown:(NSEvent *)theEvent;
- (void) mouseUp:(NSEvent *)theEvent;
- (void) mouseDragged:(NSEvent *)theEvent;
- (void) scrollWheel:(NSEvent *)theEvent;

- (void) drawRect:(NSRect)rect;

	// - (void) rightMouseDown:(NSEvent *)theEvent;
	// - (void) otherMouseDown:(NSEvent *)theEvent;
	// - (void) rightMouseUp:(NSEvent *)theEvent;
	// - (void) otherMouseUp:(NSEvent *)theEvent;
	// - (void) rightMouseDragged:(NSEvent *)theEvent;
	// - (void) otherMouseDragged:(NSEvent *)theEvent;

@end
