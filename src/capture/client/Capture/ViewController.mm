
//  AppDelegate.h
//  capture
//
//  Created by Daniel Suo on 6/19/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "ViewController.h"
#include <algorithm>

#warning Bad things happen if record while uploading
#warning Bad things happen if delete while uploading
@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self view].backgroundColor = [UIColor whiteColor];
    
    // Initialize state
    _state = [[State alloc] init];
    
    // Initialize GUI Controller
    _guiController = [[GUIController alloc] init];
    [self.view addSubview:_guiController.view];
    _guiController.guiControllerDelegate = self;
    _guiController.controlsViewShowing = NO;
    
    // Initialize Sensor Controller
    _inputController = [[InputController alloc] init];
    _inputController.inputControllerDelegate = self;
    
    // Initialize IO Controller
    _outputController = [[OutputController alloc] init];
    _outputController.outputControllerDelegate = self;
}

- (void)dealloc {}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [Utilities sendLog:@"LOG: View appeared!"];
    
    [self startInputs];
    
    [self setNeedsStatusBarAppearanceUpdate];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
    return NO;
}

#pragma mark - Helper methods

- (NSString *)metadata
{
    int angle = [[Utilities getSettingsValue:kSettingsDeviceAngle] intValue];
    if (!angle) {
        angle = -1;
    }
    
    NSString *metadata = [NSString stringWithFormat:
                          @"device_type:%@\n" \
                          @"device_id:%@\n" \
                          @"device_angle:%d\n" \
                          "scan_id:%@\n" \
                          "color_exposure_duration:%d,%d\n" \
                          "color_iso:%d\n" \
                          "color_fps:%lld,%d\n" \
                          "color_resolution:%@x%@\n",
                          [SDiPhoneVersion deviceName],
                          [Utilities getSettingsValue:kSettingsDeviceID],
                          angle,
                          [Utilities stringFromDate:_inputController.frameDate],
                          _inputController.color.currentExposureDuration, 1000,
                          _inputController.color.currentISO,
                          _inputController.color.device.activeVideoMinFrameDuration.value, _inputController.color.device.activeVideoMinFrameDuration.timescale,
                          _inputController.color.resolution[@"width"], _inputController.color.resolution[@"height"]
                          ];
    
    return metadata;
}

- (void)initNewScan
{
    [_outputController initNewScan];
    
    NSString *relativePath = [NSString stringWithFormat:@"%@/METADATA", _outputController.currScanDirectory];
    [_outputController writeText:[self metadata] relativePath:relativePath];
    
    [_inputController.gps startUpdatingLocation];
}

- (void)flashScreen
{
    // Flash the screen white and fade it out
    UIView *flashView = [[UIView alloc] initWithFrame:self.view.frame];
    [flashView setBackgroundColor:[UIColor whiteColor]];
    [self.view.window addSubview:flashView];
    
    [UIView animateWithDuration:0.5f
                     animations:^{
                         [flashView setAlpha:0.f];
                     }
                     completion:^(BOOL finished){
                         [flashView removeFromSuperview];
                     }
     ];
}

- (void)closeGracefully
{
    [_outputController stopRecording];
    [_outputController closeWriter];
    [_inputController.color stop];
    [_inputController.structure stop];
    [Utilities letDeviceSleep];
}

- (void)startInputs
{
    [_inputController.color start];
    [_inputController.structure start];
}

#warning Clean up these functions
- (void)setWriteModeTCP
{
    _outputController.writeMode = kWriteModeTCP;
    [_guiController showFileImage:NO];
}

- (void)setWriteModeFile
{
    _outputController.writeMode = kWriteModeFile;
    [_guiController showFileImage:YES];
}

- (void)toggleWriteMode
{
    if ([_outputController.writeMode isEqualToString:kWriteModeFile]) {
        [self setWriteModeTCP];
    } else if ([_outputController.writeMode isEqualToString:kWriteModeTCP]) {
        [self setWriteModeFile];
    }
    
    [Utilities sendStatus:[NSString stringWithFormat:@"INFO: Changed write mode to %@", _outputController.writeMode]];
}

- (void)startRecording
{
    [Utilities keepDeviceAwake];
    [_guiController showRecordImage:NO];
    [_outputController initializeWriter];
    [_outputController startRecording];
    [_inputController.motion startUpdating];
}

- (void)stopRecording
{
    [Utilities letDeviceSleep];
    [_guiController showRecordImage:YES];
    [_outputController stopRecording];
    [_outputController closeWriter];
    [_inputController.motion stopUpdating];
}

# pragma mark - GUIControllerDelegate

