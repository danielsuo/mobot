//
//  Operation.h
//  capture
//
//  Created by Daniel Suo on 7/12/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Operation : NSObject

@property (nonatomic, strong) NSData *data;

- (id)initWithData:(NSData *)data;
- (NSUInteger)numBytesLeft;
- (const uint8_t *)getMoreBytes;
- (void)bytesWritten:(NSUInteger)numBytes;

@end