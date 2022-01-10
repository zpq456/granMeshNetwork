
void readIO()
{
  //시간이 되면 작업 진행
  if ((dbEndTime - dbStartTime) >= dbDelayTime &&
      !digitalRead(DBSWITCH)) {
        

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
