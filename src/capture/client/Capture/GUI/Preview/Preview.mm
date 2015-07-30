//
//  Preview.mm
//  Capture
//
//  Created by Daniel Suo on 7/15/15.
//  Copyright (c) 2015 Princeton University Vision Group. All rights reserved.
//

#import "Preview.h"

@interface Preview ()
{
    UIImageView *_color;
    UIImageView *_structure;
}

@end

@implementation Preview

- (Preview *)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self) {
        CGRect colorFrame = frame;
        colorFrame.size.height /= 2;
        colorFrame.size.width /= 2;
        colorFrame.origin.y = frame.size.height / 2;
        colorFrame.origin.x = frame.size.width / 2;
        
        _color = [[UIImageView alloc] initWithFrame:colorFrame];
        _color.contentMode = UIViewContentModeScaleAspectFit;
        [self addSubview:_color];
        
        CGRect structureFrame = frame;
        structureFrame.size.height /= 2;
        structureFrame.size.width /= 2;
        structureFrame.origin.y = frame.size.height / 2;
        structureFrame.origin.x = 0;
        
        _structure = [[UIImageView alloc] initWithFrame:structureFrame];
        _structure.contentMode = UIViewContentModeScaleAspectFit;
        [self addSubview:_structure];
    }
    
    return self;
}

#pragma mark - Render images
- (void)renderImage:(UIImage *)image type:(NSString *)type
{
    if ([type isEqualToString:kFrameTypeColor]) {
        _color.image = image;
    } else {
        _structure.image = image;
    }
}

@end
