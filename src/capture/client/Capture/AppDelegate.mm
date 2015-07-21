//
//  AppDelegate.h
//  capture
//
//  Created by Daniel Suo on 6/19/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // STWirelessLog is very helpful for debugging while your Structure Sensor is plugged in.
    // See SDK documentation for how to start a listener on your computer.
    NSError* error = nil;
    NSString *remoteLogHost = [Utilities getSettingsValue:kSettingsDebugIP];

    [STWirelessLog broadcastLogsToWirelessConsoleAtAddress:remoteLogHost usingPort:4999 error:&error];
    if (error)
        NSLog(@"ERROR: Oh no! Can't start wireless log: %@", [error localizedDescription]);
    else
        NSLog([NSString stringWithFormat:@"INFO: Connected to %@.", remoteLogHost]);
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    self.viewController = [[ViewController alloc] init];
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    
    if (![[NSUserDefaults standardUserDefaults] boolForKey:@"has_launched"])
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"has_launched"];
    }

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [self.viewController closeGracefully];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

#warning Restart app gracefully
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
