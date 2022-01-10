

/* granMeshNetwork
    - granWifi를 베이스로 ESP32 Mesh Network (painlessMesh)를 사용
    - 각 센서부의 데이터를 수집 (Mesh) 하고 외부 DB에 저장 (Wifi) 하기위한 개발

    211216 granWifi에 painlessMesh의 namedMesh를 적용

*/

#include <namedMesh.h>
#include <Arduino_JSON.h>

#define MAX_DATE    128
#define HEATER  15    //
#define LEVEL_LED  18 //sshong

#define DBSWITCH   23    // 구 LEVEL
#define TACTBTN    34    //TactButton for setting EEPROM (devMode)
#define TEMP_SENSOR   36

#define P_PUMP    0
#define P_FAN     1
#define P_LED     2
#define P_HEATER  3
#define P_LEVEL   4

//granlib 클래스 선언
#include <granlib.h>
granlib _granlib;

//board include
#include <GNet.h>
GNet _GNet;


//********************** comm **************************
char serialBuf[21]; // serial로 들어오는 데이타 저장을 위한 임시버퍼.
char *granlibBuf;   // granlib의 EEPROM 데이타 저장을 위한 임시버퍼.
int devMode = 0;    // EEPROM data 설정 모드
int devModeMSG = 0; // EEPROM data 설정 모드 안내문구 트리거
void readSerial();

//********************* develop mode Use EEPROM *************************
void developmentMode();

//********************** Timer *************************
unsigned long dbStartTime;  //DB 작업 시작시간
unsigned long dbEndTime;    //DB 작업 끝시간
int dbDelayTime;

//****************** mesh network *************************

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

String myNodeName;
String node_name;
String value;
String Msg;
void meshSendMessage(String tonode, String Msg);


void pinModeSetup(int boardType) {
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button

  switch (boardType) {
    case 2: // DO8
      pinMode(DO_1, OUTPUT);
      pinMode(DO_2, OUTPUT);
      pinMode(DO_3, OUTPUT);
      pinMode(DO_4, OUTPUT);
      pinMode(DO_5, OUTPUT);
      pinMode(DO_6, OUTPUT);
      pinMode(DO_7, OUTPUT);
      pinMode(DO_8, OUTPUT);

      digitalWrite(DO_1, LOW);
      digitalWrite(DO_2, LOW);
      digitalWrite(DO_3, LOW);
      digitalWrite(DO_4, LOW);
      digitalWrite(DO_5, LOW);
      digitalWrite(DO_6, LOW);
      digitalWrite(DO_7, LOW);
      digitalWrite(DO_8, LOW);
      break;
  }
}

void setup()
{
  Serial.begin(115200);
  pinModeSetup(BOARD_TYPE);
  delay(1000);

  //EEPROM SETTING
  _granlib._EEPROM.EEPROM_begin();
  _granlib._EEPROM.getEEPROM();
  dbDelayTime = ((String)_granlib._EEPROM.getDelayTime()).toInt() * 1000;

  // devMode 확인
  if (devMode) {
    Serial.println("---------------------------------------------------------");
    Serial.println("Please Set EEPROM Data and Restart Arduino Board");
    Serial.println("---------------------------------------------------------");
  }
  else { // 정상작동

  }

  //타이머 초기화
  dbStartTime = millis();
  dbEndTime = millis();

  //*********************** mesh network ***************************88
  initMesh();

}

void loop() {

  if (devMode) {  //setting data use Serial Mointor
    developmentMode();
  }
  else {
    readIO();
    mesh.update();
    devModeMSG = 0; //Serial 설정 안내문구
  }

  //Develop Mode Start Check
  if (!(digitalRead(TACTBTN))) {
    devMode = 1;
    Serial.println("Develop Mode Start...");
  }

  // 작업 주기 시간 초기화
  if ((dbEndTime - dbStartTime) < dbDelayTime ) {
    dbEndTime = millis();
  } else {
    dbStartTime = millis();
    dbEndTime = millis();
  }

}
