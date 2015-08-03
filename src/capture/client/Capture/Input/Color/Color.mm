//
//  Color.mm
//  Capture
//
//  Created by Daniel Suo on 6/24/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Color.h"

@implementation Color

#pragma mark - Public methods

- (void)setup
{
    // If already setup, skip it
    if (_session)
        return;
    
    self.authorized = [self queryCameraAuthorizationStatusAndNotifyUserIfNotGranted];
    
    if (!self.authorized)
    {
        return;
    }
    
    // Set up Capture Session.
    _session = [[AVCaptureSession alloc] init];
    [_session beginConfiguration];
    
    // InputPriority allows us to select a more precise format (below)
    [_session setSessionPreset:AVCaptureSessionPresetInputPriority];
    
    // Create a video device and input from that Device.  Add the input to the capture session.
    _device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if (_device == nil)
        assert(0);
    
    // Configure Focus, Exposure, and White Balance
    NSError *error;
    
    // iOS8 supports manual focus at near-infinity, but iOS7 doesn't.
    bool avCaptureSupportsManualFocus = [_device respondsToSelector:@selector(setFocusModeLockedWithLensPosition:completionHandler:)];
    
    NSAssert(avCaptureSupportsManualFocus, @"This sample now requires iOS 8 or above (for manual focus).");
    
    // Use auto-exposure, and auto-white balance and set the focus to infinity.
    if([_device lockForConfiguration:&error])
    {
        
        // Select capture format
        [self selectCaptureFormat:self.resolution];
        
        // Allow exposure to change
        if ([_device isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure])
            [_device setExposureMode:AVCaptureExposureModeContinuousAutoExposure];
        
        // Allow white balance to change
        if ([_device isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance])
            [_device setWhiteBalanceMode:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance];
        
        // Apply to specified focus position.
        [_device setFocusModeLockedWithLensPosition:self.lensPosition completionHandler:nil];
        
        [_device unlockForConfiguration];
    }
    
    //  Add the device to the session.
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:_device error:&error];
    if (error)
    {
        [Utilities sendWarning:@"WARN: Cannot initialize AVCaptureDeviceInput"];
        assert(0);
    }
    
    [_session addInput:input]; // After this point, captureSession captureOptions are filled.
    
    //  Create the output for the capture session.
    AVCaptureVideoDataOutput* dataOutput = [[AVCaptureVideoDataOutput alloc] init];
    
    // We don't want to process late frames.
    [dataOutput setAlwaysDiscardsLateVideoFrames:YES];
    
    // Use BGRA pixel format.
    [dataOutput setVideoSettings:[NSDictionary
                                  dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA]
                                  forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    
    // Set dispatch to be on the main thread so OpenGL can do things with the data
    [dataOutput setSampleBufferDelegate:_delegate queue:dispatch_get_main_queue()];
    
    [_session addOutput:dataOutput];
    
    [self setExposureDuration:self.currentExposureDuration];
    [self setISO:self.currentISO];
    [self setFPS:self.currentFPS];
    
    [_session commitConfiguration];
}

- (void)start
{
    [Utilities sendLog:@"LOG: Starting color camera..."];
    
    if (_session && [_session isRunning])
        return;
    
    // Re-setup so focus is lock even when back from background
    if (_session == nil)
        [self setup];
    
    // Start streaming color images.
    [_session startRunning];
    [Utilities sendStatus:@"INFO: Color camera started!"];
}

- (void)stop
{
    if ([_session isRunning])
    {
        // Stop the session
        [_session stopRunning];
    }
    
    _session = nil;
    _device = nil;
}

- (void)reset
{
    [self stop];
    [self start];
}

- (void)setExposureDuration:(int)ms
{
    if([_device lockForConfiguration:nil])
    {
        [_device setExposureModeCustomWithDuration:CMTimeMake(ms, 1000) ISO:AVCaptureISOCurrent completionHandler:nil];
        self.currentExposureDuration = ms;
        [_device unlockForConfiguration];
    }
}

- (void)setISO:(int)ISO
{
    if([_device lockForConfiguration:nil])
    {
        [_device setExposureModeCustomWithDuration:AVCaptureExposureDurationCurrent ISO:ISO completionHandler:nil];
        self.currentISO = ISO;
        [_device unlockForConfiguration];
    }
}

