#include "granConvert.h"

//*******************************************
//*************** Public 함수 ***************
//*******************************************

granConvert::granConvert()
{
}

//char[] to int
int granConvert::CharArrayToInt(char data[]){
    int result = 0;
    int len = strlen(data) - 1 ;

    for(int i=0; i < strlen(data); i++){
        int temp = 1;
        for(int j=0; j < len; j++){
            temp = temp * 10;
        }
        len--;
        result += (data[i]- 48) * temp;
    }

    return result;
}

//float -> byte hex[]
void granConvert::FloatToHexArray(float f, unsigned char *hex)
{
    unsigned char *f_byte = reinterpret_cast<unsigned char *>(&f);
    memcpy(hex, f_byte, 4);
}

//byte hex[] -> float
float granConvert::HexArrayToFloat(unsigned char hexData[])
{
    hexfloat.b[0] = hexData[1]; //B
    hexfloat.b[1] = hexData[0]; //A
    hexfloat.b[2] = hexData[3]; //D
    hexfloat.b[3] = hexData[2]; //C

    return hexfloat.f;
}

//*******************************************
//************** Private 함수 ***************
//*******************************************
