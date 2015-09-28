//
//  TCPWriter.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Utilities.h"
#import "Operation.h"

@protocol TCPWriterDelegate <NSObject>

@required
- (void)tcpWriterReady;
- (void)tcpWriterError:(NSError *)error;

@end

@interface TCPWriter : NSObject <NSStreamDelegate>

@property (nonatomic, strong) NSString *documentsDirectory;
@property (nonatomic, strong) id <TCPWriterDelegate> tcpWriterDelegate;

- (void)initializeWriter;
- (void)closeWriter;
- (NSString *)getAbsolutePath:(NSString *)relativePath;
- (void)createDirectory:(NSString *)relativePath;
- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath timestamp:(Float64)timestamp;

@end