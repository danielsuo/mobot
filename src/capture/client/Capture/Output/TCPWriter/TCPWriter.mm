//
//  TCPWriter.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

/*******************************************************************************
 * TCPWriter.mm: Objective-C++ implementation of PVCapture TCP client
 *
 * Serial packet format
 * --------------------------------------------------------------
 * Description                     |    Variable | Length (bytes)
 * --------------------------------------------------------------
 * 1. File type (0: dir, 1: file)  |         N/A |              1
 * 2. Timestamp                    |         N/A |              8
 * 3. String length of file path   | PATH_LENGTH |              1
 * 4. File path                    |        N/A  |    PATH_LENGTH
 * 5. Byte length of file          | FILE_LENGTH |              4
 * 6. File contents                |        N/A  |    FILE_LENGTH
 *
 ******************************************************************************/

#import "TCPWriter.h"

@interface TCPWriter ()
{
    NSString *_ip;
    int _port;
    
    NSInputStream *_istream;
    NSOutputStream *_ostream;
    
    //    dispatch_queue_t _dispatch_queue;
    NSOperationQueue *_queue;
    NSMutableArray *_aqueue;
    
    NSInteger _maxFrameQueue;
}

@end

@implementation TCPWriter

- (TCPWriter *)init
{
    self = [super init];
    
    if (self) {
        _documentsDirectory = [NSString stringWithFormat:@"image_data/%@", [Utilities deviceName]];
        
        _ip = [Utilities getSettingsValue:kSettingsTCPHostname];
        _port = [Utilities getSettingsValue:kSettingsTCPPort].intValue;

        _maxFrameQueue = 100;
        
        _aqueue = [[NSMutableArray alloc] init];
        _queue = [[NSOperationQueue alloc] init];
        [_queue setMaxConcurrentOperationCount:1];
        
#warning We should start the timer only when we need to, but for now, we just always keep it ticking.
        //        [[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
    }
    
    return self;
}

- (void)initializeWriter
{
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: Opening TCP connection to %@:%d", _ip, _port]];
    [self open];
}

#warning rename this to close and remove the other close
- (void)error
{
    [_aqueue removeAllObjects];
    [_queue cancelAllOperations];
    [_tcpWriterDelegate tcpWriterError:NULL];
}

- (void)open
{
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)_ip, _port, &readStream, &writeStream);
    
    if (readStream && writeStream) {
        CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
        CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
        
        //Set up inpustream
        _istream = (__bridge NSInputStream *)readStream;
        [_istream setDelegate:self];
        [_istream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [_istream open];
        
        //Set up outputstream
        _ostream = (__bridge NSOutputStream *)writeStream;
        [_ostream setDelegate:self];
        [_ostream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [_ostream open];
        
        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Opened connection to %@:%d", _ip, _port]];
    } else {
        [Utilities sendWarning:@"WARN: Can't bind to remote TCP!"];
    }
}

- (void)close
{
    // Don't close if we still have data to transmit
    if ([_queue operationCount] > 0) return;
    
    //    NSLog(@"Closing connections...");
    //#warning Consider saving data we haven't uploaded yet
    //    [_istream close];
    //    [_istream removeFromRunLoop:[NSRunLoop currentRunLoop]
    //                        forMode:NSDefaultRunLoopMode];
    //    _istream = nil;
    //
    //    [_ostream close];
    //    [_ostream removeFromRunLoop:[NSRunLoop currentRunLoop]
    //                        forMode:NSDefaultRunLoopMode];
    //    _ostream = nil;
}

- (void)closeWriter
{
    [self close];
}

- (NSString *)getAbsolutePath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _documentsDirectory, relativePath];
}

- (void)addData:(NSData *)data
{
    if ([_queue operationCount] > kSettingsTCPMaxQueueSize) {
        [self error];
    }
    
    NSBlockOperation *operation = [[NSBlockOperation alloc] init];
    
    __weak NSBlockOperation *weakOperation = operation;
    
    [operation addExecutionBlock:^{
        NSUInteger index = 0;
        const uint8_t *bytes = (const uint8_t *)[data bytes];
        
        while (index < [data length]) {
            
            if ([weakOperation isCancelled]) {
                return;
            }
            
            NSUInteger chunk_size = MIN([data length] - index, kSettingsTCPChunkSize);
            
            if ([_ostream hasSpaceAvailable]) {
                NSUInteger n = [_ostream write:bytes + index maxLength:chunk_size];
                if (n > 0) {
                    index += n;
                }
            }
        }
    }];
    
    [_queue addOperation:operation];
}

- (NSData *)createMetadata:(char)fileType timestamp:(Float64)timestamp path:(NSString *)path dataLength:(uint32_t)dataLength
{
    NSMutableData *metadata = [[NSMutableData alloc] init];
    
    // File type (1 byte)
    char type[1] = {fileType};
    [metadata appendBytes:type length:1];
    
    // Timestamp (8 bytes)
    [metadata appendBytes:&timestamp length:sizeof(Float64)];
    
    // Path length (1 byte)
    uint8_t pathLength = (uint8_t)[path length];
    [metadata appendBytes:&pathLength length:sizeof(uint8_t)];
    
    // Path (?? bytes)
    [metadata appendBytes:[[path dataUsingEncoding:NSUTF8StringEncoding] bytes] length:pathLength];
    
    // Data length (4 bytes)
    [metadata appendBytes:&dataLength length:sizeof(uint32_t)];
    
    return metadata;
}

- (void)createDirectory:(NSString *)relativePath
{
    NSString *absolutePath = [self getAbsolutePath:relativePath];
    
    NSData *metadata = [self createMetadata:kSettingsTCPFileTypeDirectory timestamp:0 path:absolutePath dataLength:0];
    
    [self addData:metadata];
}

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath timestamp:(Float64)timestamp
{
    NSString *absolutePath = [self getAbsolutePath:relativePath];
    NSData *metadata = [self createMetadata:kSettingsTCPFileTypeRegular timestamp:timestamp path:absolutePath dataLength:(uint32_t)[data length]];
    
    [self addData:metadata];
    [self addData:data];
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
            [Utilities sendLog:@"LOG: TCP data available"];
            break;
        }
            
        case NSStreamEventErrorOccurred:
        {
            NSError *error = [stream streamError];
            [Utilities sendWarning:[NSString stringWithFormat:@"WARN: TCP stream error %i: %@", (int)[error code], [error localizedDescription]]];
            [self error];
            break;
        }
            
        case NSStreamEventEndEncountered:
        {
            [self error];
            break;
        }
            
        case NSStreamEventOpenCompleted:
        {
            if (stream == _istream) {
                [Utilities sendLog:@"LOG: TCP input stream opened!"];
            } else if (stream == _ostream) {
                [Utilities sendLog:@"LOG: TCP output stream opened!"];
                [_tcpWriterDelegate tcpWriterReady];
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