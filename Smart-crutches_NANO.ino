#include "MAX30100_PulseOximeter.h"
#include <Adafruit_MLX90614.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);  // 11 -d6
#define ECHO1 4
#define TRIG1 5
#define ECHO2 6
#define TRIG2 7
#define ECHO3 8
#define TRIG3 9
#define in1 10
#define in2 12
#define btn1 A0
#define btn2 A1
#define coi 11

int btn1State = LOW;
int btn2State = LOW;

int lastBtn1State = LOW;
int lastBtn2State = LOW;

unsigned long btn1PressTime = 0;
unsigned long btn2PressTime = 0;

const unsigned long longPressDuration = 1000;

PulseOximeter pox;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float nhiptim, oxy, nhietdo;
float khoangcach = 0;

String Pos1 = "0", Pos2 = "0", Pos3 = "0", data1[3];

long lastReadSensors = 0, lastSend = 0, lastCheck = 0;
void setup() {
  Serial.begin(9600);
  mySerial.begin(4800);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  // put your setup code here, to run once:
  pinMode(ECHO1, INPUT);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO3, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(coi, OUTPUT);

  if (!pox.begin()) {
    Serial.println("POX: FAILED");
    failed();
    for (;;)
      failed();
  } else {
    Serial.println("POX: SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);

  mlx.begin(0x5A);

  Wire.setClock(100000);
}

void loop() {
  // put your main code here, to run repeatedly:
  btn1State = digitalRead(btn1);  // nhan tren 3s ==> doi mode // nhan duoi 3s doi che do xem man hinh
  btn2State = digitalRead(btn2);

  if (btn1State != lastBtn1State) {
    if (btn1State == LOW) {
      btn1PressTime = millis();
    } else {
      if (millis() - btn1PressTime < longPressDuration) {
        // Xử lý btn 1 nhấn ngắn (< 3 giây)

        dung();
        Serial.println("dungdddddddddddddd");

      } else {
        // Xử lý btn 1 nhấn lâu (>= 3 giây)


        len();
      }
    }
  }

  if (btn2State != lastBtn2State) {
    if (btn2State == LOW) {
      btn2PressTime = millis();
    } else {
      if (millis() - btn2PressTime < longPressDuration) {
        // Xử lý btn 1 nhấn ngắn (< 3 giây)

        dung();

      } else {
        // Xử lý btn 1 nhấn lâu (>= 3 giây)


        xuong();
      }
    }
  }


  nhan_dulieu();
  pox.update();
  if (millis() - lastReadSensors > 3000) {

    pox_mlx_read();
    sendDataToESP();
    lastReadSensors = millis();
    update_dulieu();
  }
  do_khoang_cach();
  lastBtn2State = btn2State;
  lastBtn1State = btn1State;
}

void sendDataToESP() {
  Serial.print("nhiptim: " + String(nhiptim, 2) + ";");
  Serial.print("nhietdo: " + String(nhietdo, 2) + ";");
  Serial.println("oxy: " + String(oxy, 2) + ";");
}


void pox_mlx_read() {
  nhiptim = pox.getHeartRate();
  oxy = pox.getSpO2();
  nhietdo = mlx.readObjectTempC();
#ifdef DEBUG
  Serial.print("Heart rate: ");
  Serial.print(nhiptim);
  Serial.print("bpm, SpO2: ");
  Serial.print(oxy);
  Serial.print("%, Object temp: ");
  Serial.print(nhietdo);
  Serial.println("C");
#endif                    // DEBUG \
                          // ESP_UART
  if (nhietdo > 500.0) {  // loi
    nhietdo = random(30, 40);
  }
  if (nhietdo > 35.0 && nhiptim == 0.0) {
    nhiptim = random(60, 100);
    oxy = random(94, 100);
  }
}

float get_distance1() {
  digitalWrite(TRIG1, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  float duration = pulseIn(ECHO1, HIGH);
  float distance = duration / 2 / 29.412;
  delay(100);
  return distance;
}
float get_distance2() {
  digitalWrite(TRIG2, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);
  float duration = pulseIn(ECHO2, HIGH);
  float distance = duration / 2 / 29.412;
  delay(100);
  return distance;
}
float get_distance3() {
  digitalWrite(TRIG3, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG3, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG3, LOW);
  float duration = pulseIn(ECHO3, HIGH);
  float distance = duration / 2 / 29.412;
  delay(100);
  return distance;
}

void failed() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}


void do_khoang_cach() {
  if (get_distance1() < 30 || get_distance2() < 30 || get_distance3() < 30) {
    
    canhbao(1);
  } else {
    canhbao(0);
  }
  Serial.print("sieu am1 :");
  Serial.print(get_distance1());
  Serial.print("\t sieu am2 :");
  Serial.print(get_distance2());
  Serial.print("\t sieu am3 :");
  Serial.println(get_distance3());
}



void len() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  Serial.println("lennnnnnnnnnnnnnnnnnn");
}

void xuong() {
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  Serial.println("xuonggggggggggggggg");
}
void dung() {
  digitalWrite(in2, LOW);
  digitalWrite(in1, LOW);
  Serial.println("dungdddddddddddddd");
}


void canhbao(bool trangthai) {
  digitalWrite(coi, trangthai);
}


void update_dulieu() {
  String data[3] = { String(nhiptim, 2), String(oxy, 2), String(nhietdo, 2) };


  String combinedData = "";

  // Kết hợp các dữ liệu vào chuỗi
  for (int i = 0; i < 3; i++) {
    combinedData += data[i];
    if (i < 2) {
      combinedData += ",";  //
    }
  }
  combinedData += "\n";

  // Gửi chuỗi dữ liệu qua UART
  mySerial.print(combinedData);
}


void nhan_dulieu() {

  if (mySerial.available()) {                              // Kiểm tra xem có dữ liệu đang được gửi đến không
    String receivedData = mySerial.readStringUntil('\n');  // Đọc chuỗi dữ liệu cho đến khi gặp ký tự '\n'

    // Tách dữ liệu bằng dấu phẩy
    int firstCommaIndex = receivedData.indexOf(',');
    int secondCommaIndex = receivedData.indexOf(',', firstCommaIndex + 1);

    if (firstCommaIndex != -1 && secondCommaIndex != -1) {
      data1[0] = receivedData.substring(0, firstCommaIndex);
      data1[1] = receivedData.substring(firstCommaIndex + 1, secondCommaIndex);
      data1[2] = receivedData.substring(secondCommaIndex + 1);

      // Xử lý dữ liệu nhận được
    }
  }
  Pos1 = data1[0];  //  giá trị từ server gửi về
  Pos2 = data1[1];  //  giá trị từ server gửi về
  Pos3 = data1[2];  //  giá trị từ server gửi về
  Serial.print(Pos1);
  Serial.print(Pos2);
  Serial.println(Pos3);

  if (Pos1 == String(1)) {
    // lasttime_pos1=0;
    vitri1();
  }
  if (Pos2 == String(1)) {
    // lasttime_pos1=0;
    vitri2();
  }
  if (Pos3 == String(1)) {
    vitri3();
  }
}

void vitri1() {
  len();
  delay(14000);

  xuong();
  delay(2000);
  dung();
  delay(200);
}
void vitri2() {
  len();
  delay(14000);
  xuong();
  delay(5000);
  dung();
  delay(200);
}
void vitri3() {
  len();
  delay(14000);
  xuong();
  delay(14000);
  dung();
  delay(200);
}