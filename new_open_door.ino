/*
* 智能语言控制舵机，支持多平台控制
* 融合原版程序中的舵机角度参数和时间控制逻辑
*原版代码QQ群：566565915
* 项目示例：通过指令控制舵机动作
* 官网：bemfa.com
*/

#include <ESP8266WiFi.h>               //WIFI头文件
#include "PubSubClient.h"              //MQTT库文件
#include <Servo.h>                     //舵机库
#include <WiFiUdp.h>
#include <NTPClient.h>                 //NTP时间库

//********************需要修改的部分*******************//
const char* ssid = "No Study Come Play";           //你的路由WIFI名字
const char* password = "1234567890";     //你的WIFI密码
#define ID_MQTT  "4d06d448363143cdba6e96325785xxxx"     //用户私钥，控制台获取
const char*  topic = "light002";       //主题名字，可在巴法云控制台创建
const int servoPin = 16;               //舵机控制引脚(GPIO16/D0)，与原版一致
//**************************************************//

// NTP 客户端设置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800); // 北京时间偏移量(8小时)

// 定义有效时间范围，与原版一致
const int validStartHour = 7;
const int validEndHour = 24;

const char* mqtt_server = "bemfa.com"; //MQTT服务器
const int mqtt_server_port = 9501;     //MQTT服务器端口
WiFiClient espClient;
PubSubClient client(espClient);
Servo myServo;                         //创建舵机对象
int homePosition = 95;                 //舵机归位角度，与原版一致
int actionPosition = 10;               //舵机动作角度，与原版一致

// 记录上次检查网络连接的时间
unsigned long lastNetworkCheckTime = 0;
// 网络检查间隔时间（毫秒）
const unsigned long networkCheckInterval = 5000; 

//函数声明
void setServoAction();
bool isTimeValid();
void checkNetworkConnection();

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // 初始化NTP客户端
  timeClient.begin();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic:");
  Serial.println(topic);
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Received command:");
  Serial.println(msg);
  
  // 检查时间是否在有效范围内
  if (!isTimeValid()) {
    Serial.println("Command not valid at this time.");
    return;
  }
  
  // 处理命令 - 兼容"on"指令和角度指令
  if (msg == "on" || msg == "open") {
    setServoAction();
  } else {
    // 尝试将消息转换为角度值
    int angle = msg.toInt();
    if (angle >= 0 && angle <= 180) {
      Serial.print("Moving servo to ");
      Serial.print(angle);
      Serial.println(" degrees");
      myServo.write(angle);
      delay(500);
    } else {
      Serial.println("Invalid command! Send 'on' or angle (0-180)");
    }
  }
}

void reconnect() {
  // 循环直到重新连接成功
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 尝试连接
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(topic);
      // 订阅主题
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // 5秒后重试
      delay(5000);
    }
  }
}

// 检查当前时间是否在有效范围内
bool isTimeValid() {
    timeClient.update();
    int currentHour = timeClient.getHours(); 
    return currentHour >= validStartHour && currentHour < validEndHour;
}

// 检查网络连接并尝试重新连接
void checkNetworkConnection() {
    if (millis() - lastNetworkCheckTime >= networkCheckInterval) {
        lastNetworkCheckTime = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected, trying to reconnect...");
            setup_wifi();
        }
    }
}

// 舵机动作函数，与原版逻辑一致
void setServoAction() {
  Serial.println("Servo performing action...");
  myServo.write(actionPosition);  // 转动到动作角度(10度)
  delay(1000);                    // 延时1秒，与原版一致
  myServo.write(homePosition);    // 回到归位角度(95度)
  delay(500);                     // 等待归位完成
}

void setup() {
  myServo.attach(servoPin);            // 初始化舵机引脚(GPIO16)
  myServo.write(homePosition);         // 设置舵机初始角度为归位角度
  Serial.begin(115200);                // 设置波特率
  setup_wifi();                        // 连接WiFi
  client.setServer(mqtt_server, mqtt_server_port); // 设置MQTT服务器
  client.setCallback(callback);        // 设置MQTT消息回调函数
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  checkNetworkConnection();  // 检查网络连接
}
