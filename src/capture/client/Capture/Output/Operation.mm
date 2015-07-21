//
//  Operation.m
//  capture
//
//  Created by Daniel Suo on 7/12/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Operation.h"

@interface Operation ()
{
    NSUInteger _currentByteIndex;
    NSUInteger _length;
    
    const uint8_t *_bytes;
}

@end

@implementation Operation

- (id)initWithData:(NSData *)data
{
    self = [super init];
    
    if (self) {
        [self setData:data];
        
        _bytes = (const uint8_t *)[data bytes];
        _length = [data length];
        _currentByteIndex = 0;
    }
    
    return self;
}

- (NSUInteger)numBytesLeft
{
    return MAX(0, _length - _currentByteIndex);
    
}

- (const uint8_t *)getMoreBytes
{
    return &_bytes[_currentByteIndex];
}

- (void)bytesWritten:(NSUInteger)numBytes
{
    _currentByteIndex += numBytes;
}

@end