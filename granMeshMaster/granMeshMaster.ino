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

//****************** mesh network *************************

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

String myNodeName;
String node_name;
String value;
String Msg;
void meshSendMessage(String tonode, String Msg);


//********************** Data table ************************
struct dataTable {
  float tempSensor[4];
  int sensorWarning[4][8]; // 4 node * 8 rellay
} DataT;

dataTable DataTtemp;

//********************** comm **************************
char serialBuf[21]; // serial로 들어오는 데이타 저장을 위한 임시버퍼.
char *granlibBuf;   // granlib의 EEPROM 데이타 저장을 위한 임시버퍼.
int devMode = 0;    // EEPROM data 설정 모드
int devModeMSG = 0; // EEPROM data 설정 모드 안내문구 트리거
void readSerial();
void developmentMode();


//********************** Timer *************************
unsigned long dbStartTime;  //DB 작업 시작시간
unsigned long dbEndTime;    //DB 작업 끝시간
int dbDelayTime;


void pinModeSetup(int boardType) {
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button
  pinMode(LED_BOARD_LEVEL, OUTPUT);
  
  //esp32 pin 0 LOW
  pinMode(P_PUMP, OUTPUT);           
  digitalWrite(P_PUMP, LOW);

  switch (boardType) {
    case 0: // MasterDI4DO4
      pinMode(TEMP_SENSOR, INPUT_PULLUP);      //  Temp Sensor
      break;
  }
}

//********************* HardwareSerial ***************************
HardwareSerial Unit1(1);

void setup()
{
  Serial.begin(115200);
  pinModeSetup(BOARD_TYPE);

  Unit1.begin(115200, SERIAL_8N1, 17, 16);

  delay(1000);

  //EEPROM SETTING
  _EEPROM.EEPROM_begin();
  _EEPROM.getEEPROM(BOARD_TYPE);
  dbDelayTime = ((String)_EEPROM.getDelayTime()).toInt() * 1000;

  //타이머 초기화
  dbStartTime = millis();
  dbEndTime = millis();

  initMesh();

}

void loop() {

  if (devMode) {  //setting data use Serial Mointor
  }
  else {
  }

  developmentMode();
  readIO();
  //  devModeMSG = 0; //Serial 설정 안내문구
  mesh.update();

  //Send Sensor Data Button LED Check
  if (!(digitalRead(DBSWITCH))) {
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
}
