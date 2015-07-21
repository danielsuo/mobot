//
//  Preview.h
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#define HAS_LIBCXX
#import <Structure/Structure.h>

#import "Utilities.h"

@protocol PreviewDelegate <NSObject>

@required
- (void)frameReadyToRecord:(NSData *)data withType:(NSString *)type;

@end

@interface Preview : UIView

@property (nonatomic, strong) id <PreviewDelegate> previewDelegate;

- (Preview *)initWithFrame:(CGRect)frame;

- (void)renderColorFrame:(CMSampleBufferRef)colorFrame depthFrame:(STDepthFrame *)depthFrame infraredFrame:(STInfraredFrame *)infraredFrame;

@end
