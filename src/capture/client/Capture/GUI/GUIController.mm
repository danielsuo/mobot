//
//  ViewController+GUI.m
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "GUIController.h"

@interface GUIController ()
{
    Preview *_preview;
    Toolbar *_toolbar;
    Status *_status;
    Controls *_controls;
}

@end

@implementation GUIController

- (void)viewDidLoad
{
    NSUInteger toolbarHeight = [Toolbar getToolbarHeight];
    NSUInteger statusbarHeight = [UIApplication sharedApplication].statusBarFrame.size.height;
    NSUInteger totalbarHeight = toolbarHeight + statusbarHeight;
    
    // Initialize preview
    _preview = [[Preview alloc] initWithFrame:self.view.frame];
    [self.view addSubview:_preview];
    
    // Initialize toolbar
    _toolbar = [[Toolbar alloc] initWithFrame:CGRectMake(0, statusbarHeight, self.view.frame.size.width, toolbarHeight)];
    _toolbar.toolbarDelegate = self;
    [self.view addSubview:_toolbar];
    
    // Initialize status
    _status = [[Status alloc] initWithFrame:CGRectMake(0, totalbarHeight, self.view.frame.size.width, self.view.frame.size.height / 2 - totalbarHeight)];
    [self.view addSubview:_status];
    
    // Initialize controls
    _controls = [[Controls alloc] initWithFrame:CGRectMake(0, totalbarHeight, self.view.frame.size.width, self.view.frame.size.height / 2 - totalbarHeight)];
    _controls.controlsDelegate = self;
}

#pragma mark - Toolbar

#pragma mark ToolbarDelegate

- (void)toolbarButtonPressed:(NSString *)buttonName toggleControlsView:(BOOL)toggle formatString:(NSString *)formatString
{
    [_guiControllerDelegate toolbarButtonPressed:buttonName toggleControlsView:toggle formatString:formatString];
}

#pragma mark - Controls

- (void)showControls:(NSString *)formatString value:(float)value min:(float)min max:(float)max
{
    [self.view addSubview:_controls];
    [_controls show:formatString value:value min:min max:max];
}

- (void)updateControls:(float)value
{
    [_controls update:value];
}

- (void)hideControls
{
    [_controls removeFromSuperview];
    _controlsShowing = nil;
    [_controls hide];
}

- (void)showRecordImage:(BOOL)show
{
    [_toolbar showRecordImage:show];
}

- (void)showFileImage:(BOOL)show
{
    [_toolbar showFileImage:show];
}

- (void)showDepthImage:(BOOL)show
{
    [_toolbar showDepthImage:show];
}

#pragma mark ControlsDelegate

- (void)controlsSliderValueChanged:(int)value
{
    [_guiControllerDelegate controlsSliderValueChanged:value];
}

#pragma mark - Status

#pragma mark - Preview

- (void)renderImage:(UIImage *)image type:(NSString *)type
{
    [_preview renderImage:image type:type];
}

@end
