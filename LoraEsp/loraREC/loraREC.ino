#include "heltec.h"

#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6

String currentTemperature;
String currentHumidity;
String pHvalue;

void setup() {
    //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

}

void loop() {
 //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
       currentTemperature = LoRa.readStringUntil('|');
      //Serial.println(currentTemperature);
      currentHumidity = LoRa.readStringUntil('|');
      //Serial.println(currentHumidity);
      pHvalue = LoRa.readStringUntil('|');
      //Serial.println(currentLightIntensity);
    }
}
  float humValue = currentHumidity.toFloat();
  float temperatureValueC = currentTemperature.toFloat();
  float pH = pHvalue.toFloat();
  
  Serial.print(humValue);     // get value of humidity
  Serial.print(",");          // create space after the value of humidity
  Serial.print(temperatureValueC);  // get value of temperature in degrees Celcius
  Serial.print(",");
  Serial.println(pH);
  delay(1000);  // print value after interval of time

}
