//
//  SFTPWriter.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "SFTPWriter.h"

@interface SFTPWriter ()
{
    NSString *_hostname;
    int _port;
    NSString *_username;
    NSString *_password;
    
    NSString *_homeDirectory;
    
    dispatch_queue_t _queue;
    NMSSHSession *_session;
    
    NSTimer *_timer;
}

@end

@implementation SFTPWriter

- (SFTPWriter *)init
{
    self = [super init];
    
    if (self) {
        _hostname = [Utilities getSettingsValue:kSettingsSFTPHostname];
        _port = [Utilities getSettingsValue:kSettingsSFTPPort].intValue;
        _username = [Utilities getSettingsValue:kSettingsSFTPUsername];
        _password = [Utilities getSettingsValue:kSettingsSFTPPassword];
        
        _homeDirectory = [Utilities getSettingsValue:kSettingsSFTPDirectory];
        _documentsDirectory = [NSString stringWithFormat:@"%@/%@", _homeDirectory, [Utilities getSettingsValue:kSettingsDeviceID]];
        
        _queue = dispatch_queue_create(kSettingsSFTPQueueName, DISPATCH_QUEUE_SERIAL);
        
        [NMSSHLogger sharedLogger].logBlock = ^(NMSSHLogLevel level, NSString *format) {
            // Dump everything for now
            [Utilities sendLog:[NSString stringWithFormat:@"LOG: %@", format]];
        };
    }
    
    return self;
}

#warning This isn't quite right; we don't want to continually reallocate for the main session
- (NMSSHSession *)getMainSession
{
    if (!_session || !_session.isConnected || !_session.isAuthorized) {
        _session = [self open];
    }
    return _session;
}

- (NMSSHSession *)open
{
    NMSSHSession *session = [NMSSHSession connectToHost:_hostname port:_port withUsername:_username];
    
    [Utilities sendStatus:[NSString stringWithFormat:@"INFO: Connecting to %@", _hostname]];
    
    if (session.isConnected) {
        [session authenticateByPassword:_password];
        if (!session.isAuthorized) {
            [session authenticateByKeyboardInteractiveUsingBlock:^(NSString *) {
                return _password;
            }];
        }
        
        if (session.isAuthorized) {
            [session.sftp connect];
            return session;
        } else {
            [Utilities sendWarning:@"WARN: Failed to authorize SFTP"];
        }
    } else {
        [Utilities sendWarning:@"WARN: Failed to connect via SFTP"];
    }
    
    return nil;
}

- (void)close:(NMSSHSession *)session
{
    [session disconnect];
}

- (NSString *)getAbsolutePath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _documentsDirectory, relativePath];
}

- (NSString *)getLocalPath:(NSString *)relativePath
{
    return [NSString stringWithFormat:@"%@/%@", _localDocumentsDirectory, relativePath];
}

- (void)createDirectory:(NSString *)relativePath
{
    [self createDirectory:relativePath withSession:[self getMainSession]];
}

- (void)createDirectory:(NSString *)relativePath withSession:(NMSSHSession *)session
{
    NSString *absolutePath = [self getAbsolutePath:relativePath];
    while (![session.sftp directoryExistsAtPath:absolutePath]) {
        if ([session.sftp createDirectoryAtPath:absolutePath]) {
            [Utilities setAttribute:kSettingsSFTPUploadFileAttribute withValue:kSettingsSFTPUploadFileAttributeUploaded onFile:[self getLocalPath:relativePath]];
            [Utilities sendLog:[NSString stringWithFormat:@"Creating directory: %@", relativePath]];
        }
    }
}

- (BOOL)isUploaded:(NSString *)path
{
    return [Utilities getAttribute:kSettingsSFTPUploadFileAttribute onFile:path] == kSettingsSFTPUploadFileAttributeUploaded;
}

- (void)setUploadState:(int)state forPath:(NSString *)path
{
    [Utilities setAttribute:kSettingsSFTPUploadFileAttribute withValue:state onFile:path];
}

