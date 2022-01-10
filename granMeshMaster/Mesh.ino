//****************************** user custom code *******************************

String ackJsonMsg () {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = BOARD_TYPE;
  jsonReadings["node_name"] = _GNet.getmyNodeName();
  return JSON.stringify(jsonReadings);
}

String DO8JsonMsg (int* OutputValue) {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = 2; // DO8
  jsonReadings["node_name"] = _GNet.getmyNodeName();

  //set data json
  JSONVar jsonDatas;
  jsonDatas["data_type"] = 0;
  jsonDatas["DO_1"] = OutputValue[0];
  jsonDatas["DO_2"] = OutputValue[1];
  jsonDatas["DO_3"] = OutputValue[2];
  jsonDatas["DO_4"] = OutputValue[3];
  jsonDatas["DO_5"] = OutputValue[4];
  jsonDatas["DO_6"] = OutputValue[5];
  jsonDatas["DO_7"] = OutputValue[6];
  jsonDatas["DO_8"] = OutputValue[7];

  jsonReadings["Data"] = JSON.stringify(jsonDatas);
  return JSON.stringify(jsonReadings);
}

//mesh callback
void receivedCallback( uint32_t from, String &msg ) {

  Serial.println("");
  Serial.println(msg.c_str());
  Serial.println("");

  JSONVar json_object = JSON.parse(msg.c_str());
  int board_type = json_object["board_type"];
  const char* strbuf = json_object["node_name"];
  String node_name = strbuf;
  const char* strbuf2 = json_object["Data"];
  String dataString = strbuf2;

  JSONVar data_object;
  int data_type;

  switch (board_type) {
    case 0: // sensor nodeLiveCheck ack msg
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];

      switch (data_type) {
        case 1: // temp sensor xx.xx'C
          dataString = data_object["Msg"];
          if (node_name.compareTo("NS001") == 0) {
            DataT.tempSensor[0] = DataTtemp.tempSensor[0];
          } else if (node_name.compareTo("NS002") == 0) {
            DataT.tempSensor[1] = DataTtemp.tempSensor[1];
          } else if (node_name.compareTo("NS003") == 0) {
            DataT.tempSensor[2] = DataTtemp.tempSensor[2];
          }
          checkWarning();
          break;
      }

      Serial.println("");
      Serial.println(dataString);
      Serial.println("");

      break;
    case 2: // DO8 Node ackMsg
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];


      break;
    case 5: // Sensor Node AI msg
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];

      switch (data_type) {
        case 1: // temp sensor xx.xx'C
          value = data_object["value"];
          if (node_name.compareTo("NS001") == 0) {
            DataTtemp.tempSensor[0] = value.toFloat();
          } else if (node_name.compareTo("NS002") == 0) {
            DataTtemp.tempSensor[1] = value.toFloat();
          } else if (node_name.compareTo("NS003") == 0) {
            DataTtemp.tempSensor[2] = value.toFloat();
          }
          meshSendMessage(node_name, ackJsonMsg());
          break;
      }
      break;
  }

}

//*****************************************************************************

void initMesh(){
  //*********************** mesh network ***************************
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  //mesh.setDebugMsgTypes(ERROR | DEBUG );  // set before init() so that you can see startup messages

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
