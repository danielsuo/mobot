//
//  GPS.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "GPS.h"

@implementation GPS

- (id)init
{
    self = [super init];
    
    if (self) {
        _locationManager = [[CLLocationManager alloc] init];
        _locationManager.desiredAccuracy = kCLLocationAccuracyBest;
        _locationManager.distanceFilter = kCLDistanceFilterNone;
        
        [_locationManager requestWhenInUseAuthorization];
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

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    [Utilities sendWarning:[NSString stringWithFormat:@"WARN: GPS didFailWithError: %@", error]];
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *currentLocation = [locations lastObject];
    [Utilities sendLog:[NSString stringWithFormat:@"LOG: didUpdateToLocation: %@", currentLocation]];
    
    [_gpsDelegate locationManager:manager didUpdateLocations:locations];
    
    [self stopUpdatingLocation];
}

@end
