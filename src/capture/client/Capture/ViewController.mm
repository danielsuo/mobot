
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
    
    [_inputController.color reset];
    [_inputController.structure reset];
    
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
    NSString *metadata = [NSString stringWithFormat:
                          @"device_type:%@\n" \
                          @"device_id:%@\n" \
                          "scan_id:%@\n" \
                          "color_exposure_duration:%d,%d\n" \
                          "color_iso:%d\n" \
                          "color_fps:%lld,%d\n" \
                          "color_resolution:%@x%@\n",
                          [SDiPhoneVersion deviceName],
                          [Utilities getSettingsValue:kSettingsDeviceID],
                          [Utilities stringFromDate:_inputController.frameTimestamp],
                          _inputController.color.currentShutterSpeed, 1000,
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
            
            if ([_guiController.controlsShowing isEqualToString:@"shutterspeed"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentShutterSpeed min:1 max:500];
            } else if ([_guiController.controlsShowing isEqualToString:@"iso"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentISO min:[_inputController.color minISO] max:[_inputController.color maxISO]];
            } else if ([_guiController.controlsShowing isEqualToString:@"fps"]) {
                [_guiController showControls:formatString value:(float)_inputController.color.currentFPS min:2 max:60];
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
                [Utilities letDeviceSleep];
                [_guiController showRecordImage:YES];
                [_outputController stopRecording];
                [_outputController closeWriter];
            } else {
                [Utilities keepDeviceAwake];
                [_guiController showRecordImage:NO];
                [_outputController initializeWriter];
                [_outputController startRecording];
            }
        } else if ([buttonName isEqualToString:@"upload"]) {
            [_outputController upload];
        } else if ([buttonName isEqualToString:@"file"]) {
            if ([_outputController.writeMode isEqualToString:kWriteModeFile]) {
                _outputController.writeMode = kWriteModeTCP;
                [_guiController showFileImage:NO];
            } else if ([_outputController.writeMode isEqualToString:kWriteModeTCP]) {
                _outputController.writeMode = kWriteModeFile;
                [_guiController showFileImage:YES];
            }
            [Utilities sendStatus:[NSString stringWithFormat:@"INFO: Changed write mode to %@", _outputController.writeMode]];
        }
    }
}

- (void)controlsSliderValueChanged:(int)value
{
    if ([_guiController.controlsShowing isEqualToString:@"shutterspeed"]) {
        [_inputController.color setShutterSpeed:value];
    } else if ([_guiController.controlsShowing isEqualToString:@"iso"]) {
        [_inputController.color setISO:value];
    } else if ([_guiController.controlsShowing isEqualToString:@"fps"]) {
        [_inputController.color setFPS:value];
    }
}

- (void)frameReadyToRecord:(NSData *)data withType:(NSString *)type
{
    if (_outputController.writerReady && [_outputController isRecording]) {
        NSString *filename = [NSString stringWithFormat:@"%@-%010lu.%@",
                              [Utilities stringFromDate:_inputController.frameTimestamp],
                              _inputController.frameIndex,
                              ([type isEqualToString:kFrameTypeColor] ? kExtensionJPG : kExtensionPNG)];

        NSString *relativePath = [NSString stringWithFormat:@"%@/%@/%@",
                                  _outputController.currScanDirectory, type, filename];
        
        [_outputController writeData:data relativePath:relativePath];
    }
}

# pragma mark - SensorControllerDelegate

- (void)sensorDidOutputColorFrame:(CMSampleBufferRef)colorFrame depthFrame:(STDepthFrame *)depthFrame infraredFrame:(STInfraredFrame *)infraredFrame
{
    [_guiController renderColorFrame:colorFrame depthFrame:depthFrame infraredFrame:infraredFrame];
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
    
}

@end
