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
#warning move these to Utilities?
        // COLOR SENSOR
        _color = [[Color alloc] init];
        _color.delegate = self;
        
        _color.currentExposureDuration = 2;
        _color.currentISO = 300;
        _color.currentFPS = 15;
        _color.lensPosition = 1.0f; // Set focus at the maximum position allowable (e.g., "near-infinity") to get the best color/depth alignment
        
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
        // - 4032x3024 (4:3) iPhone 6S
        _color.resolution = @{ @"width": @(640),
                               @"height": @(480)};
        
        // STRUCTURE SENSOR
        _structure = [[Structure alloc] init];
        _structure.delegate = self;
        
#warning condition on iPad -> registered depth?
        _structure.streamConfig = STStreamConfigRegisteredDepth640x480;
        _structure.colorLensPosition = self.color.lensPosition;
        
        // GPS SENSOR
        _gps = [[GPS alloc] init];
        _gps.gpsDelegate = self;
        
        // MOTION SENSOR
        _motion = [[Motion alloc] init];
        
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
    if ([_color.session isRunning]) {
        numDevices++;
    }
    
    if ([_structure isReady]) {
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
    
    NSString *autoExposeValue = [Utilities getSettingsValue:kSettingsColorAutoExpose];
//    BOOL autoExpose = autoExposeValue != [NSNull null] && [autoExposeValue boolValue];
    BOOL autoExpose = NO;

    if (autoExpose) {
        [self autoAdjustColorSensorSettings:ptr rows:rows cols:cols];
    }
    
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
                                        cols * 4,
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

#pragma mark - Calibrate sensors

- (void)autoAdjustColorSensorSettings:(unsigned char *)ptr rows:(size_t)rows cols:(size_t)cols
{
#warning This could use some refactoring
    // Adjust exposure and gain to try to make image reasonably lit
    // Don't adjust gain every frame
    if (_frameIndex % kColorAutoExposureSampleFramePeriod == 0) {
        //    if (_frameIndex == 10) {
        int numPixelsSampled = 0;
        float totalBrightness = 0;
        for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
            int rowOffset = rowIndex * (int)cols;
            for (int colIndex = 0; colIndex < cols; colIndex++) {
                // Don't sample every pixel
                if (rowIndex % kColorAutoExposureSamplePixelPeriod == 0 && colIndex % kColorAutoExposureSamplePixelPeriod == 0) {
                    
                    int pixelNum = rowOffset + colIndex;
                    
                    // We have four bytes per pixel
                    int dataIndex = pixelNum * 4;
                    
                    numPixelsSampled++;
                    
                    // We receive data in BGRA format and convert using Rec 709 LUMA values for grayscale image conversion
                    totalBrightness += 0.0722 * ptr[dataIndex] + 0.7152 * ptr[dataIndex + 1] + 0.2126 * ptr[dataIndex + 2];
                }
            }
        }
        
        float averageBrightness = totalBrightness / numPixelsSampled;
        NSLog(@"%0.5f", averageBrightness);
        
        float stepRatio = 0.5;
        
        // If average brightness is below our min bound, try to increase available light
        if (averageBrightness < kColorMinBrightness) {
            // First try to increase ISO
            if (_color.currentISO < [_color maxISO]) {
                int deltaToMax = [_color maxISO] - _color.currentISO;
                
                // Increase ISO
                int newISO = _color.currentISO + MIN(deltaToMax, int(deltaToMax * stepRatio) + 1);
                [_color setISO:newISO];
                
                [Utilities sendLog:[NSString stringWithFormat:@"LOG: Increasing ISO to %d", newISO]];
                [_inputControllerDelegate didChangeCameraSettings:@"iso" value:newISO];
            }
            
            // If we can't, increase exposure duration
            else if (_color.currentExposureDuration < kColorMaxExposureDuration) {
                NSLog(@"Can't budge on ISO");
                int deltaToMax = kColorMaxExposureDuration - _color.currentExposureDuration;
                
                int newExposureDuration = _color.currentExposureDuration + MIN(deltaToMax, int(deltaToMax * stepRatio) + 1);
                [_color setExposureDuration:newExposureDuration];
                
                [Utilities sendLog:[NSString stringWithFormat:@"LOG: Increasing exposure duration to %d/1000s", newExposureDuration]];
                [_inputControllerDelegate didChangeCameraSettings:@"exposureduration" value:newExposureDuration];
            }
            
            // Otherwise, don't do anything
            else {
                NSLog(@"Brightness maxed out");
            }
        }
        
        // If average brightness is above our min bound, try to decrease available light
        else if (averageBrightness > kColorMaxBrightness) {
            // First make sure we bring down shutter speed
            if (_color.currentExposureDuration > kColorMinExposureDuration) {
                int deltaToMin = _color.currentExposureDuration - kColorMinExposureDuration;
                
                int newExposureDuration = _color.currentExposureDuration - MIN(deltaToMin, int(deltaToMin * stepRatio) + 1);
                [_color setExposureDuration:newExposureDuration];
                
                [Utilities sendLog:[NSString stringWithFormat:@"LOG: Decreasing exposure duration to %d/1000s", newExposureDuration]];
                [_inputControllerDelegate didChangeCameraSettings:@"exposureduration" value:newExposureDuration];
            }
            
            // If needed, reduce ISO
            else if (_color.currentISO > [_color minISO]) {
                int deltaToMin = _color.currentISO - [_color minISO];
                
                // Decrease ISO
                int newISO = _color.currentISO - MIN(deltaToMin, int(deltaToMin * stepRatio) + 1);
                [_color setISO:newISO];
                
                [Utilities sendLog:[NSString stringWithFormat:@"LOG: Decreasing ISO to %d", newISO]];
                [_inputControllerDelegate didChangeCameraSettings:@"iso" value:newISO];
            }
            
            // Otherwise, don't do anything
            else {
                NSLog(@"Brightness mined out");
            }
        }
    }
}

