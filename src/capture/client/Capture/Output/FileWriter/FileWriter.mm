//
//  FileWriter.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "FileWriter.h"

@implementation FileWriter

- (FileWriter *)init
{
    self = [super init];
    
    if (self) {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        _documentsDirectory = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], [Utilities deviceName]];
    }
    
    return self;
}

- (void)initializeWriter
{
    [_fileWriterDelegate fileWriterReady];
}

- (NSString *)getAbsolutePath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _documentsDirectory, relativePath];
}

- (void)createDirectory:(NSString *)relativePath
{
    NSString *absolutePath = [self getAbsolutePath:relativePath];
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: Created dir at %@", absolutePath]];
    if (![[NSFileManager defaultManager] fileExistsAtPath:absolutePath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:absolutePath withIntermediateDirectories:YES attributes:nil error:nil];
        
        [Utilities setAttribute:kSettingsSFTPUploadFileAttribute
                      withValue:kSettingsSFTPUploadFileAttributeNotUploaded
                         onFile:absolutePath];
    }
}

- (void)writeData:(NSData *)data relativePath:(NSString *)relativePath
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void) {
        NSString *absolutePath = [self getAbsolutePath:relativePath];
        
        NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:absolutePath];
        
        if (fileHandle) {
            [fileHandle seekToEndOfFile];
            [fileHandle writeData:data];
            [fileHandle closeFile];
        } else {
            [data writeToFile:absolutePath atomically:YES];
        }
        
        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Wrote file at %@", absolutePath]];
        
        [Utilities setAttribute:kSettingsSFTPUploadFileAttribute
                      withValue:kSettingsSFTPUploadFileAttributeNotUploaded
                         onFile:absolutePath];
    });
}

- (BOOL)deleteFile:(NSString *)path relative:(BOOL)relative
{
    BOOL success = false;
    NSString *absolutePath = relative ? [self getAbsolutePath:path] : path;
    if ([[NSFileManager defaultManager] fileExistsAtPath:absolutePath]) {
        while (!success) {
            [Utilities sendLog:[NSString stringWithFormat:@"LOG: Deleting file at %@", absolutePath]];
            success = [[NSFileManager defaultManager] removeItemAtPath:absolutePath error:nil];
        }
    }
}

// Recursively delete any directories that are empty or only have subdirectories (no files)
- (BOOL)deleteEmptyDirectories:(NSString *)directoryPathRelativeToParent parent:(NSString *)absoluteParentPath
{
    // Get absolute path of directory
    NSString *absoluteDirectoryPath = [NSString stringWithFormat:@"%@/%@", absoluteParentPath, directoryPathRelativeToParent];
    
    // Return YES if current directory doesn't exist
    if (![[NSFileManager defaultManager] fileExistsAtPath:absoluteDirectoryPath]) {
        return YES;
    }
    
    // List all directory contents
    NSArray *contents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:absoluteDirectoryPath error:nil];
    
    // If there are no contents, delete the directory and return YES
    if ([contents count] == 0) {
        [self deleteFile:absoluteDirectoryPath relative:NO];
        return YES;
    }
    
    // Otherwise, loop through contents
    for (NSString *file in contents) {
        
        // Get full file path string relative to _documentsDirectory
        NSString *absoluteFilePath = [NSString stringWithFormat:@"%@/%@", absoluteDirectoryPath, file];
        
        // Get file attributes
        NSDictionary *fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:absoluteFilePath
                                                                                        error:nil];
        
        // If the file is not directory or is not empty return NO
        if (fileAttributes[NSFileType] != NSFileTypeDirectory ||
            ![self deleteEmptyDirectories:file parent:absoluteDirectoryPath]) {
            return NO;
        }
    }
    
    // Delete the current directory
    [self deleteFile:absoluteDirectoryPath relative:NO];
    
    return YES;
}

#warning Clean up
- (void)handleTrash
{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"DELETE DATA"
                                                                   message:@"This is dangerous!"
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *deleteAllAction = [UIAlertAction actionWithTitle:@"DELETE ALL" style:UIAlertActionStyleDestructive
                                                            handler:^(UIAlertAction * action) {
                                                                UIAlertController *areYouSureAlert = [UIAlertController alertControllerWithTitle:@"DELETE ALL DATA"
                                                                                                                                         message:@"Are you sure you want to delete all data?"
                                                                                                                                  preferredStyle:UIAlertControllerStyleAlert];
                                                                
                                                                UIAlertAction *deleteAllData = [UIAlertAction actionWithTitle:@"YES I'M SURE" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
                                                                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void) {
                                                                        for (NSString *file in [[NSFileManager defaultManager] contentsOfDirectoryAtPath:_documentsDirectory error:nil]) {
                                                                            [self deleteFile:file relative:YES];
                                                                        }
                                                                        [self deleteFile:[Utilities deviceName] relative:YES];
                                                                    });
                                                                }];
                                                                
                                                                UIAlertAction *cancelDelete = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * action) {}];
                                                                
                                                                [areYouSureAlert addAction:deleteAllData];
                                                                [areYouSureAlert addAction:cancelDelete];
                                                                [_fileWriterDelegate alertPresentRequested:areYouSureAlert];
                                                            }];
    
    UIAlertAction *deleteSinceLastUploadAction = [UIAlertAction actionWithTitle:@"Delete since last upload" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void) {
            // Delete files first
            NSDirectoryEnumerator *de = [[NSFileManager defaultManager] enumeratorAtPath:_documentsDirectory];
            for (NSString *file in de) {
                if (de.fileAttributes[NSFileType] == NSFileTypeRegular) {
                    BOOL uploaded = [Utilities getAttribute:kSettingsSFTPUploadFileAttribute
                                                     onFile:[self getAbsolutePath:file]] == kSettingsSFTPUploadFileAttributeUploaded;
                    if (uploaded) {
                        [self deleteFile:file relative:YES];
                    }
                }
            }
            
            // Delete empty directories next
            [self deleteEmptyDirectories:@"" parent:_documentsDirectory];
        });
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * action) {}];
    
    [alert addAction:deleteSinceLastUploadAction];
    [alert addAction:deleteAllAction];
    [alert addAction:cancelAction];
    [_fileWriterDelegate alertPresentRequested:alert];
}

@end
