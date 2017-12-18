# flyCAN SDK和示例

本项目包括flyCAN SDK库(sdk-release.aar)以及一个简单的使用示例。

## 编译过程

### 准备步骤

下载源代码

``` bash
git clone https://github.com/flyPaaS/flySDK.git

```

### 编译项目

编译调试版本
``` bash
gradle assembleDebug
```


## flycan接入流程

请参见文档：
[flyCAN 开发者接入手册1.1.docx](https://github.com/flyPaaS/flyAPI/raw/master/Rest/doc/flyCAN 开发者接入手册1.1.docx)


## flySDK使用流程

### 客户端使用流程
- 调用flycan_Init初始化SDK
- 调用flycan_AddCallBack 和 flycan_IceAddCallBack注册回调函数
- 调用flycan_Register向flyCan注册
- 上述步骤成功后调用flycan_CreateSession创建一个会话
- 调用flycan_ListenSession监听创建的会话（也可以不调用）
- 调用flycan_ConnectSession发起会话连接
- 接收到被叫EVT_SESSION_ACCEPT消息，握手成功，可以通信
- 调用flycan_Send进行发送数据,接收数据通过回调返回
- 接收到被叫EVT_SESSION_REJECT消息，握手不成功，连接断开
- 通信完成后，调用flycan_ReleaseSession关闭创建的会话
- 调用flycan_UnRegister注销释放资源
- 调用flycan_UnInit()去初始化


### 服务端使用流程
- 调用flycan_Init初始化SDK
- 调用flycan_AddCallBack增加回调函数
- 调用flycan_Register向flyCan注册
- 上述步骤成功后调用flycan_CreateSession创建一个会话
- 调用flycan_ListenSession监听创建的会话
- 接收到EVT_SESSION_INCOMING消息，等待响应
- 调用flycan_AcceptSession接收连接请求,握手成功，可以通信
- 调用flycan_Send进行发送数据,接收数据通过回调返回
- 调用flycan_RejectSession拒绝连接请求,握手不成功，连接断开
- 通信完成后，调用flycan_ReleaseSession关闭创建的会话
- 调用flycan_UnRegister注销释放资源
- 调用flycan_UnInit()去初始化



## flySDK相关类使用参考

### flySDK类
```java
    /**
     * SDK初始化，sid/token/appid由flycan运营平台生成，sdkid由开发者应用生成，用于标识终端用户.
     * @param sid sid
     * @param token token
     * @param appid appid
     * @param sdkid sdkid
     * @param mode 0(MODE_FLYCAN)表示仅中转网络, 1(MODE_P2P_FLYCAN)表示P2P优先，如不通则走中转网络
     */
    public synchronized void flycan_Init(String sid, String token, String appid, String sdkid, int mode);

    /**
     * SDK反初始化
     */    
    public synchronized void flycan_UnInit();

    /**
     * 设置事件和数据回调
     * @param cb 回调监听对象
     */
    public synchronized void flycan_AddCallBack(OnflyListener cb);

    /**
     * 删除回调
     * @param cb 回调监听对象
     */
    public synchronized void flycan_RemoveCallBack(OnflyListener cb);

    /**
     * 设置ICE(即P2P)相关事件回调
     * 注：数据接收回调onRxData在OnflyListener和IceListener中均有实现，但在IceListener中有更多的信息(sourceaddr),上层可根据业务需求选择回调的入口
     * @param cb
     */
    public synchronized void flycan_AddIceCallBack(IceListener cb);


    /**
     *
     * 删除ICE(即P2P)相关事件回调
     * @param cb IceListener回调监听对象
     */
    public synchronized void flycan_RemoveIceCallBack(IceListener cb);

    /**
     * 账号注册
     */
    public synchronized void flycan_Register();



    /**
     * 账号注销
     */
    public synchronized void flycan_UnRegister();

    /**
     * 创建一个指定协议会话,返回会话句柄
     * @param nProtocol 协议编号，目前仅支持0(udp)
      * @return
     */
    public synchronized int flycan_CreateSession(int nProtocol);

    /**
     * 释放会话
     * @param handle 会话句柄
     */
    public synchronized void flycan_ReleaseSession(int handle);

    /**
     * 监听会话(作为服务器端调用)
     * @param handle
     */
    public synchronized void flycan_ListenSession(int handle);


    /**
     * 连接服务器
     * @param handle 会话句柄
     * @param strip IP地址
     * @param port 端口
     * @return
     */
    public synchronized boolean flycan_ConnectServer(int handle, String strip, int port);


    /**
     * 接收来自对方的连接请求，作为服务器端接收到连接请求时调用
     * @param handle
     * @param objs
     * @return
     */

    public synchronized boolean flycan_AcceptSession(int handle);

    /**
     * 拒绝对方的连接请求
     * @param handle
     * @return
     */
    public synchronized boolean flycan_RejectSession(int handle);


    /**
     * 发送数据给对方.
     * 注：接收数据通过回调返回
     * @param handle 会话句柄
     * @param pData 需要发送的数据字节数组
     * @param nLen 数据字节长度
     * @param nChannalId 数据通道 0-7为中转通道, 10001为P2P通道, -1(CHANNEL_ID_AUTO，推荐使用)为自动选择
     * @return
     */
    public synchronized boolean flycan_Send(int handle, byte[] pData, int nLen, int nChannalId);

```

### IceListner接口

```java

/**
 * P2P(ICE)事件监听器
 */
public interface IceListener {

    /**
     * 接收到数据回调方法
     * 注：P2P通道接收到数据后，onflyListener中的onRxData也会被回调，应用可根据需要实现回调
     *
     * @param sessionId 会话句柄
     * @param compid P2P内部通道ID，从1开始。注：flycanListener中的通道ID为本compid加上10000
     * @param data 接收到的数据字节数组
     * @param sourceaddr 对方源IP地址和端口，格式IP:Port
     */
    void onRxData(int sessionId, int compid, byte[] data, String sourceaddr);


    /**
     * P2P(ICE)通道协商完成后的回调方法.
     *
     * @param sessionId 会话句柄
     * @param op 操作ID  0(PJ_ICE_STRANS_OP_INIT, 初始化), 1(PJ_ICE_STRANS_OP_NEGOTIATION,协商),2(PJ_ICE_STRANS_OP_ADDR_CHANGE,IP地址变化)
     * @param status 状态 0为成功 >0为ICE错误码
     */
    void onIceCompleted(int sessionId, int op, int status);

    /**
     *  NAT类型检测完成.
     *
     * @param status 0 成功, >0 失败
     * @param nattype  0(UNKNOWN),1(ERR_UNKNOWN),2(OPEN),3(BLOCKED),4(FULL_CONE),5(SYMMETRIC),6(RESTRICTED),7(PORT_RESTRICTED)
     */
    void onNatDetected(int status, int nattype);

}

```

### OnflyLsstener接口

```java
/**
 * flycan事件回调监听器
 */
public interface OnflyListener {
    /**
     * 事件回调
     * @param nEvent 事件ID
     * @param nCode 可用通道数目
     * @param pData 会话句柄（以字节数组表示）
     * @param objs 事件自定义对象
     */
    void CallBackEvent(int nEvent, int nCode, byte[] pData, Object... objs);

    /**
     * 接收数据回调函数
     * @param nSession 会话句柄
     * @param pData 接收到的数据字节数组
     * @param nLen 数据长度
     * @param nChannalId 数据通道ID 0-7为中转通道, 10001为P2P通道
     */
    void CallBackRecv(int nSession, byte[] pData, int nLen, int nChannalId);
}
```
