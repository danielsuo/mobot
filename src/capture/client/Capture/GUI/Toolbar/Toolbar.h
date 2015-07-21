//
//  Toolbar.h
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "Button.h"

#define TOOLBAR_HEIGHT 44;

@protocol ToolbarDelegate <NSObject>

@required
- (void)toolbarButtonPressed:(NSString *)buttonName toggleControlsView:(BOOL)toggle formatString:(NSString *)formatString;

@end

@interface Toolbar : UIToolbar

@property (nonatomic, strong) id <ToolbarDelegate> toolbarDelegate;

- (Toolbar *)initWithFrame:(CGRect)frame;
- (void)showRecordImage:(BOOL)show;
- (void)showFileImage:(BOOL)show;
- (void)showDepthImage:(BOOL)show;
+ (NSUInteger)getToolbarHeight;

@end
