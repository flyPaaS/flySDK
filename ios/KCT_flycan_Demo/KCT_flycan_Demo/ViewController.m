//
//  ViewController.m
//  KCT_flycan_Demo
//
//  Created by KCMac on 2017/8/24.
//  Copyright © 2017年 flypass. All rights reserved.
//

#import "ViewController.h"
#import "HttpRequestEngine.h"

#define kCallSdkId  @"callsdkid"
#define kCalledSdkId @"calledsdkid"

@interface ViewController ()
{
    flycan *flyEngin;
    int sessionId;
    int newSessionId;
    NSUserDefaults *userdefault;
    BOOL isServier;
    int count;
    NSMutableString *showString;
}

@property(nonatomic,weak)IBOutlet UITextField *call;
@property(nonatomic,weak)IBOutlet UITextField *called;
@property(nonatomic,weak)IBOutlet UITextView *statueLabel;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    showString = [[NSMutableString alloc] init];
    self.statueLabel.editable = NO;
    count = 0;
#ifdef kTestEnvironment
    NSString *accountSid = @"809d2c33e8ba713460083ee65de42a71";
    NSString *token = @"980e7daebdf7fc4aa6f55bda74b41d17";
    NSString *appid = @"47201f1d7bd043c18069375172f82572";
#else
    NSString *accountSid = @"b64e977c108810429b9056208059d362";
    NSString *token = @"cd1e4ce88775dcaf8bbf9236e9811c4a";
    NSString *appid = @"57993353d8724285904ba22a20d51ee9";
#endif
    
//    [[HttpRequestEngine engineInstance] applySDKID:accountSid appid:appid token:token successBlock:^(NSDictionary *responseDict) {
//        NSLog(@"resp %@",responseDict);
//    } failBlock:^(NSDictionary *responseDict) {
//        
//    }];
    
    userdefault = [NSUserDefaults standardUserDefaults];
    NSString *callid = [userdefault objectForKey:kCallSdkId];
    NSString *calledid = [userdefault objectForKey:kCalledSdkId];
    if (!callid) {
        callid = @"62395051197521";
    }
    if (!calledid) {
        calledid = @"62395051197520";
    }
    
#ifdef kTestEnvironment
    self.call.text = callid;
    self.called.text = calledid;
    
#else
    self.call.text = @"62508051197250";
    self.called.text = @"62508051197251";
#endif
    
    
    flyEngin = [[flycan alloc] init];
    flyEngin.delegate = self;
    
    NSString *sdkid = self.call.text;
    NSString *peerid = self.called.text;
    
    [flyEngin flycan_init:accountSid token:token appid:appid sdkid:sdkid];
    [userdefault setObject:sdkid forKey:kCallSdkId];
    [userdefault setObject:peerid forKey:kCalledSdkId];
    // Do any additional setup after loading the view, typically from a nib.
}


- (void)flycanEvent:(FlycanEventType)eventType
{
    NSString *showText;
    
    if (eventType == EVT_REGISTER_SUCCUSS) {
        sessionId = [flyEngin flycan_create_session:TransportProtocolUDP];
        [flyEngin flycan_session_listen:sessionId session_num:23];
        showText = [NSString stringWithFormat:@"注册成功 create sessionId: %d\n",sessionId];
    }
    if (eventType == EVT_SESSION_INCOMING) {
        isServier = YES;
        newSessionId = [flyEngin flycan_accept_session:sessionId];
        showText = [NSString stringWithFormat:@"收到 sessionId: %d 的请求连接\n",newSessionId];
        //[flyEngin flycan_reject_session:session];
    }
    if (eventType == EVT_SESSION_BIND_SUCCUSS) {
        showText = [NSString stringWithFormat:@"session bind success\n"];
    }
    if (eventType == EVT_SESSION_BIND_FAILURE) {
        showText = [NSString stringWithFormat:@"session bind fail\n"];
    }
    if (eventType == EVT_UNREGISTER) {
        showText = [NSString stringWithFormat:@"unRegister !!!\n"];
    }
    if (showText != NULL) {
        [self updateState:showText];
    }
}


- (void)flycanRecv:(int)sessionId2 buf:(void *)buf len:(int)len channel:(int)channel_indxe
{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *showText = [NSString stringWithFormat:@"data recv :%s size :%d sessionId :%d channel: %d\n",buf,len,sessionId2,channel_indxe];
        [self updateState:showText];
    });
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)login:(id)sender {
    [flyEngin flycan_register];
    NSString *showText = [NSString stringWithFormat:@"beed login...\n"];
    [self updateState:showText];
}


- (IBAction)connect:(id)sender {
    [flyEngin flycan_connect_session:sessionId peerId:self.called.text];
    NSString *showText = [NSString stringWithFormat:@"beed connecting sessionId: %d\n",sessionId];
    [self updateState:showText];
}

- (IBAction)disconnect:(id)sender {
    [flyEngin flycan_release_session:sessionId];
    NSString *showText = [NSString stringWithFormat:@"disconnect sessionId: %d\n",sessionId];
    [self updateState:showText];
}

- (IBAction)eixt:(id)sender {
    [flyEngin flycan_unRegister];
    //exit(0);
}

- (IBAction)sendData:(id)sender {
    NSString *str = @"flycan Sdk";
    NSData *data =  [str dataUsingEncoding:NSUTF8StringEncoding];
    int sendSessionId = sessionId;
    if (isServier) {
        sendSessionId = newSessionId;
    }
    [flyEngin flycan_send:sendSessionId data:data len:data.length port_index:-1];
    NSString *showText = [NSString stringWithFormat:@"send buffer: %@ sessionId: %d\n",str,sendSessionId];
    [self updateState:showText];
}

- (void)updateState:(NSString *)context {
    [showString appendString:context];
    self.statueLabel.text = showString;
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

@end
