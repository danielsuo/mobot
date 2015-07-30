//
//  ViewController+GUI.h
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Preview.h"
#import "Toolbar.h"
#import "Status.h"
#import "Controls.h"

@protocol GUIControllerDelegate <NSObject>

@required
- (void)toolbarButtonPressed:(NSString *)buttonName toggleControlsView:(BOOL)toggle formatString:(NSString *)formatString;
- (void)controlsSliderValueChanged:(int)value;
- (void)frameReadyToRecord:(NSData *)data withType:(NSString *)type;

@end

@interface GUIController : UIViewController <ToolbarDelegate, ControlsDelegate>

// GUI state
@property (nonatomic, assign, getter=isControlsViewShowing) BOOL controlsViewShowing;
@property (nonatomic, strong) NSString *controlsShowing;

// Delegates
@property (nonatomic, assign) id <GUIControllerDelegate> guiControllerDelegate;

// Public controls methods
- (void)showControls:(NSString *)formatString value:(float)value min:(float)min max:(float)max;
- (void)hideControls;

// Button-specific methods
- (void)showRecordImage:(BOOL)show;
- (void)showFileImage:(BOOL)show;
- (void)showDepthImage:(BOOL)show;

// Public preview methods
- (void)renderImage:(UIImage *)image type:(NSString *)type;

@end
