//
//  JoystickView.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "JoystickView.h"

@implementation JoystickView

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        self.multipleTouchEnabled = NO;
    }
    
    return self;
}

- (void)setSeatView:(UIImageView *)aSeatImage
{
    [_seatView removeFromSuperview];
    
    _seatView = aSeatImage;
    _seatView.userInteractionEnabled = NO;
    _seatView.exclusiveTouch = NO;
    
    [self addSubview:_seatView];
    
    [self bringSubviewToFront:_stickView];
}

- (void)setStickView:(UIImageView *)aStickImage
{
    [_stickView removeFromSuperview];
    
    _stickView = aStickImage;
    _stickView.userInteractionEnabled = NO;
    _stickView.exclusiveTouch = NO;
    
    [self addSubview:_stickView];
}

@end
