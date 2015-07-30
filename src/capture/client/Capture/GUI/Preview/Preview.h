//
//  Preview.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#define HAS_LIBCXX
#import <Structure/Structure.h>

#import "Utilities.h"

@interface Preview : UIView

- (Preview *)initWithFrame:(CGRect)frame;
- (void)renderImage:(UIImage *)image type:(NSString *)type;

@end
