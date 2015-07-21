//
//  AppDelegate.h
//  capture
//
//  Created by Daniel Suo on 6/19/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <SDiPhoneVersion/SDiPhoneVersion.h>

#import "State.h"
#import "Utilities.h"

#import "GUIController.h"
#import "InputController.h"
#import "OutputController.h"

@interface ViewController : UIViewController <GUIControllerDelegate, InputControllerDelegate, OutputControllerDelegate>

@property (nonatomic, strong) State *state;
@property (nonatomic, strong) GUIController *guiController;
@property (nonatomic, strong) InputController *inputController;
@property (nonatomic, strong) OutputController *outputController;

- (void)closeGracefully;

@end