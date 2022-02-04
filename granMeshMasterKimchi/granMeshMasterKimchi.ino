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
#define WARNING1 30
#define WARNING2 30
#define WARNING3 30
#define WARNING4 30

struct dataTable {
  float tempSensor[4];
  int sensorWarning[8];
} DataT;

dataTable DataTtemp;

void initDataTable() {
  DataT.tempSensor[0] = 0;
  DataT.tempSensor[1] = 0;
  DataT.tempSensor[2] = 0;
  DataT.tempSensor[3] = 0;
  DataT.sensorWarning[0] = 0;
  DataT.sensorWarning[1] = 0;
  DataT.sensorWarning[2] = 0;
  DataT.sensorWarning[3] = 0;
  DataT.sensorWarning[4] = 0;
  DataT.sensorWarning[5] = 0;
  DataT.sensorWarning[6] = 0;
  DataT.sensorWarning[7] = 0;
}

//온도값에 따라서 DO8 보드의 릴레이를 작동시킨다.
void checkWarning() {
  bool checkChange = false;
  if (DataT.tempSensor[0] > WARNING1 && DataT.sensorWarning[0] == 0) {
    DataT.sensorWarning[0] = 1;
    checkChange = true;
  } else if (DataT.tempSensor[0] <= WARNING1 && DataT.sensorWarning[0] == 1) {
    DataT.sensorWarning[0] = 0;
    checkChange = true;
  }
  if (DataT.tempSensor[1] > WARNING2 && DataT.sensorWarning[1] == 0) {
    DataT.sensorWarning[1] = 1;
    checkChange = true;
  } else if (DataT.tempSensor[1] <= WARNING2 && DataT.sensorWarning[1] == 1) {
    DataT.sensorWarning[1] = 0;
    checkChange = true;
  }
  if (DataT.tempSensor[2] > WARNING3 && DataT.sensorWarning[2] == 0) {
    DataT.sensorWarning[2] = 1;
    checkChange = true;
  } else if (DataT.tempSensor[2] <= WARNING3 && DataT.sensorWarning[2] == 1) {
    DataT.sensorWarning[2] = 0;
    checkChange = true;
  }
  if (DataT.tempSensor[3] > WARNING4 && DataT.sensorWarning[3] == 0) {
    DataT.sensorWarning[3] = 1;
    checkChange = true;
  } else if (DataT.tempSensor[3] <= WARNING4 && DataT.sensorWarning[3] == 1) {
    DataT.sensorWarning[3] = 0;
    checkChange = true;
  }

  if (checkChange) {
    meshSendMessage(NODE_DO_1, DO8JsonMsg(&DataT.sensorWarning[0]));
  }
}

void printDataTable() {
  Serial.println("");
  Serial.println(DataT.tempSensor[0]);
  Serial.println(DataT.tempSensor[1]);
  Serial.println(DataT.tempSensor[2]);
  Serial.println(DataT.tempSensor[3]);
  Serial.println("");
}

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
    case 7: // MasterKimchi
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

  initDataTable();

  initMesh();

}

void loop() {

  if (devMode) {  //setting data use Serial Mointor
    developmentMode();
  }
  else {
    readIO();
    devModeMSG = 0; //Serial 설정 안내문구
  }

  mesh.update();

  //Send Sensor Data Button LED Check
  if (!(digitalRead(DBSWITCH))) {
    digitalWrite(LED_BOARD_LEVEL, true);
  } else {
    //    digitalWrite(P_PUMP, LOW);  // LED Off
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
