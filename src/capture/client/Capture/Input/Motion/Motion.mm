//
//  Motion.mm
//  Capture
//
//  Created by Daniel Suo on 9/16/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Motion.h"

@implementation Motion

- (id)init
{
    self = [super init];
    
    if (self) {
        _motionManager = [[CMMotionManager alloc] init];
        _motionManager.deviceMotionUpdateInterval = 0.01;
    }
    
    return self;
}

- (void)startUpdating
{
    [_motionManager startDeviceMotionUpdates];
}

- (void)stopUpdating
{
    [_motionManager stopDeviceMotionUpdates];
}

- (NSString *)getData
{
    NSMutableString *result = [[NSMutableString alloc] init];
    
    // Get attitude i.e., device orientation (CMAttitudeReferenceFrameXArbitraryZVertical describes
    // a device laying flat (vertical Z-axis) with an "arbitrary" X-axis.
    // In practice, the X-axis is fixed to the orientation of the device
    // when you first start device motion updates).
    [result appendFormat:@"%f,%f,%f,%f;",
     _motionManager.deviceMotion.attitude.quaternion.w,
     _motionManager.deviceMotion.attitude.quaternion.x,
     _motionManager.deviceMotion.attitude.quaternion.y,
     _motionManager.deviceMotion.attitude.quaternion.z];
    
    // Get rotation rate (radians / sec)
    [result appendFormat:@"%f,%f,%f;",
     _motionManager.deviceMotion.rotationRate.x,
     _motionManager.deviceMotion.rotationRate.y,
     _motionManager.deviceMotion.rotationRate.z];
    
    // Get gravity
    [result appendFormat:@"%f,%f,%f;",
     _motionManager.deviceMotion.gravity.x,
     _motionManager.deviceMotion.gravity.y,
     _motionManager.deviceMotion.gravity.z];
    
    // Get user acceleration
    [result appendFormat:@"%f,%f,%f",
     _motionManager.deviceMotion.userAcceleration.x,
     _motionManager.deviceMotion.userAcceleration.y,
     _motionManager.deviceMotion.userAcceleration.z];
    
    if (_motionManager.deviceMotion.magneticField.accuracy != CMMagneticFieldCalibrationAccuracyUncalibrated) {
        [result appendFormat:@";%f,%f,%f",
         _motionManager.deviceMotion.magneticField.field.x,
         _motionManager.deviceMotion.magneticField.field.y,
         _motionManager.deviceMotion.magneticField.field.z];
    }
    
    [result appendString:@"\n"];
    
    return result;
}

@end