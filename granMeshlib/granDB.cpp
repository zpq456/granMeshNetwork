#include "granDB.h"

//*******************************************
//*************** Public 함수 ***************
//*******************************************

granDB::granDB()
{
    
}

void granDB::insertDatabaseAll(String serial, String salt, String temp, String tableName)
{
    WiFiClient client;
    HTTPClient http;

    String serverName = domainName + tableName + phpName_insert ;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    //--------------------------------------Kimchi sujo1 teable Data Send format-----------------------------------------
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&serial=" + serial + "&salt=" + salt + "&temp=" + temp + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");

    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    client.stop();
}

void granDB::postSensorSetting(int type, String serial, String tablename, int delaytime)
{
    WiFiClient client;
    HTTPClient http;

    String serverName;
    switch(type){
        case 0:
        serverName = domainName + phpName_insert_sensor_setting ;
        break;
        case 1:
        serverName = domainName + phpName_update_sensor_setting ;
        break;
    }
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

   // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&serial=" + serial + "&tablename=" + tablename + "&delaytime=" + delaytime + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    client.stop();
}

void granDB::getSensorSetting(String serial){
    WiFiClient client;
    HTTPClient http;

    String serverName = domainName + phpName_select_sensor_setting +
                        "?serial='" + serial + "'";

    Serial.println(serverName);
      
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        setSensorReadings(http.getString());
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    client.stop();
}

void granDB::parsingSensorSettingJson(){
    StaticJsonDocument<200> doc;
    
    Serial.print("sensorReadings : ");
    Serial.println(getSensorReadings());

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, getSensorReadings());
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
       return;
    }

    setSensorSerial(doc["serial"]);
    setSensorTablename(doc["tablename"]);
    setSensorDelaytime(doc["delaytime"]);

    Serial.print(" serial : ");
    Serial.println(getSensorSerial());
    Serial.print(" tablename : ");
    Serial.println(getSensorTablename());
    Serial.print(" delaytime : ");
    Serial.println(getSensorDelaytime());

}

void granDB::setSensorReadings(String data) {sensorReadings = data;}
void granDB::setSensorSerial(String data) {sensor_setting_struct.serial = data;}
void granDB::setSensorTablename(String data){sensor_setting_struct.tablename = data;}
void granDB::setSensorDelaytime(int data){sensor_setting_struct.delaytime = data;}

String granDB::getSensorReadings() {return sensorReadings;}
String granDB::getSensorSerial(){return sensor_setting_struct.serial;}
String granDB::getSensorTablename(){return sensor_setting_struct.tablename;}
int granDB::getSensorDelaytime(){return sensor_setting_struct.delaytime;}



//******************************* MasterDI4DO4 ************************************************
void granDB::insertDBData_MasterDI4DO4(String serial, String tableName, float temp1, float temp2, float temp3, float temp4){
    WiFiClient client;
    HTTPClient http;

    String serverName = domainName + tableName + phpName_update_MasterDi4Do4;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

   // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + 
                             "&serial=" + serial + 
                             "&temp1=" + temp1 + 
                             "&temp2=" + temp2 + 
                             "&temp3=" + temp3 + 
                             "&temp4=" + temp4 + 
                             "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    client.stop();
}



//*******************************************
//************** Private 함수 ***************
//*******************************************
