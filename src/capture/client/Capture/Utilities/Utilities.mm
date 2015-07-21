//
//  Utilities.m
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Utilities.h"

@implementation Utilities

+ (NSString *)deviceName
{
    return [Utilities getSettingsValue:kSettingsDeviceID];
}

+ (void) setSettingsValue:(NSString *) value forKey:(NSString *) key {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setValue:value forKey:key];
    [defaults synchronize];
};

+ (NSString *) getSettingsValue:(NSString *) key {
    return [[NSUserDefaults standardUserDefaults] valueForKey:key];
};

+ (void) setSettingsObject:(NSObject *) object forKey:(NSString *) key {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:object forKey:key];
    [defaults synchronize];
};

+ (NSObject *) getSettingsObject:(NSString *) key {
    return [[NSUserDefaults standardUserDefaults] objectForKey:key];
};

+ (NSString *)stringFromDate:(NSDate *)date
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:kFormattedStringDate];
    return [formatter stringFromDate:date];
}

+ (NSDate *)dateFromString:(NSString *)string
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:kFormattedStringDate];
    return [formatter dateFromString:string];
}

+ (void)keepDeviceAwake
{
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
}

+ (void)letDeviceSleep
{
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

+ (void)setAttribute:(NSString *)attribute withValue:(int)value onFile:(NSString *)path
{
    setxattr([path cStringUsingEncoding:NSASCIIStringEncoding],
             [attribute cStringUsingEncoding:NSASCIIStringEncoding],
             &value, sizeof(value), 0, 0);
}

+ (int)getAttribute:(NSString *)attribute onFile:(NSString *)path
{
    int result;
    
    getxattr([path cStringUsingEncoding:NSASCIIStringEncoding],
                    [attribute cStringUsingEncoding:NSASCIIStringEncoding],
                    &result, sizeof(result), 0, 0);
    
    return result;
}

+ (void)postNotification:(NSString *)name userInfo:(NSDictionary *)userInfo
{
    [[NSNotificationCenter defaultCenter] postNotificationName:name object:nil userInfo:userInfo];
}

+ (void)sendMessage:(NSString *)message type:(NSString *)type flush:(BOOL)flush
{
    NSDictionary *userInfo = @{
                               kNotificationFlush : [NSNumber numberWithBool:flush],
                               kNotificationMessage : message
                               };
    
    [self postNotification:type userInfo:userInfo];
}

+ (void)sendWarning:(NSString *)warning;
{
    [self sendWarning:warning flush:NO];
}

+ (void)sendLog:(NSString *)log
{
    [self sendLog:log flush:NO];
}

+ (void)sendStatus:(NSString *)status
{
    [self sendStatus:status flush:NO];
}

+ (void)sendWarning:(NSString *)warning flush:(BOOL)flush {
    [self sendMessage:warning type:kNotificationTypeWarning flush:flush];
}

+ (void)sendLog:(NSString *)log flush:(BOOL)flush {
    [self sendMessage:log type:kNotificationTypeLog flush:flush];
}

+ (void)sendStatus:(NSString *)status flush:(BOOL)flush
{
    [self sendMessage:status type:kNotificationTypeStatus flush:flush];
}

vm_size_t usedMemory(void) {
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes
}

vm_size_t freeMemory(void) {
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;
    vm_statistics_data_t vm_stat;
    
    host_page_size(host_port, &pagesize);
    (void) host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
}

void logMemoryUsage(void) {
    // compute memory usage and log if different by >= 100k
    static long prevMemUsage = 0;
    long curMemUsage = usedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;
    
    if (memUsageDiff > 100000 || memUsageDiff < -100000) {
        prevMemUsage = curMemUsage;
        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Memory used %7.1f (%+5.0f), free %7.1f kb", curMemUsage/1000.0f, memUsageDiff/1000.0f, freeMemory()/1000.0f]];
    }
}

@end
