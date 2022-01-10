//****************************** user custom code *******************************

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

  //set data json
  JSONVar jsonDatas;
  jsonDatas["data_type"] = DATA_TYPE; //1
  jsonDatas["Msg"] = Msg;

  jsonReadings["Data"] = JSON.stringify(jsonDatas);
  return JSON.stringify(jsonReadings);
}

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
      meshSendMessage(_GNet.gettoNodeMain(),obtain_readings_nodeLiveCheck_ackmsg());
      break;
  }
}



//*****************************************************************************

void initMesh(){
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
}


void meshSendMessage(String tonode, String Msg) {
  mesh.sendSingle(tonode, Msg);
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
