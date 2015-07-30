//
//  Structure.mm
//  Capture
//
//  Created by Daniel Suo on 6/24/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Structure.h"

@implementation Structure

- (void)setup
{
    // Get the sensor controller singleton
    _sensor = [STSensorController sharedController];
    
    // Set ourself as the delegate to receive sensor data.
    _sensor.delegate = _delegate;
    
    [Utilities sendLog:@"LOG: Connecting to Structure Sensor..."];
    STSensorControllerInitStatus result = [_sensor initializeSensorConnection];
    
    BOOL didSucceed = (result == STSensorControllerInitStatusSuccess || result == STSensorControllerInitStatusAlreadyInitialized);
    if (didSucceed)
    {
        _status = SensorStatusOk;
        [Utilities sendStatus:@"INFO: Structure Sensor started"];
    }
    else
    {
        if (result == STSensorControllerInitStatusSensorNotFound)
            [Utilities sendWarning:@"WARN: No Structure Sensor found!"];
        else if (result == STSensorControllerInitStatusOpenFailed)
            [Utilities sendWarning:@"WARN: Structure Sensor failed to open!"];
        else if (result == STSensorControllerInitStatusSensorIsWakingUp)
            [Utilities sendWarning:@"WARN: Structure Sensor low power!"];
        else if (result != STSensorControllerInitStatusSuccess)
            [Utilities sendWarning:[NSString stringWithFormat:@"Structure Sensor failed with status %d", (int)result]];
        
        _status = SensorStatusNeedsUserToConnect;
    }
}

- (BOOL)isReady
{
    return [_sensor isConnected] && ![_sensor isLowPower] && _status == SensorStatusOk;
}

- (void)start
{
    [self setup];
    
    if ([self isReady]) {
        [Utilities sendLog:@"LOG: Starting structure sensor..."];
        
        // Request that we receive depth frames with synchronized color pairs
        // After this call, we will start to receive frames through the delegate methods
        NSError* error = nil;
        
        self.syncConfig = STFrameSyncDepthAndRgb;
        if (self.streamConfig == STStreamConfigInfrared640x488) {
            self.syncConfig = STFrameSyncInfraredAndRgb;
        }
        BOOL optionsAreValid = [_sensor startStreamingWithOptions:@{kSTStreamConfigKey : @(self.streamConfig),
                                                                    kSTFrameSyncConfigKey : @(self.syncConfig),
                                                                    kSTColorCameraFixedLensPositionKey: @(self.colorLensPosition)}
                                                            error:&error];
        if (!optionsAreValid)
        {
            [Utilities sendWarning:[NSString stringWithFormat:@"Error during streaming start: %s", [[error localizedDescription] UTF8String]]];
        }
    }
}

- (void)stop
{
    [_sensor stopStreaming];
}

- (void)reset
{
    [self stop];
    [self start];
}

- (void)toggleMode
{
    if (self.streamConfig == STStreamConfigInfrared640x488) {
        if (IS_IPHONE) {
            self.streamConfig = STStreamConfigDepth640x480;
        } else {
            self.streamConfig = STStreamConfigRegisteredDepth640x480;
        }
    } else {
        self.streamConfig = STStreamConfigInfrared640x488;
    }
    
    [self reset];
}

@end
