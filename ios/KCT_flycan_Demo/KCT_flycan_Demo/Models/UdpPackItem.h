//
//  UdpPackItem.h
//  KCT_flycan_Demo
//
//  Created by KCMac on 2017/11/20.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UdpPackItem : NSObject
@property(nonatomic,assign)BOOL isRecv;
@property(nonatomic,assign)int seq;
@property(nonatomic,assign)double sendTimeSp;
@property(nonatomic,assign)double recvTimeSp;
@end
