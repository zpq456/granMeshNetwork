int temp = 0;

void readSerial() {
  Serial.println("in Serial.");

  SerialGets(&serialBuf[0], 21);

  switch (serialBuf[0]) {
    case 'S': //SerialNumber 설정
      _granlib._EEPROM.setSerialNumber(&serialBuf[1]);
      Serial.print("SerialNumber(S)    : "); printCharArray(_granlib._EEPROM.getSerialNumber(), 6);
      break;
    case 'I': //SerialNumber 설정
      _granlib._EEPROM.setWifiSSID(&serialBuf[1]);
      Serial.print("Wifi SSID(I)       : "); printCharArray(_granlib._EEPROM.getWifiSSID(), 16);
      break;
    case 'P': //SerialNumber 설정
      _granlib._EEPROM.setWifiPWD(&serialBuf[1]);
      Serial.print("Wifi Passward(P)   : "); printCharArray(_granlib._EEPROM.getWifiPWD(), 16);
      break;
    case 'D': //SerialNumber 설정
      _granlib._EEPROM.setDBTable(&serialBuf[1]);
      Serial.print("Wifi DB Table Name(D) : "); printCharArray(_granlib._EEPROM.getDBTable(), 16);
      break;
    case 'T': //SerialNumber 설정
      _granlib._EEPROM.setDelayTime(&serialBuf[1]);
      Serial.print("Delay Time(T) [001~999 sec] : "); printCharArray(_granlib._EEPROM.getDelayTime(), 4);
      break;
    case 'N': //ToNodeName 설정
      _granlib._EEPROM.setToNodeName(&serialBuf[1]);
      Serial.print("To NodeName(N) : "); printCharArray(_granlib._EEPROM.getToNodeName(), 6);
      break;
    case '*': // Restore default value
      temp  = (serialBuf[1] - 48) * 100;
      temp += (serialBuf[2] - 48) * 10;
      temp += serialBuf[3] - 48;
      if (temp == 777) {
        _granlib._EEPROM.setDefaultValue();
        Serial.println("Factory default values are restored.");
        _granlib._EEPROM.printStruct();
      }
      break;
    case 'R': // Restore default value
      //yyyymmddhhmmss
      SetRTCTime(&serialBuf[1]);
      Serial.print("RTC Time : ");
      Serial.println(getRTCTime(rtc));
      break;
    case '!':   //EEPROM Save
      _granlib._EEPROM.EEPROM_write_All();
      Serial.println("=================================================");
      _granlib._EEPROM.printStruct();
      Serial.println("PARAMETER Saved.");
      break;
    case '@':   //EEPROM Read
      Serial.print("E="); Serial.println('E');
      _granlib._EEPROM.EEPROM_read_All();

      Serial.println("=================================================");
      Serial.println("PARAMETER Read.");
      _granlib._EEPROM.printStruct();
      Serial.println("@:Read parameter, !:Save parameter, ?:Help, ~:Exit Develop Modes");
      Serial.println("R:Set RTC Time [YYYYMMDDhhmmss]");
      break;
    case '?':
      Serial.println("=================================================");
      Serial.println("Help");
      _granlib._EEPROM.printStruct();
      Serial.println("Txx is 2 digit, Lxxx, Bxxx is 3 digit");
      Serial.println("*777:Restore factory default value.");
      Serial.println("@:Read parameter, !:Save parameter, ?:Help, ~:Exit Develop Modes");
      Serial.println("R:Set RTC Time [YYYYMMDDhhmmss]");
      break;
    case '~':
      devMode = 0;
      Serial.println("---------------------------------------------------------");
      Serial.println("Develop Mode End...");
      Serial.println("---------------------------------------------------------");
      ESP.restart();
      break;
    default :
      break;
  }
  Serial.println("out of Serial.");

  Serial.flush();
}


// UART를 통해 들어오는 것을 배열 버퍼에 넣기 위한 함수.
uint8_t SerialGets(char dest[], uint8_t length) {
  uint8_t textlen = 0; // Current text length
  char ch; // current character
  do {
    ch = Serial.read(); // UartGetc(); // Get a character from UART
    switch (ch) {
      case 59: // ; pressed.
        dest[textlen] = '\0';//0; // null terminate
        //        Serial.print(ch); // echo typed character
        break;
      default: // if input length is within limit and input is valid
        if ((textlen < length) & ((ch >= 0x20) & (ch < 0x7F))) { // valid characters
          dest[textlen] = ch; // append character to buffer
          textlen++;
          //          Serial.print(ch); // echo typed character
        }
        break;
    } // end switch
  } while (ch != 59);

  if (ch == 59) {
    return 1; // if received ;
  } else {
    return 0; // received other.
  }
}


//char 배열 출력
void printCharArray(char* data, byte datasize) {
  for (int i = 0; i < datasize; i++) {
    if (data[i] == '\0') {
      break;
    }
    Serial.print(data[i]);
  }
  Serial.println("");
}
