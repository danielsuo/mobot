//
//  OutputController.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "OutputController.h"

@interface OutputController ()
{
    NSUInteger _imagesToRecord;
}

@end

@implementation OutputController

- (id)init
{
    self = [super init];
    
    if (self) {
#warning Think about abstract class to simplify logic
        _imagesToRecord = 0;
        _fileWriter = [[FileWriter alloc] init];
        _fileWriter.fileWriterDelegate = self;
        
        _tcpWriter = [[TCPWriter alloc] init];
        _tcpWriter.tcpWriterDelegate = self;
        
        _sftpWriter = [[SFTPWriter alloc] init];
        _sftpWriter.localDocumentsDirectory = _fileWriter.documentsDirectory;
        
        _writeMode = kWriteModeFile;
    }
    
    return self;
}

- (BOOL)isWriteMode:(NSString *)mode
{
    return [_writeMode isEqualToString:mode];
}

- (void)initNewScan
{
    [self createNewScanDirectories];
}

- (void)createNewScanDirectories
{
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: Write mode %@", _writeMode]];
    self.currScanDirectory = [Utilities stringFromDate:[NSDate date]];
    
    NSArray *directories = @[[NSString stringWithFormat:@"%@", _currScanDirectory],
                             [NSString stringWithFormat:@"%@/%@", _currScanDirectory, kFrameTypeColor],
                             [NSString stringWithFormat:@"%@/%@", _currScanDirectory, kFrameTypeDepth],
                             [NSString stringWithFormat:@"%@/%@", _currScanDirectory, kFrameTypeInfrared]];
    
    for (NSString *directory in directories) {
        if ([self isWriteMode:kWriteModeFile]) {
            [_fileWriter createDirectory:directory];
        } else if ([self isWriteMode:kWriteModeTCP]) {
            [_tcpWriter createDirectory:directory];
        }
    }
}

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath
{
    if ([self isWriteMode:kWriteModeFile]) {
        [_fileWriter writeData:data relativePath:relativePath];
    } else if ([self isWriteMode:kWriteModeTCP]) {
        [_tcpWriter writeData:data relativePath:relativePath];
    }
}

- (void)writeText:(NSString *)text relativePath:(NSString *)relativePath
{
    [self writeData:[text dataUsingEncoding:NSUTF8StringEncoding] relativePath:relativePath];
}

#pragma mark - SFTPWriter

- (void)upload
{
    [_sftpWriter upload];
}

#pragma mark - Recording methods

- (void)initializeWriter
{
    if ([self isWriteMode:kWriteModeFile]) {
        [_fileWriter initializeWriter];
    } else if ([self isWriteMode:kWriteModeTCP]) {
        [_tcpWriter initializeWriter];
    }
}

- (void)closeWriter
{
    _writerReady = NO;
    
    if ([self isWriteMode:kWriteModeFile]) {
        ;
    } else if ([self isWriteMode:kWriteModeTCP]) {
        [_tcpWriter closeWriter];
    }
}

- (void)recordOneFrame:(int)imagesPerFrame
{
    _imagesToRecord = imagesPerFrame;
}

- (void)startRecording
{
    [Utilities sendStatus:@"INFO: Start recording"];
    _imagesToRecord = -1;
}

- (void)stopRecording
{
    [Utilities sendStatus:@"INFO: Stop recording"];
    _imagesToRecord = 0;
}

- (BOOL)isRecording
{
    if (_imagesToRecord == -1) {
        return YES;
    } else if (_imagesToRecord > 0) {
        _imagesToRecord--;
        return YES;
    } else {
        [self closeWriter];
        return NO;
    }
}

#pragma mark - FileWriterDelegate

- (void)alertPresentRequested:(UIAlertController *)alert
{
    [_outputControllerDelegate alertPresentRequested:alert];
}

- (void)fileWriterReady
{
    _writerReady = YES;
    [_outputControllerDelegate writerReady];
}

- (void)fileWriterError:(NSError *)error
{
    _writerReady = NO;
    [_outputControllerDelegate writerError:error];
}

#pragma mark - TCPWriterDelegate

- (void)tcpWriterReady
{
    _writerReady = YES;
    [_outputControllerDelegate writerReady];
}

- (void)tcpWriterError:(NSError *)error
{
    _writerReady = NO;
    [self stopRecording];
    [_outputControllerDelegate writerError:error];
}


@end