int temp = 0;

void readSerial() {
  String stringTemp;
  Serial.println("in Serial.");

  SerialGets(&serialBuf[0], 21);

  switch (serialBuf[0]) {
    case 'S': //SerialNumber 설정
      _EEPROM.setSerialNumber(&serialBuf[1]);
      Serial.print("SerialNumber(S)    : "); printCharArray(_EEPROM.getSerialNumber(), 6);
      break;
    case 'I': //Wifi ID 설정
      _EEPROM.setWifiSSID(&serialBuf[1]);
      Serial.print("Wifi SSID(I)       : "); printCharArray(_EEPROM.getWifiSSID(), 17);
      break;
    case 'P': //Wifi PW 설정
      _EEPROM.setWifiPWD(&serialBuf[1]);
      Serial.print("Wifi Passward(P)   : "); printCharArray(_EEPROM.getWifiPWD(), 17);
      break;
    case 'N': //Wifi Name 설정
      _EEPROM.setDBTable(&serialBuf[1]);
      Serial.print("Wifi DB Table Name(N) : "); printCharArray(_EEPROM.getDBTable(), 17);
      break;
    case 'T': //Delay Time 설정
      stringTemp = &serialBuf[1];
      _EEPROM.setDelayTime(stringTemp.toInt());
      Serial.print("Delay Time(T) : "); 
      Serial.println(_EEPROM.getDelayTime());
      break;
    case 'D': //Delta Time 설정
      stringTemp = &serialBuf[1];
      _EEPROM.setDeltaT(stringTemp.toFloat());
      Serial.print("Delta Time(D) : ");
      Serial.println(_EEPROM.getDeltaT());
      break;
    case '*': // Restore default value
      temp  = (serialBuf[1] - 48) * 100;
      temp += (serialBuf[2] - 48) * 10;
      temp += serialBuf[3] - 48;
      if (temp == 777) {
        _EEPROM.setDefaultValue(BOARD_TYPE);
        Serial.println("Factory default values are restored.");
        _EEPROM.printStruct(BOARD_TYPE);
      }
      break;
    case '!':   //EEPROM Save
      _EEPROM.putEEPROM(BOARD_TYPE);
      //      _EEPROM.EEPROM_write_All();
      Serial.println("=================================================");
      _EEPROM.printStruct(BOARD_TYPE);
      Serial.println("PARAMETER Saved.");
      break;
    case '@':   //EEPROM Read
      Serial.print("E="); Serial.println('E');
      _EEPROM.getEEPROM(BOARD_TYPE);

      Serial.println("=================================================");
      Serial.println("PARAMETER Read.");
      _EEPROM.printStruct(BOARD_TYPE);
      Serial.println("@:Read parameter, !:Save parameter, ?:Help, ~:Exit Develop Modes");
      break;
    case '?':
      Serial.println("=================================================");
      Serial.println("Help");
      _EEPROM.printStruct(BOARD_TYPE);
      Serial.println("Txx is 2 digit, Lxxx, Bxxx is 3 digit");
      Serial.println("*777:Restore factory default value.");
      Serial.println("@:Read parameter, !:Save parameter, ?:Help, ~:Exit Develop Modes");
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
