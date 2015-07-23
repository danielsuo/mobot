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
 * Description                    |     Variable | Length (bytes)
 * --------------------------------------------------------------
 * 1. File type (0: dir, 1: file)  |        N/A  |              1
 * 2. String length of file path   | PATH_LENGTH |              3
 * 3. File path                    |        N/A  |    PATH_LENGTH
 * 4. Byte length of file          | FILE_LENGTH |             10
 * 5. File contents                |        N/A  |    FILE_LENGTH
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
//    uint8_t *bytes = (uint8_t *)malloc(sizeof(uint8_t));
//    bytes[0] = '\0';
//    [_ostream write:bytes maxLength:1];
//    [self write:kSettingsTCPEOF];
    [self close];
}

- (void)write:(NSString *)string
{
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: string sent via TCP: %@", string]];
    const uint8_t * rawstring = (const uint8_t *)[string UTF8String];
    [_ostream write:rawstring maxLength:strlen((const char *)rawstring)];
}

- (NSString *)getAbsolutePath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _documentsDirectory, relativePath];
}

- (void)createDirectory:(NSString *)relativePath
{
    dispatch_async(_queue, ^(void) {
        NSString *absolutePath = [self getAbsolutePath:relativePath];
        NSString *metadata = [NSString stringWithFormat:kSettingsTCPMetadataFormat,
                              kSettingsTCPFileTypeDirectory,
                              (int)[absolutePath length],
                              absolutePath,
                              0];
        
        [self write:metadata];
    });
}

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath
{
    dispatch_async(_queue, ^(void) {
        NSDate *start = [NSDate date];
        
        NSString *absolutePath = [self getAbsolutePath:relativePath];
        NSString *metadata = [NSString stringWithFormat:kSettingsTCPMetadataFormat,
                              kSettingsTCPFileTypeRegular,
                              (int)[absolutePath length],
                              absolutePath,
                              (int)[data length]];
        
        Operation *currOperation = [[Operation alloc] initWithData:data];
        
        while (![_ostream hasSpaceAvailable]) {
            [Utilities sendLog:@"LOG: Waiting for stream..."];
        }
        
        [self write:metadata];
        
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
            [self close];
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