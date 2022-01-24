#include <namedMesh.h>
#include <Arduino_JSON.h>
#include <math.h>

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

//********************* Sensor *************************
void developmentMode();
float temp_value, temp_value_org;
float salt_value, salt_value_org;


//********************** Timer *************************
unsigned long dbStartTime;        //DB 작업 시작시간
unsigned long dbEndTime;          //DB 작업 끝시간
unsigned long sendMeshStartTime;  //Send Mesh 지연 시작시간
unsigned long sendMeshEndTime;    //Send Mesh 지연 끝시간
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

  switch (boardType) {
    case 5: // AICT
      pinMode(TEMP_SENSOR, INPUT_PULLUP);      //  Temp Sensor
      pinMode(P_PUMP, OUTPUT);             //Send Sensor Data Button LED
      digitalWrite(P_PUMP, LOW);  // LED Off
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

  //센서값 초기화
  value = (String)GetTemperature(analogRead(TEMP_SENSOR));

  // devMode 확인
  if (devMode) {
    Serial.println("---------------------------------------------------------");
    Serial.println("Please Set EEPROM Data and Restart Arduino Board");
    Serial.println("---------------------------------------------------------");
  }
  else { // 정상작동

  }

  //타이머 초기화
  temp_value = 0.0;
  salt_value = 0.0;
  dbStartTime = millis();
  dbEndTime = millis();
  sendMeshStartTime = millis();
  sendMeshEndTime = millis();
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

  //Send Sensor Data Button LED Check
  if (!(digitalRead(DBSWITCH))) {
    digitalWrite(P_PUMP, HIGH); // LED On
  } else {
    digitalWrite(P_PUMP, LOW);  // LED Off
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

  //메쉬 샌드 주기 시간
  sendMeshEndTime = millis();

  //노드 연결 불안정일 때 스스로 재부팅
  if (rebootFlag) {
    rebootEndTime = millis();
    if ((rebootEndTime - rebootStartTime) >= RebootTime * 1000) {
      Serial.println("");
      Serial.println("reboot noard");
      Serial.println("");
      ESP.restart();
    }
  }
}
