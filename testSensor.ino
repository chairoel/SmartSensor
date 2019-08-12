#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <MAX44009.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


MAX44009 light;
Adafruit_BME280 bme; 
const char *ssid = "SUPER_TEAM";
const char *pass = "e13cPipe";
int cahaya = 0;
float Temp = 0;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    WiFi.begin(ssid, pass);
    Serial.print("Connecting");
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    digitalWrite(D4, LOW);

    if (light.begin())
    {
        Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
        while (1);
    }
    unsigned status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }
}

void loop()
{
    Serial.print("Light (lux):    ");
    Serial.println(light.get_lux());
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    Temp = bme.readTemperature();
    cahaya = light.get_lux();
    kirim();
    delay(1000);
}

void kirim()
{
    if (WiFi.status() == WL_CONNECTED)
    { //Check WiFi connection status
        HTTPClient http;
        http.begin("http://192.168.77.90:8090/post");                            //Specify destination for HTTP request
        http.addHeader("Content-Type", "text/plain");                            //Specify content-type header
        int httpResponseCode = http.POST("Cahaya = " + String(cahaya) + " lux & Suhu = " + String(Temp,2) + " *C"); //Send the actual POST request

        if (httpResponseCode > 0)
        {
            String response = http.getString(); //Get the response to the request
            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }
        http.end(); //Free resources
    }
    else
    {
        Serial.println("Error in WiFi connection");
    }
}
