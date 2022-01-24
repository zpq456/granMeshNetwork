#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Wire.h>

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
granDB _DB;
granConvert _CONVERT;

//board include
#include <GNet.h>
GNet _GNet;

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

//********************** WiFi **************************
#define ENABLE_SUJO_SELECT 0  //[ 1: sujo01 DB테이블 불러오기 사용 / 0: 미사용 ] 
#define ENABLE_SENSOR_SETTING_SELECT 1 // [ 1: 센서 세팅값 DB에 저장 사용 / 0: 미사용 ]
WiFiServer server(80);


void pinModeSetup(int boardType) {
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button

  switch (boardType) {
    case 6: // MasterWifi
      pinMode(P_PUMP, OUTPUT);             //Send Sensor Data Button LED
      digitalWrite(P_PUMP, LOW);  // LED Off
      break;
  }
}


//********************* HardwareSerial ***************************
HardwareSerial Unit2(1);

void setup()
{
  Serial.begin(115200);
  pinModeSetup(BOARD_TYPE);

  Unit2.begin(115200, SERIAL_8N1, 17, 16);

  delay(1000);

  //EEPROM SETTING
  _EEPROM.EEPROM_begin();
  _EEPROM.getEEPROM(BOARD_TYPE);
  dbDelayTime = ((String)_EEPROM.getDelayTime()).toInt() * 1000;

  // WiFi network에 접속
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_EEPROM.getWifiSSID());
  Serial.println(_EEPROM.getWifiPWD());

  WiFi.begin(_EEPROM.getWifiSSID(), _EEPROM.getWifiPWD());
  Serial.println(WiFi.macAddress());

  //WiFi 연결 확인 (10초동안 응답이 없으면 EEPROM 설정으로 넘어감
  int wifiCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WIFI RSSI : ");
    Serial.println(WiFi.RSSI());
    wifiCounter++;
    if (wifiCounter > 10000) { // wait 60s for wifi connect
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


}

void loop() {

  if (devMode) {  //setting data use Serial Mointor
    developmentMode();
  }
  else {
    readIO();
    devModeMSG = 0; //Serial 설정 안내문구
  }

  if (Unit2.available()) {
    if (Unit2.read() == 0x02) {
      Unit2.readBytes((uint8_t *) &DataT, sizeof DataT );

      //DB에 데이터를 송신
      printDataTable();
      _DB.insertDBData_MasterDI4DO4(
        _EEPROM.getSerialNumber(),
        _EEPROM.getDBTable(),
        DataT.tempSensor[0],
        DataT.tempSensor[1],
        DataT.tempSensor[2],
        DataT.tempSensor[3]
      );
    }
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
