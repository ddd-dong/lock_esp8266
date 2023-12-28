#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
const char* ssid     = "Pixel_6990";
const char* password = "asunaasuna"; 
ESP8266WebServer server(80);


#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN D8
#define RST_PIN D0
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
byte empty_byte;


#include <Servo.h>
#define SERVO_PIN D3
Servo myservo;


String rfid_message = "";

void send_id() {
  if(rfid_message!=""){
    Serial.println(rfid_message);
  }
  server.send(200, "text/plain", rfid_message);
  String message = server.arg("message");
  if(message != "False"){ 
    Serial.println(message);
    unlock_servo();
  }
  rfid_message="";
}

void unlock_servo(){
  myservo.write(0);  //旋轉到0度，就是一般所說的歸零
  delay(500);
  myservo.write(90); //旋轉到90度
  delay(500);
  myservo.write(0);  //旋轉到0度，就是一般所說的歸零
}


void setup() {
  myservo.attach(SERVO_PIN);
  
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());  

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();

  for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
  }
  Serial.println();
  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  server.on("/test", send_id);
  server.begin();
}

void loop() {
  server.handleClient();
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
      return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
      return;
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
          piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
          piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
  }
  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
          rfid.uid.uidByte[1] != nuidPICC[1] ||
          rfid.uid.uidByte[2] != nuidPICC[2] ||
          rfid.uid.uidByte[3] != nuidPICC[3] ) {
      Serial.println(F("A new card has been detected."));

      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
      }
      for (byte i = 0; i < 4; i++) {
        rfid_message += String(nuidPICC[i], HEX);
      }
      Serial.println(F("The NUID tag is:"));
      Serial.print(F("In hex: "));
      printHex(rfid.uid.uidByte, rfid.uid.size);
      Serial.println();
      Serial.print(F("In dec: "));
      printDec(rfid.uid.uidByte, rfid.uid.size);
      Serial.println();
  }
  else {
    Serial.println(F("Card read previously."));
    memset(nuidPICC, empty_byte, sizeof nuidPICC);
    Serial.println("reset");
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}





void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
  }
}

/**
    Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], DEC);
  }
}
