//// cài đặt fribase
#include <ESP8266WiFi.h>
#include<FirebaseESP8266.h>
#include <ACS712.h>
#include <ArduinoJson.h>

FirebaseData firebaseData;
FirebaseData booleanData;
FirebaseData DoubleData;
FirebaseData IntData;
FirebaseJson json;

#define FIREBASE_HOST "https://smartelectricityprovider-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "5a84h46EUhB1ra0JNn4N9X1ytsADy6an8SlEC0JN"
String dulieu;
char* ssid = "Tony House";    //enter your wifi name
char* password = "songvanminh"; // enter your wifi password
float RealPower;
int checkOnOff = D5;
int i = 0;
boolean isOn = false;

int day = 0;
int month = 0;
int year = 0;
String fullday = "";
int timezone = 7 * 3600;
int dst = 0;
int sec = 0;
int hour = 0;
int lastSecond = 0;
int gapSecond = 0;
float LastPower = 0;

ACS712 sensor(ACS712_05B, A0);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(". ");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for server");
  while (!time(nullptr)) {
    Serial.print("#");
    delay(500);
  }
  Serial.println("Time respone is ok!");
  
  Serial.println("Đảm bảo không có dòng điện đi qua cảm biến trong quá trình cân bằng");
  delay(1000);
  Serial.println("Calibrating...");
  delay(200);
  sensor.calibrate();
  Serial.println("Quá trình cân bằng hoàn tất!!!");

  pinMode(checkOnOff, OUTPUT);
}

void loop() {

  Firebase.getDouble(DoubleData, "/Sensor1/WebRead/" + fullday);
  LastPower = DoubleData.doubleData();
  delay(100);

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  day = p_tm->tm_mday;
  month = p_tm->tm_mon + 1;
  year = p_tm->tm_year + 1900;
  sec = p_tm->tm_sec;
  hour =  p_tm->tm_hour + 1;

  fullday = String(hour) + " hour"+ "-" + String(day)  + "-" + String(month) + "-" +  String(year);

  Firebase.getInt(IntData, "/Sensor1/ArduinoRead/Sec");
  delay(100);

  if(sec > IntData.intData()) {
    gapSecond = sec - IntData.intData();
  } else {
    gapSecond = sec + 60 - IntData.intData();
  }

  Firebase.setDouble(firebaseData, "/Sensor1/WebRead/" + fullday, measureThePower() * ((float) sec) + LastPower);
  delay(100);

  Firebase.setInt(firebaseData, "/Sensor1/ArduinoRead/Sec", p_tm->tm_sec);
  delay(100);

  if(digitalRead(checkOnOff) == HIGH) {
    isOn = false;
  } else {
    isOn = true;
  }

  Firebase.setBool(firebaseData, "/Sensor1/WebRead/isOn", isOn);////test
  delay(100);

  Firebase.getBool(booleanData, "/Sensor1/ArduinoRead/isOn");
  if(booleanData.boolData() == true) {
    digitalWrite(checkOnOff, LOW);
  } else {
    digitalWrite(checkOnOff, HIGH);
  }
  delay(100);
}

float measureThePower() {
  float I_temp, tong, I_TB, mA, power;
  for (int i=0; i<=100; i++)
  {
    I_temp = sensor.getCurrentAC();
    tong = tong + I_temp;
    
    delay(5);
  }
  I_TB = tong/100;
  tong = 0;
  power = I_TB * 220;
  return power;
}