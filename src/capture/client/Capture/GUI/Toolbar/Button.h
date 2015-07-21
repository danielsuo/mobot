//
//  Button.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Button : UIBarButtonItem

@property (nonatomic, strong) NSString *name;
@property (nonatomic) BOOL hasControlsView;
@property (nonatomic, strong) NSString *formatString;

- (id)initWithName:(NSString *)name target:(id)target action:(SEL)selector hasControlsView:(BOOL)hasControlsView formatString:(NSString *)formatString;

@end
