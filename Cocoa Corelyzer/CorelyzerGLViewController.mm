//
//  CorelyzerGLViewController.m
//  Cocoa Corelyzer
//
//  Created by Julian on 9/3/09.
//  Copyright 2009 Julian Yu-Chung Chen. All rights reserved.
//

#import "CorelyzerGLViewController.h"
#import "CRServiceSearchDelegate.h"

@implementation CorelyzerGLViewController

- (void) awakeFromNib
{
		// Bonjour
	CRServiceSearchDelegate *delegateObject = [[CRServiceSearchDelegate alloc] init];
	[delegateObject setController:self];
	NSNetServiceBrowser *serviceBrowser;
	
	serviceBrowser = [[NSNetServiceBrowser alloc] init];
	[serviceBrowser setDelegate:delegateObject];
	[serviceBrowser searchForServicesOfType:@"_sageServer._tcp." inDomain:@"local"];			
}

#pragma mark IBActions
static bool hasCanvasGrid = false;

- (IBAction)turnGridOn:(id)sender
{
	hasCanvasGrid = !hasCanvasGrid;
	enableCanvasGrid(hasCanvasGrid);
	
	if(hasCanvasGrid) {
		setCanvasGridColor(0.3, 0.3, 0.3);
	}
	
	[view setNeedsDisplay:YES];
}

- (IBAction)turnCrosshairOn:(id)sender
{
	bool b = hasCrossHair();
	setCrossHair(!b);
	
	[view setNeedsDisplay:YES];
}

- (IBAction)loadImageTest:(id)sender
{
		//	NSButton * button = (NSButton *) sender;
		//	[button setEnabled:NO];	

		// Simple single section track
	int * trackSectIds = [self loadASection:@"/Users/julian/Downloads/phot-32a-09-fullres.tif" 
								  intoTrack:@"Simple Track"];
	
	NSLog(@"- IDs: %d, %d", trackSectIds[0], trackSectIds[1]);
	
	sceneTrackId = trackSectIds[0];
	sceneSectionId = trackSectIds[1];
	
	free(trackSectIds);
	
	[view setNeedsDisplay:YES];
}

- (IBAction)removeImageTest:(id)sender
{
		// TODO:
	NSLog(@"- [TODO] Remove section (trackId: %d, sectionId: %d)", sceneTrackId, sceneSectionId);
	deleteTrack(sceneTrackId);
	
	[view setNeedsDisplay:YES];
}

- (IBAction)doSAGECheck:(id)sender
{		
	if([sender state] == NSOnState) {
		view.doSAGEStream = YES;
		[comboBox setEnabled:NO];
		
			// TODO: resolve selected service's (fsManager) IP and port
				
	} else {
		view.doSAGEStream = NO;
		[comboBox setEnabled:YES];
	}	
}

#pragma mark Other methods
- (int *) loadASection:(NSString *) filepath intoTrack:(NSString *) trackName
{
	int * trackSectionIds = (int *) malloc(sizeof(int) * 2);
	trackSectionIds[0] = -1;
	trackSectionIds[1] = -1;
	
		// Create a track
	int trackId = addTrack([trackName UTF8String]);
	
		// Load a section into the track
	int sectionId = -1;
	
	int genblockStatus = genTextureBlocks([filepath UTF8String]);
	if(genblockStatus == -1) {
		NSLog(@"- [INFO] Generate texture failed.");
	} else {
		int imageId = loadImage([filepath UTF8String]);
		
		int defaultSectionId = 0;
		sectionId = addSectionToTrack(trackId, defaultSectionId);
		
		if(sectionId != -1) {
			setSectionName(trackId, sectionId, "My first section");
			setImageURL(imageId, "unknown");
			
			if(imageId != -1) {
				addSectionImageToTrack(trackId, sectionId, imageId);

					// IODP section are vertical by default
					// setSectionOrientation(trackId, sectionId, true);
			}
		}
	}
	
	trackSectionIds[0] = trackId;
	trackSectionIds[1] = sectionId;
	
	return trackSectionIds;
}

- (void)  removeASection
{
		// TODO
}

- (void)setComboText:(NSString *)aString
{
		// [comboBox removeAllItems]; // clear first
	[comboBox addItemWithObjectValue:aString];
	[comboBox selectItemAtIndex:0];
}

@end
