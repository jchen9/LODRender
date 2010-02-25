//
//  CRServiceResolutionDelegate.m
//  Cocoa Corelyzer
//
//  Created by Julian on 9/18/09.
//  Copyright 2009 Electronic Visualization Laboratory, University of Illinois at Chicago. All rights reserved.
//

#import "CRServiceResolutionDelegate.h"


@implementation CRServiceResolutionDelegate

- (id)init
{
    self = [super init];
    services = [[NSMutableArray alloc] init];
    return self;
}

- (void)dealloc
{
    [services release];
    [super dealloc];
}

	// Sent when addresses are resolved
- (void)netServiceDidResolveAddress:(NSNetService *)netService
{
	NSLog(@"Enter Did resolve address");
	
		// Make sure [netService addresses] contains the
		// necessary connection information
    if ([self addressesComplete:[netService addresses]
				 forServiceType:[netService type]]) {
        [services addObject:netService];
		
			// FIXME
		/*
		 NSEnumerator *iter = [[netService addresses] objectEnumerator];
		 id addr;
		 while(addr = [iter nextObject])
		 {
		 NSLog([NSString stringWithFormat:@"Address: %@", addr]);
		 }
		 */
    }
}


- (void) handleError:(id) errorDict
{
		// todo
}

	// Sent if resolution fails
- (void)netService:(NSNetService *)netService
	 didNotResolve:(NSDictionary *)errorDict
{
	NSLog(@"Enter Did not resolve address");
	
    [self handleError:[errorDict objectForKey:NSNetServicesErrorCode]];
    [services removeObject:netService];
}

	// Verifies [netService addresses]
- (BOOL)addressesComplete:(NSArray *)addresses
		   forServiceType:(NSString *)serviceType
{
	NSLog(@"Enter address Complete");
	
		// Perform appropriate logic to ensure that [netService addresses]
		// contains the appropriate information to connect to the service
    return YES;
}

	// Error handling code
- (void)handleError:(NSNumber *)error withService:(NSNetService *)service
{
    NSLog(@"An error occurred with service %@.%@.%@, error code = %@",
		  [service name], [service type], [service domain], error);
		// Handle error here
}

@end
