//
//  TCP.mm
//  Capture
//
//  Created by Daniel Suo on 7/27/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "TCP.h"

@interface TCP ()
{
    CFSocketRef _socket;
    NSInputStream *_istream;
    NSOutputStream *_ostream;
}

- (void)handleNewConnectionFromAddress:(NSData *)addr
                           inputStream:(NSInputStream *)istream
                          outputStream:(NSOutputStream *)ostream;

@end

#warning move to Utilities
NSString * const kTCPServerErrorDomain = @"TCPServerErrorDomain";

typedef enum {
    kTCPServerCouldNotBindToIPv4Address = 1,
    kTCPServerCouldNotBindToIPv6Address = 2,
    kTCPServerNoSocketsAvailable = 3,
} TCPServerErrorCode;

static void socketCallback(CFSocketRef socket,
                           CFSocketCallBackType type,
                           CFDataRef address,
                           const void *data,
                           void *info) {
    
    TCP *server = (__bridge TCP *)info;
    
    if (kCFSocketAcceptCallBack == type) {
        NSString *addressString = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)address)->sin_addr)];
        [Utilities sendStatus:[NSString stringWithFormat:@"INFO: New connection from %@", addressString]];
        
        // for an AcceptCallBack, the data parameter is a pointer to a CFSocketNativeHandle
        CFSocketNativeHandle nativeSocketHandle = *(CFSocketNativeHandle *)data;
        uint8_t peerName[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(peerName);
        NSData *peer = nil;
        
        if (0 == getpeername(nativeSocketHandle, (struct sockaddr *)peerName, &namelen)) {
            peer = [NSData dataWithBytes:peerName length:namelen];
        }
        
        CFReadStreamRef readStream = NULL;
        CFWriteStreamRef writeStream = NULL;
        CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);
        
        if (readStream && writeStream) {
            CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            [server handleNewConnectionFromAddress:peer
                                       inputStream:(__bridge NSInputStream *)readStream
                                      outputStream:(__bridge NSOutputStream *)writeStream];
        } else {
            // on any failure, need to destroy the CFSocketNativeHandle
            // since we are not going to use it any more
            close(nativeSocketHandle);
        }
        
        if (readStream)
            CFRelease(readStream);
        
        if (writeStream)
            CFRelease(writeStream);
    }
}

@implementation TCP

