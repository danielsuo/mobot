//
//  Controls.m
//  Capture
//
//  Created by Daniel Suo on 7/14/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Controls.h"

@interface Controls ()
{
    NSString *_formatString;
    UITextView *_controlsTextView;
    UISlider *_controlsSlider;
}

@end

@implementation Controls

- (Controls *)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self) {
        self.backgroundColor = [UIColor redColor];
        
        // Initialize slider
        _controlsSlider = [[UISlider alloc] initWithFrame:CGRectMake((self.frame.size.width - self.frame.size.width * 3 / 4) / 2, (self.frame.size.height - 10) * 3 / 4, self.frame.size.width * 3 / 4, 10)];
        _controlsSlider.continuous = YES;
        [_controlsSlider addTarget:self action:@selector(handleValueChange:) forControlEvents:UIControlEventValueChanged];
        
        // Initialize text view
        _controlsTextView = [[UITextView alloc] initWithFrame:CGRectMake((self.frame.size.width - self.frame.size.width * 3 / 4) / 2, (self.frame.size.height - 10) / 4, self.frame.size.width * 3 / 4, 30)];
        _controlsTextView.backgroundColor = self.backgroundColor;
        _controlsTextView.scrollEnabled = NO;
        _controlsTextView.textAlignment = NSTextAlignmentCenter;
        _controlsTextView.editable = NO;
    }
    
    return self;
}

- (void)show:(NSString *)formatString value:(float)value min:(float)min max:(float)max
{
    [self addSubview:_controlsSlider];
    [self addSubview:_controlsTextView];

    _formatString = formatString;
    [self setText:[NSString stringWithFormat:_formatString, (int)value]];
    [self setSlider:value min:min max:max];
}

- (void)hide
{
    [[self subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
}

- (void)setText:(NSString *)text
{
    _controlsTextView.text = text;
}

- (void)setSlider:(float)value min:(float)min max:(float)max
{
    _controlsSlider.minimumValue = min;
    _controlsSlider.maximumValue = max;
    _controlsSlider.value = value;
}

- (void)handleValueChange:(id)sender
{
    UISlider *slider = (UISlider *)sender;
    [self setText:[NSString stringWithFormat:_formatString, (int)slider.value]];
    [self.controlsDelegate controlsSliderValueChanged:(int)slider.value];
}

@end