#warning Error if create directory, upload half files in directory, and then change server. Won't create new directory on new server
- (void)upload
{
    [Utilities sendStatus:@"INFO: Begin upload"];
    
    // We want the device to stay awake while we're working
    [Utilities keepDeviceAwake];
    
    dispatch_async(_queue, ^(void) {
        NMSSHSession *mainSession = [self getMainSession];;
        
        // If we can't connect, return immediately
        if (!(mainSession.isConnected && mainSession.isAuthorized)) {
            return;
        }
        
        // Create _documentsDirectory. We assume that _homeDirectory already exists, so creating the device directory
        [self createDirectory:@""];
        
        int totalNumFiles = 0;
        
        // Count number of files and create directories as needed
        NSDirectoryEnumerator *de = [[NSFileManager defaultManager] enumeratorAtPath:_localDocumentsDirectory];
        
        for (NSString *filePath in de) {
            // If we have a directory, create a new dictionary entry to track total number of files and files uploaded
            if (de.fileAttributes[NSFileType] == NSFileTypeDirectory) {
                BOOL directoryUploaded = ![self isUploaded:[self getLocalPath:filePath]];
                [Utilities sendLog:[NSString stringWithFormat:@"Checking file: %d %@", directoryUploaded, filePath]];
                if (directoryUploaded) {
                    [self createDirectory:filePath];
                }
            }
            
            else if (de.fileAttributes[NSFileType] == NSFileTypeRegular) {
                // Ignore files we've already uploaded
                if (![self isUploaded:[self getLocalPath:filePath]]) {
                    totalNumFiles++;
                }
            };
        }
            
        [self close:[self getMainSession]];
        
#warning Refactor with dispatch groups http://macoscope.com/blog/gcd-dispatch-groups-with-an-additional-level-of-inception/
#warning Stop uploading gracefully
#warning Invalid pointer dequeued session
        if (totalNumFiles > 0) {
            // Use multiple threads for multiple SFTP connections
            // Each thread should upload chunkSize number of files
            int chunkSize = totalNumFiles / kSettingsSFTPNumQueues + 1;
            
            // Keep track of total number of files uploaded
            __block int totalNumFilesUploaded = 0;
            
            // Keep track of number of SFTP connections
            __block int totalNumConnections = 0;
            __block int numConnectionsClosed = 0;
            
            // Create a dispatch group
            dispatch_group_t group = dispatch_group_create();
            
            for (int i = 0; i < kSettingsSFTPNumQueues; i++) {
                int lower = i * chunkSize;
                int upper = MIN((i + 1) * chunkSize, totalNumFiles);
                
                // Make sure we actually have files to write
                if (lower < upper) {
                    
                    // Run each SFTP connection concurrently
                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void) {
                        __block int fileIndex = 0;
                        __block int numFilesUploadedByQueue = 0;
                        
                        // Each thread has its own sftp session
                        NMSSHSession *session = [self open];
                        totalNumConnections++;
                        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Open SFTP session %d of %d", totalNumConnections, kSettingsSFTPNumQueues]];
                        
                        if (!(session.isConnected && session.isAuthorized)) {
                            return;
                        }
                        
                        // Loop over all the files, but only process those in the lower-upper window this thread cares about
                        NSDirectoryEnumerator *de = [[NSFileManager defaultManager] enumeratorAtPath:_localDocumentsDirectory];
                        
                        for (NSString *filePath in de) {
                            
                            if (de.fileAttributes[NSFileType] == NSFileTypeRegular) {
                                BOOL fileUploaded = [self isUploaded:[self getLocalPath:filePath]];
                                
                                if (!fileUploaded) {
                                    if (fileIndex >= lower && fileIndex < upper) {
                                        [self writeFile:filePath withSession:session];
                                        
                                        // Do this in serial
                                        dispatch_async(_queue, ^(void) {
                                            [self setUploadState:kSettingsSFTPUploadFileAttributeUploaded
                                                         forPath:[self getLocalPath:filePath]];
                                            
                                            totalNumFilesUploaded++;
                                            numFilesUploadedByQueue++;
                                            [Utilities sendLog:[NSString stringWithFormat:@"LOG: Writing %d of %d", totalNumFilesUploaded, totalNumFiles]];
                                            
                                            if (numFilesUploadedByQueue == upper - lower) {
                                                [self close:session];
                                                numConnectionsClosed++;
                                                [Utilities sendLog:[NSString stringWithFormat:@"LOG: Closed SFTP session %d of %d", numConnectionsClosed, totalNumConnections]];
                                            }
                                            
                                            if (totalNumFilesUploaded == totalNumFiles) {
                                                [Utilities sendStatus:@"INFO: Upload complete!"];
                                            }
                                        });
                                    }
                                    fileIndex++;
                                }
                            }
                        }
                    });
                }
            }
        } else {
            [Utilities sendStatus:@"INFO: Nothing to upload"];
        }
        
        [Utilities letDeviceSleep];
    });
}

- (void)writeFile:(NSString *)relativePath
{
    [self writeFile:relativePath withSession:[self getMainSession]];
}

- (void)writeFile:(NSString *)relativePath withSession:(NMSSHSession *)session
{
    NSString *localFilePath = [NSString stringWithFormat:@"%@/%@", _localDocumentsDirectory, relativePath];
    NSString *remoteFilePath = [self getAbsolutePath:relativePath];
    BOOL success = false;
    
    while (!success) {
        success = [session.sftp writeFileAtPath:localFilePath toFileAtPath:remoteFilePath];
    }
}

#pragma mark - NMSSHSessionDelegate
#warning Implement this
- (void)session:(NMSSHSession *)session didDisconnectWithError:(NSError *)error
{
    
}

@end