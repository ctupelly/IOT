#include<stdlib.h>
#include <SoftwareSerial.h>

\
#define SSID "purna"
#define PASS "purna1035"
#define IP "184.106.153.149" // thingspeak
String GET = "GET /update?key=KOHYZVC94RQT24ES&field3=";
SoftwareSerial wifiport(10, 11); // RX, TX
const int temperaturePin = 0;

void setup()
{
  wifiport.begin(9600);
  Serial.begin(9600);
  sendDebug("AT");
  delay(5000);
  if(wifiport.find("OK")){
    Serial.println("RECEIVED: OK");
    sendDebug("AT+CIPMUX=0");
  }
  if(wifiport.find("Error")){
    Serial.println("RECEIVED: Error");
    return;
//    connectWiFi();
  }
}

void loop(){
  delay(2000);
  Serial.println("newloop");
  sendDebug("AT");
  delay(5000);
  if(wifiport.find("OK")){
    Serial.println("RECEIVED: OK");
  }
  if(wifiport.find("Error")){
    Serial.println("RECEIVED: Error");
    return;
  }
  float voltage, degreesC, degreesF;
  voltage = getVoltage(temperaturePin);
  degreesC = (voltage - 0.5) * 100.0;
  degreesF = degreesC * (9.0/5.0) + 32.0;
//  // These statements will print lines of data like this:
//  // "voltage: 0.73 deg C: 22.75 deg F: 72.96"
  delay(5000); // repeat once per second (change as you wish!)
////  float tempC = sensors.getTempCByIndex(0);
////  tempC = DallasTemperature::toFahrenheit(tempC);
  char buffer[10];
  String tempF = dtostrf(degreesF, 4, 1, buffer);
  updateTemp(tempF);
  delay(60000);

}

float getVoltage(int pin){
  return (analogRead(pin) * 0.004882814);
}
//
void updateTemp(String tenmpF){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  sendDebug(cmd);
  delay(2000);
  if(wifiport.find("Error")){
    Serial.print("RECEIVED: Error");
    return;
  }
  cmd = GET;
  cmd += tenmpF;
  cmd += "\r\n";
  wifiport.print("AT+CIPSEND=");
  wifiport.println(cmd.length());
  if(wifiport.find(">")){
    Serial.print(">");
    wifiport.print(cmd);
    Serial.print(cmd);
  }else{
    sendDebug("AT+CIPCLOSE");
  }
  if(wifiport.find("OK")){
    Serial.println("RECEIVED: OK");
  }else{
    Serial.println("RECEIVED: Error");
  }
}

void sendDebug(String cmd){
  wifiport.println(cmd);
  Serial.println("SEND: ");
  Serial.println(cmd);
} 
 
boolean connectWiFi(){
  wifiport.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendDebug(cmd);
  delay(5000);
  if(wifiport.find("OK")){
    Serial.println("RECEIVED: OK");
    return true;
  }else{
    Serial.println("RECEIVED: Error");
    return false;
  }
}
