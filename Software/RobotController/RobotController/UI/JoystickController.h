//
//  JoystickController.h
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "JoystickView.h"

@interface JoystickTilt : NSObject

@property (nonatomic) float pitch;
@property (nonatomic) float roll;

- (id)initWithPitch:(float)pitch roll:(float)roll;
+ (JoystickTilt *)tiltWithPitch:(float)pitch roll:(float)roll;

@end

@interface JoystickController : UIViewController

@property (nonatomic, strong) JoystickView *view;
@property (nonatomic) NSInteger joystickId;

- (id)initWithTarget:(NSObject *)aTarget actionCallback:(SEL)aCallback;

@end
