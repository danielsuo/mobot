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

#import "Utilities.h"

@protocol GPSDelegate <NSObject>

@required
- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations;

@end

@interface GPS : NSObject

@property (nonatomic, strong) id <GPSDelegate> gpsDelegate;

@property (nonatomic, strong) CLLocationManager *locationManager;
@property (nonatomic) float lat;
@property (nonatomic) float lon;

- (void)startUpdatingLocation;
- (void)stopUpdatingLocation;

@end
