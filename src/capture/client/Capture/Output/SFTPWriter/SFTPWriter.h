//
//  SFTPWriter.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <NMSSH/NMSSH.h>

#import "Utilities.h"

@interface SFTPWriter : NSObject <NMSSHSessionDelegate>

@property (nonatomic, strong) NSString *documentsDirectory;
@property (nonatomic, strong) NSString *localDocumentsDirectory;

- (NSString *)getAbsolutePath:(NSString *)relativePath;
- (void)createDirectory:(NSString *)relativePath;
- (void)writeFile:(NSString *)relativePath;
- (void)upload;

@end
