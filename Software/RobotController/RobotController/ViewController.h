//
//  ViewController.h
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "JoystickController.h"
#import "PowerButton.h"
#import "RobotCommander.h"

@interface ViewController : UIViewController <RobotCommanderProtocol>

@property (nonatomic, strong) PowerButton *powerButton;
@property (nonatomic, strong) JoystickController *joystickLeft;
@property (nonatomic, strong) JoystickController *joystickRight;
@property (nonatomic, strong) RobotCommander *robotCommander;

@end

