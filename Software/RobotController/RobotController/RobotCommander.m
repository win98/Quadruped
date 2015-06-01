//
//  RobotCommander.m
//  RobotController
//
//  Created by Sergey on 5/30/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "RobotCommander.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

// Min data length for commands
#define ROBOT_CMD_SET_CHANNEL_MDL       4
#define ROBOT_CMD_SET_LIN_VEL_MDL       2
#define ROBOT_CMD_SET_LAT_VEL_MDL       2
#define ROBOT_CMD_SET_ANG_VEL_MDL       2
#define ROBOT_CMD_SET_MODE_MDL          2

@implementation RobotRequest

@end

@implementation RobotResponse

@end

@interface RobotCommander()
{
    struct sockaddr_in *robotAddr;
    struct sockaddr_in *clientAddr;
    int robotSocket;
}

@property (nonatomic, strong) NSThread *commThread;
@property (nonatomic, strong) NSMutableArray *requestsQueue;
@property (nonatomic, strong) RobotRequest *currentRequest;

@end

@implementation RobotCommander

- (id)initWithRobotAddress:(NSString *)robotAddress robotPort:(uint16_t)robotPort clientPort:(uint16_t)clientPort
{
    if (self = [super init])
    {
        _robotIP = robotAddress;
        _robotPort = robotPort;
        _clientPort = clientPort;
        _commTimeout = 100;
        
        self.requestsQueue = [[NSMutableArray alloc] init];
        
        robotAddr = nil;
        clientAddr = nil;
        robotSocket = 0;
    }
    
    return self;
}

+ (RobotRequest *)requestToConnect
{
    uint8_t buffer[1] = {WF_ROBOT_TAKE_CONTROL};
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_TAKE_CONTROL;
    res.data = [[NSData alloc] initWithBytes:buffer length:1];    
    
    return res;
}

+ (RobotRequest *)requestToDisconnect
{
    uint8_t buffer[1] = {WF_ROBOT_DROP_CONTROL};
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_DROP_CONTROL;
    res.data = [[NSData alloc] initWithBytes:buffer length:1];
    
    return res;
}

+ (RobotRequest *)requestToSetChannel:(uint8_t)channel withValue:(uint16_t)value
{
    uint8_t buffer[ROBOT_CMD_SET_CHANNEL_MDL];
    
    buffer[0] = WF_ROBOT_SET_CHANNEL;
    buffer[1] = channel;
    buffer[2] = (uint8_t)(value & 0x00FF);          // Value should be sent
    buffer[3] = (uint8_t)((value >> 8) & 0x00FF);   // in Little-endian format.
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_CHANNEL;
    res.data = [[NSData alloc] initWithBytes:buffer length:ROBOT_CMD_SET_CHANNEL_MDL];
    
    return res;
}

+ (RobotRequest *)requestToSetChannels:(uint16_t *)values channelsTotal:(uint8_t)count
{
    uint8_t buffer[1 + 2 * count];
    
    buffer[0] = WF_ROBOT_SET_CHANNELS;
    
    for (int i = 0; i < count; i++)
    {
        buffer[1 + i * 2] = (uint8_t)(values[i] & 0x00FF);              // Values should be sent
        buffer[2 + i * 2 ] = (uint8_t)((values[i] >> 8) & 0x00FF);      // in Little-endian format.
    }
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_CHANNELS;
    res.data = [[NSData alloc] initWithBytes:buffer length:1 + 2 * count];
    
    return res;
}

+ (RobotRequest *)requestToSetControlMode:(ROBOT_CONTROL_MODE)mode
{
    uint8_t buffer[ROBOT_CMD_SET_MODE_MDL];
    
    buffer[0] = WF_ROBOT_SET_CONTROL_MODE;
    buffer[1] = mode;
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_CONTROL_MODE;
    res.data = [[NSData alloc] initWithBytes:buffer length:ROBOT_CMD_SET_MODE_MDL];
    
    return res;
}

+ (RobotRequest *)requestToSetStraightVelocity:(int8_t)velocity
{
    uint8_t buffer[ROBOT_CMD_SET_LIN_VEL_MDL];
    
    buffer[0] = WF_ROBOT_SET_STRAIGHT_VELOCITY;
    buffer[1] = velocity;
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_STRAIGHT_VELOCITY;
    res.data = [[NSData alloc] initWithBytes:buffer length:ROBOT_CMD_SET_LIN_VEL_MDL];
    
    return res;
}

+ (RobotRequest *)requestToSetLateralVelocity:(int8_t)velocity
{
    uint8_t buffer[ROBOT_CMD_SET_LAT_VEL_MDL];
    
    buffer[0] = WF_ROBOT_SET_LATERAL_VELOCITY;
    buffer[1] = velocity;
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_LATERAL_VELOCITY;
    res.data = [[NSData alloc] initWithBytes:buffer length:ROBOT_CMD_SET_LAT_VEL_MDL];
    
    return res;
}

+ (RobotRequest *)requestToSetAngularVelocity:(int8_t)velocity
{
    uint8_t buffer[ROBOT_CMD_SET_ANG_VEL_MDL];
    
    buffer[0] = WF_ROBOT_SET_ANGULAR_VELOCITY;
    buffer[1] = velocity;
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_SET_ANGULAR_VELOCITY;
    res.data = [[NSData alloc] initWithBytes:buffer length:ROBOT_CMD_SET_ANG_VEL_MDL];
    
    return res;
}

+ (RobotRequest *)requestToPing
{
    uint8_t buffer[1] = {WF_ROBOT_PING};
    
    RobotRequest *res = [[RobotRequest alloc] init];
    res.command = WF_ROBOT_PING;
    res.data = [[NSData alloc] initWithBytes:buffer length:1];
    
    return res;
}

