//
//  CRServiceSearchDelegate.m
//  Cocoa Corelyzer
//
//  Created by Julian on 9/18/09.
//  Copyright 2009 Electronic Visualization Laboratory, University of Illinois at Chicago. All rights reserved.
//

#import "CRServiceSearchDelegate.h"
#import "CRServiceResolutionDelegate.h"

@implementation CRServiceSearchDelegate

- (id)init
{
    self = [super init];
    services = [[NSMutableArray alloc] init];
    searching = NO;
    return self;
}

- (void)dealloc
{
	[uiController release];
    [services release];
    [super dealloc];
}

	// Sent when browsing begins
- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)browser
{
    searching = YES;
    [self updateUI];
}

	// Sent when browsing stops
- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)browser
{
    searching = NO;
    [self updateUI];
}

	// Sent if browsing fails
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
			 didNotSearch:(NSDictionary *)errorDict
{
    searching = NO;
    [self handleError:[errorDict objectForKey:NSNetServicesErrorCode]];
}

	// Sent when a service appears
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		   didFindService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing
{
    [services addObject:aNetService];
    if(!moreComing)
    {
        [self updateUI];
    }
}

	// Sent when a service disappears
- (void)netServiceBrowser:(NSNetServiceBrowser *)browser
		 didRemoveService:(NSNetService *)aNetService
			   moreComing:(BOOL)moreComing
{
    [services removeObject:aNetService];
	
    if(!moreComing)
    {
        [self updateUI];
    }
}

	// Error handling code
- (void)handleError:(NSNumber *)error
{
    NSLog(@"An error occurred. Error code = %d", [error intValue]);
		// Handle error here
}

	// UI update code
- (void)updateUI
{
    if(searching) {
			// Update the user interface to indicate searching
			// Also update any UI that lists available services
		NSEnumerator *iter = [services objectEnumerator];
		NSNetService *service;
		
		while(service = [iter nextObject]) 
		{		    
				// service type
			NSLog(@"- Found service: %@", [service name]);
			
				// FIXME: my uiController doesn't have the protocol yet...
			[uiController setComboText:[service name]];			
						
				// need to resolve?
			CRServiceResolutionDelegate *delegateObject = [[CRServiceResolutionDelegate alloc] init];
			[service setDelegate:delegateObject];
			[delegateObject release];
			
				// fixme
			int numAdrs = [[service addresses] count];
			NSLog(@"Number of addresses: %d", numAdrs);			
		}		
    } else {
			// Update the user interface to indicate not searching
		NSLog(@"- Not searching");
    }
}

- (void)setController:(CorelyzerGLViewController *)controller
{
	uiController = [controller retain];
}

@end
