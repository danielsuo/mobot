//
//  GPS.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

@interface GPS : NSObject <CLLocationManagerDelegate>

@property (nonatomic, strong) id delegate;

@property (nonatomic) float lat;
@property (nonatomic) float lon;

- (void)startUpdatingLocation;
- (void)stopUpdatingLocation;

@end
