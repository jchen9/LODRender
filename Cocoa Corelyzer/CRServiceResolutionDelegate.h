//
//  CRServiceResolutionDelegate.h
//  Cocoa Corelyzer
//
//  Created by Julian on 9/18/09.
//  Copyright 2009 Electronic Visualization Laboratory, University of Illinois at Chicago. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface CRServiceResolutionDelegate : NSObject <NSNetServiceDelegate>{
    NSMutableArray *services;
}

	// NSNetService delegate methods for publication
- (void)netServiceDidResolveAddress:(NSNetService *)netService;
- (void)netService:(NSNetService *)netService didNotResolve:(NSDictionary *)errorDict;

	// Other methods
- (BOOL)addressesComplete:(NSArray *)addresses forServiceType:(NSString *)serviceType;
- (void)handleError:(NSNumber *)error withService:(NSNetService *)service;

@end
