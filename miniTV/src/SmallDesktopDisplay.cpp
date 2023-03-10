/* *****************************************************************

   SmallDesktopDisplay
      小型桌面显示器

   原  作  者：Misaka
   修      改：云梦泽创意空间
   创 建 日 期：2021.07.19
   最后更改日期：2021.12.17
   更 改 说 明：V1.1添加串口调试，波特率115200\8\n\1；增加版本号显示。
              V1.2亮度和城市代码保存到EEPROM，断电可保存
              V1.3.1 更改smartconfig改为WEB配网模式，同时在配网的同时增加亮度、屏幕方向设置。
              V1.3.2 增加wifi休眠模式，仅在需要连接的情况下开启wifi，其他时间关闭wifi。增加wifi保存至eeprom（目前仅保存一组ssid和密码）
              V1.3.3  修改WiFi保存后无法删除的问题。目前更改为使用串口控制，输入 0x05 重置WiFi数据并重启。
                      增加web配网以及串口设置天气更新时间的功能。
              V1.3.4  修改web配网页面设置，将wifi设置页面以及其余设置选项放入同一页面中。
                      增加web页面设置是否使用DHT传感器。（使能DHT后才可使用）
              TXDRAW1.0 增加开机页面设定
                         解决开机花屏问题
                         增加USB为基准的二方向镜向设置

                    
                         
   技术讨论QQ群 1群 761503009   2群 887171863  3群 811058758


   原创不易，鼓励大家创新，若转手小电视或从事商业活动，请先与创作团队沟通。

   引 脚 分 配： SCK   GPIO14
               MOSI  GPIO13
               RES   GPIO2
               DC    GPIO0
               LCDBL GPIO5

               增加DHT11温湿度传感器，传感器接口为 GPIO 12

 * *****************************************************************/

/* *****************************************************************
    库文件、头文件
 * *****************************************************************/
#include "ArduinoJson.h"
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <TJpg_Decoder.h>
#include <EEPROM.h>
#include "qr.h"
#include "number.h"
#include "weathernum.h"


#define Version  "for luilui. V1.1"
/* *****************************************************************
    配置使能位
 * *****************************************************************/
//WEB配网使能标志位----WEB配网打开后会默认关闭smartconfig功能
#define WM_EN   1
//设定DHT11温湿度传感器使能标志
#define DHT_EN  0
//设置太空人图片是否使用
#define imgAst_EN 1

//设置开机加载图片是否使用
#define imgAst_ENLoad 1

#if WM_EN
#include <WiFiManager.h>
//WiFiManager 参数
WiFiManager wm; // global wm instance
// WiFiManagerParameter custom_field; // global param ( for non blocking w params )
#endif

#if DHT_EN
#include "DHT.h"
#define DHTPIN  12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#endif





/* *****************************************************************
    字库、图片库
 * *****************************************************************/
//#include "font/ZdyLwFont_20.h"
#include "font/hzsel.h"
#include "img/temperature.h"
#include "img/humidity.h"

#if imgAst_EN
#include "img/picture/tkr/hutao.h"
/*
#include "img/picture/tkr/i0.h"
#include "img/picture/tkr/i1.h"
#include "img/picture/tkr/i2.h"
#include "img/picture/tkr/i3.h"
#include "img/picture/tkr/i4.h"
#include "img/picture/tkr/i5.h"
#include "img/picture/tkr/i6.h"
#include "img/picture/tkr/i7.h"
#include "img/picture/tkr/i8.h"
#include "img/picture/tkr/i9.h"

/*
#include "img/picture/tkr/i10.h"
#include "img/picture/tkr/i11.h"
#include "img/picture/tkr/i12.h"
#include "img/picture/tkr/i13.h"
#include "img/picture/tkr/i14.h"
#include "img/picture/tkr/i15.h"
#include "img/picture/tkr/i16.h"
#include "img/picture/tkr/i17.h"
#include "img/picture/tkr/i18.h"
#include "img/picture/tkr/i19.h"
#include "img/picture/tkr/i20.h"
//#include "img/picture/tkr/i21.h"
//#include "img/picture/tkr/i22.h"
//#include "img/picture/tkr/i23.h"
//#include "img/picture/tkr/i24.h"
//#include "img/picture/tkr/i25.h"
//#include "img/picture/tkr/i26.h"
//#include "img/picture/tkr/i27.h"
//#include "img/picture/tkr/i28.h"
//#include "img/picture/tkr/i29.h"
//#include "img/picture/tkr/i30.h"
//#include "img/picture/tkr/i31.h"
*/

int Anim = 0;           //太空人图标显示指针记录
int luiA=0;
int AprevTime = 0;      //太空人更新时间记录
int luiTime = 0;
#endif



#if imgAst_ENLoad
#include "img/picture/donghua/lui.h"
#include "img/picture/donghua/ii0.h"
#include "img/picture/donghua/ii1.h"
#include "img/picture/donghua/ii2.h"
#include "img/picture/donghua/ii3.h"
#include "img/picture/donghua/ii4.h"
#include "img/picture/donghua/ii5.h"
#include "img/picture/donghua/ii6.h"
#include "img/picture/donghua/ii7.h"
#include "img/picture/donghua/ii8.h"
#include "img/picture/donghua/ii9.h"
#include "img/picture/donghua/ii10.h"
#include "img/picture/donghua/ii11.h"
#include "img/picture/donghua/ii12.h"
#include "img/picture/donghua/ii13.h"
#include "img/picture/donghua/ii14.h"
#include "img/picture/donghua/ii15.h"
#include "img/picture/donghua/ii16.h"
#include "img/picture/donghua/ii17.h"
#include "img/picture/donghua/ii18.h"
#include "img/picture/donghua/ii19.h"
#include "img/picture/donghua/ii20.h"
#include "img/picture/donghua/ii21.h"
#include "img/picture/donghua/ii22.h"
#include "img/picture/donghua/ii23.h"
#include "img/picture/donghua/ii24.h"
#include "img/picture/donghua/ii25.h"
#include "img/picture/donghua/ii26.h"
#include "img/picture/donghua/ii27.h"
#include "img/picture/donghua/ii28.h"
#include "img/picture/donghua/ii29.h"
#include "img/picture/donghua/ii30.h"
#include "img/picture/donghua/ii31.h"
#include "img/picture/donghua/ii32.h"
#include "img/picture/donghua/ii33.h"
#include "img/picture/donghua/ii34.h"
#include "img/picture/donghua/ii35.h"
#include "img/picture/donghua/ii36.h"

