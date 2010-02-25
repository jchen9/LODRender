//
//  CorelyzerGLView.m
//  Cocoa Corelyzer
//
//  Created by Julian on 9/2/09.
//  Copyright 2009 Julian Yu-Chung Chen. All rights reserved.
//

#import "CorelyzerGLView.h"

#pragma mark Preferences
#define VIEW_WIDTH	(1024)
#define VIEW_HEIGHT	(768)
#define DPI_X		(72.0f)
#define DPI_Y		(72.0f)
#define BLOCK_DIR	"/Users/julian/Documents/Corelyzer/Caches/imgblocks/"

@implementation CorelyzerGLView

#pragma mark SAGE stuff
@synthesize doSAGEStream;
static GLubyte *rgbBuffer = NULL;
static sail sageInf;
static BOOL isSAGEInitialized = false;


- (void) initSAGEWithServerAddress:(NSString *) serverAddress
{
	NSLog(@"- ");
}

- (void) initSAGE
{
	NSLog(@"- Init SAGE");
	
	int nodeId = 0;
	
	sageRect crImageMap;
	crImageMap.left = 0.0;
	crImageMap.right = 1.0;
	crImageMap.bottom = 0.0;
	crImageMap.top = 1.0;
	
		// FIXME: can be replaced by in-place settings
	sailConfig scfg;
	scfg.init((char *) "/Users/julian/Desktop/Corelyzer.conf"); 
	
		//----
	
	scfg.setAppName((char *) "Corelyzer"); 
	scfg.rank = nodeId;
	
	scfg.resX = size.width;
	scfg.resY = size.height;
	scfg.imageMap = crImageMap;
	scfg.pixFmt = PIXFMT_8888;
	scfg.rowOrd = BOTTOM_TO_TOP;
	scfg.master = true;
	
		// scfg.appID = 1234; // FIXME	
		// scfg.nwID  = 4321; // FIXME
	
	sageInf.init(scfg);
	
		// alloc graphics buffer memory
	if (rgbBuffer) {
	    delete [] rgbBuffer;
	}
	
    rgbBuffer = (GLubyte *)sageInf.getBuffer();
	
	isSAGEInitialized = true;
	
	std::cout << "sail initialized " << std::endl;	
}

- (void) cleanSAGE
{
	NSLog(@"- Close SAGE");
	
	if (rgbBuffer) {
	    delete [] rgbBuffer;
	}
		// More to do to gracefully cleanup here
	sageInf.shutdown();
	isSAGEInitialized = false;
}

- (void) myInit
{
	canvasId = -1;
	scenePos = (float *) malloc(sizeof(float) * 2);
	
	setCanvasRowsAndColumns(1, 1);
	
		// What's current working dir?
	char * cwd = getcwd(NULL, -1);
	printf("- Cwd: %s\n", cwd);
	free(cwd);
	
	canvasId = genCanvas(0.0f, 0.0f, VIEW_WIDTH, VIEW_HEIGHT, DPI_X, DPI_Y);
	setCanvasBottomRow(canvasId, true);
	
	NSLog(@"- Canvas ID is: %d", canvasId);
	
	NSRect bounds = [self bounds];
	size = [self convertSize:bounds.size fromView:nil];
	
	startUp();
	setTexBlockDirectory(BLOCK_DIR);
	
		// Prefs
		// enableCanvasGrid(true);
		// setCrossHair(true);	
}

- (void) awakeFromNib
{
		// Initialization code here.
	[self myInit];
}

- (void) drawRect:(NSRect)rect
{	
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(0.0, 525.0, 125.0, 0.0);

	if(canvasId != -1) {
		render(canvasId);				
	}
	
		// Do an optional pixels copy for SAGE
		// TODO: should put this into a separate sending thread & maybe compress it.
		//		1024*768*4 = about 3 MB per frame/update!!!
	if(doSAGEStream == YES) {
		if(!isSAGEInitialized) {
			[self initSAGE];
		}
		
		if(rgbBuffer != NULL) {
			glReadPixels(0, 0, size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, rgbBuffer);
			sageInf.swapBuffer();
			rgbBuffer = (GLubyte *)sageInf.getBuffer();
		} else {
			NSLog(@"- Null RGB Buffer !!!");
		}
	} else {
		if(isSAGEInitialized) {
			NSLog(@"- CleanSAGE");
			[self cleanSAGE];
		} else {
				// NSLog(@"- SAGE not inited");
		}
	}
	
	glFlush();	
}