- (void)sendNextRequestFromQueue
{
    if (self.requestsQueue.count > 0)
    {
        RobotRequest *request = self.requestsQueue[0];
        
        self.currentRequest = request;
        
        // Create new thread.
        NSThread *commThread = [[NSThread alloc] initWithTarget:self selector:@selector(sendDatagramWithRequest:) object:request];
        [commThread start];
    }
    else
    {
        self.currentRequest = nil;
    }
}

- (void)cancelPreviousRequests
{
    [self.requestsQueue removeAllObjects];
}

- (void)sendRequest:(RobotRequest *)request
{
    if (!request)
    {
        return;
    }
    
    [self.requestsQueue addObject:request];
    
    // Return if any request is already in progress.
    // Perform requests continuosly.
    if (self.requestsQueue.count > 1)
    {
        return;
    }
    
    self.currentRequest = request;
    
    // Create new thread.
    NSThread *commThread = [[NSThread alloc] initWithTarget:self selector:@selector(sendDatagramWithRequest:) object:request];
    [commThread start];
}

- (BOOL)createSocketForAddress:(NSString *)ip port:(uint16_t)port withSourcePort:(uint16_t)sourcePort
{
    // Create UDP socket.
    robotSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (robotSocket < 0)
    {
        return NO;
    }
    
    // Create remote sockaddr.
    robotAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    memset(robotAddr, 0, sizeof(struct sockaddr_in));
    robotAddr->sin_family = AF_INET;
    robotAddr->sin_port = htons(port);
    
    const char *ipStr = [ip UTF8String];
    if (inet_aton(ipStr , &robotAddr->sin_addr) == 0)
    {
        return NO;
    }
    
    // Create client sockaddr (for fixed outgoing port).
    clientAddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    clientAddr->sin_family = AF_INET;
    clientAddr->sin_addr.s_addr= htonl(INADDR_ANY);
    clientAddr->sin_port=htons(sourcePort);
    int res = bind(robotSocket,(struct sockaddr *)clientAddr,sizeof(struct sockaddr_in));
    if (res == -1)
    {
        return NO;
    }
    
    // Set timeout.
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = (__darwin_suseconds_t)(self.commTimeout * 1000);
    res = setsockopt(robotSocket, SOL_SOCKET, SO_RCVTIMEO,&timeout,sizeof(timeout));
    
    if (res == -1)
    {
        return NO;
    }
    
    
    return YES;
}

- (void)sendDatagramWithRequest:(RobotRequest *)request
{
    uint8_t buffer[100];
    uint32_t bufferLength;
    socklen_t slen;
    
    if (![self createSocketForAddress:self.robotIP port:self.robotPort withSourcePort:self.clientPort])
    {
        [self freeResources];
        
        [self performSelectorOnMainThread:@selector(requestFailed:) withObject:request waitUntilDone:YES];
        
        [NSThread exit];
    }
    
    slen = sizeof(struct sockaddr_in);
    bufferLength = sizeof(buffer);

    // Send request.
    ssize_t res = sendto(robotSocket, request.data.bytes, request.data.length, 0, (struct sockaddr *)robotAddr, slen);
    
    if (res == -1)
    {
        [self freeResources];
        
        [self performSelectorOnMainThread:@selector(requestFailed:) withObject:request waitUntilDone:YES];
        
        [NSThread exit];
    }
    
    // Receive response.
    res = recvfrom(robotSocket, buffer, bufferLength, 0, (struct sockaddr *)clientAddr, &slen);

    if (res == -1)
    {
        [self freeResources];
        
        [self performSelectorOnMainThread:@selector(requestTimeout:) withObject:request waitUntilDone:YES];
        
        [NSThread exit];
    }
    else
    {
        RobotResponse *response = [[RobotResponse alloc] init];
        response.command = buffer[0];
        response.data = [NSData dataWithBytes:buffer length:res];
        response.length = (uint32_t)res;
        
        [self freeResources];
        
        [self performSelectorOnMainThread:@selector(requestReceivedResponse:)
                               withObject:@{
                                                @"request" : request,
                                                @"response" : response
                                            }
                            waitUntilDone:YES];
        
        [NSThread exit];
    }
}

- (void)requestTimeout:(RobotRequest *)request
{
    NSLog(@"#Request for command: %u timeout.", request.command);
    
    if ([self.delegate respondsToSelector:@selector(requestDidTimeOut:)])
    {
        [self.delegate requestDidTimeOut:request];
    }
    
    [self.requestsQueue removeObject:request];
    
    self.commThread = nil;
    
    [self sendNextRequestFromQueue];
}

- (void)requestReceivedResponse:(NSDictionary *)data
{
    RobotRequest *request = data[@"request"];
    RobotResponse *response = data[@"response"];
    
    NSLog(@"#Response for command: %u received:\n%@", response.command, response.data);
    
    if ([self.delegate respondsToSelector:@selector(request:didReceiveResponse:)])
    {
        [self.delegate request:request didReceiveResponse:response];
    }
    
    [self.requestsQueue removeObject:request];
    
    self.commThread = nil;
    
    [self sendNextRequestFromQueue];
}

- (void)requestFailed:(RobotRequest *)request
{
    NSLog(@"#Request for command: %u failed.", request.command);
    
    if ([self.delegate respondsToSelector:@selector(requestDidFail:)])
    {
        [self.delegate requestDidFail:request];
    }
    
    [self.requestsQueue removeObject:request];
    
    self.commThread = nil;
    
    [self sendNextRequestFromQueue];
}

- (void)freeResources
{
    close(robotSocket);
    free(robotAddr);
    free(clientAddr);
    robotSocket = 0;
    robotAddr = nil;
    clientAddr = nil;
}

- (void)dealloc
{
    [self freeResources];
}

@end
