// 김치연구소 kimchi Sensor Lib for ESP32

#ifndef granEEPROM_esp32_h
#define granEEPROM_esp32_h

#include <EEPROM.h>

#define EEPROM_SIZE 64

class granEEPROM_esp32{
public:
granEEPROM_esp32();

//************** M1 구조체 변수 **************
struct PARAM_{
    char SerialNumber[6];	//보드 고유번호 == 노드 이름(NodeName)
    char WifiSSID[16]; 		//Wifi ssid
    char WifiPWD[16];   	//Wifi pw
    char DBTable[12];       //DB테이블 이름 == 메쉬 이름(MeshID)
    char DelayTime[4];  	//딜레이 시간
    char ToNodeName[6];	    //연결될 노드 이름
} M1 ;
//*******************************************

void setSerialNumber(char* data);
void setWifiSSID(char* data);
void setWifiPWD(char* data);
void setDBTable(char* data);
void setDelayTime(char* data);
void setToNodeName(char* data);

char* getSerialNumber();
char* getWifiSSID();
char* getWifiPWD();
char* getDBTable();
char* getDelayTime();
char* getToNodeName();

void setDefaultValue();
void printStruct();

//************ EEPROM 관련 함수 *************
void EEPROM_begin();
void EEPROM_read_All();
void EEPROM_write_All();
void EEPROM_write(char *data, int startbyte, byte datasize);
// void getEEPROM();
// void putEEPROM();
//*******************************************
private:

void printCharArray(char* data, byte datasize);
void setCharArrayData(char *libData, char *data, byte datasize);


};

#endif