#include "img/picture/donghua/acg01.h"
#include "img/picture/donghua/acg02.h"
#include "img/picture/donghua/acg03.h"
#include "img/picture/donghua/acg04.h"
#include "img/picture/donghua/acg05.h"


/*

*/
int AnimLoad = 0;           //开机图标显示指针记录
int AprevTimeLoad = 0;      //更新时间记录
int rollNum = 0;
#endif
void imgAnimLoad();
void getCityWeater();
void getCityCode();
void scrollBanner(int rollNum);
void imgAnim();
void luiAnim();
void weaterData(String *cityDZ, String *dataSK, String *dataFC ,String *words,String *wabi);
String week();
String monthDay();
void saveParamCallback();
int currentIndex = 0;
int lightOri=0;



/* *****************************************************************
    参数设置
 * *****************************************************************/

struct config_type
{
  char stassid[32];//定义配网得到的WIFI名长度(最大32字节)
  char stapsw[64];//定义配网得到的WIFI密码长度(最大64字节)
};

//---------------修改此处""内的信息--------------------
//如开启WEB配网则可不用设置这里的参数，前一个为wifi ssid，后一个为密码
config_type wificonf = {{""}, {""}};

//天气更新时间  X 分钟
int updateweater_time = 10;

//----------------------------------------------------

//LCD屏幕相关设置
TFT_eSPI tft = TFT_eSPI();  // 引脚请自行配置tft_espi库中的 User_Setup.h文件
TFT_eSprite clk = TFT_eSprite(&tft);
#define LCD_BL_PIN 5    //LCD背光引脚
uint16_t bgColor = 0x0000;

//其余状态标志位
int LCD_Rotation = 0;   //LCD屏幕方向
int LCD_BL_PWM = 40;//屏幕亮度0-100，默认10
uint8_t Wifi_en = 1; //wifi状态标志位  1：打开    0：关闭
uint8_t UpdateWeater_en = 0; //更新时间标志位
int prevTime = 0;       //滚动显示更新标志位
int DHT_img_flag = 0;   //DHT传感器使用标志位


time_t prevDisplay = 0;       //显示时间显示记录
unsigned long weaterTime = 0; //天气更新时间记录


/*** Component objects ***/
Number      dig;
WeatherNum  wrat;

uint32_t targetTime = 0;
String cityCode = "0";  //天气城市代码 
int tempnum = 0;   //温度百分比
int huminum = 0;   //湿度百分比
int tempcol = 0xffff;  //温度显示颜色
int humicol = 0xffff;  //湿度显示颜色


//EEPROM参数存储地址位
int BL_addr = 1;//被写入数据的EEPROM地址编号  1亮度
int Ro_addr = 2; //被写入数据的EEPROM地址编号  2 旋转方向
int DHT_addr = 3;//3 DHT使能标志位
int CC_addr = 10;//被写入数据的EEPROM地址编号  10城市
int wifi_addr = 30; //被写入数据的EEPROM地址编号  20wifi-ssid-psw


//NTP服务器参数
static const char ntpServerName[] = "ntp6.aliyun.com";
const int timeZone = 8;     //东八区

//wifi连接UDP设置参数
WiFiUDP Udp;
WiFiClient wificlient;
unsigned int localPort = 8000;
float duty = 0;


//函数声明
time_t getNtpTime();
void digitalClockDisplay(int reflash_en);
void printDigits(int digits);
String num2str(int digits);
void sendNTPpacket(IPAddress &address);
void LCD_reflash(int en);
void savewificonfig();
void readwificonfig();
void deletewificonfig();


/* *****************************************************************
    函数
 * *****************************************************************/

//wifi ssid，psw保存到eeprom
void savewificonfig()
{
  //开始写入
  uint8_t *p = (uint8_t*)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++)
  {
    EEPROM.write(i + wifi_addr, *(p + i)); //在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit();//执行写入ROM
  delay(10);
}
//删除原有eeprom中的信息
void deletewificonfig()
{
  config_type deletewifi = {{""}, {""}};
  uint8_t *p = (uint8_t*)(&deletewifi);
  for (int i = 0; i < sizeof(deletewifi); i++)
  {
    EEPROM.write(i + wifi_addr, *(p + i)); //在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit();//执行写入ROM
  delay(10);
}

//从eeprom读取WiFi信息ssid，psw
void readwificonfig()
{
  uint8_t *p = (uint8_t*)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++)
  {
    *(p + i) = EEPROM.read(i + wifi_addr);
  }
  Serial.printf("Read WiFi Config.....\r\n");
  Serial.printf("SSID:%s\r\n", wificonf.stassid);
  Serial.printf("PSW:%s\r\n", wificonf.stapsw);
  Serial.printf("Connecting.....\r\n");
}

//TFT屏幕输出函数
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  // Return 1 to decode next block
  return 1;
}


//进度条函数
byte loadNum = 6;           //原始默认为6
void loading(byte delayTime)//绘制进度条
{
  clk.setColorDepth(8);
  clk.createSprite(200, 30);//创建窗口
	clk.fillSprite(0xFFFF);   //填充率
  imgAnimLoad();
  clk.fillSprite(bgColor);
  clk.setTextColor(TFT_WHITE, 0x0000);
  clk.drawRightString(Version, 200, 10, 1);
  clk.pushSprite(10, 220); //窗口位置

  clk.unloadFont();
  clk.deleteSprite();

  loadNum += 1;
  delay(delayTime);
}

