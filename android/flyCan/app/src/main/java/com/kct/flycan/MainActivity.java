package com.kct.flycan;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.kct.flycan.sdk.listener.OnflyListener;
import com.kct.flycan.sdk.ssl.ByteUtil;

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

public class MainActivity extends AppCompatActivity implements OnflyListener{
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
    public TextView mTextInfo = null;
    // 常量
    public String sid = "b64e977c108810429b9056208059d362";
    public String token = "cd1e4ce88775dcaf8bbf9236e9811c4a";
    public String appid = "57993353d8724285904ba22a20d51ee9";
    // 变量
    public int nSessionId = 0;
    public int nMaxChannal = 0;

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
        mTextInfo = (TextView) findViewById(R.id.textInfo);
        // 按钮事件
        mBtnReg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        com.kct.flycan.sdk.flySDK.flycan_Init(sid, token, appid, mEditNorItem.getText().toString());
                        com.kct.flycan.sdk.flySDK.flycan_AddCallBack(MainActivity.this);
                        com.kct.flycan.sdk.flySDK.flycan_Register();
                    }
                }).start();
            }
        });
        mBtnCrt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        nSessionId = com.kct.flycan.sdk.flySDK.flycan_CreateSession(0);
                        Log.e(TAG, "flycan_CreateSession id = " + nSessionId);
                        // 延时2秒监听
                        if (nSessionId != 0) {
                            try {
                                Thread.sleep(2000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            com.kct.flycan.sdk.flySDK.flycan_ListenSession(nSessionId, 5);
                        }
                    }
                }).start();
            }
        });
        mBtnCon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        com.kct.flycan.sdk.flySDK.flycan_ConnectSession(nSessionId, mEditSubItem.getText().toString());
                    }
                }).start();
            }
        });
        mBtnSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (nMaxChannal > 0) {
                            String szData = "Only you can love me!";
                            com.kct.flycan.sdk.flySDK.flycan_Send(nSessionId, szData.getBytes(), szData.length(), 0);
                        }
                    }
                }).start();
            }
        });
        mBtnUnCon.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (nSessionId != 0) {
                            com.kct.flycan.sdk.flySDK.flycan_ReleaseSession(nSessionId);
                        }
                    }
                }).start();
            }
        });
        mBtnUnReg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        com.kct.flycan.sdk.flySDK.flycan_UnRegister();
                        com.kct.flycan.sdk.flySDK.flycan_UnInit();
                    }
                }).start();

            }
        });
    }

    @Override
    public void CallBackEvent(int nEvent, int nCode, byte[] pData) {
        Log.e(TAG, "nEvent = " + nEvent + ", nCode = " + nCode);
        mHandler.sendEmptyMessage(nEvent);
        // 分析事件
        if (nEvent == EVT_SESSION_INCOMING) {
            nMaxChannal = nCode;
            com.kct.flycan.sdk.flySDK.flycan_AcceptSession(ByteUtil.Byte2Int(pData));
            //com.kct.flycan.sdk.flySDK.flycan_RejectSession(ByteUtil.Byte2Int(pData));
        }
        if (nEvent == EVT_SESSION_ACCEPT) {
            nMaxChannal = nCode;
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
        Message msg = mHandler.obtainMessage(10000);
        msg.setData(bundle);
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
            if (msg.what == 10000) {
                Bundle bundle = msg.getData();
                if (bundle != null) {
                    int nChannal = bundle.getInt("channal");
                    String str = bundle.getString("data");
                    str = "接收到数据 channal = " + nChannal + " data = " + str;
                    mTextInfo.setText(str);
                }
            }
        }
    };
}
