//
//  FileWriter.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "Utilities.h"

@protocol FileWriterDelegate <NSObject>

@required
- (void)alertPresentRequested:(UIAlertController *)alert;
- (void)fileWriterReady;
- (void)fileWriterError:(NSError *)error;

@end

@interface FileWriter : NSObject

@property (nonatomic, strong) NSString *documentsDirectory;
@property (nonatomic, strong) id <FileWriterDelegate> fileWriterDelegate;

- (void)initializeWriter;
- (NSString *)getAbsolutePath:(NSString *)relativePath;
- (void)createDirectory:(NSString *)relativePath;
- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath timestamp:(Float64)timestamp;

- (void)handleTrash;

@end