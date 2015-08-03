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
#import "GPS.h"
#import "TCP.h"

@protocol InputControllerDelegate <NSObject>

@required
- (void)sensorDidOutputImage:(UIImage *)image type:(NSString *)type;
- (void)gpsDidUpdateLocationWithLatitude:(float)lat longitude:(float)lon;
- (void)didReceiveTCPCommand:(NSString *)command;

@end

@interface InputController : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate, STSensorControllerDelegate, GPSDelegate, TCPDelegate>

@property (nonatomic, strong) id <InputControllerDelegate> inputControllerDelegate;

// Sensors
@property (nonatomic, strong) Color *color;
@property (nonatomic, strong) Structure *structure;
@property (nonatomic, strong) GPS *gps;

// TCP Server
@property (nonatomic, strong) TCP *tcp;

// Recording state
@property (nonatomic) NSUInteger frameIndex;
#warning  think about GMT?
@property (nonatomic, strong) NSDate *frameTimestamp;

- (int)numActiveSensors;

@end
