#include "GNet.h"

GNet::GNet()
{

}

//*******************************************
//*************** Public 함수 ***************
//*******************************************


void GNet::printBoardName(){
    Serial.println("");
    Serial.print("Board Name : ");
    Serial.println(BOARD_NAME);
    Serial.println("");
}

void GNet::setMESH_SSID(String data){ MESH_SSID = data; }
void GNet::setmyNodeName(String data){ myNodeName = data; }    

String GNet::getMESH_SSID(){return MESH_SSID;}
String GNet::getmyNodeName(){return myNodeName;}
String GNet::gettoNodeMain(){return toNodeMain;}


//*******************************************
//************** Private 함수 ***************
//*******************************************

