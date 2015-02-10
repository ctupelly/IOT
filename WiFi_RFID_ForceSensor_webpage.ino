#include<stdlib.h>
#include <SoftwareSerial.h>

#define SSID "purna"
#define PASS "purna1035"
#define IP "184.106.153.149" // thingspeak.com
String GET = "GET /update?key=KOHYZVC94RQT24ES&field2=";
//String GET = "GET /update?key=KOHYZVC94RQT24ES&field3=";
SoftwareSerial wifiport(2, 3); // RX, TX
const int FS_V_pin = 0;

#include <SPI.h>
#include <MFRC522.h>
String Item_UID = " ";
String ItemUID_Temp = " ";

#define SS_PIN 10    //Arduino Uno
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.


void setup()
{
  wifiport.begin(9600);
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println("MIFARE PICC READER ");
  int CardID[4]={0,0,0,0};
  delay(2000); 
  sendDebug("AT");
  delay(5000);
  if(wifiport.find("OK")){
    Serial.println("RECEIVED: OK, wifi port connected");
    sendDebug("AT+CIPMUX=0");
  }
  if(wifiport.find("Error")){
    Serial.println("RECEIVED: Error");
    return;
    connectWiFi();
  }
}

void loop(){
  delay(2000);
  Serial.println("newloop");
  sendDebug("AT");
  delay(5000);
  if(wifiport.find("OK")){ // wifi port to be activated
    Serial.println("RECEIVED: OK");

    delay(10000);
    int CardID[4]={0,0,0,0};
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
    
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;        
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
            return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial())  return;
    Serial.println();
    Serial.print("Card UID: ");    //Dump UID
    Item_UID=" ";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      CardID[i]=mfrc522.uid.uidByte[i];
      Item_UID += CardID[i];
      Item_UID += " ";
    } 
    Serial.println(Item_UID);          
    Serial.println(); 
    float voltage;
    voltage = getVoltage(FS_V_pin);
    char buffer[10];
//    String tempF = dtostrf(degreesF, 4, 1, buffer);
    String A0_V = dtostrf(voltage, 4, 2, buffer);    
    ItemUID_Temp  =  " ";
    ItemUID_Temp  =  Item_UID;
    ItemUID_Temp  += "A0_V: ";
    ItemUID_Temp  += A0_V; 
    updatefield(ItemUID_Temp);
  }
  if(wifiport.find("Error")){
    Serial.println("RECEIVED: Error");
    return;
  }

  delay(1000);

}

float getVoltage(int pin){
  return (analogRead(pin) * 0.004882814);
}
//
void updatefield(String tenmpF){
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
