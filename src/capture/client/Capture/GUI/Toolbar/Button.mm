//
//  Button.m
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Button.h"

@implementation Button

- (id)initWithName:(NSString *)name target:(id)target action:(SEL)selector hasControlsView:(BOOL)hasControlsView formatString:(NSString *)formatString
{
    self = [super init];
    
    if (self) {
        self.title = name;
        self.image = [UIImage imageNamed:name];
        self.target = target;
        self.action = selector;
        self.hasControlsView = hasControlsView;
        self.formatString = formatString;
    }
    
    return self;
}

@end
