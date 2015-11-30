/*
  EasyWebsocket.cpp - WebSocket for ESP-WROOM-02 ( esp8266 )
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

#include "Arduino.h"
#include "EasyWebsocket.h"

const char* GUID_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WiFiClient client;
WiFiServer server(80);

EasyWebSocket::EasyWebSocket(){}

//********AP(Router) Connection****
void EasyWebSocket::AP_Connect(const char* ssid, const char* password)
{
  Serial.begin(115200);
  // Connect to WiFi network
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
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  delay(10);
}

//********WebSocket Hand Shake ****************
void EasyWebSocket::EWS_HandShake(const char* _LocalIPaddress, String _view_scale, byte _s_rate, String _res_html)
{
  client = server.available();
  delay(1);
  String req;
  String hash_req_key;
  
  while(!_WS_on){
    delay(1);
    switch(_Ini_html_on){
    case false:
      client = server.available();
      while(client){
        if(client.available()){
          req = client.readStringUntil('\n');
          Serial.println(req);
          if (req.indexOf("GET / HTTP") != -1){
            Serial.println("-----from Browser FirstTime HTTP Request---------");
            Serial.println(req);

            while(req.indexOf("\r") != 0){
              req = client.readStringUntil('\n');

              if(req.indexOf("Android") != -1){
                _Android_or_iPad = 'A';
              }else if(req.indexOf("iPad") != -1){
                _Android_or_iPad = 'i';
              }else if(req.indexOf("iPhone") != -1){
                _Android_or_iPad = 'P';
              }
              Serial.println(req);
            }
            req = "";
            delay(10);

            client.print(F("HTTP/1.1 200 OK\r\n"));
            client.print(F("Content-Type:text/html\r\n"));
            client.print(F("Connection:close\r\n\r\n"));
            
            SPIFFS.begin();
            
            File f1 = SPIFFS.open("/spiffs_01.txt", "r");
            char c = f1.read();
            client.print(c);
            while(c<0xDE){
              c= f1.read();
              if(c>0xDD) break;
              client.print(c);
            }
            
            client.print(_view_scale);
            
            File f2 = SPIFFS.open("/spiffs_02.txt", "r");
            c = f2.read();
            client.print(c);
            while(c<0xDE){
              c= f2.read();
              if(c>0xDD) break;
              client.print(c);
            }
            
            client.print(_LocalIPaddress);
            
            File f3 = SPIFFS.open("/spiffs_03.txt", "r");
            c = f3.read();
            client.print(c);
            while(c<0xDE){
              c= f3.read();
              if(c>0xDD) break;
              client.print(c);
            }
            
            client.print(String(_s_rate));
            
            File f4 = SPIFFS.open("/spiffs_04.txt", "r");
            c = f4.read();
            client.print(c);
            while(c<0xDE){
              c= f4.read();
              if(c>0xDD) break;
              client.print(c);
            }
            
            client.print(_res_html);
            
            File f5 = SPIFFS.open("/spiffs_05.txt", "r");
            c = f5.read();
            client.print(c);
            while(c<0xDE){
              c= f5.read();
              if(c>0xDD) break;
              client.print(c);
            }
            
            delay(1);
            client.stop();

            Serial.println("\nGET HTTP client stop--------------------");
            req = "";
            _Ini_html_on = true;

            if(_Android_or_iPad == 'i'){
              break;
            }
          }
        }
      }
      break;
    case true:
      switch(_WS_on){
        case false:
          EasyWebSocket::EWS_HTTP_Responce();
          _PingLastTime = millis();
          _PongLastTime = millis();
          
          break;
        case true:
          Serial.println("WS_Conplete!!!!!!!");
          break;
      }
      break;
    }
  }
}

//************HTTP Response**************************
void EasyWebSocket::EWS_HTTP_Responce()
{
  client = server.available();
  delay(1);
  String req;
  String hash_req_key;
  
  while(client){
    if(client.available()){
      req = client.readStringUntil('\n');
      Serial.println(req);
      if (req.indexOf("websocket") != -1){
        Serial.println("-----from Browser HTTP WebSocket Request---------");
        Serial.println(req);
 
        while(req.indexOf("\r") != 0){
          req = client.readStringUntil('\n');
          Serial.println(req);
          if(req.indexOf("Sec-WebSocket-Key")>=0){
            hash_req_key = req.substring(req.indexOf(':')+2,req.indexOf('\r'));
            Serial.println();
            Serial.print("hash_req_key =");
            Serial.println(hash_req_key);
          }        
        }
  
        delay(10);
        req ="";
  
        char h_resp_key[29];

        EasyWebSocket::Hash_Key(hash_req_key, h_resp_key);
        
        Serial.print("h_resp_key = ");
        Serial.println(h_resp_key);
        String str;

        str = "HTTP/1.1 101 Switching Protocols\r\n";
        str += "Upgrade: websocket\r\n";
        str += "Connection: Upgrade\r\n";
        str += "Sec-WebSocket-Accept: ";
        for(byte i=0; i<28; i++){
          str += h_resp_key[i];
        }

        str += "\r\n\r\n";
        
        Serial.println("-----HTTP Respons start-------");
        Serial.println(str);
        client.print(str);
        str = "";
  
        _WS_on = true;
        break;
  
      }else if(req.indexOf("favicon") != -1){

        Serial.println();
        Serial.println("--****GET favicon Request************");
        Serial.print(req);
        while(client.available()){

          Serial.write(client.read());
        }
        delay(1);
        client.stop();
        delay(1);
        Serial.println();
        Serial.println("favicon_Client Stop--------------");
      }
    }
  }
}

//************Hash sha1 base64 encord**************************
void EasyWebSocket::Hash_Key(String h_req_key, char* h_resp_key)
{
  char Base64[65] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
                      'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
                      'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
                      'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
                      '='
                    };
  byte hash_six[27];
  byte dummy_h1, dummy_h2;
  byte bb;
  byte i, j;
  i=0;
  j=0;
  
  String merge_str;

  merge_str = h_req_key + String(GUID_str);
  Serial.println();
  Serial.print("merge_str =");
  Serial.println(merge_str);
  Serial.print("SHA1:");
  Serial.println(sha1(merge_str));

  byte hash[20];
  sha1(merge_str, &hash[0]);

  for( i = 0; i < 20; i++) {
    hash_six[j] = hash[i]>>2;
    
    hash_six[j+1] = hash[i+1] >> 4;
    bitWrite(hash_six[j+1], 4, bitRead(hash[i],0));
    bitWrite(hash_six[j+1], 5, bitRead(hash[i],1));
    
    if(j+2 < 26){
      hash_six[j+2] = hash[i+2] >> 6;
      bitWrite(hash_six[j+2], 2, bitRead(hash[i+1],0));
      bitWrite(hash_six[j+2], 3, bitRead(hash[i+1],1));
      bitWrite(hash_six[j+2], 4, bitRead(hash[i+1],2));
      bitWrite(hash_six[j+2], 5, bitRead(hash[i+1],3));
    }else if(j+2 == 26){
      dummy_h1 = 0;
      dummy_h2 = 0;
      dummy_h2 = hash[i+1] << 4;
      dummy_h2 = dummy_h2 >>2;
      hash_six[j+2] = dummy_h1 | dummy_h2;
    }
    
    if( j+3 < 27 ){
      hash_six[j+3] = hash[i+2];
      bitWrite(hash_six[j+3], 6, 0);
      bitWrite(hash_six[j+3], 7, 0);
    }else if(j+3 == 27){
      hash_six[j+3] = '=';
    }
    
    h_resp_key[j] = Base64[hash_six[j]];
    h_resp_key[j+1] = Base64[hash_six[j+1]];
    h_resp_key[j+2] = Base64[hash_six[j+2]];
    
    if(j+3==27){
      h_resp_key[j+3] = Base64[64];
      break;
    }else{
      h_resp_key[j+3] = Base64[hash_six[j+3]];
    }
    
    i = i + 2;
    j = j + 4;
  }
  h_resp_key[28] = '\0';

  Serial.print("hash_six = ");
  for(i=0; i<28; i++){
    Serial.print(hash_six[i],BIN);
    Serial.print('_');
  }
  Serial.println();
}
  
void EasyWebSocket::EWS_String_DATA_SEND(String str, String id)
{
  str += '|' + id + '|' +'\0';
  client.write(B10000001);
  client.write(str.length());
  client.print(str); 
}

signed int EasyWebSocket::EWS_DATA_Receive(int pTime)
{
  byte b=0;
  byte data_len;
  byte mask[4];
  byte data_b;
  byte i;
  
  if(pTime > 0){
    if(millis()-_PingLastTime > pTime){
      EasyWebSocket::EWS_PING_SEND();
      _PingLastTime = millis();
    }

    if((millis() - _PongLastTime) > (pTime + 500)){
      delay(1);
      client.stop();
      delay(1);
      Serial.println();
      Serial.println("Pong_Client.STOP-----------------");
      _WS_on = false;
      _Ini_html_on = false;
      return -100;
    }
  }
  
  
  if(client.available()){
    b = client.read();
    if(b == B10000001 || b == B10001010){
      switch (b){
        case B10000001:
          _PingLastTime = millis();
          _PongLastTime = millis();
          break;
        case B10001010:
          _PongLastTime = millis();
          Serial.println("Pong Receive**********");
          break;
      }

      b = client.read();
      data_len = b - B10000000;
      
      for(i=0; i<4; i++){
        mask[i] = client.read();
      }
      
      byte m_data[data_len];
      char data_c[data_len];
             
      for(i = 0; i<data_len; i++){
        m_data[i] = client.read();
        data_c[i] = mask[i%4]^m_data[i];
      }
      Serial.println();   
                        
      switch (data_len){
        case 1:
          data_b = data_c[0]-0x30;
          break;
        case 2:
          data_b = ((data_c[0]-0x30)*10) + (data_c[1]-0x30);
          break;
        case 3:
          data_b = ((data_c[0]-0x30)*100) + ((data_c[1]-0x30)*10) + (data_c[2]-0x30);
          break;
      }

      return data_b;

    }else if(b == B10001000){
      Serial.println("Close Receive------------");
      delay(1);
      client.write(B10001000);
      delay(1);
      Serial.println("Close Send------------");
      Serial.println(b,BIN);
    }
  }else{
    return -1;
  }
}

String EasyWebSocket::EWS_Char_Receive(int pTime)
{
  byte b=0;
  byte data_len;
  byte mask[4];
  byte i;
  String str_close = "_close";

  if(pTime > 0){
    if(millis()-_PingLastTime > pTime){
      EasyWebSocket::EWS_PING_SEND();
      _PingLastTime = millis();
    }

    if((millis() - _PongLastTime) > (pTime + 500)){
      delay(1);
      client.stop();
      delay(1);
      Serial.println();
      Serial.println("Pong_Client.STOP-----------------");
      _WS_on = false;
      _Ini_html_on = false;
      return str_close;
    }
  }
  
  if(client.available()){
    b = client.read();
    if(b == B10000001 || b == B10001010){
      switch (b){
        case B10000001:
          _PingLastTime = millis();
          _PongLastTime = millis();
          break;
        case B10001010:
          _PongLastTime = millis();
          Serial.println("Pong Receive**********");
          break;
      }

      b = client.read();
      data_len = b - B10000000;

      for(i=0; i<4; i++){
        mask[i] = client.read();
      }
      
      byte m_data[data_len];
      char data_c[data_len + 1];
          
      for(i = 0; i<data_len; i++){
        m_data[i] = client.read();
        data_c[i] = mask[i%4]^m_data[i];
      }
      data_c[data_len] = '\0';

      return String(data_c);

    }else if(b == B10001000){
      Serial.println("Close Receive------------");
      delay(1);
      client.write(B10001000);
      delay(1);
      Serial.println("Close Send------------");
      Serial.println(b,BIN);
      return str_close;
    }
  }else{
    return String('\0');
  }
}

void EasyWebSocket::EWS_PING_SEND()
{
  client.write(B10001001);
  client.write(4);
  client.print("Ping");
  Serial.println();
  Serial.println("Ping Send-----------");
}

String EasyWebSocket::EWS_OnOff_Button(String button_id, int width, int height, byte font_size, String f_color, String b_color)
{
  String str;
  str = "<input type='button' value='OFF' onClick=\"OnOffBt(this,'";
  str += button_id;
  str += "')\"";
  str += " style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px; color:";
  str += f_color;
  str += "; background-color:";
  str += b_color;
  str += ";' >\r\n";
  
  return str;
}

String EasyWebSocket::EWS_Touch_Slider(String slider_id, String vbox_id)
{
  String str;
  str += "<input type='range' ontouchmove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += vbox_id;
  str += "').value=this.value;\">\r\n";
  return str;
}

String EasyWebSocket::EWS_Mouse_Slider(String slider_id, String vbox_id)
{
  String str;
  str += "<input type='range' onMousemove=\"doSend(this.value,'";
  str += slider_id;
  str += "'); document.getElementById('";
  str += vbox_id;
  str += "').value=this.value;\">\r\n";
  return str;
}

String EasyWebSocket::EWS_Slider_BoxText(String vbox_id, int width, int height, byte font_size, String color)
{
  String str;
  str = "<input type='number' id='";
  str += vbox_id;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px; color:";
  str += String(color);
  str += ";' >\r\n";
  return str;
}

String EasyWebSocket::EWS_Body_style(String text_color, String bg_color)
{
  String str;
  str = "<body text='";
  str += text_color;
  str += "' bgcolor='";
  str += bg_color;
  str += "'>\r\n";
  return str;
}

String EasyWebSocket::EWS_Receive_Text(String id, String fnt_size, String fnt_col)
{
  String str;
  str = "<span id='" + id + "' style='font-size:" + fnt_size + "; color:" + fnt_col + ";'></span>\r\n";
  return str;
}

String EasyWebSocket::EWS_Close_Button(String name, int width, int height, byte font_size)
{
  String str;
  str = "<input type='button' value='";
  str += name;
  str += "' style='width:";
  str += String(width);
  str += "px; ";
  str += "height:";
  str += String(height);
  str += "px; font-size:";
  str += String(font_size);
  str += "px;' onclick='WS_close()'>\r\n";
  return str;
}