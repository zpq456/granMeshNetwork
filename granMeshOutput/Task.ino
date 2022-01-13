
void readIO()
{
  //정해진 시간이 되면 현재값을 마스터에 송신 (생존신고)
  if ((dbEndTime - dbStartTime) >= dbDelayTime &&
      !digitalRead(DBSWITCH)) {

    //아웃풋 설정값을 마스터에 송신
    meshSendMessage(_GNet.gettoNodeMain(), obtain_readings_nodeLiveCheck_ackmsg());
    rebootFlag = true;
  }

}

void developmentMode() {
  if (devModeMSG == 0) {
    Serial.println("---------------------------------------------------------");
    Serial.println("[EEPROM Develop Mode]");
    Serial.println("please type [?;] to Serial Mointor.");
    Serial.println("---------------------------------------------------------");
    devModeMSG = 1;
  }

  //waiting Serial Message
  if (Serial.available() != 0) {
    readSerial();
  }
}
