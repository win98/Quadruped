//
//  PowerButton.h
//  RobotController
//
//  Created by Sergey on 5/30/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PowerButton : UIView

@property (nonatomic) BOOL powerOn;

- (void)setTarget:(id)target action:(SEL)action;

@end
