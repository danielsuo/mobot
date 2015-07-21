//
//  OutputController.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Utilities.h"

#import "FileWriter.h"
#import "TCPWriter.h"
#import "SFTPWriter.h"

@protocol OutputControllerDelegate <NSObject>

@required
- (void)alertPresentRequested:(UIAlertController *)alert;
- (void)writerReady;
- (void)writerError:(NSError *)error;

@end

@interface OutputController : NSObject <FileWriterDelegate, TCPWriterDelegate>

@property (nonatomic, strong) id <OutputControllerDelegate> outputControllerDelegate;

@property (nonatomic, strong) NSString *writeMode;

@property (nonatomic, strong) FileWriter *fileWriter;
@property (nonatomic, strong) TCPWriter *tcpWriter;
@property (nonatomic, strong) SFTPWriter *sftpWriter;

@property (nonatomic, strong) NSString *currScanDirectory;
@property (nonatomic) BOOL writerReady;

- (void)initializeWriter;
- (void)closeWriter;
- (void)startRecording;
- (void)stopRecording;
- (void)recordOneFrame:(int)imagesPerFrame;
- (BOOL)isRecording;

- (void)initNewScan;

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath;
- (void)writeText:(NSString *)text relativePath:(NSString *)relativePath;

- (void)upload;

@end
