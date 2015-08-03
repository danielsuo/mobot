//
//  Toolbar.m
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Toolbar.h"

@implementation Toolbar

- (Toolbar *)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self) {
        NSMutableArray *buttons = [[NSMutableArray alloc] init];
        UIBarButtonItem *separator = [[UIBarButtonItem alloc]
                                      initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                      target:nil
                                      action:nil];
        
        // Create buttons from left to right order
        [buttons addObject:[self createButton:@"trash" hasControlsView:NO formatString:nil]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"exposureduration" hasControlsView:YES formatString:@"Exposure duration: %d / 1000 s"]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"iso" hasControlsView:YES formatString:@"ISO: %d"]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"fps" hasControlsView:YES formatString:@"FPS: %d"]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"depth" hasControlsView:NO formatString:nil]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"file" hasControlsView:NO formatString:nil]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"upload" hasControlsView:NO formatString:nil]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"camera" hasControlsView:NO formatString:nil]];
        [buttons addObject:separator];
        
        [buttons addObject:[self createButton:@"record" hasControlsView:NO formatString:nil]];
        
        self.items = buttons;
    }
    
    return self;
}

- (void)showRecordImage:(BOOL)show
{
    ((UIBarButtonItem *)[self.items lastObject]).image = [UIImage imageNamed:(show ? @"record" : @"stop")];
}

#warning fix these magic number catastrophes
- (void)showFileImage:(BOOL)show
{
    ((UIBarButtonItem *)[self.items objectAtIndex:[self.items count] - 6 - 1]).image = [UIImage imageNamed:(show ? @"file" : @"tcp")];
}

- (void)showDepthImage:(BOOL)show
{
    ((UIBarButtonItem *)[self.items objectAtIndex:[self.items count] - 8 - 1]).image = [UIImage imageNamed:(show ? @"depth" : @"infrared")];
}

- (Button *)createButton:(NSString *)name hasControlsView:(BOOL)hasControlsView formatString:(NSString *)formatString
{
    return [[Button alloc] initWithName:name target:self
                                 action:@selector(handleButton:)
                        hasControlsView:hasControlsView
                           formatString:formatString];
}

- (void)handleButton:(id)sender
{
    Button *button = (Button *)sender;
    [self.toolbarDelegate toolbarButtonPressed:button.title
                            toggleControlsView:button.hasControlsView
                                  formatString:button.formatString];
}

+ (NSUInteger)getToolbarHeight
{
    return TOOLBAR_HEIGHT;
}

@end
