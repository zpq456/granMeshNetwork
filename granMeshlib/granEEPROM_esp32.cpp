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
    setCharArrayData(M1.WifiSSID, data, 17);
}
void granEEPROM_esp32::setWifiPWD(char *data)
{
    setCharArrayData(M1.WifiPWD, data, 17);
}
void granEEPROM_esp32::setDBTable(char *data)
{
    setCharArrayData(M1.DBTable, data, 17);
}
void granEEPROM_esp32::setDelayTime(int data)
{
    M1.DelayTime = data;
}

char *granEEPROM_esp32::getSerialNumber() { return &M1.SerialNumber[0]; }
char *granEEPROM_esp32::getWifiSSID() { return &M1.WifiSSID[0]; }
char *granEEPROM_esp32::getWifiPWD() { return &M1.WifiPWD[0]; }
char *granEEPROM_esp32::getDBTable() { return &M1.DBTable[0]; }
int granEEPROM_esp32::getDelayTime() { return M1.DelayTime; }

void granEEPROM_esp32::setDefaultValue(int boardType)
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
    M1.WifiSSID[8] = '2'; 
    M1.WifiSSID[9] = '.'; 
    M1.WifiSSID[10] = '4'; 
    M1.WifiSSID[11] = '\0';

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
    M1.DBTable[0] = 'M';
    M1.DBTable[1] = 'a';
    M1.DBTable[2] = 's';
    M1.DBTable[3] = 't';
    M1.DBTable[4] = 'e';
    M1.DBTable[5] = 'r';
    M1.DBTable[6] = 'D';
    M1.DBTable[7] = 'I';
    M1.DBTable[8] = '4';
    M1.DBTable[9] = 'D';
    M1.DBTable[10] = 'O';
    M1.DBTable[11] = '4';
    M1.DBTable[12] = '\0';

    M1.DelayTime = 60;

    switch(boardType){
        case 0:
            setDefaultValueMasterDI4DO4();
        break;
        case 2:
            setDefaultValueDO8();
        break;
        case 6:
            setDefaultValueAICT();
        break;
    }

}

void granEEPROM_esp32::printStruct(int boardType)
{
    Serial.print("SerialNumber(S)    : ");
    printCharArray(M1.SerialNumber, 6);
    Serial.print("Wifi SSID(I)       : ");
    printCharArray(M1.WifiSSID, 17);
    Serial.print("Wifi Passward(P)   : ");
    printCharArray(M1.WifiPWD, 17);
    Serial.print("Wifi DB Table Name(N) : ");
    printCharArray(M1.DBTable, 17);
    Serial.print("Delay Time(T) : ");
    Serial.println(M1.DelayTime);
    Serial.println("");

    switch(boardType){
        case 0:
            printStructMasterDI4DO4();
        break;
        case 2:
            printStructDO8();
        break;
        case 6:
            printStructAICT();
        break;
    }
}



//**************** AUCT INPUT 함수 ***************
void granEEPROM_esp32::setDeltaT(float data)
{
    AICT.DeltaT = data;
}
float granEEPROM_esp32::getDeltaT() { return AICT.DeltaT; }

void granEEPROM_esp32::setDefaultValueAICT()
{
    AICT.DeltaT = 5.0;
}

void granEEPROM_esp32::printStructAICT()
{
    Serial.print("Delta Time(D) : ");
    Serial.println(AICT.DeltaT);
    Serial.println("");
}



//**************** DO8 OUTPUT 함수 ***************
void granEEPROM_esp32::setDO8relayValue(int num, int data){
    DO8.relayValue[num] = data;
}
int granEEPROM_esp32::getDO8relayValue(int num){
    return DO8.relayValue[num];
}

void granEEPROM_esp32::setDefaultValueDO8()
{
    for(int i=0;i<sizeof(DO8.relayValue);i++){
        DO8.relayValue[i] = 0;
    }
}

