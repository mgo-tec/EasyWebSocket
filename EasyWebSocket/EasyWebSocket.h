/*
  EasyWebsocket.h - WebSocket for ESP-WROOM-02 ( esp8266 )
  This is still a beta version of the beta.

The MIT License (MIT)

Copyright (c) 2015 Mgo-tec

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef EasyWebSocket_h_
#define EasyWebSocket_h_
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "Hash.h"
#include "FS.h"

class EasyWebSocket
{
public:
  EasyWebSocket();

  void AP_Connect(const char* ssid, const char* password);
  void EWS_HandShake(const char* _LocalIPaddress, String _view_scale, byte _s_rate, String _res_html);
  void EWS_HTTP_Responce();
  void Hash_Key(String h_req_key, char* h_resp_key);
  void EWS_String_DATA_SEND(String str, String id);
  void EWS_PING_SEND();
  signed int EWS_DATA_Receive(int pTime);
  String EWS_Char_Receive(int pTime);
  String EWS_OnOff_Button(String button_id, int width, int height, byte font_size, String f_color, String b_color);
  String EWS_Touch_Slider(String slider_id, String vbox_id);
  String EWS_Mouse_Slider(String slider_id, String vbox_id);
  String EWS_Slider_BoxText(String vbox_id, int width, int height, byte font_size, String color);
  String EWS_Body_style(String text_color, String bg_color);
  String EWS_Receive_Text(String id, String fnt_size, String fnt_col);
  String EWS_Close_Button(String name, int width, int height, byte font_size);
  
private:
  boolean _Ini_html_on = false;
  boolean _WS_on = false;
  char _Android_or_iPad;
  long _PingLastTime;
  long _PongLastTime;

};

#endif