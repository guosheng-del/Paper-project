package com.example.aliyunapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;


public class MainActivity extends AppCompatActivity {

    private MqttClient client;
    private MqttConnectOptions options;
    private Handler handler;
    private ScheduledExecutorService scheduler;

    //***************阿里云配置开始*******************//
    //阿里云三元组
    private String productKey = "k2806c8lYSI";
    private String deviceName = "APPDUAN";
    private String deviceSecret = "28d85a5e9df32d0071dac9e213258f11";
    //Topic
    private final String pub_topic = "/k2806c8lYSI/APPDUAN/user/APPDUAN";
    private final String sub_topic ="/k2806c8lYSI/APPDUAN/user/get";
//    private final String clientid = "a1ydvjfxdXg.wechat|securemode=2,signmethod=hmacsha256,timestamp=1732586176114|";
//***************阿里云配置结束*******************//


    private TextView Number;
    private TextView Font;
    private TextView hourInput;
    private TextView minuteInput;
    private TextView secondInput;
    private  TextView Money;
    private RadioGroup radioGroup;
    private RadioButton selectedRadioButton;
    private int statusCode4 = 0;
    private int RenstatusCode = 0;
    private int WaterstatusCode = 0;
    private int LedstatusCode = 0;

    private int PlaystatusCode=0;



    private EditText highestSmogEditText;
    private EditText highestTempEditText;
    private EditText highesthumiEditText;
    private EditText highestco2EditText;

    @SuppressLint("MissingInflatedId")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

          //控制设备前后台绑定
        Font = findViewById(R.id.Font);
        Number = findViewById(R.id.Number);






        Money = findViewById(R.id.Money);


        mqtt_init();
        start_reconnect();

        //省份与unicode


