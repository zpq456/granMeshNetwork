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
void granEEPROM_esp32::setDelayTime(char * data)
{
    setCharArrayData(M1.DelayTime, data, 4);
}
void granEEPROM_esp32::setToNodeName(char * data)
{
    setCharArrayData(M1.ToNodeName, data, 6);
}

char *granEEPROM_esp32::getSerialNumber() { return &M1.SerialNumber[0]; }
char *granEEPROM_esp32::getWifiSSID() { return &M1.WifiSSID[0]; }
char *granEEPROM_esp32::getWifiPWD() { return &M1.WifiPWD[0]; }
char *granEEPROM_esp32::getDBTable() { return &M1.DBTable[0]; }
char *granEEPROM_esp32::getDelayTime() { return &M1.DelayTime[0]; }
char *granEEPROM_esp32::getToNodeName() { return &M1.ToNodeName[0]; }

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

    // delay time 001 sec 
    M1.DelayTime[0] = '0';
    M1.DelayTime[1] = '6';
    M1.DelayTime[2] = '0';
    M1.DelayTime[3] = '\0';

    //SA001
    M1.ToNodeName[0] = 'S';
    M1.ToNodeName[1] = 'A';
    M1.ToNodeName[2] = '0';
    M1.ToNodeName[3] = '0';
    M1.ToNodeName[4] = '1';
    M1.ToNodeName[5] = '\0';
}

void granEEPROM_esp32::printStruct()
{
    Serial.print("SerialNumber(S)    : ");
    printCharArray(M1.SerialNumber, 6);
    Serial.print("Wifi SSID(I)       : ");
    printCharArray(M1.WifiSSID, 15);
    Serial.print("Wifi Passward(P)   : ");
    printCharArray(M1.WifiPWD, 15);
    Serial.print("Wifi DB Table Name(D) : ");
    printCharArray(M1.DBTable, 12);
    Serial.print("Delay Time(T) [001~999 sec] : ");
    printCharArray(M1.DelayTime, 4);
    Serial.print("To NodeName(N) : ");
    printCharArray(M1.ToNodeName, 6);
    Serial.println("");
}



//************ EEPROM 관련 함수 *************
void granEEPROM_esp32::EEPROM_begin(){
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("failed to initialise EEPROM");
        delay(1000000);
    }
}
// void granEEPROM_esp32::getEEPROM(){ EEPROM.get(0, M1); }
// void granEEPROM_esp32::putEEPROM(){ EEPROM.put(0, M1); }

void granEEPROM_esp32::EEPROM_read_All()
{
    int i = 0;

    M1.SerialNumber[0] = EEPROM.read(i++);M1.SerialNumber[1] = EEPROM.read(i++);M1.SerialNumber[2] = EEPROM.read(i++);
    M1.SerialNumber[3] = EEPROM.read(i++);M1.SerialNumber[4] = EEPROM.read(i++);M1.SerialNumber[5] = EEPROM.read(i++);

    M1.WifiSSID[0] = EEPROM.read(i++);M1.WifiSSID[1] = EEPROM.read(i++);M1.WifiSSID[2] = EEPROM.read(i++);
    M1.WifiSSID[3] = EEPROM.read(i++);M1.WifiSSID[4] = EEPROM.read(i++);M1.WifiSSID[5] = EEPROM.read(i++);
    M1.WifiSSID[6] = EEPROM.read(i++);M1.WifiSSID[7] = EEPROM.read(i++);M1.WifiSSID[8] = EEPROM.read(i++);
    M1.WifiSSID[9] = EEPROM.read(i++);M1.WifiSSID[10] = EEPROM.read(i++);M1.WifiSSID[11] = EEPROM.read(i++);
    M1.WifiSSID[12] = EEPROM.read(i++);M1.WifiSSID[13] = EEPROM.read(i++);M1.WifiSSID[14] = EEPROM.read(i++);
    M1.WifiSSID[15] = EEPROM.read(i++);

    M1.WifiPWD[0] = EEPROM.read(i++);M1.WifiPWD[1] = EEPROM.read(i++);M1.WifiPWD[2] = EEPROM.read(i++);
    M1.WifiPWD[3] = EEPROM.read(i++);M1.WifiPWD[4] = EEPROM.read(i++);M1.WifiPWD[5] = EEPROM.read(i++);
    M1.WifiPWD[6] = EEPROM.read(i++);M1.WifiPWD[7] = EEPROM.read(i++);M1.WifiPWD[8] = EEPROM.read(i++);
    M1.WifiPWD[9] = EEPROM.read(i++);M1.WifiPWD[10] = EEPROM.read(i++);M1.WifiPWD[11] = EEPROM.read(i++);
    M1.WifiPWD[12] = EEPROM.read(i++);M1.WifiPWD[13] = EEPROM.read(i++);M1.WifiPWD[14] = EEPROM.read(i++);
    M1.WifiPWD[15] = EEPROM.read(i++);

    M1.DBTable[0] = EEPROM.read(i++);M1.DBTable[1] = EEPROM.read(i++);M1.DBTable[2] = EEPROM.read(i++);
    M1.DBTable[3] = EEPROM.read(i++);M1.DBTable[4] = EEPROM.read(i++);M1.DBTable[5] = EEPROM.read(i++);
    M1.DBTable[6] = EEPROM.read(i++);M1.DBTable[7] = EEPROM.read(i++);M1.DBTable[8] = EEPROM.read(i++);
    M1.DBTable[9] = EEPROM.read(i++);M1.DBTable[10] = EEPROM.read(i++);M1.DBTable[11] = EEPROM.read(i++);

    M1.DelayTime[0] = EEPROM.read(i++);M1.DelayTime[1] = EEPROM.read(i++);
    M1.DelayTime[2] = EEPROM.read(i++);M1.DelayTime[3] = EEPROM.read(i++);

    M1.ToNodeName[0] = EEPROM.read(i++);M1.ToNodeName[1] = EEPROM.read(i++);M1.ToNodeName[2] = EEPROM.read(i++);
    M1.ToNodeName[3] = EEPROM.read(i++);M1.ToNodeName[4] = EEPROM.read(i++);M1.ToNodeName[5] = EEPROM.read(i++);
}

void granEEPROM_esp32::EEPROM_write_All(){
    EEPROM_write(&M1.SerialNumber[0], 0, 6);
    EEPROM_write(&M1.WifiSSID[0], 6, 16);
    EEPROM_write(&M1.WifiPWD[0], 22, 16);
    EEPROM_write(&M1.DBTable[0], 38, 12);
    EEPROM_write(&M1.DelayTime[0], 50, 4);
    EEPROM_write(&M1.ToNodeName[0], 54, 6);
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


//void granEEPROM_esp32::getEEPROM() { EEPROM.get(0, M1); }
//void granEEPROM_esp32::putEEPROM() { EEPROM.put(0, M1); }


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
