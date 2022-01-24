//---------------------- mov_average.h -----------------------
#ifndef __AVERAGE_H__
#define __AVERAGE_H__
#define FILTERDATA 200
//  100    : 1:1연결 => 메시지가 많이 주고받아짐, 끊어짐 없음
//  100    : 1:3연결 => 메시지가 많이 주고받아짐, 가끔 끊어짐
//  10     : 1:1연결 => 통신이 원활하지만 가끔 끊어짐
//  500    :
#define SendMeshDelay 500

#define SchmittRange 70

float avg (float x);
#endif // __AVERAGE_H__
float saltdata[FILTERDATA];
float tempdata[FILTERDATA];
int avgCounter = 0; // 이동평균 횟수를 완수할때까지 대기시키기
//-------------------------------------------------------------

void readIO()
{
  //*********************** Sensor Data Check *************************
  temp_value_org = GetTemperature(analogRead(TEMP_SENSOR));
  temp_value = avg(temp_value_org, &tempdata[0]);


  if (avgCounter < FILTERDATA) {
    avgCounter++;
    if (avgCounter == FILTERDATA - 1) {
      Serial.println("");
      Serial.println("Sensor check ready");
      Serial.println("");
    }
  }
  else {
    //슈미트 트리거 방식으로 확인 (상위 70% 하위 30%)
    if (checkSchmitt(value.toFloat() ,temp_value ,_EEPROM.getDeltaT())) {

      //설정한 분해능으로 값을 반올림하기
      float roundValue = getRoundValue(temp_value, _EEPROM.getDeltaT());

      // 일정 수치이상 변화가 일어났을 때에 현재 온도값을 갱신한다.
      if (roundValue != value.toFloat()) {
        value = (String)roundValue;
        Serial.print("change temp over DeltaT : ");
        Serial.println(value);

        //온도값을 마스터에 송신
        if ( (sendMeshEndTime - sendMeshStartTime) >= SendMeshDelay) {
          meshSendMessage(_GNet.gettoNodeMain(), obtain_readings_sendTempSensor());
          rebootFlag = true;
          //메쉬 딜레이 초기화
          sendMeshStartTime = millis();
          sendMeshEndTime = millis();
        }
      }
    }else{
//      Serial.println("");
//      Serial.println("Schmitt Triger Activate!!");
//      Serial.println("");
    }
  }


  //정해진 시간이 되면 현재값을 마스터에 송신 (생존신고)
  if ((dbEndTime - dbStartTime) >= dbDelayTime &&
      !digitalRead(DBSWITCH)) {
    Serial.print(" temp : ");
    Serial.println(value);

    //온도값을 마스터에 송신
    meshSendMessage(_GNet.gettoNodeMain(), obtain_readings_sendTempSensor());
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

float getRoundValue(float x, float deltaT) {
  //     ( ( 현재값+(분해능/2) ) / 분해능 ) * 분해능
  float a = x + (deltaT / 2.0);
  int b = (int)(a / deltaT);
  return (float)b * deltaT;
}

//센서값 떨림 방지를 위한 함수 (슈미트 트리거)
bool checkSchmitt(float oldValue, float newValue, float deltaT) {
  if (newValue >= oldValue + (deltaT * SchmittRange / 100.0)) {
    //관측값 > 현재값 + 분해능 70%
    return true;
  }
  if (newValue <= oldValue - (deltaT * SchmittRange / 100.0)) {
    //관측값 < 현재값 - 분해능 70%
    return true;
  }

  return false;
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
