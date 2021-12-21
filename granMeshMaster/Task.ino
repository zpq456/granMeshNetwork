//---------------------- mov_average.h -----------------------
#ifndef __AVERAGE_H__
#define __AVERAGE_H__
#define FILTERDATA 100
float avg (float x);
#endif // __AVERAGE_H__
float saltdata[FILTERDATA];
float tempdata[FILTERDATA];
//-------------------------------------------------------------

void readIO()
{
  //시간이 되면 작업 진행
  if ((dbEndTime - dbStartTime) >= _granlib._DB.getSensorDelaytime() * 1000 &&
      !digitalRead(DBSWITCH)) {

//    Serial.print("WIFI RSSI : ");
//    Serial.println(WiFi.RSSI());

    //get RTC data
    String RTCtime = getRTCTime(rtc);

    //*********************** Sensor Data Check *************************


    //********************** selet saving method *************************
    if (ENABLE_USB == 1) {            // USB Read/Write Mode
      //usbWriteSensorData(temp_value, salt_value, RTCtime);
    }
    else {                            // write DB table
//      _granlib._DB.insertDatabaseAll(
//        _granlib._EEPROM.getSerialNumber(),
//        (String)salt_value,
//        (String)temp_value,
//        _granlib._DB.getSensorTablename()
//      );
    }

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
  //insert or update sensor setting
  else if ((dbEndTime - dbStartTime) >= _granlib._DB.getSensorDelaytime() * 1000 &&
           !(digitalRead(TACTBTN)) && (digitalRead(DBSWITCH))) {
    getSensorFromDB();
  }

  //waiting AP Message
  //checkAPMessage();
}

double GetTemperature(int v)
{
  // double Temp;
  //       Temp = log(10000.0*((4096.0 / v - 1)));// for pull-down configuration
  //      // Temp = log(10000.0 / (4096.0 / v - 1));
  //       Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp))* Temp);
  //       Temp = Temp - 210.15; // Convert Kelvin to Celcius
  //      // Temp = (Temp * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit
  //return Temp;

  float R1 = 10000;
  float logR2, R2, T, Tc;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  R2 = R1 * (4095.0 / (float)v - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 270.15;  //273.15;
  return Tc;
}

float avg (float x , float * data)
{
  unsigned char i;
  float sum = 0, average;
  for (i = 0; i < FILTERDATA - 1; i++)
    data[i] = data[i + 1];
  data[FILTERDATA - 1] = x;
  for (i = 0; i < FILTERDATA; i++)
    sum += data[i];
  average = sum / FILTERDATA;
  return average;
}


void usbWriteSensorData(float temp, float salt, String RTCtime) {

  if (flashDrive.checkIntMessage()) {
    if (flashDrive.getDeviceStatus()) {
      Serial.println(F("Flash drive attached!"));
    } else {
      Serial.println(F("Flash drive detached!"));
    }
  }
  if (createLogFlag < 2 && flashDrive.driveReady()) {
    //기존에 남아있는 LOG.TXT 삭제
    if (createLogFlag == 0) {
      flashDrive.setFileName("LOG.TXT");  //set the file name
      flashDrive.deleteFile();              //delete file
    }
    flashDrive.setFileName("LOG.TXT");  //set the file name
    flashDrive.openFile();                //open the file
    char text[] = "time,salt,temp\n";

    for (int a = 0; a < 1; a++) {        //write text from string(adat) to flash drive 20 times
      flashDrive.writeFile(text, strlen(text)); //string, string length
    }
    flashDrive.closeFile();               //at the end, close the file

    createLogFlag++;
  } else { //atach Sensor Data
    String atachText = RTCtime + "," + (String)salt + "," + (String)temp + "\n";
    atachText.toCharArray(adatBuffer, atachText.length() + 1);
    Serial.print(atachText);

    flashDrive.setFileName("LOG.TXT");  //set the file name
    if (flashDrive.openFile() == ANSW_USB_INT_SUCCESS) {             //open the file
      flashDrive.moveCursor(flashDrive.getFileSize()); // is almost the same as CURSOREND, because we put our cursor at end of the file
    }
    for (int a = 0; a < 1; a++) {        //write text from string(adat) to flash drive 20 times
      if (flashDrive.getFreeSectors()) { //check the free space on the drive
        flashDrive.writeFile(adatBuffer, strlen(adatBuffer));//atachText.length());
      } else {
        printInfo("Disk full");
      }
    }
    flashDrive.closeFile();               //at the end, close the file
  }

}


//Print information
void printInfo(const char info[]) {

  int infoLength = strlen(info);
  if (infoLength > 40) {
    infoLength = 40;
  }
  Serial.print(F("\n\n"));
  for (int a = 0; a < infoLength; a++) {
    Serial.print('*');
  }
  Serial.println();
  Serial.println(info);
  for (int a = 0; a < infoLength; a++) {
    Serial.print('*');
  }
  Serial.print(F("\n\n"));
}

//*************************** RTC 함수 ******************************
void SetRTCTime(String ss) //yyyymmddhhmmss
{
  int y, m, d, h, n, s;
  char buf[5];
  char dt[15];
  strcpy(dt, String2char(ss));
  strncpy(buf, &dt[0], 4);
  buf[4] = 0;
  y = atoi(buf);
  strncpy(buf, &dt[4], 2);
  buf[2] = 0;
  m = atoi(buf);
  strncpy(buf, &dt[6], 2);
  d = atoi(buf);
  strncpy(buf, &dt[8], 2);
  h = atoi(buf);
  strncpy(buf, &dt[10], 2);
  n = atoi(buf);
  strncpy(buf, &dt[12], 2);
  s = atoi(buf);
  rtc.adjust(DateTime(y, m, d, h, n, s));
}

char* String2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

String getRTCTime(RTC_DS1307 RTC) {
  DateTime cur_dt;
  cur_dt = RTC.now();
  String RTCtime =
    (String)cur_dt.year() + "." +
    (String)cur_dt.month() + "." +
    (String)cur_dt.day() + "." +
    (String)cur_dt.hour() + ":" +
    (String)cur_dt.minute() + ":" +
    (String)cur_dt.second();

  return RTCtime;
}
