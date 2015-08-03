//
//  Color.h
//  Capture
//
//  Created by Daniel Suo on 6/24/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "Utilities.h"

@interface Color : NSObject

@property (nonatomic, strong) id delegate;

@property (nonatomic, strong) AVCaptureSession *session;
@property (nonatomic, strong) AVCaptureDevice *device;

@property (nonatomic) BOOL authorized;
@property (nonatomic, strong) NSDictionary *resolution;
@property (nonatomic) float lensPosition;

@property (nonatomic) int currentExposureDuration;
@property (nonatomic) int currentISO;
@property (nonatomic) int currentFPS;

- (void)setup;
- (void)start;
- (void)stop;
- (void)reset;

- (void)setExposureDuration:(int)ms;
- (void)setISO:(int)ISO;
- (void)setFPS:(int)FPS;

- (float)minISO;
- (float)maxISO;

@end
