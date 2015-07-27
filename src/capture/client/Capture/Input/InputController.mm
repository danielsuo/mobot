//
//  InputController.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "InputController.h"

@implementation InputController

- (InputController *)init
{
    self = [super init];
    
    if (self) {
        // COLOR SENSOR
        _color = [[Color alloc] init];
        _color.delegate = self;
        
        _color.currentShutterSpeed = 2;
        _color.currentISO = 544;
        _color.currentFPS = 15;
        _color.lensPosition = 0.75;
        _color.resolution = @{ @"width": @(640),
                                   @"height": @(480)};
        
        // STRUCTURE SENSOR
        _structure = [[Structure alloc] init];
        _structure.delegate = self;
        
        #warning condition on iPad -> registered depth?
        _structure.streamConfig = STStreamConfigDepth640x480;
        _structure.colorLensPosition = self.color.lensPosition;
        
        // GPS SENSOR
        _gps = [[GPS alloc] init];
        _gps.gpsDelegate = self;
        
        // Initialize indices
        _frameIndex = 0;
        
        // TCP SERVER
        _tcp = [[TCP alloc] init];
        _tcp.tcpDelegate = self;
        [_tcp start:nil];
    }
    
    return self;
}

- (int)numActiveSensors
{
    int numDevices = 0;
    if ([self.color.session isRunning]) {
        numDevices++;
    }
    
    if ([self.structure isReady]) {
        numDevices++;
    }
    
    return numDevices;
}

#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    // Pass into the driver. The sampleBuffer will return later with a synchronized depth or IR pair.
    if (self.structure.status == SensorStatusOk) {
        [self.structure.sensor frameSyncNewColorBuffer:sampleBuffer];
    } else {
        self.frameIndex++;
        self.frameTimestamp = [NSDate date];
        [self.inputControllerDelegate sensorDidOutputColorFrame:sampleBuffer depthFrame:nil infraredFrame:nil];
    }
}

#pragma mark - STSensorControllerDelegate

- (void)sensorDidDisconnect
{
    [Utilities sendWarning:@"INFO: Structure Sensor disconnected!"];
    self.structure.status = SensorStatusNeedsUserToConnect;
    [self.structure stop];
}

- (void)sensorDidConnect
{
    [Utilities sendStatus:@"INFO: Structure Sensor connected!"];
    [self.structure reset];
}

- (void)sensorDidLeaveLowPowerMode
{
    [Utilities sendWarning:@"INFO: Structure Sensor powered!"];
    self.structure.status = SensorStatusNeedsUserToConnect;
}

#warning This isn't working
- (void)sensorBatteryNeedsCharging
{
    [Utilities sendWarning:@"WARN: Structure Sensor needs charging!"];
    self.structure.status = SensorStatusNeedsUserToCharge;
}

- (void)sensorDidStopStreaming:(STSensorControllerDidStopStreamingReason)reason {}

- (void)sensorDidOutputDepthFrame:(STDepthFrame *)depthFrame {}

- (void)sensorDidOutputSynchronizedDepthFrame:(STDepthFrame *)depthFrame
                                andColorFrame:(STColorFrame *)colorFrame
{
    self.frameIndex++;
    self.frameTimestamp = [NSDate date];
    [self.inputControllerDelegate sensorDidOutputColorFrame:colorFrame.sampleBuffer depthFrame:depthFrame infraredFrame:nil];
}

- (void)sensorDidOutputSynchronizedInfraredFrame:(STInfraredFrame *)infraredFrame
                                   andColorFrame:(STColorFrame *)colorFrame
{
    self.frameIndex++;
    self.frameTimestamp = [NSDate date];
    [self.inputControllerDelegate sensorDidOutputColorFrame:colorFrame.sampleBuffer depthFrame:nil infraredFrame:infraredFrame];
}

#pragma mark - GPSDelegate

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *currentLocation = [locations lastObject];
    
    if (currentLocation != nil) {
        self.gps.lat = currentLocation.coordinate.longitude;
        self.gps.lon = currentLocation.coordinate.latitude;
        
        [self.inputControllerDelegate gpsDidUpdateLocationWithLatitude:self.gps.lat longitude:self.gps.lon];
    }
}

#pragma mark - TCPDelegate
- (void)handleNewConnectionFromAddress:(NSData *)addr inputStream:(NSInputStream *)istream outputStream:(NSOutputStream *)ostream
{
    NSLog(@"Handling new connection from InputController...");
}

@end