//湿度图标显示函数
void humidityWin()
{
  clk.setColorDepth(8);

  huminum = huminum / 2;
  clk.createSprite(52, 6);  //创建窗口
  clk.fillSprite(0x0000);    //填充率
  clk.drawRoundRect(0, 0, 52, 6, 3, 0xFFFF); //空心圆角矩形  起始位x,y,长度，宽度，圆弧半径，颜色
  clk.fillRoundRect(1, 1, huminum, 4, 2, humicol); //实心圆角矩形
  clk.pushSprite(45, 222); //窗口位置
  clk.deleteSprite();
}

//温度图标显示函数
void tempWin()
{
  clk.setColorDepth(8);

  clk.createSprite(52, 6);  //创建窗口
  clk.fillSprite(0x0000);    //填充率
  clk.drawRoundRect(0, 0, 52, 6, 3, 0xFFFF); //空心圆角矩形  起始位x,y,长度，宽度，圆弧半径，颜色
  clk.fillRoundRect(1, 1, tempnum, 4, 2, tempcol); //实心圆角矩形
  clk.pushSprite(45, 192); //窗口位置
  clk.deleteSprite();
}



String SMOD = "";//0亮度
//串口调试设置函数
void Serial_set()
{
  String incomingByte = "";
  if (Serial.available() > 0)
  {

    while (Serial.available() > 0) //监测串口缓存，当有数据输入时，循环赋值给incomingByte
    {
      incomingByte += char(Serial.read());//读取单个字符值，转换为字符，并按顺序一个个赋值给incomingByte
      delay(2);//不能省略，因为读取缓冲区数据需要时间
    }
    if (SMOD == "0x01") //设置1亮度设置
    {
      int LCDBL = atoi(incomingByte.c_str());//int n = atoi(xxx.c_str());//String转int
      if (LCDBL >= 0 && LCDBL <= 100)
      {
        EEPROM.write(BL_addr, LCDBL);//亮度地址写入亮度值
        EEPROM.commit();//保存更改的数据
        delay(5);
        LCD_BL_PWM = EEPROM.read(BL_addr);
        delay(5);
        SMOD = "";
        Serial.printf("亮度调整为：");
        analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));
        Serial.println(LCD_BL_PWM);
        Serial.println("");
      }
      else
        Serial.println("亮度调整错误，请输入0-100");
    }
    if (SMOD == "0x02") //设置2地址设置
    {
      int CityCODE = 0;
      int CityC = atoi(incomingByte.c_str());//int n = atoi(xxx.c_str());//String转int
      if (CityC >= 101000000 && CityC <= 102000000 || CityC == 0)
      {
        for (int cnum = 0; cnum < 5; cnum++)
        {
          EEPROM.write(CC_addr + cnum, CityC % 100); //城市地址写入城市代码
          EEPROM.commit();//保存更改的数据
          CityC = CityC / 100;
          delay(5);
        }
        for (int cnum = 5; cnum > 0; cnum--)
        {
          CityCODE = CityCODE * 100;
          CityCODE += EEPROM.read(CC_addr + cnum - 1);
          delay(5);
        }

        cityCode = CityCODE;

        if (cityCode == "0")
        {
          Serial.println("城市代码调整为：自动");
          getCityCode();  //获取城市代码
        }
        else
        {
          Serial.printf("城市代码调整为：");
          Serial.println(cityCode);
        }
        Serial.println("");
        getCityWeater();//更新城市天气
        SMOD = "";
      }
      else
        Serial.println("城市调整错误，请输入9位城市代码，自动获取请输入0");
    }
    if (SMOD == "0x03") //设置3屏幕显示方向
    {
      int RoSet = atoi(incomingByte.c_str());
      if (RoSet >= 0 && RoSet <= 3)
      {
        EEPROM.write(Ro_addr, RoSet);//屏幕方向地址写入方向值
        EEPROM.commit();//保存更改的数据
        SMOD = "";
        //设置屏幕方向后重新刷屏并显示
        tft.setRotation(RoSet);
        tft.fillScreen(0x0000);
        LCD_reflash(1);//屏幕刷新程序
        UpdateWeater_en = 1;
        //TJpgDec.drawJpg(15, 183, temperature, sizeof(temperature)); //温度图标
        //TJpgDec.drawJpg(15, 213, humidity, sizeof(humidity)); //湿度图标


        Serial.print("屏幕方向设置为：");
        Serial.println(RoSet);
      }
      else
      {
        Serial.println("屏幕方向值错误，请输入0-5内的值");
      }
    }
    if (SMOD == "0x04") //设置天气更新时间
    {
      int wtup = atoi(incomingByte.c_str());//int n = atoi(xxx.c_str());//String转int
      if (wtup >= 1 && wtup <= 60)
      {
        updateweater_time = wtup;
        SMOD = "";
        Serial.printf("天气更新时间更改为：");
        Serial.print(updateweater_time);
        Serial.println("分钟");
      }
      else
        Serial.println("更新时间太长，请重新设置（1-60）");
    }
    else
    {
      SMOD = incomingByte;
      delay(2);
      if (SMOD == "0x01")
        Serial.println("请输入亮度值，范围0-100");
      else if (SMOD == "0x02")
        Serial.println("请输入9位城市代码，自动获取请输入0");
      else if (SMOD == "0x03")
      {
        Serial.println("请输入屏幕方向值，");
        Serial.println("0-USB接口朝下");
        Serial.println("1-USB接口朝上");
        Serial.println("2-USB Mirror up");
        Serial.println("3-USB Mirror up");
      }
      else if (SMOD == "0x04")
      {
        Serial.print("当前天气更新时间：");
        Serial.print(updateweater_time);
        Serial.println("分钟");
        Serial.println("请输入天气更新时间（1-60）分钟");
      }
      else if (SMOD == "0x05")
      {
        Serial.println("重置WiFi设置中......");
        delay(10);
        wm.resetSettings();
        deletewificonfig();
        delay(10);
        Serial.println("重置WiFi成功");
        SMOD = "";
        ESP.restart();
      }
      else
      {
        Serial.println("");
        Serial.println("请输入需要修改的代码：");
        Serial.println("亮度设置输入        0x01");
        Serial.println("地址设置输入        0x02");
        Serial.println("屏幕方向设置输入    0x03");
        Serial.println("更改天气更新时间    0x04");
        Serial.println("重置WiFi(会重启)    0x05");
        Serial.println("");
      }
    }
  }
}