#pragma mark Event handling

- (void) convertMousePoint:(NSPoint) mp toSceneSpace: (float *) pos
{	
	float x, y, w, s;
	int canvasWidth = 1024; // TODO: get view rect width
	
	x = getCanvasPositionX(canvasId);
	y = getCanvasPositionY(canvasId);
	w = getCanvasWidth(canvasId);
	s = w / ((float) canvasWidth);
	
	pos[0] = mp.x * s + x;
	pos[1] = mp.y * s + y;						   
}

- (void) mouseDown:(NSEvent *)theEvent
{
	NSPoint currentPos = [self convertPoint:[theEvent locationInWindow] fromView:nil];	
	currentPos.y = size.height - currentPos.y; // Diff coords in Java & Cocoa

	prePos.x = currentPos.x;
	prePos.y = currentPos.y;
	
	NSLog(@"- mouse down: %.2f\t%.2f", prePos.x, prePos.y);
}

- (void) mouseUp:(NSEvent *)theEvent
{
	prePos.x = 0.0f;
	prePos.y = 0.0f;
	
	NSLog(@"- mouse up");
}


- (void) mouseDragged:(NSEvent *)theEvent
{
	NSPoint currentPos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	currentPos.y = size.height - currentPos.y; // Diff coords in Java & Cocoa
	
	int dX = currentPos.x - prePos.x;
	int dY = currentPos.y - prePos.y;
	
	float sx, sy;
	float w, h;
	
		// canvasLock.lock();
	
	w = getCanvasWidth(canvasId);
	h = getCanvasHeight(canvasId);
		
	sx = w / ((float) size.width);
	sy = h / ((float) size.height);

		// NSLog(@"- Mouse drag from %.2f, %.2f to %.2f, %.2f\t\t%d, %d", 
		//  prePos.x, prePos.y, currentPos.x, currentPos.y, dX, dY);	
	
	[self convertMousePoint:currentPos toSceneSpace:scenePos];	
			
		//	NSLog(@"- Scene pos: %.2f\t%.2f", scenePos[0], scenePos[1]);
	
	panScene(-dX * sx, -dY * sy);
	
		// canvasLock.unlock();

	prePos.x = currentPos.x;
	prePos.y = currentPos.y;
	
	[self setNeedsDisplay: YES];
}

- (void) scrollWheel:(NSEvent *)theEvent
{
		// Normal scrollwheel has only deltaY values
	float scroll = [theEvent deltaY];	
	NSLog(@"- Mouse scroll deltaY: %.2f", scroll);
		// 	scroll = scroll * 10.0f;

	NSPoint mousePos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	mousePos.y = size.height - mousePos.y; // Diff coords in Java & Cocoa

	float *cp = (float *) malloc(sizeof(float) * 2);
	cp[0] = 0.0f;
	cp[1] = 0.0f;
	
	float *sc = (float *) malloc(sizeof(float) * 2);
	sc[0] = 0.0f;
	sc[1] = 0.0f;
		
	float s = 1.0f;
		
	sc[0] = getSceneCenterX();
	sc[1] = getSceneCenterY();
	
	if(scroll <= 0) {
		s = (float) pow(1.05, fabs(scroll));
			// s = 1.05f;
	} else {
		s = (float) pow(0.95, fabs(scroll));
			// s = 0.95f;
	}

	NSLog(@"Scale is: %.2f", s);
	scaleScene(s);

	[self convertMousePoint:mousePos toSceneSpace:cp];

	float *ncp = (float *) malloc(sizeof(float) * 2);
	ncp[0] = 0.0f;
	ncp[1] = 0.0f;
	[self convertMousePoint:mousePos toSceneSpace:ncp];
	
	ncp[0] = ncp[0] - cp[0];
	ncp[1] = ncp[1] - cp[1];
	
	panScene(-ncp[0], -ncp[1]);
	
	free(cp);
	free(sc);
	free(ncp);
	
	[self setNeedsDisplay:YES];
}

	// TODO: what's the right way to dealloc the view?
- (void) dealloc
{
	NSLog(@"- close down");
	destroyCanvases();
	closeDown();	
	
	[super dealloc];
}

@end
