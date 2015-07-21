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

- (void)renderColorFrame:(CMSampleBufferRef)colorFrame depthFrame:(STDepthFrame *)depthFrame infraredFrame:(STInfraredFrame *)infraredFrame
{
    if (colorFrame) {
        [self renderColorFrame:colorFrame];
    }
    
    if (depthFrame) {
        [self renderDepthFrame:depthFrame];
    }
    
    if (infraredFrame) {
        [self renderInfraredFrame:infraredFrame];
    }
}

#pragma mark - Render frames

- (void)renderColorFrame:(CMSampleBufferRef)sampleBuffer
{
    CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    size_t cols = CVPixelBufferGetWidth(pixelBuffer);
    size_t rows = CVPixelBufferGetHeight(pixelBuffer);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    unsigned char *ptr = (unsigned char *) CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
    
    NSData *data = [[NSData alloc] initWithBytes:ptr length:rows*cols*4];
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipFirst;
    bitmapInfo |= kCGBitmapByteOrder32Little;
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data);
    
    CGImageRef imageRef = CGImageCreate(cols,
                                        rows,
                                        8,
                                        8 * 4,
                                        cols*4,
                                        colorSpace,
                                        bitmapInfo,
                                        provider,
                                        NULL,
                                        false,
                                        kCGRenderingIntentDefault);
    
    _color.image = [[UIImage alloc] initWithCGImage:imageRef];
    
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    NSData *imageData = UIImageJPEGRepresentation(_color.image, 0.8);
    
    [self.previewDelegate frameReadyToRecord:imageData withType:kFrameTypeColor];
}

- (void)renderDepthFrame:(STDepthFrame *)depthFrame
{
    size_t cols = depthFrame.width;
    size_t rows = depthFrame.height;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipLast;
    bitmapInfo |= kCGBitmapByteOrder16Little;
    
    size_t numPixels = cols * rows;
    uint16_t *depthData = (uint16_t *)malloc(numPixels * sizeof(uint16_t));
    
    for (int i = 0; i < numPixels; i++) {
        depthData[i] = (uint16_t)(depthFrame.depthInMillimeters[i]) << 3;
    }
    
    depthFrame = nil;
    
    NSData *data = [NSData dataWithBytes:depthData length:cols * rows * 2];
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data); //toll-free ARC bridging
    
    CGImageRef imageRef = CGImageCreate(cols,                        //width
                                        rows,                        //height
                                        16,                          //bits per component
                                        16,                          //bits per pixel
                                        cols * 2,                    //bytes per row
                                        colorSpace,                  //Quartz color space
                                        bitmapInfo,                  //Bitmap info (alpha channel?, order, etc)
                                        provider,                    //Source of data for bitmap
                                        NULL,                        //decode
                                        false,                       //pixel interpolation
                                        kCGRenderingIntentDefault);  //rendering intent
    
    _structure.image = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    free(depthData);
    
    NSData *imageData = UIImagePNGRepresentation(_structure.image);
    
    [self.previewDelegate frameReadyToRecord:imageData withType:kFrameTypeDepth];
}

- (void)renderInfraredFrame:(STInfraredFrame *)infraredFrame
{
    size_t cols = infraredFrame.width;
    size_t rows = infraredFrame.height;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    
    CGBitmapInfo bitmapInfo;
    bitmapInfo = (CGBitmapInfo)kCGImageAlphaNoneSkipLast;
    bitmapInfo |= kCGBitmapByteOrder16Big;
    
    NSData *data = [NSData dataWithBytes:infraredFrame.data length:cols * rows * 2];
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data); //toll-free ARC bridging
    
    CGImageRef imageRef = CGImageCreate(cols,                        //width
                                        rows,                        //height
                                        16,                          //bits per component
                                        16,                          //bits per pixel
                                        cols * 2,                    //bytes per row
                                        colorSpace,                  //Quartz color space
                                        bitmapInfo,                  //Bitmap info (alpha channel?, order, etc)
                                        provider,                    //Source of data for bitmap
                                        NULL,                        //decode
                                        false,                       //pixel interpolation
                                        kCGRenderingIntentDefault);  //rendering intent
    
    _structure.image = [UIImage imageWithCGImage:imageRef];
    
    _structure.image = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    NSData *imageData = UIImagePNGRepresentation(_structure.image);
    
    [self.previewDelegate frameReadyToRecord:imageData withType:kFrameTypeInfrared];
}

@end