#if WM_EN
//WEB配网LCD显示函数
void Web_win()
{
  clk.setColorDepth(8);

  clk.createSprite(200, 90);//创建窗口
  clk.fillSprite(0x0000);   //填充率

  clk.setTextDatum(CC_DATUM);   //设置文本数据
  clk.setTextColor(TFT_GREEN, 0x0000);
  clk.drawString("WiFi Connect Fail!", 100, 30, 2);
  clk.drawString("SSID:", 45, 60, 2);
  clk.setTextColor(TFT_YELLOW, 0x0000);
  clk.drawString("AutoConnectAP", 125, 60, 2);
  clk.pushSprite(20, 37); //窗口位置


  clk.pushSprite(20, 37); //窗口位置
  clk.deleteSprite();

}

//WEB配网函数
void Webconfig()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  delay(3000);
  wm.resetSettings(); // wipe settings

  // add a custom input field
  int customFieldLength = 40;


  const char* set_rotation = "<br/><label for='set_rotation'>Set Rotation</label>\
                              <input type='radio' name='set_rotation' value='0' checked> One<br>\
                              <input type='radio' name='set_rotation' value='1'> Two<br>\
                              <input type='radio' name='set_rotation' value='2'> Three<br>\
//                              <input type='radio' name='set_rotation' value='3'> Four<br>\
//                              <input type='radio' name='set_rotation' value='4'> Five<br>\
                             <input type='radio' name='set_rotation' value='3'> Six<br>";
  WiFiManagerParameter  custom_rot(set_rotation); // custom html input
  WiFiManagerParameter  custom_bl("LCDBL", "LCD BackLight(1-100)", "8", 3);
#if DHT_EN
  WiFiManagerParameter  custom_DHT11_en("DHT11_en", "Enable DHT11 sensor", "0", 1);
#endif
  WiFiManagerParameter  custom_weatertime("WeaterUpdateTime", "Weather Update Time(Min)", "10", 3);
  WiFiManagerParameter  custom_cc("CityCode", "CityCode", "0", 9);
  WiFiManagerParameter  p_lineBreak_notext("<p></p>");

  wm.addParameter(&p_lineBreak_notext);
  wm.addParameter(&custom_cc);
  wm.addParameter(&p_lineBreak_notext);
  wm.addParameter(&custom_bl);
  wm.addParameter(&p_lineBreak_notext);
  wm.addParameter(&custom_weatertime);
  wm.addParameter(&p_lineBreak_notext);
  wm.addParameter(&custom_rot);
#if DHT_EN
  wm.addParameter(&p_lineBreak_notext);
  wm.addParameter(&custom_DHT11_en);
#endif
  wm.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> menu = {"wifi", "restart"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%

  bool res;
  res = wm.autoConnect("AutoConnectAP"); // anonymous ap

  while (!res);
}

String getParam(String name) {
  //read parameter from server, for customhmtl input
  String value;
  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback() {
  int CCODE = 0, cc;

  Serial.println("[CALLBACK] saveParamCallback fired");
  //将从页面中获取的数据保存
#if DHT_EN
  DHT_img_flag = getParam("DHT11_en").toInt();
#endif
  updateweater_time = getParam("WeaterUpdateTime").toInt();
  cc =  getParam("CityCode").toInt();
  LCD_Rotation = getParam("set_rotation").toInt();
  LCD_BL_PWM = getParam("LCDBL").toInt();

  //对获取的数据进行处理
  //城市代码
  Serial.print("CityCode = ");
  Serial.println(cc);
  if (cc >= 101000000 && cc <= 102000000 || cc == 0)
  {
    for (int cnum = 0; cnum < 5; cnum++)
    {
      EEPROM.write(CC_addr + cnum, cc % 100); //城市地址写入城市代码
      EEPROM.commit();//保存更改的数据
      cc = cc / 100;
      delay(5);
    }
    for (int cnum = 5; cnum > 0; cnum--)
    {
      CCODE = CCODE * 100;
      CCODE += EEPROM.read(CC_addr + cnum - 1);
      delay(5);
    }
    cityCode = CCODE;
  }
  //屏幕方向
  Serial.print("LCD_Rotation = ");
  Serial.println(LCD_Rotation);
  if (EEPROM.read(Ro_addr) != LCD_Rotation)
  {
    EEPROM.write(Ro_addr, LCD_Rotation);
    EEPROM.commit();
    delay(5);
  }
  tft.setRotation(LCD_Rotation);
  tft.fillScreen(0x0000);
  Web_win();
  loadNum--;
  loading(1);
  if (EEPROM.read(BL_addr) != LCD_BL_PWM)
  {
    EEPROM.write(BL_addr, LCD_BL_PWM);
    EEPROM.commit();
    delay(5);
  }
  //屏幕亮度
  Serial.printf("亮度调整为：");
  analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));
  Serial.println(LCD_BL_PWM);
  //天气更新时间
  Serial.printf("天气更新时间调整为：");
  Serial.println(updateweater_time);

}
#endif





