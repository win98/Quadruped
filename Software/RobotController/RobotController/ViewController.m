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

#define FAIL_REQUESTS_NUMBER_TO_STOP    5

#define LEFT_JOYSTICK_POSITION_CALC(S)      CGPointMake(S.width * 0.2f, S.height * 0.65f)
#define RIGHT_JOYSTICK_POSITION_CALC(S)     CGPointMake(S.width * 0.8f, S.height * 0.65f)
#define POWER_BUTTON_POSITION_CALC(S)       CGPointMake(S.width * 0.5f, S.height * 0.3f)
#define SETTINGS_BUTTON_POSITION_CALC(S)    CGPointMake(S.width * 0.955f, S.height * 0.065f)

#define JOYSTICK_SCALE          8.0f

#define MIN_SERVO_PULSE_WIDTH         1000
#define MAX_SERVO_PULSE_WIDTH         2000

@interface ViewController ()
{
    uint8_t failedRequestsCounter;
    
    // We assume that robot receives
    // relative velocities values in
    // range -8 .. +8. Joystick has much
    // more bigger resolution and every
    // callback from it has "new" position
    // value even if it changed for example
    // by 0.001. Every callback generates
    // request - it's to often.
    // To prevent too often request store
    // values to check if new value
    // from joystick is changed.
    int8_t curVelStraight;
    int8_t curVelLateral;
    int8_t curVelAngular;
}

@property (nonatomic) BOOL connected;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    CGSize s = [UIScreen mainScreen].bounds.size;
    
    // Set view size to fit screen.
    self.view.frame = CGRectMake(0.f, 0.f, s.width, s.height);
    self.view.backgroundColor = [UIColor grayColor];
    
    UIImage *backImage = [UIImage imageNamed:@"backGrayAbstract.png"];
    UIImageView *backView = [[UIImageView alloc] initWithImage:backImage];
    backView.frame = CGRectMake(0.f, 0.f, backImage.size.width, backImage.size.height);
    CGPoint center = CGPointMake(s.width * 0.5f, s.height * 0.5f);
    backView.center = center;
    [self.view addSubview:backView];
    
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
    
    // Settings button.
    self.settingsButton = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage *setBtnImg = [UIImage imageNamed:@"settingsButton@2x.png"];
    [self.settingsButton setImage:setBtnImg forState:UIControlStateNormal];
    self.settingsButton.frame = CGRectMake(0, 0, setBtnImg.size.width, setBtnImg.size.height);
    self.settingsButton.center = SETTINGS_BUTTON_POSITION_CALC(s);
    [self.view addSubview:self.settingsButton];
    self.settingsButton.enabled = self.connected;
    [self.settingsButton addTarget:self action:@selector(settingsButtonPressed:)
                  forControlEvents:UIControlEventTouchUpInside];
    
    // Robot controller.
    self.robotCommander = [[RobotCommander alloc] initWithRobotAddress:ROBOT_ADDRESS robotPort:ROBOT_PORT
                                                            clientPort:CLIENT_PORT];
    self.robotCommander.commTimeout = 200;
    self.robotCommander.delegate = self;
    self.connected = NO;
    
    failedRequestsCounter = 0;
    
    curVelAngular = 0;
    curVelLateral = 0;
    curVelStraight = 0;
}

#pragma mark - Settings button callback

- (void)settingsButtonPressed:(UIButton *)button
{
    if (self.connected)
    {
        RobotRequest *request = [RobotCommander requestToSetControlMode:RCM_DIRECT_SERVO_MODE];
        
        [self.robotCommander sendRequest:request];
    }
}

#pragma mark - Direct control screen callbacks

- (void)closeButtonPressed:(UIButton *)button
{
    RobotRequest *request = [RobotCommander requestToSetControlMode:RCM_NORMAL_MODE];
    
    [self.robotCommander sendRequest:request];
}

- (void)sliderSet:(UISlider *)slider
{
    uint8_t channelId = slider.tag;
    uint16_t value = slider.value;
    
    RobotRequest *request = [RobotCommander requestToSetChannel:channelId withValue:value];
    [self.robotCommander sendRequest:request];
}

#pragma mark - Joysticks callbacks