- (void)toolbarButtonPressed:(NSString *)buttonName toggleControlsView:(BOOL)toggle formatString:(NSString *)formatString
{
    NSLog(@"INFO: %@ pressed", buttonName);

    if (toggle) {
        if ([_guiController isControlsViewShowing] && [buttonName isEqualToString:_guiController.controlsShowing]) {
            [_guiController hideControls];
            _guiController.controlsViewShowing = NO;
        } else {
            _guiController.controlsShowing = buttonName;
            _guiController.controlsViewShowing = YES;
            
            if ([_guiController.controlsShowing isEqualToString:@"exposureduration"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentExposureDuration min:kColorMinExposureDuration max:kColorMaxExposureDuration];
            } else if ([_guiController.controlsShowing isEqualToString:@"iso"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentISO min:[_inputController.color minISO] max:[_inputController.color maxISO]];
            } else if ([_guiController.controlsShowing isEqualToString:@"fps"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentFPS min:kColorMinFPS max:kColorMaxFPS];
            }
        }
    } else {
        [_guiController hideControls];
        _guiController.controlsViewShowing = NO;
#warning In need of some serious cleanup
        if ([buttonName isEqualToString:@"depth"]) {
            if (_inputController.structure.streamConfig == STStreamConfigDepth640x480) {
                [_guiController showDepthImage:NO];
            } else {
                [_guiController showDepthImage:YES];
            }
            
            [_inputController.structure toggleMode];
        } else if ([buttonName isEqualToString:@"trash"]) {
            [_outputController.fileWriter handleTrash];
        } else if ([buttonName isEqualToString:@"camera"]) {
            [Utilities sendStatus:@"INFO: Frame taken"];
            [Utilities letDeviceSleep];
            [_guiController showRecordImage:YES];
            [_outputController stopRecording];
            [_outputController initializeWriter];
            [_outputController recordOneFrame:[_inputController numActiveSensors]];
            [self flashScreen];
        } else if ([buttonName isEqualToString:@"record"]) {
            if ([_outputController isRecording]) {
                [self stopRecording];
            } else {
                [self startRecording];
            }
        } else if ([buttonName isEqualToString:@"upload"]) {
            [_outputController upload];
        } else if ([buttonName isEqualToString:@"file"]) {
            [self toggleWriteMode];
        }
    }
}

- (void)controlsSliderValueChanged:(int)value
{
    if ([_guiController.controlsShowing isEqualToString:@"exposureduration"]) {
        [_inputController.color setExposureDuration:value];
    } else if ([_guiController.controlsShowing isEqualToString:@"iso"]) {
        [_inputController.color setISO:value];
    } else if ([_guiController.controlsShowing isEqualToString:@"fps"]) {
        [_inputController.color setFPS:value];
    }
}

# pragma mark - InputControllerDelegate

- (void)sensorDidOutputImage:(UIImage *)image type:(NSString *)type
{
    [_guiController renderImage:image type:type];

#warning Stop using UIImage
    NSData *imageData;
    
    if ([type isEqualToString:kFrameTypeColor]) {
        imageData = UIImageJPEGRepresentation(image, 0.8);
    } else {
        imageData = UIImagePNGRepresentation(image);
    }
    
    if (_outputController.writerReady && [_outputController isRecording]) {
        NSLog(@"Received frame!");
        NSString *filename = [NSString stringWithFormat:@"%@-%010lu.%@",
                              [Utilities stringFromDate:_inputController.frameDate],
                              (unsigned long)_inputController.frameIndex,
                              ([type isEqualToString:kFrameTypeColor] ? kExtensionJPG : kExtensionPNG)];
        
        NSString *relativePath = [NSString stringWithFormat:@"%@/%@/%@",
                                  _outputController.currScanDirectory, type, filename];
        
        [_outputController writeData:imageData relativePath:relativePath timestamp:_inputController.frameTimestamp];
        
        [_outputController writeText:[_inputController.motion getData] relativePath:[NSString stringWithFormat:@"%@/MOTION", _outputController.currScanDirectory]];
    }
}

- (void)gpsDidUpdateLocationWithLatitude:(float)lat longitude:(float)lon
{
    NSString *locationData = [NSString stringWithFormat:
                              @"gps_lat:%0.2f\n" \
                              @"gps_lon:%0.2f",
                              _inputController.gps.lat,
                              _inputController.gps.lon
                              ];
    
    [_outputController writeText:locationData relativePath:[NSString stringWithFormat:@"%@/LOCATION", _outputController.currScanDirectory]];
}

- (void)didReceiveTCPCommand:(const uint8_t)command argument:(const uint8_t *)argument length:(NSUInteger)length;
{
    switch ((TCPDeviceCommand)command) {
        case TCPDeviceCommandStartRecording:
            [self startRecording];
            break;
        case TCPDeviceCommandStopRecording:
            [self stopRecording];
            break;
        case TCPDeviceCommandUpload:
            [_outputController upload];
            break;
        case TCPDeviceCommandFileModeTCP:
            [self setWriteModeTCP];
            break;
        case TCPDeviceCommandDimScreen:
            [UIScreen mainScreen].brightness = 0.0;
            break;
        default:
            break;
    }
}

- (void)didChangeCameraSettings:(NSString *)setting value:(float)value
{
    if (_guiController.controlsViewShowing && [_guiController.controlsShowing isEqualToString:setting]) {
        [_guiController updateControls:value];
    }
}

# pragma mark - OutputControllerDelegate

- (void)alertPresentRequested:(UIAlertController *)alert
{
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)writerReady
{
    [Utilities sendLog:@"LOG: Writer ready"];
    [self initNewScan];
}

- (void)writerError:(NSError *)error
{
    [self stopRecording];
}

@end
