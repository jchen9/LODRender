//
//  CRServiceSearchDelegate.h
//  Cocoa Corelyzer
//
//  Created by Julian on 9/18/09.
//  Copyright 2009 Electronic Visualization Laboratory, University of Illinois at Chicago. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#import "CorelyzerGLViewController.h"

@interface CRServiceSearchDelegate : NSObject <NSNetServiceBrowserDelegate> {
		// Keeps track of available services
    NSMutableArray *services;
	
		// Keeps track of search status
    BOOL searching;
	
		// Controller to talk to UI
	CorelyzerGLViewController *uiController;	
}

	// NSNetServiceBrowser delegate methods for service browsing
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
			 didNotSearch:(NSDictionary *)errorDict;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		   didFindService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		 didRemoveService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing;

	// Other methods
- (void)handleError:(NSNumber *)error;
- (void)updateUI;

	// setter
- (void)setController:(CorelyzerGLViewController *)controller; 

@end
