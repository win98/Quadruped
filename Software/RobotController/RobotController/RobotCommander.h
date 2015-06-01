//
//  RobotCommander.h
//  RobotController
//
//  Created by Sergey on 5/30/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum
{
    WF_ROBOT_NO_CMD = 0,
    WF_ROBOT_TAKE_CONTROL = '1',
    WF_ROBOT_DROP_CONTROL,
    WF_ROBOT_SET_CHANNEL,
    WF_ROBOT_SET_CHANNELS,
    WF_ROBOT_SET_CONTROL_MODE,
    WF_ROBOT_SET_STRAIGHT_VELOCITY,
    WF_ROBOT_SET_LATERAL_VELOCITY,
    WF_ROBOT_SET_ANGULAR_VELOCITY,
    WF_ROBOT_PING
} WF_ROBOT_COMMAND;

// Robot control mode.
typedef enum
{
    RCM_DIRECT_SERVO_MODE = 1,
    RCM_NORMAL_MODE
} ROBOT_CONTROL_MODE;

// Status of command execution.
typedef enum
{
    WF_CMD_OK = '0',
    WF_CMD_ERROR
} WF_COMMAND_STATUS;

@interface RobotRequest : NSObject

@property (nonatomic) WF_ROBOT_COMMAND command;
@property (nonatomic, strong) NSData *data;

@end

@interface RobotResponse : NSObject

@property (nonatomic) WF_ROBOT_COMMAND command;
@property (nonatomic, strong) NSData *data;
@property (nonatomic) uint32_t length;

@end

@protocol RobotCommanderProtocol

- (void)request:(RobotRequest *)request didReceiveResponse:(RobotResponse *)response;
- (void)requestDidFail:(RobotRequest *)request;
- (void)requestDidTimeOut:(RobotRequest *)request;

@end

@interface RobotCommander : NSObject

@property (nonatomic, weak) NSObject <RobotCommanderProtocol> *delegate;
@property (nonatomic, strong) NSString *robotIP;
@property (nonatomic) uint16_t robotPort;
@property (nonatomic) uint16_t clientPort;
@property (nonatomic) NSUInteger commTimeout; //ms

+ (RobotRequest *)requestToConnect;
+ (RobotRequest *)requestToDisconnect;
+ (RobotRequest *)requestToSetChannel:(uint8_t)channel withValue:(uint16_t)value;
+ (RobotRequest *)requestToSetChannels:(uint16_t *)values channelsTotal:(uint8_t)count;
+ (RobotRequest *)requestToSetControlMode:(ROBOT_CONTROL_MODE)mode;
+ (RobotRequest *)requestToSetStraightVelocity:(int8_t)velocity;
+ (RobotRequest *)requestToSetLateralVelocity:(int8_t)velocity;
+ (RobotRequest *)requestToSetAngularVelocity:(int8_t)velocity;
+ (RobotRequest *)requestToPing;

- (id)initWithRobotAddress:(NSString *)robotAddress robotPort:(uint16_t)robotPort clientPort:(uint16_t)clientPort;
- (void)sendRequest:(RobotRequest *)request;
- (void)cancelPreviousRequests;

@end
