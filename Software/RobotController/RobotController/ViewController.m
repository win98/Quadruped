//
//  ViewController.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "ViewController.h"
#import "JoystickController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    JoystickController *ctr = [[JoystickController alloc] init];
    
    [self.view addSubview:ctr.view];
    
    ctr.view.center = CGPointMake(200, 200);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
