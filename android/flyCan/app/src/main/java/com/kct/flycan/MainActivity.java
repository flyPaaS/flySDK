package com.kct.flycan;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.kct.flycan.sdk.flySDK;
import com.kct.flycan.sdk.ice.IceListener;
import com.kct.flycan.sdk.listener.OnflyListener;
import com.kct.flycan.sdk.ssl.ByteUtil;
import com.kct.flycan.sdk.utils.NetUtils;

import java.util.ArrayList;

import static com.kct.flycan.sdk.flyBase.EVT_LOGINOUT_SUCCUSS;
import static com.kct.flycan.sdk.flyBase.EVT_REGISTER_FAILURE;
import static com.kct.flycan.sdk.flyBase.EVT_REGISTER_SUCCUSS;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_ACCEPT;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_BIND_FAILURE;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_BIND_SUCCUSS;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_INCOMING;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_LISTEN_FAILURE;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_LISTEN_SUCCUSS;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_REJECT;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_RELEASE_FAILURE;
import static com.kct.flycan.sdk.flyBase.EVT_SESSION_RELEASE_SUCCUSS;

public class MainActivity extends AppCompatActivity implements OnflyListener, IceListener{
    // TAG
    private static final String TAG = "KC";
    // 控件
    public EditText mEditNorItem = null;
    public EditText mEditSubItem = null;
    public Button mBtnReg = null;
    public Button mBtnCrt = null;
    public Button mBtnCon = null;
    public Button mBtnSend = null;
    public Button mBtnUnCon = null;
    public Button mBtnUnReg = null;
    public Button mBtnSendBig = null;

    public TextView mTextInfo = null;


    // 常量, production
    public String sid = "b64e977c108810429b9056208059d362";
    public String token = "cd1e4ce88775dcaf8bbf9236e9811c4a";
    public String appid = "57993353d8724285904ba22a20d51ee9";

    //test
//    public String sid ="809d2c33e8ba713460083ee65de42a71";
//    public String token = "980e7daebdf7fc4aa6f55bda74b41d17";
//    public String appid = "cac7d7ae1d3040b886b2766d69ce11b3";
    // 变量
    public int nSessionId = 0;
    public int nMaxChannal = 0;
    // 主被叫
    public boolean bClient = true;

    /**
     * 当前状态
     */
    public int status = STATUS_INIT;

    /**
     * 初始状态
     */
    public static int STATUS_INIT = 0;

    /**
     * 已注册
     */
    public static int STATUS_REGISTERED = 1;

    /**
     * 已创建会话
     */
    public static int STATUS_SESSION_CREATED = 2;

    /**
     * 客户端连接中
     */
    public static int STATUS_CLIENT = 3;
    /**
     * 服务器连接中
     */
    public static int STATUS_SERVER = 4;


    /**
     * Is any command running
     */
    public static boolean workingStatus=false;

    /**
     * Is sending big data running
     */
    public static boolean runningBigData = false;

    /**
     * Max count of sending big data
     */
    public static int BIG_DATA_COUNT = 1000;


    /**
     * Receiving data
     */
    public static int MSG_RECEIVING_DATA = 10000;

    /**
     * Message that start sending big data
     */
    public static int MSG_START_SENDING_BIG_DATA = 10001;
    /**
     * Message that stop sending big data
     */
    public static int MSG_STOP_SENDING_BIG_DATA = 10002;
    /**
     * Message that send big data
     */
    public static int MSG_SEND_BIG_DATA = 10003;

    // 连接的会话ID
    public ArrayList<Integer> mSessionList = new ArrayList<>();
    /**
     * Counter for message
     */
    public static int counter = 0;

