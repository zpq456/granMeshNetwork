

/* granMeshNetwork
    - granWifi를 베이스로 ESP32 Mesh Network (painlessMesh)를 사용
    - 각 센서부의 데이터를 수집 (Mesh) 하고 외부 DB에 저장 (Wifi) 하기위한 개발

    211216 granWifi에 painlessMesh의 namedMesh를 적용

*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>
#include <Ch376msc.h>
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

struct dataTable {
  float tempSensor[3];
  int sensorWarning[8];
} DataT;

dataTable DataTtemp;

void initDataTable() {
  DataT.tempSensor[0] = 0;
  DataT.tempSensor[1] = 0;
  DataT.tempSensor[2] = 0;
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

  if (checkChange) {
    meshSendMessage("No004", DO8JsonMsg(&DataT.sensorWarning[0]));
  }
}

void printDataTable() {
  Serial.println("");
  Serial.println(DataT.tempSensor[0]);
  Serial.println(DataT.tempSensor[1]);
  Serial.println(DataT.tempSensor[2]);
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

//********************** WiFi **************************
#define ENABLE_SUJO_SELECT 0  //[ 1: sujo01 DB테이블 불러오기 사용 / 0: 미사용 ] 
#define ENABLE_SENSOR_SETTING_SELECT 1 // [ 1: 센서 세팅값 DB에 저장 사용 / 0: 미사용 ]
WiFiServer server(80);


void pinModeSetup(int boardType) {
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button

  switch (boardType) {
    case 0: // MasterDI4DO4
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
  _granlib._EEPROM.EEPROM_begin();
  _granlib._EEPROM.getEEPROM();
  dbDelayTime = ((String)_granlib._EEPROM.getDelayTime()).toInt() * 1000;

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
    if (wifiCounter > 1) { // wait 60s for wifi connect
      //devMode = 1; //set develop mode flag
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    //getSensorFromDB();
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
}
