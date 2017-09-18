//
//  flycan.h
//  flycan
//
//  Created by KCMac on 2017/8/17.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    TransportProtocolUDP = 0,
    TransportProtocolTCP = 1,
    TransportProtocolRTP = 2,
    TransportProtocolUnknow = 255,
}TransportProtocol;

typedef enum {
    EVT_REGISTER_SUCCUSS = 0,//注册成功
    EVT_REGISTER_FAILURE = 1,//注册失败
    EVT_SESSION_BIND_SUCCUSS = 2,//会话绑定成功
    EVT_SESSION_BIND_FAILURE = 3,//会话绑定失败
    EVT_SESSION_CONNECTED_SUCCESS = 4,//会话连接成功
    EVT_SESSION_CONNECTED_FAILUER = 5,//会话连接失败
    EVT_SESSION_INCOMING = 6,//会话连接请求事件
    EVT_PACKET_RCVD = 7,//会话数据接收事件
    EVT_SESSION_DISCONNED = 8,//会话已经断开事件
    EVT_UNREGISTER = 9,//注销成功
}FlycanEventType;


@protocol flycanDelegate <NSObject>

- (void)flycanEvent:(FlycanEventType)eventType;
- (void)flycanRecv:(int)sessionId buf:(void *)buf len:(int)len channel:(int)channel_indxe;

@end



@interface flycan : NSObject

@property (nonatomic,assign) id<flycanDelegate>delegate;

/*
 功能：SDK本地初始化
 输出参数：
 返回StateCodeSuccess:成功，StateCodeFail:失败
 */
- (void)flycan_init:(NSString *)sid token:(NSString *)token appid:(NSString *)appid sdkid:(NSString *)sdkid;

/*
 功能: 向flyCan注册，结果通过回调异步返回
 输入参数:
 无
 返回参数:
 无,成功失败通过回调消息返回
 EVT_REGISTER_SUCCUSS		成功
 EVT_REGISTER_FAILURE		失败

 */
- (void)flycan_register;

/*
 
 */
- (int)flycan_create_session:(TransportProtocol)protocol;

/*
 
 */
- (void)flycan_session_listen:(int)sessionId session_num:(int)session_num;

/*
 
 */
- (void)flycan_connect_session:(int)sessionId peerId:(NSString *)peerId;

/*
 
 */
- (int)flycan_accept_session:(int)sessionId;

/*
 
 */
- (void)flycan_reject_session:(int)sessionId;

/*
 
 */
- (int)flycan_send:(int)sessionId data:(NSData *)data len:(NSInteger)len port_index:(NSInteger)port_index;

/*
 
 */
- (void)flycan_release_session:(int)sessionId;

/*
 功能: SDK发送注销请求,断开和flyCan的连接
 */
- (void)flycan_unRegister;

@end
