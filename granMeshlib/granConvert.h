// 김치연구소 kimchi Sensor Lib for ESP32

#ifndef granConvert_h
#define granConvert_h

#include <String.h>
#include <Arduino.h>

class granConvert
{
public:
    union
    {
        byte b[4];
        float f;
    } hexfloat;

    granConvert();

    int CharArrayToInt(char data[]);                   //char[] -> int
    void FloatToHexArray(float f, unsigned char *hex); //float -> byte hex[]
    float HexArrayToFloat(unsigned char hexData[]);    //byte hex[] -> float

private:
};

#endif
