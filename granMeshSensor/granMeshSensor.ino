

/* granMeshNetwork
    - granWifi를 베이스로 ESP32 Mesh Network (painlessMesh)를 사용
    - 각 센서부의 데이터를 수집 (Mesh) 하고 외부 DB에 저장 (Wifi) 하기위한 개발

    211216 granWifi에 painlessMesh의 namedMesh를 적용

*/

#include <RTClib.h>
#include <SoftwareSerial.h>
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

//********************* Sensor *************************
void developmentMode();
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

//********************** Timer *************************
unsigned long dbStartTime;  //DB 작업 시작시간
unsigned long dbEndTime;    //DB 작업 끝시간


//*********************** RTC ***************************
RTC_DS1307 rtc;

//****************** softwareSerial **********************
#define MYPORT_TX 25
#define MYPORT_RX 26
#define DE 12
SoftwareSerial myPort;
byte softSerialBuf[13] = {0,};
int softSerialBufNum = 0;

//****************** mesh network *************************

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

String myNodeName;
String node_name;
String value;
String Msg;

String obtain_readings_sendTempSensor () {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = BOARD_TYPE;
  jsonReadings["node_name"] = _GNet.getmyNodeName();

  //set data json
  JSONVar jsonDatas;
  jsonDatas["data_type"] = DATA_TYPE; //1
  jsonDatas["value"] = value;

  jsonReadings["Data"] = JSON.stringify(jsonDatas);
  return JSON.stringify(jsonReadings);
}

//마스터의 노드 확인 답장
String obtain_readings_nodeLiveCheck_ackmsg () {
  int board_type = 0; // To Master Board
  String Msg = "AI Sensor " + _GNet.getmyNodeName() + " is alive!!";

  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = board_type;
  jsonReadings["node_name"] = _GNet.getmyNodeName();
  jsonReadings["Data"] = Msg;
  return JSON.stringify(jsonReadings);
}

//1000sec 마다 센서값 전송
Task taskSendMessage( TASK_SECOND * 10, TASK_FOREVER, []() {
  String tonode = _GNet.gettoNodeMain();
  Msg = obtain_readings_sendTempSensor();
  mesh.sendSingle(tonode, Msg);
}); // start with a one second interval

//// sensor nodeLiveCheck ack msg
//Task taskAckMessage( TASK_SECOND * 10, TASK_FOREVER, []() {
//  String tonode = _GNet.gettoNodeMain();
//  Msg = obtain_readings_nodeLiveCheck_ackmsg();
//  mesh.sendSingle(tonode, Msg);
//}); // start with a one second interval

//mesh callback
void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  Serial.println("");
  Serial.println(msg.c_str());
  Serial.println("");

  JSONVar json_object = JSON.parse(msg.c_str());
  int mode_type = json_object["mode_type"];
  const char* strbuf = json_object["node_name"];
  String node_name = strbuf;

  switch (mode_type) {
    // sensor nodeLiveCheck ack msg
    case 0:
      String tonode = _GNet.gettoNodeMain();
      Msg = obtain_readings_nodeLiveCheck_ackmsg();
      mesh.sendSingle(tonode, Msg);
      //      userScheduler.addTask(taskAckMessage);
      //      taskAckMessage.enable();
      break;
  }
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}


void setup()
{
  Serial.begin(115200);
  pinMode(TACTBTN, INPUT_PULLUP);      //  BOOT MODE SELECT BUTTON
  pinMode(DBSWITCH, INPUT_PULLUP);     //Send Sensor Data Button
  pinMode(TEMP_SENSOR, INPUT_PULLUP);      //  Temp Sensor
  pinMode(P_PUMP, OUTPUT);             //Send Sensor Data Button LED
  digitalWrite(P_PUMP, LOW);  // LED Off
  delay(1000);

  //EEPROM SETTING
  _granlib._EEPROM.EEPROM_begin();
  _granlib._EEPROM.EEPROM_read_All();


  //******************************* RTC ******************************
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

  //****************************softwareSerial****************************
  pinMode(DE, OUTPUT);
  digitalWrite(DE, LOW);
  myPort.begin(19200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false, 256);
  if (!myPort) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  }

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

  //*********************** mesh network ***************************88
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  //get Data from eeprom
  _GNet.setMESH_SSID(_granlib._EEPROM.getDBTable());
  _GNet.setmyNodeName(_granlib._EEPROM.getSerialNumber());

  myNodeName = _GNet.getmyNodeName();
  mesh.init(_GNet.getMESH_SSID(), MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.setName(myNodeName); // This needs to be an unique name!
  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable(); // send sensor data
  //  userScheduler.addTask(taskAckMessage);
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
  if ((dbEndTime - dbStartTime) < _granlib._DB.getSensorDelaytime() * 1000 ) {
    dbEndTime = millis();
  } else {
    dbStartTime = millis();
    dbEndTime = millis();
  }
}
