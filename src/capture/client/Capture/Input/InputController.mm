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
        
        // Possible values
        // - 192x144 (4:3)
        // - 352x288 (11:9)
        // - 480x360 (4:3)
        // - 640x480 (4:3)
        // - 960x540 (16:9)
        // - 1280x720 (16:9)
        // - 1920x1080 (16:9)
        // - 2592x1936 (162:121 ~4:3)
        // - 3264x2448 (4:3)
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

#pragma mark - Raw data to image conversion
- (void)processColorFrame:(CMSampleBufferRef)sampleBuffer
{
    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    size_t cols = CVPixelBufferGetWidth(pixelBuffer);
    size_t rows = CVPixelBufferGetHeight(pixelBuffer);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    unsigned char *ptr = (unsigned char *) CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    
    NSData *data = [[NSData alloc] initWithBytes:ptr length:rows*cols*4];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipFirst;
    bitmapInfo |= kCGBitmapByteOrder32Little;
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data);
    
    CGImageRef imageRef = CGImageCreate(cols,
                                        rows,
                                        8,
                                        8 * 4,
                                        cols*4,
                                        colorSpace,
                                        bitmapInfo,
                                        provider,
                                        NULL,
                                        false,
                                        kCGRenderingIntentDefault);
    
    [_inputControllerDelegate sensorDidOutputImage:[[UIImage alloc] initWithCGImage:imageRef] type:kFrameTypeColor];
    
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
}

- (void)processDepthFrame:(STDepthFrame *)depthFrame
{
    size_t cols = depthFrame.width;
    size_t rows = depthFrame.height;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipLast;
    bitmapInfo |= kCGBitmapByteOrder16Little;
    
    size_t numPixels = cols * rows;
    uint16_t *depthData = (uint16_t *)malloc(numPixels * sizeof(uint16_t));
    
    for (int i = 0; i < numPixels; i++) {
        depthData[i] = (uint16_t)(depthFrame.depthInMillimeters[i]) << 3;
    }
    
    depthFrame = nil;
    
    NSData *data = [NSData dataWithBytes:depthData length:cols * rows * 2];
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data); //toll-free ARC bridging
    
    CGImageRef imageRef = CGImageCreate(cols,                        //width
                                        rows,                        //height
                                        16,                          //bits per component
                                        16,                          //bits per pixel
                                        cols * 2,                    //bytes per row
                                        colorSpace,                  //Quartz color space
                                        bitmapInfo,                  //Bitmap info (alpha channel?, order, etc)
                                        provider,                    //Source of data for bitmap
                                        NULL,                        //decode
                                        false,                       //pixel interpolation
                                        kCGRenderingIntentDefault);  //rendering intent
    
    [_inputControllerDelegate sensorDidOutputImage:[[UIImage alloc] initWithCGImage:imageRef] type:kFrameTypeDepth];
    
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    free(depthData);
}

- (void)processInfraredFrame:(STInfraredFrame *)infraredFrame
{
    size_t cols = infraredFrame.width;
    size_t rows = infraredFrame.height;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipLast;
    bitmapInfo |= kCGBitmapByteOrder16Big;
    
    NSData *data = [NSData dataWithBytes:infraredFrame.data length:cols * rows * 2];
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data); //toll-free ARC bridging
    
    CGImageRef imageRef = CGImageCreate(cols,                        //width
                                        rows,                        //height
                                        16,                          //bits per component
                                        16,                          //bits per pixel
                                        cols * 2,                    //bytes per row
                                        colorSpace,                  //Quartz color space
                                        bitmapInfo,                  //Bitmap info (alpha channel?, order, etc)
                                        provider,                    //Source of data for bitmap
                                        NULL,                        //decode
                                        false,                       //pixel interpolation
                                        kCGRenderingIntentDefault);  //rendering intent
    
    [_inputControllerDelegate sensorDidOutputImage:[[UIImage alloc] initWithCGImage:imageRef] type:kFrameTypeInfrared];

    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
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
        [self processColorFrame:sampleBuffer];
    }
}

#pragma mark - STSensorControllerDelegate

- (void)sensorDidDisconnect
{
    [Utilities sendStatus:@"INFO: Structure Sensor disconnected!"];
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
    [Utilities sendWarning:@"WARN: Structure Sensor powered!"];
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
    
    [self processColorFrame:colorFrame.sampleBuffer];
    [self processDepthFrame:depthFrame];
}

- (void)sensorDidOutputSynchronizedInfraredFrame:(STInfraredFrame *)infraredFrame
                                   andColorFrame:(STColorFrame *)colorFrame
{
    self.frameIndex++;
    self.frameTimestamp = [NSDate date];

    [self processColorFrame:colorFrame.sampleBuffer];
    [self processInfraredFrame:infraredFrame];
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
