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
#import <mach/mach_time.h>
#import <sys/xattr.h>
#import <unistd.h>

#define IS_IPHONE     ( [[[UIDevice currentDevice] model] rangeOfString:@"iPhone"].location != NSNotFound )
#define IS_IPAD       ( [[[UIDevice currentDevice] model] rangeOfString:@"iPad"].location != NSNotFound )

#define subarray(type, arr, off, len) (type (&)[len])(*(arr + off));

// Application settings
static NSString * const kFirstLaunchKey = @"has_launched";

// Extensions
static NSString * const kExtensionJPG = @"jpg";
static NSString * const kExtensionPNG = @"png";

// Status notifications
#warning this should be enum
static NSString * const kNotificationTypeWarning = @"notification_warning";
static NSString * const kNotificationTypeLog = @"notification_log";
static NSString * const kNotificationTypeStatus = @"notification_status";
static NSString * const kNotificationMessage = @"notification_message";
static NSString * const kNotificationFlush = @"notification_flush";

// Color sensor settings
static NSString * const kSettingsColorAutoExpose = @"color_autoexpose";
#warning this should be dependent on image size
static int const kColorAutoExposureSamplePixelPeriod = 25;
static int const kColorAutoExposureSampleFramePeriod = 5;

// Exposure duration bounds in milliseconds
static int const kColorMinExposureDuration = 1;
static int const kColorMaxExposureDuration = 16;

// FPS bounds in FPS
static int const kColorMinFPS = 1;
static int const kColorMaxFPS = 30;

// ISO bounds
//static int const kColorMinISO =

// Average brightness bounds
static int const kColorMinBrightness = 80;
static int const kColorMaxBrightness = 150;

// Frame types
#warning This should be an enum
static NSString * const kFrameTypeColor = @"color";
static NSString * const kFrameTypeDepth = @"depth";
static NSString * const kFrameTypeInfrared = @"infrared";

// Settings keys
static NSString * const kSettingsDeviceID = @"device_id";
static NSString * const kSettingsDeviceAngle = @"device_angle";

#warning This should be an enum
static NSString * const kWriteMode = @"mode_write";
static NSString * const kWriteModeFile = @"file";
static NSString * const kWriteModeTCP = @"tcp";

// TCPWriter settings
static NSString * const kSettingsTCPHostname = @"tcp_hostname";
static NSString * const kSettingsTCPPort = @"tcp_port";

#warning this should be an enum
static char kSettingsTCPFileTypeDirectory = 0x00;
static char kSettingsTCPFileTypeRegular = 0x01;
static NSUInteger kSettingsTCPMetaDataLength = 14;

static int const kSettingsTCPChunkSize = 4096;
static const char * kSettingsTCPQueueName = "edu.princeton.vision.capture.tcpWriter";
static int const kSettingsTCPMaxQueueSize = 50;
#warning remove?
static NSString * const kSettingsTCPEOF = @"edu.princeton.vision.capture.tcpWriter.EOF\r\n";

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

// TCP Server settings
typedef enum {
    TCPDeviceCommandGetMachTime,
    TCPDeviceCommandStartRecording,
    TCPDeviceCommandStopRecording,
    TCPDeviceCommandUpload,
    TCPDeviceCommandFileModeTCP,
    TCPDeviceCommandDimScreen
}TCPDeviceCommand;

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

// Timing tools
+ (Float64)getMachAbsoluteTime;

@end