- (BOOL)start:(NSError **)error
{
    [Utilities sendStatus:[NSString stringWithFormat:@"INFO: Starting TCP server on %@", [self getIPAddress]]];
    CFSocketContext socketCtxt = {0, (__bridge void *)self, NULL, NULL, NULL};
    
    // Create a new socket that calls socketCallback when a new conenction is accepted
    _socket = CFSocketCreate(kCFAllocatorDefault,
                             PF_INET,
                             SOCK_STREAM,
                             IPPROTO_TCP,
                             kCFSocketAcceptCallBack, (CFSocketCallBack)&socketCallback, &socketCtxt);
    
    // If we weren't able to create a socket, return NO
    if (_socket == NULL) {
        if (error) {
            *error = [[NSError alloc] initWithDomain:kTCPServerErrorDomain
                                                code:kTCPServerNoSocketsAvailable
                                            userInfo:nil];
        }
        
        CFRelease(_socket);
        
        return NO;
    }
    
    // the SO_REUSEADDR option tells the kernel that when the socket is closed,
    // the port bound to the socket should be freed immediately rather than kept
    // in-use for some period of time.
    int yes = 1;
    setsockopt(CFSocketGetNative(_socket), SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    
    
    // Create socket address
    struct sockaddr_in sin;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_len = sizeof(sin);
    sin.sin_family = AF_INET; /* Address family */
#warning get from Utilities
    sin.sin_port = htons(8124); /* Or a specific port */
    sin.sin_addr.s_addr= INADDR_ANY;
    
    NSData *sinData = [NSData dataWithBytes:&sin length:sizeof(sin)];
    
    // If we were not able to set the socket address, return NO
    if (kCFSocketSuccess != CFSocketSetAddress(_socket, (CFDataRef)sinData)) {
        if (error) {
            *error = [[NSError alloc] initWithDomain:kTCPServerErrorDomain
                                                code:kTCPServerCouldNotBindToIPv4Address
                                            userInfo:nil];
        }
        
        CFRelease(_socket);
        _socket = NULL;
        
        return NO;
    }
    
    CFRunLoopSourceRef socketsource = CFSocketCreateRunLoopSource(kCFAllocatorDefault,
                                                                  _socket,
                                                                  0);
    
    CFRunLoopAddSource(
                       CFRunLoopGetCurrent(),
                       socketsource,
                       kCFRunLoopDefaultMode);
    
    CFRelease(socketsource);

#warning Investigate NSNetService
//    // we can only publish the service if we have a type to publish with
//    if (nil != type) {
//        NSString *publishingDomain = domain ? domain : @"";
//        NSString *publishingName = name ? name : @"";
//        netService = [[NSNetService alloc] initWithDomain:publishingDomain
//                                                     type:type
//                                                     name:publishingName
//                                                     port:port];
//        [netService publish];
//    }
    
    return YES;
}

- (BOOL)stop
{
//    [netService stop];
//    [netService release];
//    netService = nil;
    CFSocketInvalidate(_socket);
    CFRelease(_socket);
    _socket = NULL;
    return YES;
}

- (void)handleNewConnectionFromAddress:(NSData *)addr
                           inputStream:(NSInputStream *)istream
                          outputStream:(NSOutputStream *)ostream
{
    NSLog(@"Handling connection");
    _istream = istream;
    [_istream setDelegate:self];
    [_istream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [_istream open];
    
    _ostream = ostream;
    [_ostream setDelegate:self];
    [_ostream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [_ostream open];
}

- (NSString *)getIPAddress {
    
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    // Free memory
    freeifaddrs(interfaces);
    return address;
}

# pragma mark - NSStreamDelegate

- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)eventCode {
    switch(eventCode) {
        case NSStreamEventHasSpaceAvailable:
        {
            break;
        }
            
        case NSStreamEventHasBytesAvailable:
        {
            [Utilities sendLog:@"LOG: TCP server data available"];
            uint64_t test = [Utilities getMachAbsoluteTime];
            
            unsigned long receiveTime = (long int)([[NSDate date] timeIntervalSince1970] * 1000);
            NSLog(@"%ld", receiveTime);
            [_ostream write:(const uint8_t *)&test maxLength:sizeof(long int)];
            
            if (stream == _istream) {
                NSMutableData *data = [[NSMutableData alloc] init];
                uint8_t buf[1024];
                long len = 0;
                
                len = [(NSInputStream *)stream read:buf maxLength:1024];
                
                unsigned long hostTime = (unsigned long)buf[0] | (unsigned long)buf[1] << 8 | (unsigned long)buf[2] << 16 | (unsigned long)buf[3] << 24 | (unsigned long)buf[4] << 32 | (unsigned long)buf[5] << 40;
                NSLog(@"%ld", hostTime);
                
                if(len) {
                    [data appendBytes:(const void *)buf length:len];

                    NSString *raw_data = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
                    
                    NSArray *parsed_data = [raw_data componentsSeparatedByString:@" "];
                    NSString *command = [parsed_data objectAtIndex:0];
                    
                    NSString *argument = NULL;
                    
                    if ([parsed_data count] > 1) {
                        argument = [parsed_data objectAtIndex:1];
                    }
                    
                    [_tcpDelegate didReceiveTCPCommand:command argument:argument];
                } else {
                    NSLog(@"no buffer!");
                }
            }
            break;
        }
            
        case NSStreamEventErrorOccurred:
        {
            NSError *error = [stream streamError];
            [Utilities sendWarning:[NSString stringWithFormat:@"WARN: TCP stream error %i: %@", (int)[error code], [error localizedDescription]]];
            break;
        }
            
        case NSStreamEventEndEncountered:
        {
            break;
        }
            
        case NSStreamEventOpenCompleted:
        {
            if (stream == _istream) {
                [Utilities sendLog:@"LOG: TCP server input stream opened!"];
            } else if (stream == _ostream) {
                [Utilities sendLog:@"LOG: TCP server output stream opened!"];
            }
            
            break;
        }
            
        case NSStreamEventNone:
        {
            break;
        }
            
        default:
        {
            break;
        }
    }
}

@end
