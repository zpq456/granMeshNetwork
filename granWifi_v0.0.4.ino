

/* granWifi v0.0.0
   - 특정 와이파이에 접속가능
   - mysql기반의 DB와 연동하여 데이터를 테이블에 삽입 가능
   - 온도센서를 사용하여 온도값 수집 가능
   - I2C 모듈 설치완료
   - EEPROM을 사용하여 사용자 설정값을 저장&불러오기

   210907 granWifi v0.0.1
   - DB서버에 접근해서 SELECT TABLE수행 => 추후에 JSON방식을 도입할것
   [ 최근 데이터부터 10개까지 가져와서 Serial에 출력 ]

   210908 granWifi v0.0.2
   - Serial2를 통해 USE모듈의 저장소에 데이터를 텍스트화 하여 저장
   - RTC 모듈로 현재 시간을 불러올 수 있음

   210908 granWifi v0.0.3
   - softwareSerial을 통해 485통신으로 센서값을 받아옴
   - 받아온 통신값(hex)를 float값으로 변경
   - RTC값을 지정할 수 있게 Serial 명령어로 추가.
   - USB에 Write할 시간을 EEPROM에 저장하여 관리
   - USB에 Write할 시간을 정할 수 있게 Serial 명령어로 추가.

   2100930 granWifi v0.0.4
   - DB서버의 SELECT TABLE 데이터를 JSON방식으로 받아옴 (sensor master table)
   - 센서 세팅값을 DB에 추가.
   - 새로운 장비가 추가되면 DB에 해당 장비의 serial 데이터를 기본키로 새로운 데이터를 추가
   - EEPROM과 DB에 센서 세팅값을 저장하여 운용 
   [ DB에 데이터 저장 (DB) / USB에 데이터 저장 (EEPROM)]

*/

#include <RTClib.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>
#include "src/granlib.h"

#include <Ch376msc.h>
#include <SoftwareSerial.h>

#define MAX_DATE    128
#define HEATER  15    //
#define LEVEL_LED  18 //sshong

#define DBSWITCH   23    // 구 LEVEL
#define TACTBTN    34    //TactButton for setting EEPROM (devMode)

#define P_PUMP    0
#define P_FAN     1
#define P_LED     2
#define P_HEATER  3
#define P_LEVEL   4


//granlib 클래스 선언
granlib _granlib;


//********************** comm 관련 변수&함수 **************************
char serialBuf[21]; // serial로 들어오는 데이타 저장을 위한 임시버퍼.
char *granlibBuf;   // granlib의 EEPROM 데이타 저장을 위한 임시버퍼.
int devMode = 0;    // EEPROM data 설정 모드
int devModeMSG = 0; // EEPROM data 설정 모드 안내문구 트리거
void readSerial();

//********************* Sensor 관련 변수&함수 *************************
float temp_value, temp_value_org;
float salt_value, salt_value_org;

void getSensorFromDB() {
  String s = _granlib._EEPROM.getSerialNumber();
  _granlib._DB.getSensorSetting(s);
  _granlib._DB.parsingSensorSettingJson();

  //DB에 등록되지 않은 Serial일 경우 새로 추가해준다.
  if (_granlib._DB.getSensorSerial() != s) {
    _granlib._DB.postSensorSetting(0, s, "Edit Data", 10);
    getSensorFromDB();
  }

  //EEPROM의 데이터를 업데이트
  char delaytimeBuf[4];
  int delaytimeint = _granlib._DB.getSensorDelaytime();
  delaytimeBuf[0] = (delaytimeint / 100) + 48;
  delaytimeint -= (delaytimeint / 100) * 100;
  delaytimeBuf[1] = (delaytimeint / 10) + 48;
  delaytimeint -= (delaytimeint / 10) * 10;
  delaytimeBuf[2] = delaytimeint + 48;
  delaytimeBuf[3] = '\0';

  char tablenameBuf[16];
  _granlib._DB.getSensorTablename().toCharArray(tablenameBuf, 16);

  _granlib._EEPROM.setDBTable(&tablenameBuf[0]);
  _granlib._EEPROM.setDelayTime(&delaytimeBuf[0]);
  _granlib._EEPROM.EEPROM_write_All();

}

