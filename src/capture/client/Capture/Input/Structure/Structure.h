//
//  Structure.h
//  Capture
//
//  Created by Daniel Suo on 6/24/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#define HAS_LIBCXX
#import <Structure/Structure.h>
#import <Structure/StructureSLAM.h>

#import "Utilities.h"

enum SensorStatus
{
    SensorStatusOk,
    SensorStatusNeedsUserToConnect,
    SensorStatusNeedsUserToCharge,
};

@interface Structure : NSObject

@property (nonatomic, strong) id delegate;

@property (nonatomic, strong) STSensorController *sensor;

@property (nonatomic) STStreamConfig streamConfig;
@property (nonatomic) STFrameSyncConfig syncConfig;
@property (nonatomic) enum SensorStatus status;

@property (nonatomic) float colorLensPosition;

- (void)setup;
- (void)start;
- (void)stop;
- (BOOL)isReady;

- (void)reset;
- (void)toggleMode;

@end