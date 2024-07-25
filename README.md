# wyuIoT
基于esp8266单片机和sg90舵机的小爱同学智能灯的实现代码，适用于五邑大学智能宿舍，自动开关灯自动开门  

已添加详细注释以便于同学们根据实际情况修改调试
## 宿舍自动开门物料清单（简易）
![image](https://github.com/MYHealer/wyuIoT/assets/141480264/4a1f4b83-266a-4931-90eb-25fff0989f52)
## 程序文件  （4月16日上传）
wyusushedoor.ino——宿舍自动开门
wyususheled.ino——宿舍自动开灯
更新日志  
1.修复了小爱同学无法正常回调led/door的开关状态的bug  
2.led增加了“开关按钮功能”，可以在点灯app单键开关灯
## 补充说明
1.开发板5v供电，属于小功率电器且不含电池，**符合宿舍管理条例**  
2.宿舍自动开关灯的物料与自动开门一致（不用买铁线/铜线）  
3.电源请使用usb插线板/充电器/充电宝，自备热熔胶枪以固定舵机
## 相关链接分享
校园网WiFi共享可参考https://github.com/6DDUU6/ESurfingDialer-onekey

使用小爱同学唤醒电脑可参考项目https://github.com/tty228/udp_turn_off
