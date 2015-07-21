//
//  GPS.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "GPS.h"

@interface GPS ()
{
    CLLocationManager *_locationManager;
}

@end

@implementation GPS

- (id)init
{
    self = [super init];
    
    if (self) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.delegate = _delegate;
        _locationManager.desiredAccuracy = kCLLocationAccuracyBest;
        _locationManager.distanceFilter = kCLDistanceFilterNone;
        
        [_locationManager requestWhenInUseAuthorization];
#warning do we actually need this?
        [_locationManager requestAlwaysAuthorization];
    }
    
    return self;
}

- (void)startUpdatingLocation
{
    [_locationManager startUpdatingLocation];
}

- (void)stopUpdatingLocation
{
    [_locationManager stopUpdatingLocation];
}

@end
