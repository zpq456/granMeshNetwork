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

granEEPROM_esp32 _EEPROM;
granConvert _CONVERT;

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
unsigned long rebootStartTime;  //Send Mesh 지연 시작시간
unsigned long rebootEndTime;    //Send Mesh 지연 끝시간
int dbDelayTime;

//****************** mesh network *************************

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

String myNodeName;
String node_name;
String value;
String Msg;
void meshSendMessage(String tonode, String Msg);
#define RebootTime 600//(s)
bool rebootFlag = false; // 리부팅 트리거


void pinModeSetup(int boardType) {
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button
  pinMode(LED_BOARD_LEVEL, OUTPUT);

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

      break;
  }
}

void setup()
{
  Serial.begin(115200);
  pinModeSetup(BOARD_TYPE);
  delay(1000);

  //EEPROM SETTING
  _EEPROM.EEPROM_begin();
  _EEPROM.getEEPROM(BOARD_TYPE);
  dbDelayTime = ((String)_EEPROM.getDelayTime()).toInt() * 1000;

  //DO8 output relay value set
  _EEPROM.printStructDO8();
  digitalWrite(DO_1, _EEPROM.getDO8relayValue(0));
  digitalWrite(DO_2, _EEPROM.getDO8relayValue(1));
  digitalWrite(DO_3, _EEPROM.getDO8relayValue(2));
  digitalWrite(DO_4, _EEPROM.getDO8relayValue(3));
  digitalWrite(DO_5, _EEPROM.getDO8relayValue(4));
  digitalWrite(DO_6, _EEPROM.getDO8relayValue(5));
  digitalWrite(DO_7, _EEPROM.getDO8relayValue(6));
  digitalWrite(DO_8, _EEPROM.getDO8relayValue(7));

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
  rebootStartTime = millis();
  rebootEndTime = millis();

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

  if (!digitalRead(DBSWITCH)) {
    digitalWrite(LED_BOARD_LEVEL, true);
  } else {
    digitalWrite(LED_BOARD_LEVEL, false);
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

  //노드 연결 불안정일 때 스스로 재부팅
  if (rebootFlag) {
    rebootEndTime = millis();
    if ((rebootEndTime - rebootStartTime) >= RebootTime * 1000) {
      //재부팅 전 마지막 릴레이 값을 저장
      _EEPROM.setDO8relayValue(0, digitalRead(DO_1));
      _EEPROM.setDO8relayValue(1, digitalRead(DO_2));
      _EEPROM.setDO8relayValue(2, digitalRead(DO_3));
      _EEPROM.setDO8relayValue(3, digitalRead(DO_4));
      _EEPROM.setDO8relayValue(4, digitalRead(DO_5));
      _EEPROM.setDO8relayValue(5, digitalRead(DO_6));
      _EEPROM.setDO8relayValue(6, digitalRead(DO_7));
      _EEPROM.setDO8relayValue(7, digitalRead(DO_8));
      _EEPROM.putEEPROM(BOARD_TYPE);

      Serial.println("");
      Serial.println("reboot noard");
      Serial.println("");
      ESP.restart();
    }
  }

}