#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    // Pass into the driver. The sampleBuffer will return later with a synchronized depth or IR pair.
    if (_structure.status == SensorStatusOk) {
        [_structure.sensor frameSyncNewColorBuffer:sampleBuffer];
    } else {
        _frameIndex++;
        _frameDate = [NSDate date];
        _frameTimestamp = [Utilities getMachAbsoluteTime];
        [self processColorFrame:sampleBuffer];
    }
}

#pragma mark - STSensorControllerDelegate

- (void)sensorDidDisconnect
{
    [Utilities sendStatus:@"INFO: Structure Sensor disconnected!"];
    _structure.status = SensorStatusNeedsUserToConnect;
    [_structure stop];
}

- (void)sensorDidConnect
{
    [Utilities sendStatus:@"INFO: Structure Sensor connected!"];
    [_structure reset];
}

- (void)sensorDidLeaveLowPowerMode
{
    [Utilities sendWarning:@"WARN: Structure Sensor powered!"];
    _structure.status = SensorStatusNeedsUserToConnect;
}

#warning This isn't working
- (void)sensorBatteryNeedsCharging
{
    [Utilities sendWarning:@"WARN: Structure Sensor needs charging!"];
    _structure.status = SensorStatusNeedsUserToCharge;
}

- (void)sensorDidStopStreaming:(STSensorControllerDidStopStreamingReason)reason {}

- (void)sensorDidOutputDepthFrame:(STDepthFrame *)depthFrame {}

- (void)sensorDidOutputSynchronizedDepthFrame:(STDepthFrame *)depthFrame
                                andColorFrame:(STColorFrame *)colorFrame
{
    _frameIndex++;
    _frameDate = [NSDate date];
    _frameTimestamp = depthFrame.timestamp * 1000;
    
    [self processColorFrame:colorFrame.sampleBuffer];
    [self processDepthFrame:depthFrame];
}

- (void)sensorDidOutputSynchronizedInfraredFrame:(STInfraredFrame *)infraredFrame
                                   andColorFrame:(STColorFrame *)colorFrame
{
    _frameIndex++;
    _frameDate = [NSDate date];
    _frameTimestamp = infraredFrame.timestamp * 1000;
    
    [self processColorFrame:colorFrame.sampleBuffer];
    [self processInfraredFrame:infraredFrame];
}

#pragma mark - GPSDelegate

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *currentLocation = [locations lastObject];
    
    if (currentLocation != nil) {
        _gps.lat = currentLocation.coordinate.latitude;
        _gps.lon = currentLocation.coordinate.longitude;
        
        [_inputControllerDelegate gpsDidUpdateLocationWithLatitude:_gps.lat longitude:_gps.lon];
    }
}

#pragma mark - TCPDelegate
- (void)didReceiveTCPCommand:(const uint8_t)command argument:(const uint8_t *)argument length:(NSUInteger)length;
{
    [_inputControllerDelegate didReceiveTCPCommand:command argument:argument length:length];
}

@end
