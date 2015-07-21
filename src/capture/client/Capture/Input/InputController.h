//
//  InputController.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Utilities.h"
#import "Color.h"
#import "Structure.h"
#import "GPS/GPS.h"

@protocol InputControllerDelegate <NSObject>

@required
- (void)sensorDidOutputColorFrame:(CMSampleBufferRef)colorFrame depthFrame:(STDepthFrame *)depthFrame infraredFrame:(STInfraredFrame *)infraredFrame;
- (void)gpsDidUpdateLocationWithLatitude:(float)lat longitude:(float)lon;

@end

@interface InputController : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate, STSensorControllerDelegate, CLLocationManagerDelegate>

@property (nonatomic, strong) id <InputControllerDelegate> inputControllerDelegate;

// Sensors
@property (nonatomic, strong) Color *color;
@property (nonatomic, strong) Structure *structure;
@property (nonatomic, strong) GPS *gps;

// Recording state
@property (nonatomic) NSUInteger frameIndex;
@property (nonatomic, strong) NSDate *frameTimestamp;

- (int)numActiveSensors;

@end