//USB 저장 모드일 때 EEPROM에서 센서 세팅값 가져오기
void getSensorFromEEPROM() {
  
}

//********************** Timer 관련 변수&함수 *************************
unsigned long dbStartTime;  //DB 작업 시작시간
unsigned long dbEndTime;    //DB 작업 끝시간

//********************** WiFi 관련 변수&함수 **************************
#define ENABLE_SUJO_SELECT 0  //[ 1: sujo01 DB테이블 불러오기 사용 / 0: 미사용 ] 
#define ENABLE_SENSOR_SETTING_SELECT 1 // [ 1: 센서 세팅값 DB에 저장 사용 / 0: 미사용 ]
WiFiServer server(80);

//*********************** USB 관련 변수&함수 ***************************
#define ENABLE_USB 0        //[ 1: USB 파일저장 사용 / 0: 미사용 ] 
Ch376msc flashDrive(Serial2, 9600);
int createLogFlag = 0;
char adatBuffer[255];// max length 255 = 254 char + 1 NULL character

//*********************** RTC 관련 변수&함수 ***************************
RTC_DS1307 rtc;

//****************** softwareSerial 관련 변수&함수 **********************
#define MYPORT_TX 25
#define MYPORT_RX 26
#define DE 12
SoftwareSerial myPort;
byte softSerialBuf[13] = {0,};
int softSerialBufNum = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button
  pinMode(P_PUMP, OUTPUT);             //Send Sensor Data Button LED
  digitalWrite(P_PUMP, LOW);  // LED Off
  delay(1000);

  //EEPROM SETTING
  _granlib._EEPROM.EEPROM_begin();
  _granlib._EEPROM.EEPROM_read_All();

  if (ENABLE_USB == 1) {
    flashDrive.init();
  }

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }



  //softwareSerial
  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);
  myPort.begin(19200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false, 256);
  if (!myPort) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  }


  // WiFi network에 접속
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_granlib._EEPROM.getWifiSSID());
  Serial.println(_granlib._EEPROM.getWifiPWD());

  WiFi.begin(_granlib._EEPROM.getWifiSSID(), _granlib._EEPROM.getWifiPWD());
  Serial.println(WiFi.macAddress());

  //WiFi 연결 확인 (10초동안 응답이 없으면 EEPROM 설정으로 넘어감
  int wifiCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WIFI RSSI : ");
    Serial.println(WiFi.RSSI());
    delay(10);
    wifiCounter++;
    if (wifiCounter > 6000) { // wait 60s for wifi connect
      devMode = 1; //set develop mode flag
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    getSensorFromDB();
  }

  // devMode 확인
  if (devMode) {
    Serial.println("---------------------------------------------------------");
    Serial.println("Please Set EEPROM Data and Restart Arduino Board");
    Serial.println("---------------------------------------------------------");
  }
  else { // 정상작동
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
  }

  //타이머 초기화
  temp_value = 0.0;
  salt_value = 0.0;
  dbStartTime = millis();
  dbEndTime = millis();

}

void loop() {

  if (devMode) {  //setting data use Serial Mointor
    developmentMode();
  }
  else if (myPort.available() != 0) {   //reading Serial Message
    softSerialBuf[softSerialBufNum++] = myPort.read();

    if (softSerialBufNum > 12) {
      softSerialBufNum = 0;
      //      Serial.println(" ");
    }
  }
  else {                                //readIO()

    readIO();

    devModeMSG = 0; //Serial 설정 안내문구
  }

  //Send Sensor Data Button LED Check
  if (!(digitalRead(DBSWITCH))) {
    digitalWrite(P_PUMP, HIGH); // LED On
    delay(5);
  } else {
    digitalWrite(P_PUMP, LOW);  // LED Off
  }

  //Develop Mode Start Check
  if (!(digitalRead(TACTBTN))) {
    devMode = 1;
    //Serial.println("Develop Mode Start...");
  }

  // 작업 주기 시간 초기화
  if ((dbEndTime - dbStartTime) < _granlib._DB.getSensorDelaytime() * 1000 ) {
    dbEndTime = millis();
  } else {
    dbStartTime = millis();
    dbEndTime = millis();
  }
}
