#include <EasyWebSocket.h>
#include <ESP8266WiFi.h>
#include <Hash.h>

const char* ssid = "xxxx";
const char* password = "xxxx";
const char* LocalIPaddress = "xxx.xxx.xxx.xxx";

long CountTestTime;

byte cnt = 0;

EasyWebSocket ews;

String html_str;
String ret_str;
String view_scale = "1.1";

byte s_rate = 10;
int PingSendTime = 3000;

#define ledPin1 12
#define ledPin2 13
#define ledPin3 14

void setup() 
{

  html_str = ews.EWS_Body_style("WHITE", "BLACK");
  html_str += "<h2 style='color:#5555FF'><center>ESP-WROOM-02(ESP8266)<br>\r\n";
  html_str += "EasyWebSocket</center></h2>\r\n";
  html_str += "from WROOM DATA = \r\n";
  html_str += "<font size=4>\r\n";
  html_str += ews.EWS_Receive_Text("text","25px","RED");
  html_str += "<br><br>";
  html_str += "BLUE \r\n";
  html_str += ews.EWS_OnOff_Button("BLUE",60,25,15,"#FFFFFF","#0000FF");
  html_str += ews.EWS_Slider_BoxText("vbox1",30,20,15,"BLACK");
  html_str += ews.EWS_Touch_Slider("BLUE", "vbox1");
  html_str += "<br><br>\r\n";
  html_str += "GREEN \r\n";
  html_str += ews.EWS_OnOff_Button("GREEN",60,25,15,"#000000","#00FF00");
  html_str += ews.EWS_Slider_BoxText("vbox2",30,20,15,"#000000");
  html_str += ews.EWS_Touch_Slider("GREEN", "vbox2");
  html_str += "<br><br>\r\n";
  html_str += "RED\r\n";
  html_str += ews.EWS_OnOff_Button("RED",60,25,15,"#FFFFFF","#FF0000");
  html_str += ews.EWS_Slider_BoxText("vbox3",30,20,15,"BLACK");
  html_str += ews.EWS_Touch_Slider("RED", "vbox3");
  html_str += "<br><br>\r\n";
  html_str += "B-G-R\r\n";
  html_str += ews.EWS_Slider_BoxText("vbox4",40,20,15,"BLACK");
  html_str += ews.EWS_Touch_Slider("BGR", "vbox4") ;
  html_str += "<br><br>\r\n";
  html_str += "<div id='msg' style='font-size:25px; color:#FF0000;'></div><br>\r\n";
  html_str += ews.EWS_Close_Button("WS CLOSE",150,40,17);
  html_str += "<br>\r\n";

  
  ews.AP_Connect(ssid, password);

  ews.EWS_HandShake(LocalIPaddress, view_scale, s_rate, html_str);
  
  CountTestTime = millis();
}

void loop() {
  String str;
  if(ret_str != "_close"){
    if(millis()-CountTestTime > 500){
      if(cnt > 3){
        cnt = 0;
      }
      switch(cnt){
        case 0:
          str = "Hello! World";
          break;
        case 1:
          str = "WebSocket";
          break;
        case 2:
          str = "How are you";
          break;
        case 3:
          str = "Fine!!";
          break;
      }
      
      ews.EWS_String_DATA_SEND(str, "text");
      CountTestTime = millis();
      cnt++;
    }

    ret_str = ews.EWS_Char_Receive(PingSendTime);
    
    if(ret_str != "\0"){
      String led_id = ret_str.substring(ret_str.indexOf("|")+1, ret_str.indexOf(";"));
      Serial.print("ret_str = ");
      Serial.println(ret_str);
      Serial.print("led_id=");
      Serial.println(led_id);
      
      byte ws_data = (ret_str[0]-0x30)*100 + (ret_str[1]-0x30)*10 + (ret_str[2]-0x30);
      LED_PWM(led_id, ws_data);
    }
  }else if(ret_str == "_close"){
    delay(100);
    char res_html[html_str.length()+1];
    html_str.toCharArray(res_html, html_str.length());
    ews.EWS_HandShake(LocalIPaddress, view_scale, s_rate, html_str);
    CountTestTime = millis();
    ret_str = String('\0');
  }
}

void LED_PWM(String str_id, byte data_b)
{
  if(str_id == "BLUE"){
    analogWrite(ledPin1, data_b*2.5);
  }
  if(str_id == "GREEN"){
    analogWrite(ledPin2, data_b*2.5);
  }
  if(str_id == "RED"){
      analogWrite(ledPin3, data_b*2.5);
  }
  if(str_id == "BGR"){
    if(data_b < 34){
      analogWrite(ledPin1, data_b*8);
      analogWrite(ledPin2, 0);
      analogWrite(ledPin3, 0);
    }else if(data_b > 33 && data_b < 67){
      analogWrite(ledPin2, (data_b-33)*8);
      analogWrite(ledPin1, 0);
      analogWrite(ledPin3, 0);
    }else if(data_b > 66){
      analogWrite(ledPin3, (data_b-66)*7.78);
      analogWrite(ledPin1, 0);
      analogWrite(ledPin2, 0);
    }
  }
}
