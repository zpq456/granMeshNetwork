//****************************** user custom code *******************************


String obtain_readings_Output () {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = BOARD_TYPE;
  jsonReadings["node_name"] = _GNet.getmyNodeName();

  String outputValueArray = "[" + (String)digitalRead(DO_1) +
                            "," + (String)digitalRead(DO_2) +
                            "," + (String)digitalRead(DO_3) +
                            "," + (String)digitalRead(DO_4) +
                            "," + (String)digitalRead(DO_5) +
                            "," + (String)digitalRead(DO_6) +
                            "," + (String)digitalRead(DO_7) +
                            "," + (String)digitalRead(DO_8) +
                            "]";

  jsonReadings["Data"] = outputValueArray;
  return JSON.stringify(jsonReadings);
}

//마스터의 노드 확인 답장
String obtain_readings_nodeLiveCheck_ackmsg () {
  int board_type = 2; // To Master Board
  String Msg = "OUTPUT " + _GNet.getmyNodeName() + " is activate!!";

  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = board_type;
  jsonReadings["node_name"] = _GNet.getmyNodeName();
  jsonReadings["Data"] = Msg;
  return JSON.stringify(jsonReadings);
}

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
  String tonode;

  JSONVar data_object;
  int data_type;
  int outputvalue = 0;

  switch (board_type) {
    case 0:
      
      break;
    case 2:
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];

      switch (data_type) {
        case 0: // set output value
          outputvalue = data_object["DO_1"];
          if(outputvalue == 0){
            digitalWrite(DO_1, LOW);
          }else{
            digitalWrite(DO_1, HIGH);
          }
          outputvalue = data_object["DO_2"];
          if(outputvalue == 0){
            digitalWrite(DO_2, LOW);
          }else{
            digitalWrite(DO_2, HIGH);
          }
          outputvalue = data_object["DO_3"];
          if(outputvalue == 0){
            digitalWrite(DO_3, LOW);
          }else{
            digitalWrite(DO_3, HIGH);
          }
          outputvalue = data_object["DO_4"];
          if(outputvalue == 0){
            digitalWrite(DO_4, LOW);
          }else{
            digitalWrite(DO_4, HIGH);
          }
          outputvalue = data_object["DO_5"];
          if(outputvalue == 0){
            digitalWrite(DO_5, LOW);
          }else{
            digitalWrite(DO_5, HIGH);
          }
          outputvalue = data_object["DO_6"];
          if(outputvalue == 0){
            digitalWrite(DO_6, LOW);
          }else{
            digitalWrite(DO_6, HIGH);
          }
          outputvalue = data_object["DO_7"];
          if(outputvalue == 0){
            digitalWrite(DO_7, LOW);
          }else{
            digitalWrite(DO_7, HIGH);
          }
          outputvalue = data_object["DO_8"];
          if(outputvalue == 0){
            digitalWrite(DO_8, LOW);
          }else{
            digitalWrite(DO_8, HIGH);
          }
          break;
      }
      
      //ack msg to Master
      tonode = _GNet.gettoNodeMain();
      Msg = obtain_readings_nodeLiveCheck_ackmsg();
      mesh.sendSingle(tonode, Msg);

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
