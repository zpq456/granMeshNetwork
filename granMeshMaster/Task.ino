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
  if ((dbEndTime - dbStartTime) >= dbDelayTime &&
      !digitalRead(DBSWITCH)) {

    //    Serial.print("WIFI RSSI : ");
    //    Serial.println(WiFi.RSSI());

    //********************** send order output ***********************

    //print DataTable
    printDataTable();
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
  else if ((dbEndTime - dbStartTime) >= dbDelayTime &&
           !(digitalRead(TACTBTN)) && (digitalRead(DBSWITCH))) {
    //getSensorFromDB();
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