void granEEPROM_esp32::printStructDO8()
{
    Serial.print("RelayValue    : ");
    Serial.print("[");
    Serial.print(DO8.relayValue[0]);
    Serial.print(",");
    Serial.print(DO8.relayValue[1]);
    Serial.print(",");
    Serial.print(DO8.relayValue[2]);
    Serial.print(",");
    Serial.print(DO8.relayValue[3]);
    Serial.print(",");
    Serial.print(DO8.relayValue[4]);
    Serial.print(",");
    Serial.print(DO8.relayValue[5]);
    Serial.print(",");
    Serial.print(DO8.relayValue[6]);
    Serial.print(",");
    Serial.print(DO8.relayValue[7]);
    Serial.println("]");
    Serial.println("");
}

//*******************************************

//**************** MasterDI4DO4 함수 ***************
void granEEPROM_esp32::setMasterDI4DO4relayValue(int node, int rellay, int data){
    MasterDI4DO4.sensorWarning[node][rellay] = data;
}
int granEEPROM_esp32::getMasterDI4DO4relayValue(int node, int rellay){
    return MasterDI4DO4.sensorWarning[node][rellay];
}
int *granEEPROM_esp32::getMasterDI4DO4relayAddress(int node){
    return &MasterDI4DO4.sensorWarning[node][0];
}
void granEEPROM_esp32::setMasterDI4DO4inputValue(int num, float data){
    MasterDI4DO4.tempSensor[num] = data;
}
float granEEPROM_esp32::getMasterDI4DO4inputValue(int num){
    return MasterDI4DO4.tempSensor[num];
}

void granEEPROM_esp32::setDefaultValueMasterDI4DO4()
{
    MasterDI4DO4.tempSensor[0] = 0;
    MasterDI4DO4.tempSensor[1] = 0;
    MasterDI4DO4.tempSensor[2] = 0;
    MasterDI4DO4.tempSensor[3] = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            MasterDI4DO4.sensorWarning[i][j] = 0;
        }
    }
}

void granEEPROM_esp32::printStructMasterDI4DO4()
{
    Serial.println("");
    Serial.println(MasterDI4DO4.tempSensor[0]);
    Serial.println(MasterDI4DO4.tempSensor[1]);
    Serial.println(MasterDI4DO4.tempSensor[2]);
    Serial.println(MasterDI4DO4.tempSensor[3]);
    Serial.println("");

    Serial.println("RelayValue    : ");
    for (int i = 0; i < 4; i++) {
        Serial.print("[ ");
        for (int j = 0; j < 8; j++) {
            Serial.print(MasterDI4DO4.sensorWarning[i][j]);
            Serial.print(" ");
        }
        Serial.println("]");
    }
    
    Serial.println("Change Relay Value(C) [C NodeName.RelayNum.value / ex) CNo001.1.0]");
    Serial.println("Order  Relay Value(O) [O NodeName.RelayNum.value / ex) ONo001.1.0]");
    Serial.println("");

}

//*******************************************

//************ EEPROM 관련 함수 *************
void granEEPROM_esp32::EEPROM_begin(){
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("failed to initialise EEPROM");
        //delay(1000000);
    }
    delay(1000);
}
void granEEPROM_esp32::getEEPROM(int boardType){ 
    EEPROM.get(0, M1); 

    switch(boardType){
        case 0:
            EEPROM.get(sizeof(M1), MasterDI4DO4);
        break;
        case 2:
            EEPROM.get(sizeof(M1), DO8);
        break;
        case 6:
            EEPROM.get(sizeof(M1), AICT);
        break;
    }
}
void granEEPROM_esp32::putEEPROM(int boardType){ 
    EEPROM.put(0, M1); 

    switch(boardType){
        case 0:
            EEPROM.put(sizeof(M1), MasterDI4DO4);
        break;
        case 2:
            EEPROM.put(sizeof(M1), DO8); 
        break;
        case 6:
            EEPROM.put(sizeof(M1), AICT); 
        break;
    }
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
