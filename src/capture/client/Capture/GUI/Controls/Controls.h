//
//  Controls.h
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ControlsDelegate <NSObject>

@required
- (void)controlsSliderValueChanged:(int)value;

@end

@interface Controls : UIView

@property (nonatomic, strong) id <ControlsDelegate> controlsDelegate;

- (Controls *)initWithFrame:(CGRect)frame;

- (void)show:(NSString *)formatString value:(float)value min:(float)min max:(float)max;
- (void)hide;

- (void)setText:(NSString *)text;
- (void)setSlider:(float)value min:(float)min max:(float)max;

@end
