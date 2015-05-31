//
//  PowerButton.m
//  RobotController
//
//  Created by Sergey on 5/30/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "PowerButton.h"

@interface PowerButton()

@property (nonatomic, strong) UIImage *onImage;
@property (nonatomic, strong) UIImage *offImage;
@property (nonatomic, strong) UIImageView *image;
@property (nonatomic, weak) NSObject *target;
@property (nonatomic) SEL action;

@end

@implementation PowerButton

- (id)init
{
    if (self = [super init])
    {
        self.onImage = [UIImage imageNamed:@"power_button_on.png"];
        self.offImage = [UIImage imageNamed:@"power_button_off.png"];
        
        self.backgroundColor = [UIColor clearColor];
        self.frame = CGRectMake(0, 0, _offImage.size.width, _offImage.size.height);
        self.image = [[UIImageView alloc] initWithImage:_offImage];
        self.image.frame = self.frame;
        [self addSubview:_image];
        
        self.powerOn = NO;
    }
    
    return self;
}

- (void)setTarget:(id)target action:(SEL)action
{
    _target = target;
    _action = action;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    self.powerOn = !self.powerOn;
    
    if ([_target respondsToSelector:_action])
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
        [_target performSelector:_action withObject:self];
#pragma clang diagnostic pop
    }
    
    [super touchesBegan:touches withEvent:event];
}

- (void)setPowerOn:(BOOL)powerOn
{
    _powerOn = powerOn;
    
    UIImage *img = powerOn ? _onImage : _offImage;
    
    self.image.image = img;
}

@end
