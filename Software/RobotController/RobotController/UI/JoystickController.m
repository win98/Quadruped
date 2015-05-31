//
//  JoystickController.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "JoystickController.h"

#define STICK_ANIMATION_DURATION    0.1f
#define STICK_TILT_FACTOR           0.5f

@implementation JoystickTilt

- (id)initWithPitch:(float)pitch roll:(float)roll
{
    if (self = [super init])
    {
        self.pitch = pitch;
        self.roll = roll;
    }
    
    return self;
}

+ (JoystickTilt *)tiltWithPitch:(float)pitch roll:(float)roll
{
    return [[JoystickTilt alloc] initWithPitch:pitch roll:roll];
}

@end

@interface JoystickController ()

@property (nonatomic) float tiltRadius;
@property (nonatomic, weak) NSObject *target;
@property (nonatomic) SEL callback;

@end

@implementation JoystickController

@dynamic view;

- (id)initWithTarget:(NSObject *)aTarget actionCallback:(SEL)aCallback
{
    if (self = [super init])
    {
        self.target = aTarget;
        self.callback = aCallback;
    }
    
    return self;
}

- (void)loadView
{
    self.view = [[JoystickView alloc] init];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    // Native view.
    JoystickView *selfView = (JoystickView *)self.view;
    
    // Joystick images.
    UIImage *seatImg = [UIImage imageNamed:@"joystick_seat.png"];
    UIImage *stickImg = [UIImage imageNamed:@"joystick_stick.png"];
    
    // Set view size equal to joystick seat size.
    CGRect r = self.view.frame;
    r = CGRectMake(r.origin.x,
                   r.origin.y,
                   seatImg.size.width,
                   seatImg.size.height);
    self.view.frame = r;
    
    selfView.stickView = [[UIImageView alloc] initWithImage:stickImg];
    selfView.seatView = [[UIImageView alloc] initWithImage:seatImg];
    
    // Acceptable tilt of stick.
    self.tiltRadius = self.view.seatView.bounds.size.width * 0.5f * STICK_TILT_FACTOR;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *t = [touches anyObject];
    
    CGPoint newCenter = [t locationInView:self.view];
    CGPoint viewCenter = CGPointMake(self.view.bounds.size.width * 0.5f, self.view.bounds.size.height * 0.5f);
    
    float dx = newCenter.x - viewCenter.x;
    float dy = newCenter.y - viewCenter.y;
    float l = powf(dx * dx + dy * dy, 0.5f);
    
    if (l > _tiltRadius)
    {
        float f = _tiltRadius / l;
        dx *= f;
        dy *= f;
    }
    
    newCenter = CGPointMake(dx + viewCenter.x, dy + viewCenter.y);
    
    [self animateView:self.view.stickView toPosition:newCenter];
    
    [self sendStickPositionDx:dx dy:dy];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *t = [touches anyObject];
    
    CGPoint newCenter = [t locationInView:self.view];
    CGPoint viewCenter = CGPointMake(self.view.bounds.size.width * 0.5f, self.view.bounds.size.height * 0.5f);
    
    float dx = newCenter.x - viewCenter.x;
    float dy = newCenter.y - viewCenter.y;
    float l = powf(dx * dx + dy * dy, 0.5f);
    
    if (l > _tiltRadius)
    {
        float f = _tiltRadius / l;
        dx *= f;
        dy *= f;
    }
    
    newCenter = CGPointMake(dx + viewCenter.x, dy + viewCenter.y);
    
    [self animateView:self.view.stickView toPosition:newCenter];
    
    [self sendStickPositionDx:dx dy:dy];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint newCenter = CGPointMake(self.view.bounds.size.width * 0.5f, self.view.bounds.size.height * 0.5f);
    
    [self animateView:self.view.stickView toPosition:newCenter];
    
    [self sendStickPositionDx:0 dy:0];
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint newCenter = CGPointMake(self.view.bounds.size.width * 0.5f, self.view.bounds.size.height * 0.5f);
    
    [self animateView:self.view.stickView toPosition:newCenter];
    
    [self sendStickPositionDx:0 dy:0];
}

- (void)sendStickPositionDx:(float)dx dy:(float)dy
{
    if ([_target respondsToSelector:_callback])
    {
        JoystickTilt *tilt = [JoystickTilt tiltWithPitch:dx / _tiltRadius roll:- dy / _tiltRadius];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        [_target performSelector:_callback withObject:tilt];
#pragma clang diagnostic pop
    }
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
