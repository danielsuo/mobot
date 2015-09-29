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
    
    dispatch_queue_t _queue;
    
    NSTimer *_timer;
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
        
        _queue = dispatch_queue_create(kSettingsTCPQueueName, DISPATCH_QUEUE_SERIAL);
        _timer = [NSTimer timerWithTimeInterval:10 target:self selector:@selector(autoclose:) userInfo:nil repeats:YES];
        
#warning We should start the timer only when we need to, but for now, we just always keep it ticking.
        [[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
    }
    
    return self;
}

- (void)initializeWriter
{
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: Opening TCP connection to %@:%d", _ip, _port]];
    [self open];
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
    } else {
        [Utilities sendWarning:@"WARN: Can't bind to remote TCP!"];
    }
}

- (void)close
{
#warning Consider saving data we haven't uploaded yet
    [_istream close];
    [_istream removeFromRunLoop:[NSRunLoop currentRunLoop]
                        forMode:NSDefaultRunLoopMode];
    _istream = nil;
    
    [_ostream close];
    [_ostream removeFromRunLoop:[NSRunLoop currentRunLoop]
                        forMode:NSDefaultRunLoopMode];
    _ostream = nil;
}

- (void)autoclose:(NSTimer*)timer
{
    if ([_ostream streamStatus] == NSStreamStatusOpen &&
        [_ostream streamStatus] != NSStreamStatusWriting &&
        [_ostream streamStatus] == NSStreamStatusAtEnd) {
        [self close];
    }
}

- (void)closeWriter
{
    [self close];
}

- (void)write:(const uint8_t * )data length:(NSUInteger)length
{
    [_ostream write:data maxLength:length];
}

- (NSString *)getAbsolutePath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _documentsDirectory, relativePath];
}

- (const uint8_t *)createMetadata:(char)fileType timestamp:(Float64)timestamp path:(NSString *)path dataLength:(uint32_t)dataLength
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
    
    return (const uint8_t *)[metadata bytes];
}

- (void)createDirectory:(NSString *)relativePath
{
    dispatch_async(_queue, ^(void) {
        NSString *absolutePath = [self getAbsolutePath:relativePath];
        
        const uint8_t *metadata = [self createMetadata:kSettingsTCPFileTypeDirectory timestamp:0 path:absolutePath dataLength:0];
        
        [self write:metadata length:kSettingsTCPMetaDataLength + [absolutePath length]];
    });
}

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath timestamp:(Float64)timestamp
{
    dispatch_async(_queue, ^(void) {
        NSDate *start = [NSDate date];
        
        NSString *absolutePath = [self getAbsolutePath:relativePath];
        
        const uint8_t *metadata = [self createMetadata:kSettingsTCPFileTypeRegular timestamp:timestamp path:absolutePath dataLength:(uint32_t)[data length]];
        
        Operation *currOperation = [[Operation alloc] initWithData:data];
        
        while (![_ostream hasSpaceAvailable]) {
            [Utilities sendLog:@"LOG: Waiting for stream..."];
        }
        
        [self write:metadata length:kSettingsTCPMetaDataLength + [absolutePath length]];
        
        while ([currOperation numBytesLeft] > 0) {
            NSUInteger numBytes = MIN(kSettingsTCPChunkSize, [currOperation numBytesLeft]);
            if ([_ostream hasSpaceAvailable]) {
                while (numBytes > 0) {
                    NSUInteger numBytesWritten = [_ostream write:[currOperation getMoreBytes] maxLength:numBytes];
                    [currOperation bytesWritten:numBytesWritten];
                    numBytes -= numBytesWritten;
                }
            }
        }
        NSDate *end = [NSDate  date];
        NSTimeInterval interval = [end timeIntervalSinceDate:start];
        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Uploaded to %@ in %0.2fms", absolutePath, interval * 1000]];
    });
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
            [Utilities sendWarning:@"WARN: TCP streaming error!"];
            [_tcpWriterDelegate tcpWriterError:nil];
            break;
        }
            
        case NSStreamEventEndEncountered:
        {
            [self close];
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