void setup()
{
  Serial.begin(115200);
  EEPROM.begin(1024);
  // WiFi.forceSleepWake();
  // wm.resetSettings();    //在初始化中使wifi重置，需重新配置WiFi

  //从eeprom读取背光亮度设置
  if (EEPROM.read(BL_addr) > 0 && EEPROM.read(BL_addr) < 100)
    LCD_BL_PWM = EEPROM.read(BL_addr);
  //从eeprom读取屏幕方向设置
  if (EEPROM.read(Ro_addr) >= 0 && EEPROM.read(Ro_addr) <= 3)
    LCD_Rotation = EEPROM.read(Ro_addr);

  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));

  tft.begin(); /* TFT init */
  tft.invertDisplay(1);//反转所有显示颜色：1反转，0正常
  tft.setRotation(LCD_Rotation);
  tft.fillScreen(0x0000);
  tft.setTextColor(TFT_BLACK, bgColor);

  targetTime = millis() + 1000;
  readwificonfig();//读取存储的wifi信息
  Serial.print("正在连接WIFI ");
  Serial.println(wificonf.stassid);
  WiFi.begin(wificonf.stassid, wificonf.stapsw);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  while (WiFi.status() != WL_CONNECTED)
  {
    loading(30);

    if (loadNum >= 90)
    {
      //使能web配网后自动将smartconfig配网失效
#if WM_EN
      Web_win();
      Webconfig();
#endif

#if !WM_EN
      SmartConfig();
#endif
      break;
    }
  }
  delay(10);
  while (loadNum < 90) //让动画走完
  {
    loading(1);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("SSID:");
    Serial.println(WiFi.SSID().c_str());
    Serial.print("PSW:");
    Serial.println(WiFi.psk().c_str());
    strcpy(wificonf.stassid, WiFi.SSID().c_str()); //名称复制
    strcpy(wificonf.stapsw, WiFi.psk().c_str()); //密码复制
    savewificonfig();
    readwificonfig();
  }

  Serial.print("本地IP： ");
  Serial.println(WiFi.localIP());
  Serial.println("启动UDP");
  Udp.begin(localPort);
  Serial.println("等待同步...");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

  int CityCODE = 0;
  for (int cnum = 5; cnum > 0; cnum--)
  {
    CityCODE = CityCODE * 100;
    CityCODE += EEPROM.read(CC_addr + cnum - 1);
    delay(5);
  }
  if (CityCODE >= 101000000 && CityCODE <= 102000000)
    cityCode = CityCODE;
  else
    getCityCode();  //获取城市代码
  tft.fillScreen(TFT_BLACK);//清屏
  TJpgDec.drawFsJpg(0, 0, "acg01.jpg");
  //TJpgDec.drawJpg(15, 183, temperature, sizeof(temperature)); //温度图标
  //TJpgDec.drawJpg(15, 213, humidity, sizeof(humidity)); //湿度图标
  getCityWeater();

  WiFi.forceSleepBegin(); //wifi off
  Serial.println("WIFI休眠......");
  Wifi_en = 0;
}



void loop()
{
  LCD_reflash(0);
  Serial_set();
}

void LCD_reflash(int en)
{
  if (now() != prevDisplay || en == 1)
  {
    prevDisplay = now();
    digitalClockDisplay(en);
    prevTime = 0;
  }

  //两秒钟更新一次
  /*
  if (second() % 3 == 0 && prevTime == 0 || en == 1) {

    scrollBanner(rollNum);
  }
  */
  if (second() % 2 == 0 && prevTime == 0 & currentIndex<=7 || en == 1)
  {
    scrollBanner(rollNum);
    luiAnim();
  }
  if (second() % 1 == 0 && prevTime == 0 & currentIndex>7 || en == 1)
  //if (millis() - weaterTime > (6000 * updateweater_time) & currentIndex>7 || en == 1)
  {
    scrollBanner(rollNum);
  }
#if imgAst_EN
  if (DHT_img_flag == 0)
    imgAnim();
#endif
  luiAnim();

  //if (millis() - weaterTime > (60000 * updateweater_time) || en == 1 || UpdateWeater_en != 0) { //10分钟更新一次天气
  if (millis() - weaterTime > (2200 * updateweater_time) || en == 1 || UpdateWeater_en != 0) { //10分钟更新一次天气
    if (Wifi_en == 0)
    {
      WiFi.forceSleepWake();//wifi on
      Serial.println("WIFI恢复......");
      Wifi_en = 1;
    } 

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WIFI已连接");
      getCityWeater();
      if (UpdateWeater_en != 0) UpdateWeater_en = 0;
      weaterTime = millis();
      while (!getNtpTime());
      WiFi.forceSleepBegin(); // Wifi Off
      Serial.println("WIFI休眠......");
      Wifi_en = 0;
    }
  }
}

// 发送HTTP请求并且将服务器响应通过串口输出
void getCityCode() {
  String URL = "http://wgeo.weather.com.cn/ip/?_=" + String(now());
  //创建 HTTPClient 对象
  HTTPClient httpClient;

  //配置请求地址。此处也可以不使用端口号和PATH而单纯的
  httpClient.begin(wificlient, URL);

  //设置请求头中的User-Agent
  httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");
  httpClient.addHeader("Referer", "http://www.weather.com.cn/");

  //启动连接并发送HTTP请求
  int httpCode = httpClient.GET();
  Serial.print("Send GET request to URL: ");
  Serial.println(URL);

  //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
  if (httpCode == HTTP_CODE_OK) {
    String str = httpClient.getString();

    int aa = str.indexOf("id=");
    if (aa > -1)
    {
      //cityCode = str.substring(aa+4,aa+4+9).toInt();
      cityCode = str.substring(aa + 4, aa + 4 + 9);
      Serial.println(cityCode);
      getCityWeater();
    }
    else
    {
      Serial.println("获取城市代码失败");
    }


  } else {
    Serial.println("请求城市代码错误：");
    Serial.println(httpCode);
  }

  //关闭ESP8266与服务器连接60
  httpClient.end();
}


 
// 获取城市天气
void getCityWeater() {
  //daily words
  HTTPClient http;
  String GetUrl;  
  String response;
  String content;
  GetUrl = "http://open.iciba.com/dsapi/";
  http.setTimeout(5000);
  http.begin(GetUrl);
  int httpCode1 = http.GET();
  if (httpCode1 > 0) {
      //Serial.printf("[HTTP] GET... code: %d\n", 1);
      if (httpCode1 == HTTP_CODE_OK) {
        //读取响应内容
        response = http.getString();
        int indexStart = response.indexOf("content\":");
        int indexEnd = response.indexOf("\",\"note");
        content = response.substring(indexStart+10, indexEnd);
        Serial.println("content:"+content); 
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode1).c_str());
  }
  http.end();



  /*获取citycode*/
  HTTPClient httpCd;
  String GetUrlCd;  
  String citycode;
  String wabi;
  String light;

  GetUrlCd = "http://lil2tree.asia/lui_demo/api.php?type=wabi";
