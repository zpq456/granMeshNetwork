

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
#include <RTClib.h>
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
void developmentMode();


//********************** Timer *************************
unsigned long dbStartTime;  //DB 작업 시작시간
unsigned long dbEndTime;    //DB 작업 끝시간

//********************** WiFi **************************
#define ENABLE_SUJO_SELECT 0  //[ 1: sujo01 DB테이블 불러오기 사용 / 0: 미사용 ] 
#define ENABLE_SENSOR_SETTING_SELECT 1 // [ 1: 센서 세팅값 DB에 저장 사용 / 0: 미사용 ]
WiFiServer server(80);

//*********************** RTC ***************************
RTC_DS1307 rtc;

//****************** mesh network *************************

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

String myNodeName;
String node_name;
String value;
String Msg;

String obtain_readings_nodeLiveCheck () {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = BOARD_TYPE;
  jsonReadings["node_name"] = _GNet.getmyNodeName();
  return JSON.stringify(jsonReadings);
}

//1000sec 마다 노드 확인
Task taskSendMessage( TASK_SECOND * 50, TASK_FOREVER, []() {
  Msg = obtain_readings_nodeLiveCheck();
  mesh.sendBroadcast(Msg);
}); // start with a one second interval

//mesh callback
void receivedCallback( uint32_t from, String &msg ) {
  //  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  Serial.println("");
  Serial.println(msg.c_str());
  Serial.println("");

  JSONVar json_object = JSON.parse(msg.c_str());
  int board_type = json_object["board_type"];
  const char* strbuf = json_object["node_name"];
  String node_name = strbuf;
  const char* strbuf2 = json_object["Data"];
  String dataString = strbuf2;


  Serial.println("[board_type]");
  Serial.println(board_type);
  Serial.println("");

  switch (board_type) {
    // sensor nodeLiveCheck ack msg
    case 0:
      Serial.println("");
      Serial.println(dataString);
      Serial.println("");

      break;
    case 5: // Sensor Node AI msg
      JSONVar data_object = JSON.parse(dataString.c_str());
      int data_type = data_object["data_type"];
      Serial.println("[data_type]");
      Serial.println(data_type);
      Serial.println("");
      switch (data_type) {
        case 1: // temp sensor xx.xx'C
          value = data_object["value"];

          Serial.println("");
          Serial.print("Node: ");
          Serial.println(node_name);
          Serial.print("Temperature: ");
          Serial.println(value.toFloat());
          Serial.println("");
          break;
      }
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

  //*********************** mesh network ***************************88
  //  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR | DEBUG );  // set before init() so that you can see startup messages

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
  if ((dbEndTime - dbStartTime) < _granlib._DB.getSensorDelaytime() * 1000 ) {
    dbEndTime = millis();
  } else {
    dbStartTime = millis();
    dbEndTime = millis();
  }
}
