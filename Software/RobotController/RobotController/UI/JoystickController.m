//
//  JoystickController.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "JoystickController.h"

@interface JoystickController ()

@end

@implementation JoystickController

- (instancetype)init
{
    if (self = [super init])
    {
        
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
    r = CGRectMake(r.origin.x, r.origin.y, seatImg.size.width, seatImg.size.height);
    self.view.frame = r;
    
    selfView.stickImage = [[UIImageView alloc] initWithImage:stickImg];
    selfView.seatImage = [[UIImageView alloc] initWithImage:seatImg];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