- (void)setFPS:(int)FPS
{
    if([_device lockForConfiguration:nil])
    {
        CMTime targetFrameDuration = CMTimeMake(1, FPS);
        
        [_device setActiveVideoMaxFrameDuration:targetFrameDuration];
        [_device setActiveVideoMinFrameDuration:targetFrameDuration];
        
        self.currentFPS = FPS;
        
        [_device unlockForConfiguration];
    }
}

- (float)minISO
{
    return _device.activeFormat.minISO;
}

- (float)maxISO
{
    return _device.activeFormat.maxISO;
}

#warning refactor this for metadata, among other things
- (NSString *)getCameraSettings
{
    NSLog(@"Camera's FOV is %f", _device.activeFormat.videoFieldOfView);
    NSLog(@"Current frame rate: min: %lld, %d; max: %lld, %d",
          _device.activeVideoMinFrameDuration.value,
          _device.activeVideoMinFrameDuration.timescale,
          _device.activeVideoMaxFrameDuration.value,
          _device.activeVideoMaxFrameDuration.timescale);
    NSLog(@"   minexp: %lld, %d; maxexp: %lld, %d",
          _device.activeFormat.minExposureDuration.value,
          _device.activeFormat.minExposureDuration.timescale,
          _device.activeFormat.maxExposureDuration.value,
          _device.activeFormat.maxExposureDuration.timescale);
    NSLog(@"   currexp: %lld, %d",
          AVCaptureExposureDurationCurrent.value,
          AVCaptureExposureDurationCurrent.timescale);
    NSLog(@"   miniso: %f, maxiso: %f", _device.activeFormat.minISO, _device.activeFormat.maxISO);
    
    return @"";
}

- (BOOL)queryCameraAuthorizationStatusAndNotifyUserIfNotGranted
{
    // This API was introduced in iOS 7, but in iOS 8 it's actually enforced.
    if ([AVCaptureDevice respondsToSelector:@selector(authorizationStatusForMediaType:)])
    {
        AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
        
        if (authStatus != AVAuthorizationStatusAuthorized)
        {
            [Utilities sendWarning:@"WARN: Not authorized to use the camera!"];
            
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo
                                     completionHandler:^(BOOL granted)
             {
                 // This block fires on a separate thread, so we need to ensure any actions here
                 // are sent to the right place.
                 
                 // If the request is granted, let's try again to start an AVFoundation session. Otherwise, alert
                 // the user that things won't go well.
                 if (granted)
                 {
                     
                     dispatch_async(dispatch_get_main_queue(), ^(void) {
                         
                         [self start];
                         
                         self.authorized = true;
                         //                         [self updateAppStatusMessage];
                     });
                 }
             }];
            return false;
        }
    }
    return true;
}

#pragma mark - Private methods

- (void)selectCaptureFormat:(NSDictionary*)demandFormat
{
    AVCaptureDeviceFormat * selectedFormat = nil;
    
    for (AVCaptureDeviceFormat* format in _device.formats)
    {
//        [Utilities sendLog:[NSString stringWithFormat:@"LOG: Available format: %@", format]];
        double formatMaxFps = ((AVFrameRateRange *)[format.videoSupportedFrameRateRanges objectAtIndex:0]).maxFrameRate;
        
        CMFormatDescriptionRef formatDesc = format.formatDescription;
        FourCharCode fourCharCode = CMFormatDescriptionGetMediaSubType(formatDesc);
        
        CMVideoFormatDescriptionRef videoFormatDesc = formatDesc;
        CMVideoDimensions formatDims = CMVideoFormatDescriptionGetDimensions(videoFormatDesc);
        
        NSNumber *widthNeeded  = demandFormat[@"width"];
        NSNumber *heightNeeded = demandFormat[@"height"];
        
        if ( widthNeeded && widthNeeded.intValue!= formatDims.width )
            continue;
        
        if( heightNeeded && heightNeeded.intValue != formatDims.height )
            continue;
        
        // we only support full range YCbCr for now
        if(fourCharCode != (FourCharCode)'420f')
            continue;
        
        selectedFormat = format;
        break;
    }
    _device.activeFormat = selectedFormat;
}

@end
