// 김치연구소 kimchi Sensor Lib for ESP32

#ifndef granDB_h
#define granDB_h

#include <HTTPClient.h>
#include <WiFiClient.h>
#include <String.h>
#include <ArduinoJson.h>

#define ARRAY_LEN 10
class granDB
{
public:
    granDB();

    struct SENSOR
    {
        String serial;
        String tablename;
        int delaytime;
    } sensor_setting_struct;

    // DB Connect Info
    String domainName = "http://kimchilab.net/";
    String phpName_insert = "/post-kimchi-data.php";
    String phpName_select_all = "/select-kimchi-data-all.php";
    String phpName_select_data = "/select-kimchi-data.php";
    String phpName_insert_sensor_setting = "insert-sensor-setting.php";
    String phpName_update_sensor_setting = "update-sensor-setting.php";
    String phpName_select_sensor_setting = "select-sensor-setting.php";


    // DB Connect Info for MasterDI4DO4
    String phpName_update_MasterDi4Do4 = "/post-kimchi-data.php";
 
    String apiKeyValue = "tPmAT5Ab3j7F9";
    String sensorReadings;

    void insertDatabaseAll(String serial, String salt, String temp, String tableName);
    void postSensorSetting(int type, String serial, String tablename, int delaytime);
    void getSensorSetting(String serial);
    void parsingSensorSettingJson();

    void setSensorReadings(String data);
    void setSensorSerial(String data);
    void setSensorTablename(String data);
    void setSensorDelaytime(int data);

    void insertDBData_MasterDI4DO4(String serial, String tableName, float temp1, float temp2, float temp3, float temp4);
    
    String getSensorReadings();
    String getSensorSerial();
    String getSensorTablename();
    int getSensorDelaytime();

private:
};

#endif