//  Serial.println("正在获取城市代码:"+GetUrlCd);
  httpCd.setTimeout(5000);
  httpCd.begin(GetUrlCd);
  int httpCodeCd = httpCd.GET();
  if (httpCodeCd > 0) {
      //Serial.printf("[HTTP] GET... code: %d\n", 1);
      if (httpCodeCd == HTTP_CODE_OK) {
        //读取响应内容
        citycode = httpCd.getString();
        //Serial.println("citycode:"+citycode); 


        int indexWbStart = citycode.indexOf("|");
        int indexWbMid = citycode.indexOf("#");
        int indexWbEnd = citycode.indexOf("@");
        wabi=citycode.substring(0, indexWbStart);
        light=citycode.substring(indexWbStart+1,indexWbMid);
        cityCode = citycode.substring(indexWbMid+1, indexWbEnd);
        Serial.println("wabi:"+wabi+"   light:"+light+"  citycode:"+cityCode); 
        //设置亮度
        int lightnow = light.toInt();
        if(lightOri==0)
        {
          lightOri=lightnow;
        }
        //Serial.printf("lightOri:%d    lightnow:%d\n",lightOri,lightnow);
        if(lightOri != lightnow)
        {
          int LCDBL = lightnow;//String转int
          if (LCDBL >= 0 && LCDBL <= 100)
          {
            EEPROM.write(BL_addr, LCDBL);//亮度地址写入亮度值
            EEPROM.commit();//保存更改的数据
            delay(5);
            LCD_BL_PWM = EEPROM.read(BL_addr);
            delay(5);
            SMOD = "";
            Serial.printf("亮度调整为：");
            analogWrite(LCD_BL_PIN, 1023 - (LCD_BL_PWM * 10));
            Serial.println(LCD_BL_PWM);
          }
          lightOri=lightnow;
        }
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", httpCd.errorToString(httpCodeCd).c_str());
      cityCode = "101042500";//默认石柱
      wabi="hello!lui!❤";
  }
  httpCd.end();

  //Serial.printf("after lightOri:%d    lightnow:%d\n",lightOri,lightnow);
  WiFiClient client; //新添加
  String URL = "http://d1.weather.com.cn/weather_index/" + cityCode + ".html?_=" + String(now()); //原来
  //创建 HTTPClient 对象
  HTTPClient httpClient;
  httpClient.begin(client,URL);
  //设置请求头中的User-Agent
  httpClient.setUserAgent("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1");
  httpClient.addHeader("Referer", "http://www.weather.com.cn/");
  //启动连接并发送HTTP请求
  int httpCode = httpClient.GET();
  Serial.println("正在获取天气数据");
  Serial.println(URL);

  //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
  if (httpCode == HTTP_CODE_OK) {

    String str = httpClient.getString();
    int indexStart = str.indexOf("weatherinfo\":");
    int indexEnd = str.indexOf("};var alarmDZ");

    String jsonCityDZ = str.substring(indexStart + 13, indexEnd);
    //Serial.println(jsonCityDZ);

    indexStart = str.indexOf("dataSK =");
    indexEnd = str.indexOf(";var dataZS");
    String jsonDataSK = str.substring(indexStart + 8, indexEnd);
    //Serial.println(jsonDataSK);


    indexStart = str.indexOf("\"f\":[");
    indexEnd = str.indexOf(",{\"fa");
    String jsonFC = str.substring(indexStart + 5, indexEnd);
    //Serial.println(jsonFC);

    weaterData(&jsonCityDZ, &jsonDataSK, &jsonFC , &content ,&wabi);
    Serial.println("获取成功");

  } else {
    Serial.println("请求城市天气错误：");
    Serial.print(httpCode);
  }  

  //关闭ESP8266与服务器连接
  httpClient.end();
}


String scrollText[150];

