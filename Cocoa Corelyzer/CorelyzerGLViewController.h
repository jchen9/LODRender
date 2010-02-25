//
//  CorelyzerGLViewController.h
//  Cocoa Corelyzer
//
//  Created by Julian on 9/3/09.
//  Copyright 2009 Julian Yu-Chung Chen. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CorelyzerGLView.h"
#import "LODRender/scenegraph_c.h"

@interface CorelyzerGLViewController : NSObject {
	IBOutlet CorelyzerGLView *view;
	IBOutlet NSComboBox   *comboBox;

		// one and only track & section Ids
	int sceneTrackId;
	int sceneSectionId;
}

- (IBAction)loadImageTest:(id)sender;
- (IBAction)removeImageTest:(id)sender;
- (IBAction)turnGridOn:(id)sender;
- (IBAction)turnCrosshairOn:(id)sender;
- (IBAction)doSAGECheck:(id)sender;

- (int *) loadASection:(NSString *) filepath intoTrack:(NSString *) trackName;
- (void)  removeASection;

- (void)setComboText:(NSString *)aString;

@end
