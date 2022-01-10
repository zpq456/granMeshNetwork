#include "granEEPROM_esp32.h"

granEEPROM_esp32::granEEPROM_esp32()
{
    
}

//*******************************************
//*************** Public 함수 ***************
//*******************************************

void granEEPROM_esp32::setSerialNumber(char *data)
{
    setCharArrayData(M1.SerialNumber, data, 6);
}
void granEEPROM_esp32::setWifiSSID(char *data)
{
    setCharArrayData(M1.WifiSSID, data, 16);
}
void granEEPROM_esp32::setWifiPWD(char *data)
{
    setCharArrayData(M1.WifiPWD, data, 16);
}
void granEEPROM_esp32::setDBTable(char *data)
{
    setCharArrayData(M1.DBTable, data, 12);
}
void granEEPROM_esp32::setDelayTime(int data)
{
    M1.DelayTime = data;
}
void granEEPROM_esp32::setDeltaT(float data)
{
    M1.DeltaT = data;
}

char *granEEPROM_esp32::getSerialNumber() { return &M1.SerialNumber[0]; }
char *granEEPROM_esp32::getWifiSSID() { return &M1.WifiSSID[0]; }
char *granEEPROM_esp32::getWifiPWD() { return &M1.WifiPWD[0]; }
char *granEEPROM_esp32::getDBTable() { return &M1.DBTable[0]; }
int granEEPROM_esp32::getDelayTime() { return M1.DelayTime; }
float granEEPROM_esp32::getDeltaT() { return M1.DeltaT; }

void granEEPROM_esp32::setDefaultValue()
{
    //MA001
    M1.SerialNumber[0] = 'M';
    M1.SerialNumber[1] = 'A';
    M1.SerialNumber[2] = '0';
    M1.SerialNumber[3] = '0';
    M1.SerialNumber[4] = '1'; 
    M1.SerialNumber[5] = '\0';

    //Grantech
    M1.WifiSSID[0] = 'G';
    M1.WifiSSID[1] = 'r';
    M1.WifiSSID[2] = 'a';
    M1.WifiSSID[3] = 'n';
    M1.WifiSSID[4] = 't';
    M1.WifiSSID[5] = 'e';
    M1.WifiSSID[6] = 'c';
    M1.WifiSSID[7] = 'h'; 
    M1.WifiSSID[8] = '\0';

    //37851357
    M1.WifiPWD[0] = '3';
    M1.WifiPWD[1] = '7';
    M1.WifiPWD[2] = '8';
    M1.WifiPWD[3] = '5';
    M1.WifiPWD[4] = '1';
    M1.WifiPWD[5] = '3';
    M1.WifiPWD[6] = '5';
    M1.WifiPWD[7] = '7'; 
    M1.WifiPWD[8] = '\0';

    //Sujo1
    M1.DBTable[0] = 's';
    M1.DBTable[1] = 'u';
    M1.DBTable[2] = 'j';
    M1.DBTable[3] = 'o';
    M1.DBTable[4] = '0';
    M1.DBTable[5] = '1';
    M1.DBTable[6] = '\0';

    M1.DelayTime = 60;

    M1.DeltaT = 5.0;

}

void granEEPROM_esp32::printStruct()
{
    Serial.print("SerialNumber(S)    : ");
    printCharArray(M1.SerialNumber, 6);
    Serial.print("Wifi SSID(I)       : ");
    printCharArray(M1.WifiSSID, 15);
    Serial.print("Wifi Passward(P)   : ");
    printCharArray(M1.WifiPWD, 15);
    Serial.print("Wifi DB Table Name(N) : ");
    printCharArray(M1.DBTable, 12);
    Serial.print("Delay Time(T) : ");
    Serial.println(M1.DelayTime);
    Serial.print("Delta Time(D) : ");
    Serial.println(M1.DeltaT);
    Serial.println("");
}



//************ EEPROM 관련 함수 *************
void granEEPROM_esp32::EEPROM_begin(){
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("failed to initialise EEPROM");
        //delay(1000000);
    }
    delay(1000);
}
void granEEPROM_esp32::getEEPROM(){ EEPROM.get(0, M1); }
void granEEPROM_esp32::putEEPROM(){ 
    EEPROM.put(0, M1);
    EEPROM.commit(); 
    }


//주소명, 시작바이트, 데이터사이즈
void granEEPROM_esp32::EEPROM_write(char *data, int startbyte, byte datasize)
{
    int addr = startbyte;
    for (int i = 0; i < datasize; i++)
    {
        EEPROM.write(addr++, data[i]);
    }
    EEPROM.commit();
}


//*******************************************

//*******************************************
//************** Private 함수 ***************
//*******************************************

void granEEPROM_esp32::printCharArray(char *data, byte datasize)
{
    for (int i = 0; i < datasize; i++)
    {
        if (data[i] == '\0')//문자열 끝에 도달하면
        {
            break;
        }
        Serial.print(data[i]);
    }
    Serial.println("");
}

void granEEPROM_esp32::setCharArrayData(char *libData, char *data, byte datasize)
{
    for (int i = 0; i < datasize; i++)
    {

        libData[i] = data[i];

        if (data[i] == '\0')
        {
            break;
        }
    }
}