- (void)leftJoystickTilt:(JoystickTilt *)tilt
{
    int8_t velStraight = (int8_t)((float)tilt.roll * JOYSTICK_SCALE);
    int8_t velLateral = (int8_t)((float)tilt.pitch * JOYSTICK_SCALE);
    
    if ((curVelStraight != velStraight || curVelLateral != velLateral) && self.connected)
    {
        curVelStraight = velStraight;
        RobotRequest *setStrVelReq = [RobotCommander requestToSetStraightVelocity:velStraight];
        [self.robotCommander sendRequest:setStrVelReq];

        curVelLateral = velLateral;
        RobotRequest *setLatVelReq = [RobotCommander requestToSetLateralVelocity:velLateral];
        [self.robotCommander sendRequest:setLatVelReq];
        
        NSLog(@"#Left : %hhi, %hhi", velLateral, velStraight);
    }
}

- (void)rightJoystickTilt:(JoystickTilt *)tilt
{
    int8_t velAngular = (int8_t)((float)tilt.pitch * JOYSTICK_SCALE);
    
    if (curVelAngular != velAngular && self.connected)
    {
        curVelAngular = velAngular;
        RobotRequest *setVelAngReq = [RobotCommander requestToSetAngularVelocity:velAngular];
        [self.robotCommander sendRequest:setVelAngReq];
        
        NSLog(@"#Right : %hhi", velAngular);
    }
}

#pragma mark - Power button callback

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

#pragma mark - Robot communication callbacks

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
                // and port are ours. Maybe we have already
                // connected to the robot earlier.
                self.connected = [self parseAndCheckIsItMe:&data[2] port:&data[6]] ? YES : NO;
            }
            
            self.powerButton.powerOn = self.connected;
            self.settingsButton.enabled = self.connected;
            
            break;
            
        case WF_ROBOT_DROP_CONTROL:
            
            self.connected = NO;
            self.settingsButton.enabled = self.connected;
            
            break;
            
        case WF_ROBOT_SET_CONTROL_MODE:
            
            if (status == WF_CMD_OK)
            {
                ROBOT_CONTROL_MODE mode = data[2];
                
                if (mode == RCM_NORMAL_MODE)
                {
                    UIWindow *window = [UIApplication sharedApplication].windows[0];
                    [window.rootViewController dismissViewControllerAnimated:YES completion:nil];
                }
                else
                {
                    DirectControlViewController *vc = [[DirectControlViewController alloc] init];
                    vc.minSliderValue = MIN_SERVO_PULSE_WIDTH;
                    vc.maxSliderValue = MAX_SERVO_PULSE_WIDTH;
                    vc.delegate = self;
                    vc.view.frame = self.view.frame;
                    
                    UIWindow *window = [UIApplication sharedApplication].windows[0];
                    [window.rootViewController presentViewController:vc animated:YES completion:nil];
                }
            }
            
            break;
            
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
            self.settingsButton.enabled = self.connected;
            
            break;
            
        default:
            break;
    }
    
    // Stop sending requests as robot may be offline.
    if (++failedRequestsCounter > FAIL_REQUESTS_NUMBER_TO_STOP)
    {
        [self.robotCommander cancelPreviousRequests];
        failedRequestsCounter = 0;
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
            self.settingsButton.enabled = self.connected;
            
            break;
            
        default:
            break;
    }
    
    // Stop sending requests as robot may be offline.
    if (++failedRequestsCounter > FAIL_REQUESTS_NUMBER_TO_STOP)
    {
        [self.robotCommander cancelPreviousRequests];
        failedRequestsCounter = 0;
    }
}

#pragma mark - Utils

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

- (BOOL)parseAndCheckIsItMe:(uint8_t *)addressData port:(uint8_t *)portData
{
    // Calc IP from data.
    NSString *ip = [NSString stringWithFormat:@"%hhu.%hhu.%hhu.%hhu", addressData[3], addressData[2],
                                                                        addressData[1], addressData[0]];
    // Calc port from data.
    uint16_t port = (uint16_t)portData[0];
    port |= (uint16_t)portData[1] << 8;
    
    // Detect my WiFi IP.
    NSString *myIp = [self getIPAddress];
    
    // Is data equal to me?
    if ([ip isEqualToString:myIp] && port == CLIENT_PORT)
    {
        return YES;
    }
    
    return NO;
}

@end
