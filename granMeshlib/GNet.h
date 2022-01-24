#ifndef GNet_h
#define GNet_h

#include <painlessMesh.h>
#include "namedMesh.h"


//******************** board Select ***********************

#define BOARD_TYPE 6

#if BOARD_TYPE == 0                 // Master DI4 DO4
    #include "board/MasterDI4DO4.h"
#elif BOARD_TYPE == 1               // DI8

#elif BOARD_TYPE == 2               // DO8
    #include "board/DO8.h"
#elif BOARD_TYPE == 3               // AI8
             
#elif BOARD_TYPE == 4               // AO4
            
#elif BOARD_TYPE == 5               // AI Core Temp Sensor Board
    #include "board/AICT.h"   
#elif BOARD_TYPE == 6               // Master Wifi
    #include "board/MasterWifi.h"
#endif


//****************** mesh network *************************

//#define   MESH_SSID     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       7000


class GNet
{
public:
    String MESH_SSID;
    String myNodeName;
    String toNodeMain = "MA001";

    GNet();
    void printBoardName();

    void setMESH_SSID(String data);
    void setmyNodeName(String data);

    String getMESH_SSID();
    String getmyNodeName();
    String gettoNodeMain();

private:


};


#endif
