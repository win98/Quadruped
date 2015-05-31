//
//  ViewController.m
//  RobotController
//
//  Created by Sergey on 5/29/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "ViewController.h"

#include <ifaddrs.h>
#include <arpa/inet.h>

#define ROBOT_ADDRESS           @"192.168.1.1"
#define ROBOT_PORT              49001
#define CLIENT_PORT             49002

#define LEFT_JOYSTICK_POSITION_CALC(S)      CGPointMake(S.width * 0.2f, S.height * 0.65f)
#define RIGHT_JOYSTICK_POSITION_CALC(S)     CGPointMake(S.width * 0.8f, S.height * 0.65f)
#define POWER_BUTTON_POSITION_CALC(S)       CGPointMake(S.width * 0.5f, S.height * 0.3f)

@interface ViewController ()

@property (nonatomic) BOOL connected;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    CGSize s = [UIScreen mainScreen].bounds.size;
    
    // Set view size to fit screen.
    self.view.frame = CGRectMake(0, 0, s.width, s.height);
    self.view.backgroundColor = [UIColor orangeColor];
    
    // Power button.
    self.powerButton = [[PowerButton alloc] init];
    self.powerButton.center = POWER_BUTTON_POSITION_CALC(s);
    [self.powerButton setTarget:self action:@selector(powerButtonPressed:)];
    
    // Left joystick.
    self.joystickLeft = [[JoystickController alloc] initWithTarget:self actionCallback:@selector(leftJoystickTilt:)];
    self.joystickLeft.view.center = LEFT_JOYSTICK_POSITION_CALC(s);
    
    // Right joystick.
    self.joystickRight = [[JoystickController alloc] initWithTarget:self actionCallback:@selector(rightJoystickTilt:)];
    self.joystickRight.view.center = RIGHT_JOYSTICK_POSITION_CALC(s);
    
    [self.view addSubview:self.powerButton];
    [self.view addSubview:self.joystickLeft.view];
    [self.view addSubview:self.joystickRight.view];
    
    
    // Robot controller.
    self.robotCommander = [[RobotCommander alloc] initWithRobotAddress:ROBOT_ADDRESS robotPort:ROBOT_PORT clientPort:CLIENT_PORT];
    self.robotCommander.delegate = self;
    self.connected = NO;
}

- (void)leftJoystickTilt:(JoystickTilt *)tilt
{
    NSLog(@"#Left : %f, %f", tilt.pitch, tilt.roll);
}

- (void)rightJoystickTilt:(JoystickTilt *)tilt
{
    NSLog(@"#Right : %f, %f", tilt.pitch, tilt.roll);
}

- (void)powerButtonPressed:(PowerButton *)button
{
    NSLog(@"#Power : %@", button.powerOn ? @"ON" : @"OFF");
    
    RobotRequest *req;
    
    if (button.powerOn)
    {
        req = [RobotCommander requestToConnect];
    }
    else
    {
        req = [RobotCommander requestToDisconnect];
    }
    
    [self.robotCommander sendRequest:req];
}

- (NSString *)getIPAddress {
    
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                    
                }
                
            }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return address;
    
}

- (BOOL)parseAndCheckIsItMe:(uint8_t *)data
{
    NSString *ip = [NSString stringWithFormat:@"%hhu.%hhu.%hhu.%hhu", data[3], data[2], data[1], data[0]];
    uint16_t port = (uint16_t)data[4];
    port |= (uint16_t)data[5] << 8;
    
    NSString *myIp = [self getIPAddress];
    
    if ([ip isEqualToString:myIp] && port == CLIENT_PORT)
    {
        return YES;
    }
    
    return NO;
}

- (void)request:(RobotRequest *)request didReceiveResponse:(RobotResponse *)response
{
    uint8_t *data = (uint8_t *)response.data.bytes;
    WF_COMMAND_STATUS status = data[1];
    
    switch (response.command)
    {
        case WF_ROBOT_TAKE_CONTROL:
            
            if (status == WF_CMD_OK)
            {
                self.connected = YES;
            }
            else
            {
                // Robot sends ip and port of connected client
                // in case of command error. Check if this ip
                // and port are ours. Maybe we connected to the
                // robot earlier.
                self.connected = [self parseAndCheckIsItMe:&data[2]] ? YES : NO;
            }
            
            self.powerButton.powerOn = self.connected;
            
            break;
        case WF_ROBOT_DROP_CONTROL:
            
            self.connected = NO;
            
        default:
            break;
    }
}

- (void)requestDidFail:(RobotRequest *)request
{
    switch (request.command)
    {
        case WF_ROBOT_TAKE_CONTROL:
        case WF_ROBOT_DROP_CONTROL:
            
            self.connected = NO;
            self.powerButton.powerOn = self.connected;
            
            break;
            
        default:
            break;
    }
}

- (void)requestDidTimeOut:(RobotRequest *)request
{
    switch (request.command)
    {
        case WF_ROBOT_TAKE_CONTROL:
        case WF_ROBOT_DROP_CONTROL:
            
            self.connected = NO;
            self.powerButton.powerOn = self.connected;
            
            break;
            
        default:
            break;
    }
}

@end
