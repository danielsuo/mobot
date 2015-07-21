//
//  Status.m
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Status.h"

@interface Status ()
{
    UITextView *_logs;
    UITextView *_warnings;
    UITextView *_status;
    
    NSString *_logHeader;
    NSString *_warningHeader;
    NSString *_statusHeader;
}

@end

@implementation Status

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self) {
        // Initialize logs view
        CGRect logsFrame = frame;
        logsFrame.origin.y = 0;
        logsFrame.size.width = frame.size.width / 3;
        
        _logs = [[UITextView alloc] initWithFrame:logsFrame];
        _logs.layer.borderColor = [UIColor grayColor].CGColor;
        _logs.layer.borderWidth = 2;
        _logs.editable = NO;
        _logs.selectable = NO;
        
        _logHeader = @"LOGS";
        _logs.text = _logHeader;
        
        [self addSubview:_logs];
        
        // Initialize warnings view
        CGRect warningsFrame = logsFrame;
        warningsFrame.origin.x += frame.size.width / 3;
        
        _warnings = [[UITextView alloc] initWithFrame:warningsFrame];
        _warnings.layer.borderColor = [UIColor redColor].CGColor;
        _warnings.layer.borderWidth = 2;
        _warnings.editable = NO;
        _warnings.selectable = NO;
        
        _warningHeader = @"WARNINGS";
        _warnings.text = _warningHeader;
        
        [self addSubview:_warnings];
        
        // Initialize status view
        CGRect statusFrame = warningsFrame;
        statusFrame.origin.x += frame.size.width / 3;
        
        _status = [[UITextView alloc] initWithFrame:statusFrame];
        _status.layer.borderColor = [UIColor blueColor].CGColor;
        _status.layer.borderWidth = 2;
        _status.editable = NO;
        _status.selectable = NO;
        
        _statusHeader = @"STATUS";
        _status.text = _statusHeader;
        
        [self addSubview:_status];
        
        // Set up notification observers
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotification:) name:kNotificationTypeLog object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotification:) name:kNotificationTypeWarning object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleNotification:) name:kNotificationTypeStatus object:nil];
    }

    return self;
}

- (void)handleNotification:(NSNotification *)notification
{
    NSString *message = [notification.userInfo objectForKey:kNotificationMessage];
    BOOL flush = [((NSNumber *)[notification.userInfo objectForKey:kNotificationFlush]) boolValue];
    
    // Log all messages to console
    NSLog(@"%@", message);
    
    UITextView *output;
    NSString *header;
    
    if ([notification.name isEqualToString:kNotificationTypeLog]) {
        output = _logs;
        header = _logHeader;
    } else if ([notification.name isEqualToString:kNotificationTypeWarning]) {
        output = _warnings;
        header = _warningHeader;
    } else if ([notification.name isEqualToString:kNotificationTypeStatus]) {
        output = _status;
        header = _statusHeader;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        if (flush || [output.text length] > 1000) { // flush if getting too long
            output.text = [NSString stringWithFormat:@"%@\n%@", header, message];
        } else {
            output.text = [NSString stringWithFormat:@"%@\n%@", output.text, message];
        }
        
        NSRange range = NSMakeRange(output.text.length, 0);
        [UIView setAnimationsEnabled:NO];
        [output scrollRangeToVisible:range];
        [UIView setAnimationsEnabled:YES];
    });
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end