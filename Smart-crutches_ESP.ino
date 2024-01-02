/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6GfO9t2Ck"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "PXP28OpBG6GRinQsCssuLluNLHSTs2r6"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
// Your WiFi credentials.
// Set password to "" for open networks.


SoftwareSerial mySerial(D6, D5); 

BlynkTimer timer;
WiFiManager wifiManager;

String nhiptim, nhietdo, oxy , data[3];
int Pos1, Pos2, Pos3;
uint32_t lastSendData = 0;
// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0) {
  // Set incoming value from pin V0 to a variable
  Pos1 = param.asInt();

  // Update state
}
BLYNK_WRITE(V1) {
  // Set incoming value from pin V0 to a variable
  Pos2 = param.asInt();

  // Update state
}
BLYNK_WRITE(V2) {
  // Set incoming value from pin V0 to a variable
  Pos3 = param.asInt();

  // Update state
}

// This function is called every time the device is connected to the Blynk.Cloud


// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V6, millis() / 1000);
}

void setup() {
  // Debug console
  mySerial.begin(4800);
  Serial.begin(9600);
  wifiManager.autoConnect("Nang chong thong minh");
  Serial.println("connected... :)");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  sendUart();
if (millis()-  lastSendData >2000){
   nhandulieu();
  Blynk.virtualWrite(V3, nhiptim);
  Blynk.virtualWrite(V4, oxy);
  Blynk.virtualWrite(V5, nhietdo); }
}


void sendUart() {
  // float data1[3] = { mode_1, state1_1, state2_2 };  // Dữ liệu của 3
  String data1[3] = { String(Pos1), String(Pos2), String(Pos3) };



  String combinedData1 = "";  // Khởi tạo chuỗi kết quả

  combinedData1 = data1[0] + "," + data1[1] + "," + data1[2] + "\n";
  // Kết hợp các dữ liệu vào chuỗi

  // Gửi chuỗi dữ liệu qua UART
  mySerial.print(combinedData1);
  //Serial.print(combinedData1);
}


void nhandulieu() {
  if (mySerial.available()) {                              // Kiểm tra xem có dữ liệu đang được gửi đến không
    String receivedData = mySerial.readStringUntil('\n');  // Đọc chuỗi dữ liệu cho đến khi gặp ký tự '\n'

    // Tách dữ liệu bằng dấu phẩy

    int startIndex = 0;
    int commaIndex;

    for (int i = 0; i < 3; i++) {
      commaIndex = receivedData.indexOf(',', startIndex);
      if (commaIndex != -1) {
        data[i] = receivedData.substring(startIndex, commaIndex);
        startIndex = commaIndex + 1;
      } else {
        data[i] = receivedData.substring(startIndex);
        break;
      }
    }
    nhiptim = data[0];
    oxy = data[1];
    nhietdo = data[2];
    Serial.println(nhiptim);
     Serial.println( oxy);
      Serial.println(nhietdo);
  }
}
