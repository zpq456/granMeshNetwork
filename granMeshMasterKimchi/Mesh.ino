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

String DO8SoloJsonMsg (String rellay, String OutputValue) {
  //set send json
  JSONVar jsonReadings;
  jsonReadings["board_type"] = 2; // DO8
  jsonReadings["node_name"] = _GNet.getmyNodeName();

  //set data json
  JSONVar jsonDatas;
  jsonDatas["data_type"] = 2;
  jsonDatas["DO_Num"] = rellay;
  jsonDatas["DO_Value"] = OutputValue;

  jsonReadings["Data"] = JSON.stringify(jsonDatas);
  return JSON.stringify(jsonReadings);
}

String DO8AckMsg () {
  JSONVar jsonReadings;
  jsonReadings["board_type"] = 2; // DO8
  jsonReadings["node_name"] = _GNet.getmyNodeName();

  //set data json
  JSONVar jsonDatas;
  jsonDatas["data_type"] = 1;

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
  bool dataDifferenceFalg = false;
  int outputvalue = 0;

  switch (board_type) {
    case 0: // sensor nodeLiveCheck ack msg
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];

      switch (data_type) {
        case 1: // temp sensor xx.xx'C
          dataString = data_object["Msg"];
          if (node_name.compareTo(NODE_DI_1) == 0) {
            DataT.tempSensor[0] = DataTtemp.tempSensor[0];
          } else if (node_name.compareTo(NODE_DI_2) == 0) {
            DataT.tempSensor[1] = DataTtemp.tempSensor[1];
          } else if (node_name.compareTo(NODE_DI_3) == 0) {
            DataT.tempSensor[2] = DataTtemp.tempSensor[2];
          } else if (node_name.compareTo(NODE_DI_4) == 0) {
            DataT.tempSensor[3] = DataTtemp.tempSensor[3];
          }
          checkWarning();
          break;
        case 2: // Output Node Ack Msg
          dataString = data_object["Msg"];

          outputvalue = data_object["DO_1"];
          if (outputvalue != DataT.sensorWarning[0]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_2"];
          if (outputvalue != DataT.sensorWarning[1]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_3"];
          if (outputvalue != DataT.sensorWarning[2]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_4"];
          if (outputvalue != DataT.sensorWarning[3]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_5"];
          if (outputvalue != DataT.sensorWarning[4]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_6"];
          if (outputvalue != DataT.sensorWarning[5]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_7"];
          if (outputvalue != DataT.sensorWarning[6]) {
            dataDifferenceFalg = true;
          }
          outputvalue = data_object["DO_8"];
          if (outputvalue != DataT.sensorWarning[7]) {
            dataDifferenceFalg = true;
          }

          if (dataDifferenceFalg && devMode == 0) {
            meshSendMessage(NODE_DO_1, DO8JsonMsg(&DataT.sensorWarning[0]));
          }
          else {
            meshSendMessage(NODE_DO_1, DO8AckMsg());
          }
          break;
      }

      Serial.println("");
      Serial.println(dataString);
      Serial.println("");

      break;
    case 2: // DO8 Node ackMsg
      meshSendMessage(NODE_DO_1, DO8AckMsg());

      break;
    case 6: // Sensor Node AI msg
      data_object = JSON.parse(dataString.c_str());
      data_type = data_object["data_type"];

      switch (data_type) {
        case 1: // temp sensor xx.xx'C
          value = data_object["value"];
          if (node_name.compareTo(NODE_DI_1) == 0) {
            DataTtemp.tempSensor[0] = value.toFloat();
          } else if (node_name.compareTo(NODE_DI_2) == 0) {
            DataTtemp.tempSensor[1] = value.toFloat();
          } else if (node_name.compareTo(NODE_DI_3) == 0) {
            DataTtemp.tempSensor[2] = value.toFloat();
          } else if (node_name.compareTo(NODE_DI_4) == 0) {
            DataTtemp.tempSensor[3] = value.toFloat();
          }

          meshSendMessage(node_name, ackJsonMsg());
          break;
      }
      break;
  }

}

//*****************************************************************************

void initMesh() {
  //*********************** mesh network ***************************
//  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR | DEBUG );  // set before init() so that you can see startup messages

  //get Data from eeprom
  _GNet.setMESH_SSID(_EEPROM.getDBTable());
  _GNet.setmyNodeName(_EEPROM.getSerialNumber());

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