    //获取指定位数的随机字符串(包含小写字母、大写字母、数字,0<length)
    public static String getRandomString(int length) {
        //随机字符串的随机字符库
        String KeyString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        StringBuffer sb = new StringBuffer();
        int len = KeyString.length();
        for (int i = 0; i < length; i++) {
            sb.append(KeyString.charAt((int) Math.round(Math.random() * (len - 1))));
        }
        return sb.toString();
    }
    public String genBigData(int index) {
        String randomString = getRandomString(2000);
        return String.format("%04d_%s", index, randomString);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // 控件
        mEditNorItem = (EditText) findViewById(R.id.editView1);
        mEditSubItem = (EditText) findViewById(R.id.editView2);
        mBtnReg = (Button) findViewById(R.id.btnReg);
        mBtnCrt = (Button) findViewById(R.id.btnCreate);
        mBtnCon = (Button) findViewById(R.id.btnConnect);
        mBtnSend = (Button) findViewById(R.id.btnSend);
        mBtnUnCon = (Button) findViewById(R.id.btnUnConnect);
        mBtnUnReg = (Button) findViewById(R.id.btnUnReg);
        mBtnSendBig = (Button) findViewById(R.id.btnSendBig);
        mTextInfo = (TextView) findViewById(R.id.textInfo);

        // 按钮事件
        mBtnReg.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_INIT) {
                    String str = "已经注册过";
                    mTextInfo.setText(str);
                    return;
                }

                setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        status = STATUS_REGISTERED;
                        flySDK.getInstance().flycan_Init(sid, token, appid, mEditNorItem.getText().toString(), flySDK.MODE_P2P_FLYCAN);
                        //flySDK.getInstance().flycan_Init(sid, token, appid, mEditNorItem.getText().toString(), flySDK.MODE_FLYCAN);
                        flySDK.getInstance().flycan_AddCallBack(MainActivity.this);
                        flySDK.getInstance().flycan_AddIceCallBack(MainActivity.this);
                        flySDK.getInstance().flycan_Register();
                        setWorkingStatus(false);

                    }
                }).start();
            }
        });
        mBtnCrt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //if (nSessionId != 0) {

                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_REGISTERED) {
                    String str = "已经创建过会话或未注册";
                    mTextInfo.setText(str);
                    return;
                }
                 setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        status = STATUS_SESSION_CREATED;
                        nSessionId = com.kct.flycan.sdk.flySDK.getInstance().flycan_CreateSession(0);
                        Log.e(TAG, "flycan_CreateSession session id = " + nSessionId);

                        // 延时2秒监听
                        if (nSessionId != 0) {
                            try {
                                Thread.sleep(2000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            com.kct.flycan.sdk.flySDK.getInstance().flycan_ListenSession(nSessionId);
                        }
                        setWorkingStatus(false);

                    }
                }).start();
            }
        });
        mBtnCon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_SESSION_CREATED) {
                    String str = "未创建会话或已经在连接中";
                    mTextInfo.setText(str);
                    return;
                }
                setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        status = STATUS_CLIENT;
                        com.kct.flycan.sdk.flySDK.getInstance().flycan_ConnectSession(nSessionId, mEditSubItem.getText().toString());
                        setWorkingStatus(false);
                    }
                }).start();
            }
        });
        mBtnSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_CLIENT && status != STATUS_SERVER) {
                    String str = "未连接";
                    mTextInfo.setText(str);
                    return;
                }
                setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (nMaxChannal > 0) {

                            String szData = "Only you can love me! count:" + counter++;
                            int channel = 0;
                            if (flySDK.getInstance().needIce()) {
                                channel = flySDK.CHANNEL_ID_AUTO;
                            }
                            if (bClient) {
                                com.kct.flycan.sdk.flySDK.getInstance().flycan_Send(nSessionId, szData.getBytes(), szData.length(), channel);
                            } else {
                                for (int i = 0; i < mSessionList.size(); i++) {
                                    com.kct.flycan.sdk.flySDK.getInstance().flycan_Send(mSessionList.get(i), szData.getBytes(), szData.length(), channel);
                                }
                            }
                        }
                        setWorkingStatus(false);
                    }
                }).start();
            }
        });
        mBtnSendBig.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_CLIENT && status != STATUS_SERVER) {
                    String str = "未连接";
                    mTextInfo.setText(str);
                    return;
                }
                setWorkingStatus(true);
                if (runningBigData) {
                    //stop running
                    runningBigData = false;
                    mBtnSendBig.setText(R.string.app_text_btn_send_big);
                    setWorkingStatus(false);
                    return;
                }
                runningBigData = true;

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (nMaxChannal > 0) {

                            //String szData = "Only you can love me! count:" + counter++;
                            int index = 0;
                            sendMessageStartSendingBigData();
                            while (runningBigData && index++ < BIG_DATA_COUNT) {
                                String szData = genBigData(index);
                                int channel = 0;
                                if (flySDK.getInstance().needIce()) {
                                    channel = flySDK.CHANNEL_ID_AUTO;
                                }
                                if (bClient) {
                                    com.kct.flycan.sdk.flySDK.getInstance().flycan_Send(nSessionId, szData.getBytes(), szData.length(), channel);
                                } else {
                                    for (int i = 0; i < mSessionList.size(); i++) {
                                        com.kct.flycan.sdk.flySDK.getInstance().flycan_Send(mSessionList.get(i), szData.getBytes(), szData.length(), channel);
                                    }
                                }
                                sendMessageSendBigData(channel, index, szData);
                                try {
                                    Thread.sleep(5);
                                } catch (InterruptedException e) {
                                    Log.e(TAG, "InterruptedException", e);
                                }
                            }
                            //complete sending big data
                            runningBigData = false;

                            sendMessageStopSendingBigData();

                        }

                    }
                }).start();
                setWorkingStatus(false);
            }
        });

        mBtnUnCon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }
                if (status != STATUS_CLIENT && status != STATUS_SERVER && status != STATUS_SESSION_CREATED) {
                    String str = "未创建会话";
                    mTextInfo.setText(str);
                    return;
                }
                setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        status = STATUS_REGISTERED;
                        //if (bClient) {
                            if (nSessionId != 0) {
                                com.kct.flycan.sdk.flySDK.getInstance().flycan_ReleaseSession(nSessionId);
                                nSessionId = 0;
                            }
                            bClient = false;
                        //} else {
                            for (int i = 0;i < mSessionList.size();i++) {
                                int nSession = mSessionList.get(i);
                                if (nSession != 0) {
                                    com.kct.flycan.sdk.flySDK.getInstance().flycan_ReleaseSession(nSession);
                                }
                            }
                            mSessionList.clear();
                        //}
                        setWorkingStatus(false);
                    }
                }).start();
            }
        });
        mBtnUnReg.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                if (workingStatus) {
                    String str = "正在处理中，请稍候";
                    mTextInfo.setText(str);
                    return;
                }

                if (status == STATUS_CLIENT || status == STATUS_SERVER ) {
                    String str = "请先断连";
                    mTextInfo.setText(str);
                    return;
                }

                setWorkingStatus(true);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        status = STATUS_INIT;
                        com.kct.flycan.sdk.flySDK.getInstance().flycan_UnRegister();
                        com.kct.flycan.sdk.flySDK.getInstance().flycan_UnInit();
                        com.kct.flycan.sdk.flySDK.getInstance().flycan_RemoveCallBack(MainActivity.this);
                        com.kct.flycan.sdk.flySDK.getInstance().flycan_RemoveIceCallBack(MainActivity.this);
                        setWorkingStatus(false);
                    }
                }).start();

            }
        });
    }

    public void setWorkingStatus(boolean workingStatus) {
        Log.i(TAG, "set working status:" + workingStatus);
        this.workingStatus = workingStatus;
    }

    @Override
    public void CallBackEvent(int nEvent, int nCode, byte[] pData, Object... objs) {
        Log.e(TAG, "nEvent = " + nEvent + ", nCode = " + nCode);
        mHandler.sendEmptyMessage(nEvent);
        // 分析事件
        if (nEvent == EVT_SESSION_INCOMING) {
            status = STATUS_SERVER;
            nMaxChannal = nCode;
            int nSession = ByteUtil.Byte2Int(pData);
            bClient = false;
            // 接收会话
            com.kct.flycan.sdk.flySDK.getInstance().flycan_AcceptSession(nSession);

            mSessionList.add(nSession);
            // 拒绝会话
            //com.kct.flycan.sdk.flySDK.flycan_RejectSession(nSession);

        }
        if (nEvent == EVT_SESSION_ACCEPT) {
            nMaxChannal = nCode;
            bClient = true;
        }
        if (nEvent == EVT_SESSION_REJECT) {

        }

    }

    @Override
    public void CallBackRecv(int pHandle, byte[] pData, int nLen, int nChannalId) {
        String strTmp = new String(pData);
        strTmp = strTmp.substring(0, nLen);
        Log.e(TAG, "EVT_PACKET_RCVD data = " + strTmp + ", nChannalId = " + nChannalId);

        Bundle bundle = new Bundle();
        bundle.putInt("channal", nChannalId);
        bundle.putString("data", strTmp);
        Message msg = mHandler.obtainMessage(MSG_RECEIVING_DATA);
        msg.setData(bundle);
        mHandler.sendMessage(msg);
    }

    public void sendMessageSendBigData(int nChannalId, int index, String data) {
        Message msg = mHandler.obtainMessage(MSG_START_SENDING_BIG_DATA);
        Bundle bundle = new Bundle();
        bundle.putString("data", data);
        bundle.putInt("index", index);
        bundle.putInt("channal", nChannalId);
        mHandler.sendMessage(msg);
    }
    public void sendMessageStartSendingBigData() {
        Message msg = mHandler.obtainMessage(MSG_START_SENDING_BIG_DATA);
        mHandler.sendMessage(msg);
    }

    public void sendMessageStopSendingBigData() {
        Message msg = mHandler.obtainMessage(MSG_STOP_SENDING_BIG_DATA);
        mHandler.sendMessage(msg);
    }

    @SuppressLint("HandlerLeak")
    public Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == EVT_REGISTER_SUCCUSS) {
                String str = "SDK注册成功";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_REGISTER_FAILURE) {
                String str = "SDK注册失败";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_LOGINOUT_SUCCUSS) {
                String str = "SDK注销成功";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_BIND_SUCCUSS) {
                String str = "会话绑定成功";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_BIND_FAILURE) {
                String str = "会话绑定失败";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_LISTEN_SUCCUSS) {
                String str = "会话监听成功";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_LISTEN_FAILURE) {
                String str = "会话监听失败";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_RELEASE_SUCCUSS) {
                String str = "会话释放成功";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_RELEASE_FAILURE) {
                String str = "会话释放失败";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_INCOMING) {
                String str = "接收到连接请求";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_ACCEPT) {
                String str = "会话连接确认";
                mTextInfo.setText(str);
            }
            if (msg.what == EVT_SESSION_REJECT) {
                String str = "会话连接被拒绝";
                mTextInfo.setText(str);
            }
            if (msg.what == MSG_RECEIVING_DATA) {
                Bundle bundle = msg.getData();
                if (bundle != null) {
                    int nChannal = bundle.getInt("channal");
                    String str = bundle.getString("data");
                    str = "接收到数据 channal = " + nChannal + " data = " + str;
                    mTextInfo.setText(str);
                }
            }
            if (msg.what == MSG_SEND_BIG_DATA) {
                Bundle bundle = msg.getData();
                if (bundle != null) {
                    int index = bundle.getInt("index");
                    int nChannal = bundle.getInt("channel");
                    String data = bundle.getString("data");
                    String str = "发送数据  channel= " + nChannal + "index = " + index + " data = " + data;
                    mTextInfo.setText(str);
                }
            }
            if (msg.what == MSG_START_SENDING_BIG_DATA) {
                mBtnSendBig.setText(R.string.app_text_btn_send_big_stop);
            }
            if (msg.what == MSG_STOP_SENDING_BIG_DATA) {
                mBtnSendBig.setText(R.string.app_text_btn_send_big);
            }

        }
    };

    @Override
    public void onRxData(int sessionId, int compid, byte[] data, String sourceaddr) {
        //do nothing, since CallBackRecv is also called
    }

    @Override
    public void onIceCompleted(int sessionId, int op, int status) {
        int nettype = NetUtils.getNetWorkState(this);
        String nettypeName = NetUtils.getNetWorkStateById(nettype);
        Log.i(TAG, "onIceCompleted, op=" + op + ",status=" + status + ",nettype=" + nettype + ",nettypeName=" + nettypeName);
    }

    @Override
    public void onNatDetected(int status, int nattype) {
        int nettype = NetUtils.getNetWorkState(this);
        String nettypeName = NetUtils.getNetWorkStateById(nettype);
        Log.i(TAG, "onNatDetected, nattype=" + nattype + ",status=" + status + ",nettype=" + nettype + ",nettypeName=" + nettypeName);

    }

    @Override
    public void punchOnResult(int sessionId, int status) {
        //do nothing
    }

    @Override
    public void onNotifyPeer(int sessionId, byte[] data) {
        Log.i(TAG, "onNotifyPeer, data=" + new String(data));

    }
}
