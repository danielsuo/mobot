//
//  Motion.h
//  Capture
//
//  Created by Daniel Suo on 9/16/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>

#import "Utilities.h"

@interface Motion : NSObject

@property (nonatomic, strong) CMMotionManager *motionManager;

- (void)startUpdating;
- (void)stopUpdating;
- (NSData *)getData;

@end
