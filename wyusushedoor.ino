#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT         //定义小爱同学为灯
#include <Blinker.h>
#include <Servo.h>
Servo servo_16;   //舵机输出口 GIPO16/D0

char auth[] = "输入你的密钥";      // 点灯科技密钥
char ssid[] = "输入你的WiFi名称";                //无线网络名称
char pswd[] = "输入你的WiFi密码";          //无线密码

BlinkerButton Button1("btn");      //app对应的按键名称
bool oState = false;               //创建一个Bool类型的变量

void button1_callback(const String & state){    //按下按键会执行的命令
    BLINKER_LOG("get button state: ", state);   //获取app里面按键的状态
      digitalWrite(LED_BUILTIN ,LOW);                  //管脚低电平(注意大写)
      servo_16.write(10);  //舵机旋转角度 具体参数需要结合自身情况修改
        delay(1000);   //舵机动作后延时一会回到初始位置
        servo_16.write(95); //设置该角度方便手动开关灯 就是开完灯舵机归位
        digitalWrite(LED_BUILTIN , HIGH);
  }
void miotPowerState(const String & state) {     //反馈给小爱同学为电源设备（只能打开和关闭）
    BLINKER_LOG("need set power state: ", state);
    
    if (state == BLINKER_CMD_ON) {             //如果小爱同学收到打开命令
        oState = true;
        BlinkerMIOT.powerState("on");          //电源设备状态开
        BlinkerMIOT.print();                   //给小爱同学反馈
        digitalWrite(LED_BUILTIN , LOW);              //给灯引脚低电平
        servo_16.write(10);
        delay(1000);   //舵机动作后延时一会回到初始位置
        servo_16.write(95); //设置该角度方便手动开关灯 就是开完灯舵机归位
        digitalWrite(LED_BUILTIN , HIGH);
    }
    else if (state == BLINKER_CMD_OFF) {        //如果小爱同学收到关闭命令
        oState = false;
        BlinkerMIOT.powerState("off");          //电源设备状态关
        BlinkerMIOT.print();                    //给小爱同学反馈
    }
}

void miotQuery(int32_t queryCode)
{
  BLINKER_LOG("MIOT Query codes: ", queryCode, ",  onstate: ", oState);

    switch (queryCode)
  {
    case BLINKER_CMD_QUERY_ALL_NUMBER :
      BLINKER_LOG("MIOT Query All");
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    default :
      BlinkerMIOT.powerState(oState ? "on" : "off");
      BlinkerMIOT.print();
      break;
  }
}



void setup() {
    Serial.begin(115200);    //设置波特率，需要和窗口监视器一样，否则乱码（初始化串口）
    
    // 初始化板子自带的led
    pinMode(LED_BUILTIN, OUTPUT);    //设置端口为输出模式
    digitalWrite(LED_BUILTIN, HIGH); //初始设置为高电平

    servo_16.attach(16);  //初始化舵机串口D0
    
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);  //连接云服务
    Button1.attach(button1_callback); //调用前面按键函数
    BlinkerMIOT.attachQuery(miotQuery);
    BlinkerMIOT.attachPowerState(miotPowerState);  //回调小爱同学

    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();        //debug程序
}
void loop(){
    Blinker.run(); 
}