        handler = new Handler() {

            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1: //开机校验更新回传
                        break;
                    case 2:  // 反馈回传
                        break;
                    case 3:  //MQTT 收到消息回传   UTF8Buffer msg=new UTF8Buffer(object.toString());
                        String message = msg.obj.toString();
                        Log.d("nicecode", "handleMessage: " + message);

                        try {
                            if(message.contains("hour")) {


                                HashMap<String, String> provinceCodeMap = new HashMap<>();
                                addProvinceCode(provinceCodeMap);

                                JSONObject jsonObject = new JSONObject(message);
                                String font = jsonObject.getString("font");
                                String number = jsonObject.getString("number");
                                int flag = jsonObject.getInt("flag");
                                String hour = jsonObject.getString("hour");
                                String minute = jsonObject.getString("minute");
                                String sec = jsonObject.getString("sec");
                                String money = jsonObject.getString("money");





                                String provinceAbbreviation = provinceCodeMap.get(font); // 添加这行代码

                                TextView FontStatusTextView = findViewById(R.id.Font);
                                TextView NumberStatusTextView = findViewById(R.id.Number);
                                TextView FlagStatusTextView = findViewById(R.id.Flag);
                                TextView hourInputStatusTextView = findViewById(R.id.mytext);
                                TextView MoneyStatusTextView = findViewById(R.id.Money);


                                String Flagsatus = "入场";
                                switch (flag) {
                                    case 0:
                                        Flagsatus = "出场";
                                        break;
                                    case 1:
                                        Flagsatus = "入场";
                                        break;
                                }

                                FontStatusTextView.setText(provinceAbbreviation);
                                NumberStatusTextView.setText(number);
                                FlagStatusTextView.setText(Flagsatus);
                                hourInputStatusTextView.setText(hour + ":" + minute + ":" + sec);
                                MoneyStatusTextView.setText(money);



                            }else{
                                HashMap<String, String> provinceCodeMap = new HashMap<>();
                                addProvinceCode(provinceCodeMap);

                                JSONObject jsonObject = new JSONObject(message);

                                String number = jsonObject.getString("number");
                                int flag = jsonObject.getInt("flag");
                                String font = jsonObject.getString("font");


                                TextView FontStatusTextView = findViewById(R.id.Font);
                                TextView NumberStatusTextView = findViewById(R.id.Number);
                                TextView FlagStatusTextView = findViewById(R.id.Flag);
                                TextView hourInputStatusTextView = findViewById(R.id.mytext);
                                TextView MoneyStatusTextView = findViewById(R.id.Money);

                                String provinceAbbreviation = provinceCodeMap.get(font); // 添加这行代码

                                String Flagsatus = "入场";
                                switch (flag) {
                                    case 0:
                                        Flagsatus = "出场";
                                        break;
                                    case 1:
                                        Flagsatus = "入场";
                                        break;
                                }

                                FontStatusTextView.setText(provinceAbbreviation);
                                NumberStatusTextView.setText(number);
                                FlagStatusTextView.setText(Flagsatus);
                                hourInputStatusTextView.setText("0");
                                MoneyStatusTextView.setText("0");
                            }
                        } catch (JSONException e) {
                            throw new RuntimeException(e);
                        }
                        break;
                    case 30:  //连接失败
                        Toast.makeText(MainActivity.this, "连接失败", Toast.LENGTH_SHORT).show();
                        break;
                    case 31:   //连接成功
                        Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                        try {
                           client.subscribe(sub_topic, 1);
                        } catch (MqttException e) {
                            e.printStackTrace();
                        }
                        break;
                    default:
                        break;
                }
            }
        };


    }



    public void onSendButtonClicked(View view) throws JSONException {

         //阈值设备绑定

        highestTempEditText = findViewById(R.id.highestTemperatureEditText1);



        String temp = highestTempEditText.getText().toString();
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("price", temp+",");
        String jsonString = jsonObject.toString();
        publish_message(jsonString);



        }




    private void mqtt_init() {
        try {
            //阿里云配置
            String clientId = "k2806c8lYSI.APPDUAN";

            Map<String, String> params = new HashMap<String, String>(16);
            params.put("productKey", productKey);
            params.put("deviceName", deviceName);
            params.put("clientId", clientId);
            String timestamp = String.valueOf(System.currentTimeMillis());
            params.put("timestamp", timestamp);
            // cn-shanghai
            String host_url = "tcp://" + productKey + ".iot-as-mqtt.cn-shanghai.aliyuncs.com:1883";
            String client_id = clientId + "|securemode=2,signmethod=hmacsha1,timestamp=" + timestamp + "|";
            String user_name = deviceName + "&" + productKey;
            //example.自己的名字 参考package com.example.mqqttest;
            String password = com.example.aliyunapplication.AliyunIoTSignUtil.sign(params, deviceSecret, "hmacsha1");

            //host为主机名，test为clientid即连接MQTT的客户端ID，一般以客户端唯一标识符表示，MemoryPersistence设置clientid的保存形式，默认为以内存保存
            System.out.println(">>>" + host_url);
            System.out.println(">>>" + client_id);

            //connectMqtt(targetServer, mqttclientId, mqttUsername, mqttPassword);

            client = new MqttClient(host_url, client_id, new MemoryPersistence());
            //MQTT的连接设置
            options = new MqttConnectOptions();
            //设置是否清空session,这里如果设置为false表示服务器会保留客户端的连接记录，这里设置为true表示每次连接到服务器都以新的身份连接
            options.setCleanSession(false);
            //设置连接的用户名
            options.setUserName(user_name);
            //设置连接的密码
            options.setPassword(password.toCharArray());
            // 设置超时时间 单位为秒
            options.setConnectionTimeout(10);
            // 设置会话心跳时间 单位为秒 服务器会每隔1.5*20秒的时间向客户端发送个消息判断客户端是否在线，但这个方法并没有重连的机制
            options.setKeepAliveInterval(60);
            //设置回调
            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    //连接丢失后，一般在这里面进行重连
                    System.out.println("connectionLost----------");
                }

                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    //publish后会执行到这里
                    System.out.println("deliveryComplete---------" + token.isComplete());
                }

                @Override
                public void messageArrived(String topicName, MqttMessage message)
                        throws Exception {
                    //subscribe后得到的消息会执行到这里面
                    System.out.println("messageArrived----------");
                    Message msg = new Message();
                    //封装message包
                    msg.what = 3;   //收到消息标志位
                    msg.obj = message.toString();
                    //发送messge到handler
                    handler.sendMessage(msg);    // hander 回传
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void mqtt_connect() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if (!(client.isConnected()))  //如果还未连接
                    {
                        client.connect(options);
                        Message msg = new Message();
                        msg.what = 31;
                        // 没有用到obj字段
                        handler.sendMessage(msg);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Message msg = new Message();
                    msg.what = 30;
                    // 没有用到obj字段
                    handler.sendMessage(msg);
                }
            }
        }).start();
    }

    private void start_reconnect() {
        scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!client.isConnected()) {
                    mqtt_connect();
                }
            }
        }, 0 * 1000, 10 * 1000, TimeUnit.MILLISECONDS);
    }

    //向阿里云发布消息
    private void publish_message(String message) {
        //判断阿里云链接是否为空或者是否断开
        if (client == null || !client.isConnected()) {
            return;
        }
        MqttMessage mqtt_message = new MqttMessage();
        mqtt_message.setPayload(message.getBytes());
        try {
            client.publish(pub_topic, mqtt_message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }



    private void addProvinceCode(HashMap<String, String> provinceCodeMap) {
        // 在这里添加 font 和省份简称的映射关系
        provinceCodeMap.put("5180", "冀"); // 示例
        provinceCodeMap.put("664b", "晋");
        provinceCodeMap.put("8fbd", "辽");
        provinceCodeMap.put("5409", "吉");
        provinceCodeMap.put("9ed1", "黑");
        provinceCodeMap.put("82cf", "苏");
        provinceCodeMap.put("6d59", "浙");
        provinceCodeMap.put("7696", "皖");
        provinceCodeMap.put("95fd", "闽");
        provinceCodeMap.put("8d63", "赣");
        provinceCodeMap.put("9c81", "鲁");
        provinceCodeMap.put("8c6b", "豫");
        provinceCodeMap.put("9102", "鄂");
        provinceCodeMap.put("6e58", "湘");
        provinceCodeMap.put("7ca4", "粤");
        provinceCodeMap.put("743c", "琼");
        provinceCodeMap.put("5ddd", "川");
        provinceCodeMap.put("8d35", "贵");
        provinceCodeMap.put("4e91", "云");
        provinceCodeMap.put("9655", "陕");
        provinceCodeMap.put("7518", "甘");
        provinceCodeMap.put("9752", "青");
        provinceCodeMap.put("8499", "蒙");
        provinceCodeMap.put("6842", "桂");
        provinceCodeMap.put("5b81", "宁");
        provinceCodeMap.put("4eac", "京");
    }


    public static String getProvince(String code, HashMap<String, String> provinceCodeMap) {
        for (String province : provinceCodeMap.keySet()) {
            if (code.equals(provinceCodeMap.get(province))) {
                return province;
            }
        }
        return null; // 若未找到匹配的省份，返回null
    }
}