int scrollTextWidth = 0;
//天气信息写到屏幕上
void weaterData(String *cityDZ, String *dataSK, String *dataFC ,String *words,String *wabi)
{
  //解析第一段JSON
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, *dataSK);
  JsonObject sk = doc.as<JsonObject>();

  //TFT_eSprite clkb = TFT_eSprite(&tft);

  /***绘制相关文字***/
  clk.setColorDepth(8);
  //clk.loadFont(ZdyLwFont_20);
  clk.loadFont(hzsel);
  //温度
  clk.createSprite(58, 24);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(TFT_SILVER, bgColor);
  clk.drawString(sk["temp"].as<String>() + "℃", 0, 16);
  clk.pushSprite(15, 180);
  clk.deleteSprite();



  //湿度
  clk.createSprite(58, 24);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(TFT_SILVER, bgColor);
  clk.drawString(sk["SD"].as<String>(), 0, 16);
  //clk.drawString("100%",28,13);
  clk.pushSprite(15, 208);
  clk.deleteSprite();


  //城市名称
  clk.createSprite(94, 30);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextSize(4);
  clk.setTextColor(TFT_SILVER, bgColor);
  clk.drawString(sk["cityname"].as<String>(), 0, 16);
  //clk.pushSprite(15, 15);
  clk.pushSprite(15, 150);
  clk.deleteSprite();

  //PM2.5空气指数

  uint16_t pm25BgColor = tft.color565(156, 202, 127); //优
  String aqiTxt = "优";
  int pm25V = sk["aqi"];
  if (pm25V > 200) {
    pm25BgColor = tft.color565(136, 11, 32); //重度
    aqiTxt = "重度";
  } else if (pm25V > 150) {
    pm25BgColor = tft.color565(186, 55, 121); //中度
    aqiTxt = "中度";
  } else if (pm25V > 100) {
    pm25BgColor = tft.color565(242, 159, 57); //轻
    aqiTxt = "轻度";
  } else if (pm25V > 50) {
    pm25BgColor = tft.color565(247, 219, 100); //良
    aqiTxt = "良";
  }

  scrollText[0] = "实时天气 " + sk["weather"].as<String>();
  scrollText[1] = "空气质量 " + aqiTxt;
  scrollText[2] = "风向 " + sk["WD"].as<String>() + sk["WS"].as<String>();

  //scrollText[6] = atoi((sk["weathercode"].as<String>()).substring(1,3).c_str()) ;

  //天气图标
  //wrat.printfweather(170, 15, atoi((sk["weathercode"].as<String>()).substring(1, 3).c_str()));


  //左上角滚动字幕
  //解析第二段JSON
  deserializeJson(doc, *cityDZ);
  JsonObject dz = doc.as<JsonObject>();
  //Serial.println(sk["ws"].as<String>());
  //横向滚动方式
 // String aa = "今日天气:" + dz["weather"].as<String>() + "，温度:最低" + dz["tempn"].as<String>() + "，最高" + dz["temp"].as<String>() + " 空气质量:" + aqiTxt + "，风向:" + dz["wd"].as<String>() + dz["ws"].as<String>();
  //scrollTextWidth = clk.textWidth(aa);
  //Serial.println(aa);
  scrollText[3] = "今日" + dz["weather"].as<String>();

  deserializeJson(doc, *dataFC);
  JsonObject fc = doc.as<JsonObject>();

  //deserializeJson(doc, *words);
  //JsonObject wd = doc.as<JsonObject>();

  scrollText[4] = "最低温度" + fc["fd"].as<String>() + "℃";
  //scrollText[4] = "没有你的旅行都是流浪" + fc["fd"].as<String>() + "℃";
  //scrollText[5] = "最高温度" + fc["fc"].as<String>() + "℃";
  scrollText[5] = "最高温度" + fc["fc"].as<String>() + "℃";
  scrollText[6] = String(*wabi);
  String wordsA=String(*words);
  //String words1=String(*words).substring(0,21);
  //scrollText[7] = words1;
 /* 
  int with = 18;
  int wordsLen=wordsA.length();
  int count = wordsLen/with;
  int i=0;
  rollNum=count+6+1;
  for(i=0;i<=count;i++)
  {
    scrollText[7+i]=wordsA.substring(with*i, (i+1)*with);
    Serial.println(scrollText[7+i]);
  }
*/
  int with = 27;
  int speed=1;
  int wordsLen=wordsA.length();
  int count = (wordsLen-with)/speed;
  int i=0;
  rollNum=count+6+1;
  //Serial.println("rollNum:"+String(rollNum));
  for(i=0;i<=count;i++)
  {
    scrollText[7+i]=wordsA.substring(i*speed, speed*i+with);
    //Serial.println(scrollText[7+i]);
  }
  clk.unloadFont();
}


TFT_eSprite clkb = TFT_eSprite(&tft);

void scrollBanner(int rollNum) {
  if (scrollText[currentIndex])
  {
    clkb.setColorDepth(8);
    //clkb.loadFont(ZdyLwFont_20);
    clkb.loadFont(hzsel);
    clkb.createSprite(225, 30);
    clkb.fillSprite(bgColor);
    clkb.setTextWrap(false);
    //clkb.setTextDatum(CC_DATUM);
    clkb.setTextDatum(ML_DATUM);
    clkb.setTextColor(TFT_SILVER, bgColor);
    //clkb.drawString(scrollText[currentIndex], 74, 16);
    clkb.drawString(scrollText[currentIndex], 0, 16);
    clkb.pushSprite(15, 120);

    clkb.deleteSprite();
    clkb.unloadFont();

    if (currentIndex >= rollNum)
      currentIndex = 0;  //回第一个
    else
      currentIndex += 1;  //准备切换到下一个
  }
  prevTime = 1;
}


#if imgAst_EN
void imgAnim()
{
  int x = 190, y = 15;                               //int Anim = 0;      太空人图标显示指针记录
  if (millis() - AprevTime > 50) //x ms切换一次         // int AprevTime = 0; 太空人更新时间记录

  {
    Anim++;
    AprevTime = millis();
  }
  if (Anim == 8)
    Anim = 0;

  switch (Anim)
  {
    case 0:
      TJpgDec.drawJpg(x, y, hutao1, sizeof(hutao1));
      break;
    case 1:
      TJpgDec.drawJpg(x, y, hutao2, sizeof(hutao2));
      break;
    case 2:
      TJpgDec.drawJpg(x, y, hutao3, sizeof(hutao3));
      break;
    case 3:
      TJpgDec.drawJpg(x, y, hutao4, sizeof(hutao4));
      break;
    case 4:
      TJpgDec.drawJpg(x, y, hutao5, sizeof(hutao5));
      break;
    case 5:
      TJpgDec.drawJpg(x, y, hutao6, sizeof(hutao6));
      break;
    case 6:
      TJpgDec.drawJpg(x, y, hutao7, sizeof(hutao7));
      break;
    case 7:
      TJpgDec.drawJpg(x, y, hutao8, sizeof(hutao8));
      break;
    case 8:
      TJpgDec.drawJpg(x, y, hutao9, sizeof(hutao9));
      break;
    default:
      Serial.println("显示Anim错误");
      break;
  }
}
#endif

