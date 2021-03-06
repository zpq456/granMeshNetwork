// 김치연구소 kimchi Sensor Lib for ESP32

#ifndef granEEPROM_esp32_h
#define granEEPROM_esp32_h

#include <EEPROM.h>

#define EEPROM_SIZE 512

class granEEPROM_esp32{
public:
granEEPROM_esp32();

//************** M1 구조체 변수 **************
struct PARAM_{
    char SerialNumber[6];	//보드 고유번호 == 노드 이름(NodeName)
    char WifiSSID[17]; 		//Wifi ssid
    char WifiPWD[17];   	//Wifi pw
    char DBTable[17];       //DB테이블 이름 == 메쉬 이름(MeshID)
    int DelayTime;  	    //딜레이 시간
} M1 ;

void setSerialNumber(char* data);
void setWifiSSID(char* data);
void setWifiPWD(char* data);
void setDBTable(char* data);
void setDelayTime(int data);

char* getSerialNumber();
char* getWifiSSID();
char* getWifiPWD();
char* getDBTable();
int getDelayTime();
//*******************************************

struct AICT_SENSOR_NODE{
    float DeltaT;           //센서 변화량 체크값
} AICT ;

void setDeltaT(float data);
float getDeltaT();
void setDefaultValueAICT();
void printStructAICT();

//**************** DO8 OUTPUT 변수 ***************
struct DO8DATA{
    int relayValue[8];
} DO8 ;

void setDO8relayValue(int num, int data);
int getDO8relayValue(int num);
void setDefaultValueDO8();
void printStructDO8();
//*******************************************


//**************** MasterDI4DO4 변수 ***************
struct MasterDI4DO4DATA{
    float tempSensor[4];
    int sensorWarning[4][8]; // 4 node * 8 rellay
} MasterDI4DO4 ;

void setMasterDI4DO4relayValue(int node, int rellay, int data);
int getMasterDI4DO4relayValue(int node, int rellay);
int *getMasterDI4DO4relayAddress(int node);
void setMasterDI4DO4inputValue(int num, float data);
float getMasterDI4DO4inputValue(int num);
void setDefaultValueMasterDI4DO4();
void printStructMasterDI4DO4();
//*******************************************

void setDefaultValue(int boardType);
void printStruct(int boardType);

//************ EEPROM 관련 함수 *************
void EEPROM_begin();
void EEPROM_write(char *data, int startbyte, byte datasize);
void getEEPROM(int boardType);
void putEEPROM(int boardType);
//*******************************************
private:

void printCharArray(char* data, byte datasize);
void setCharArrayData(char *libData, char *data, byte datasize);


};

#endif

