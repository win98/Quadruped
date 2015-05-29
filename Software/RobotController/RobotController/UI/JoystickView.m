//
//  JoystickView.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "JoystickView.h"

#define STICK_ANIMATION_DURATION  0.1f

@implementation JoystickView

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        self.multipleTouchEnabled = NO;
    }
    
    return self;
}

- (void)setSeatImage:(UIImageView *)aSeatImage
{
    [_seatImage removeFromSuperview];
    
    _seatImage = aSeatImage;
    _seatImage.userInteractionEnabled = NO;
    _seatImage.exclusiveTouch = NO;
    
    [self addSubview:_seatImage];
    
    [self bringSubviewToFront:_stickImage];
}

- (void)setStickImage:(UIImageView *)aStickImage
{
    [_stickImage removeFromSuperview];
    
    _stickImage = aStickImage;
    _stickImage.userInteractionEnabled = NO;
    _stickImage.exclusiveTouch = NO;
    
    [self addSubview:_stickImage];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *t = [touches anyObject];
    
    CGPoint newCenter = [t locationInView:self];
    
    [self animateView:_stickImage toPosition:newCenter];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *t = [touches anyObject];
    
    CGPoint newCenter = [t locationInView:self];
    CGPoint viewCenter = CGPointMake(self.bounds.size.width * 0.5f, self.bounds.size.height * 0.5f);
    
    float radius = _seatImage.bounds.size.width * 0.5f;
    
    float dx = newCenter.x - viewCenter.x;
    float dy = newCenter.y - viewCenter.y;
    float l = powf(dx * dx + dy * dy, 0.5f);
    
    if (l > radius)
    {
        float f = radius / l;
        newCenter = CGPointMake(dx * f + viewCenter.x, dy * f + viewCenter.y);
    }
    
    [self animateView:_stickImage toPosition:newCenter];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint newCenter = CGPointMake(self.bounds.size.width * 0.5f, self.bounds.size.height * 0.5f);
    
    [self animateView:_stickImage toPosition:newCenter];
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint newCenter = CGPointMake(self.bounds.size.width * 0.5f, self.bounds.size.height * 0.5f);
    
    [self animateView:_stickImage toPosition:newCenter];
}

- (void)animateView:(UIView *)view toPosition:(CGPoint)position
{
    [UIView animateWithDuration:STICK_ANIMATION_DURATION
                          delay:0.0f
                        options: UIViewAnimationOptionCurveEaseOut
                     animations:^ {
                         view.center = position;
                     }
                     completion:^(BOOL finished){
                     }];
}

@end