void luiAnim()
{
  int x = 113, y = 150;
  if (millis() - luiTime > 2000) //x ms切换一次         // int AprevTime = 0; 太空人更新时间记录
  {
    luiA++;
    luiTime = millis();
  }
  if (luiA == 4)
    luiA = 0;

  switch (luiA)
  {
    case 0:
      TJpgDec.drawJpg(x, y, lui1, sizeof(lui1));
      break;
    case 1:
      TJpgDec.drawJpg(x, y, lui2, sizeof(lui2));
      break;
    case 2:
      TJpgDec.drawJpg(x, y, lui3, sizeof(lui3));
      break;
    case 3:
      TJpgDec.drawJpg(x, y, lui4, sizeof(lui4));
      break;
    default:
      Serial.println("显示Anim错误");
      break;
  }

}
//屏幕加载动画替换进度条

#if imgAst_ENLoad
void imgAnimLoad()
{
  int x = 15, y = 20;
  if (millis() - AprevTimeLoad > 1) //x ms切换一次         // int AprevTimeLoad = 0; 更新时间记录

  {
    AnimLoad++;                                           //int AnimLoad = 0;      开机图标显示指针记录
    AprevTimeLoad = millis();
  }
  if (AnimLoad == 8)
    AnimLoad = 0;

  switch (AnimLoad)
  {
    case 0:
      TJpgDec.drawJpg(x, y, acg01, sizeof(acg01));
      break;
    case 1:
      TJpgDec.drawJpg(x, y, acg02, sizeof(acg02));
      break;
    case 2:
      TJpgDec.drawJpg(x, y, acg03, sizeof(acg03));
      break;
    case 3:
      TJpgDec.drawJpg(x, y, acg04, sizeof(acg04));
      break;
    case 4:
      TJpgDec.drawJpg(x, y, acg05, sizeof(acg05));
      break;
    case 5:
      TJpgDec.drawJpg(x, y, acg04, sizeof(acg04));
      break;
    case 6:
      TJpgDec.drawJpg(x, y, acg03, sizeof(acg03));
      break;
    case 7:
      TJpgDec.drawJpg(x, y, acg02, sizeof(acg02));
      break;
    case 8:
      TJpgDec.drawJpg(x, y, acg01, sizeof(acg01));
      break;    
      /*
      */
    default:
      Serial.println("显示AnimLoad错误");
      break;
  }
}
#endif






unsigned char Hour_sign   = 60;
unsigned char Minute_sign = 60;
unsigned char Second_sign = 60;
void digitalClockDisplay(int reflash_en)
{
  int timey = 55;
  if (hour() != Hour_sign || reflash_en == 1) //时钟刷新
  {
    //dig.printfW3660(20, timey, hour() / 10);
    //dig.printfW3660(60, timey, hour() % 10);

    Hour_sign = hour();
  }
  

  if (minute() != Minute_sign  || reflash_en == 1) //分钟刷新
  {
    //dig.printfO3660(101, timey, minute() / 10);
    //dig.printfO3660(141, timey, minute() % 10);
    Minute_sign = minute();
  }


  clk.createSprite(195, 70);
  clk.setTextSize(6);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(TFT_SILVER, bgColor);
  String hr=String(hour());
  String bufh;
  if(hr.length() == 1 )
  {
    bufh = "0"+hr;
  }
  else
  {
    bufh = hr;
  }
  String mnt=String(minute());
  String bufm;
  if(mnt.length() == 1 )
  {
    bufm = "0"+mnt;
  }
  else
  {
    bufm = mnt;
  }
  clk.drawString(bufh+":"+bufm, 0, 40);
  clk.pushSprite(15, timey);
  clk.deleteSprite();
  if (second() != Second_sign  || reflash_en == 1) //分钟刷新
  {
    //dig.printfW1830(182, timey + 30, second() / 10);
    //dig.printfW1830(202, timey + 30, second() % 10);
    String scd=String(second());
    String buf;
    if(scd.length() == 1 )
    {
      buf = "0"+scd;
    }
    else
    {
      buf = scd;
    }
    clk.createSprite(40, 30);
    clk.setTextSize(3);
    clk.fillSprite(bgColor);
    clk.setTextDatum(ML_DATUM);
    clk.setTextColor(TFT_SILVER, bgColor);
    clk.drawString(buf, 0, 20);
    clk.pushSprite(195, timey+30);
    Second_sign = second();
  }

  if (reflash_en == 1) reflash_en = 0;
  /***日期****/
  clk.setColorDepth(8);
  //clk.loadFont(ZdyLwFont_20);
  clk.loadFont(hzsel);

  //星期
  clk.createSprite(58, 30);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(TFT_SILVER, bgColor);
  clk.drawString(week(), 0, 16);
  clk.pushSprite(100, 26);
  //clk.pushSprite(182, 0);
  clk.deleteSprite();

  //月日
  clk.createSprite(95, 30);
  clk.fillSprite(bgColor);
  clk.setTextDatum(CC_DATUM);
  clk.setTextColor(TFT_SILVER, bgColor);
  clk.drawString(monthDay(), 49, 16);
  //clk.pushSprite(5, 150);
  clk.pushSprite(5, 26);
  clk.deleteSprite();

  clk.unloadFont();
  /***日期****/
}

//星期
String week()
{
  String wk[7] = {"日", "一", "二", "三", "四", "五", "六"};
  String s = "周" + wk[weekday() - 1];
  return s;
}

//月日
String monthDay()
{
  String s = String(month());
  s = s + "月" + day() + "日";
  return s;
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP时间在消息的前48字节中
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // 无法获取时间时返回0
}

// 向NTP服务器发送请求
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
