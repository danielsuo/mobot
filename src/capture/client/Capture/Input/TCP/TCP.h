//
//  TCP.h
//  Capture
//
//  Created by Daniel Suo on 7/27/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#import "Utilities.h"

@protocol TCPDelegate <NSObject>

@required
- (void)handleNewConnectionFromAddress:(NSData *)addr
                           inputStream:(NSInputStream *)istream
                          outputStream:(NSOutputStream *)ostream;

@end

@interface TCP : NSObject <NSStreamDelegate>

@property (nonatomic, strong) id <TCPDelegate> tcpDelegate;
- (BOOL)start:(NSError **)error;
- (BOOL)stop;

@end
