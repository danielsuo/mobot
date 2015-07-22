//
//  Utilities.h
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <mach/mach.h>
#import <sys/xattr.h>

#define IS_IPHONE     ( [[[UIDevice currentDevice] model] rangeOfString:@"iPhone"].location != NSNotFound )
#define IS_IPAD       ( [[[UIDevice currentDevice] model] rangeOfString:@"iPad"].location != NSNotFound )

// Application settings
static NSString * const kFirstLaunchKey = @"has_launched";

// Extensions
static NSString * const kExtensionJPG = @"jpg";
static NSString * const kExtensionPNG = @"png";

// Status notifications
static NSString * const kNotificationTypeWarning = @"notification_warning";
static NSString * const kNotificationTypeLog = @"notification_log";
static NSString * const kNotificationTypeStatus = @"notification_status";
static NSString * const kNotificationMessage = @"notification_message";
static NSString * const kNotificationFlush = @"notification_flush";

// Frame types
static NSString * const kFrameTypeColor = @"color";
static NSString * const kFrameTypeDepth = @"depth";
static NSString * const kFrameTypeInfrared = @"infrared";

// Settings keys
static NSString * const kSettingsDeviceID = @"device_id";

//static NSString * const kSettingsModeWrite = @"mode_write";
//static NSString * const kSettingsModeWriteFile = @"file";
//static NSString * const kSettingsModeWriteTCP = @"tcp";
static NSString * const kWriteMode = @"mode_write";
static NSString * const kWriteModeFile = @"file";
static NSString * const kWriteModeTCP = @"tcp";

// TCPWriter settings
static NSString * const kSettingsTCPHostname = @"tcp_hostname";
static NSString * const kSettingsTCPPort = @"tcp_port";
static NSString * const kSettingsTCPMetadataFormat = @"%d%03d%@%010d";
static int const kSettingsTCPFileTypeDirectory = 0;
static int const kSettingsTCPFileTypeRegular = 1;
static int const kSettingsTCPChunkSize = 4096;
static const char * kSettingsTCPQueueName = "edu.princeton.vision.capture.tcpWriter";
#warning remove?
static NSString * const kSettingsTCPEOF = @"edu.princeton.vision.capture.tcpWriter.EOF";

// SFTPWriter settings
static NSString * const kSettingsSFTPHostname = @"sftp_hostname";
static NSString * const kSettingsSFTPPort = @"sftp_port";
static NSString * const kSettingsSFTPUsername = @"sftp_username";
static NSString * const kSettingsSFTPPassword = @"sftp_password";
static NSString * const kSettingsSFTPDirectory = @"sftp_directory";
static const char * kSettingsSFTPQueueName = "edu.princeton.vision.capture.sftpWriter";
static int const kSettingsSFTPNumQueues = 5;
static NSString * const kSettingsSFTPUploadFileAttribute = @"edu.princeton.vision.capture.sftpWriter.uploaded";
static int const kSettingsSFTPUploadFileAttributeUploaded = 1;
static int const kSettingsSFTPUploadFileAttributeNotUploaded = 0;

// Debug settings
static NSString * const kSettingsDebugIP = @"debug_ip";

// Formatted strings
static NSString * const kFormattedStringDate = @"yyyy-MM-dd'T'HH.mm.ss.SSS";

@interface Utilities : NSObject

+ (NSString *)deviceName;

// Getters and setters for NSUserDefaults
+ (void) setSettingsValue:(NSString *) value forKey:(NSString *) key;
+ (NSString *) getSettingsValue:(NSString *) key;
+ (void) setSettingsObject:(NSObject *) object forKey:(NSString *) key;
+ (NSObject *) getSettingsObject:(NSString *) key;

// Date string formatting
+ (NSString *)stringFromDate:(NSDate *)date;
+ (NSDate *)dateFromString:(NSString *)string;

// Device settings
+ (void)keepDeviceAwake;
+ (void)letDeviceSleep;

// Extended file attributes
+ (void)setAttribute:(NSString *)attribute withValue:(int)value onFile:(NSString *)path;
+ (int)getAttribute:(NSString *)attribute onFile:(NSString *)path;

// Logging
void logMemoryUsage(void);

// Notification center
+ (void)postNotification:(NSString *)name userInfo:(NSDictionary *)userInfo;

+ (void)sendWarning:(NSString *)warning;
+ (void)sendLog:(NSString *)log;
+ (void)sendStatus:(NSString *)status;

+ (void)sendWarning:(NSString *)warning flush:(BOOL)flush;
+ (void)sendLog:(NSString *)log flush:(BOOL)flush;
+ (void)sendStatus:(NSString *)status flush:(BOOL)flush;

@end
