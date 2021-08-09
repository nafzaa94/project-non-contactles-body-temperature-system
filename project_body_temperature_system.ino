#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <Adafruit_MLX90614.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

int var = 0;

int state = 0;

// pin driver motor
int in1 = 2;
int in2 = 3;

// ir sensor for stop motor
int irsensor = 4;
int irsensorvalue = 0;

// ultrasonic sensor for detact human
#define echoPin 2 
#define trigPin 3

long duration; 
int distance;

float data[5];
float dataTemp;
 
void setup() 
{
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mlx.begin();
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(irsensor, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.setCursor(2, 0);
  lcd.print("TEMPERATURE");
  lcd.setCursor(0, 1);
  lcd.print("= ");

}
void loop() 
{

  switch (var) {
  case 0:
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "BD 31 15 2B") //change here the UID of the card/cards that you want to give access
  {
    var = 1;
  }
 
 else   {
    Serial.println(" Access denied");
    delay(3000);
  }
    break;
  case 1:
    // running motor
    if (state == 0){
       digitalWrite(in1, HIGH);
       digitalWrite(in2, LOW);
       state = 1;
      }
  
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    if (distance <= 10){
      var = 2;
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      }
    break;
  case 2:
     digitalWrite(trigPin, LOW);
     delayMicroseconds(2);
     digitalWrite(trigPin, HIGH);
     delayMicroseconds(10);
     digitalWrite(trigPin, LOW);
     duration = pulseIn(echoPin, HIGH);
     distance = duration * 0.034 / 2;

     if ( distance <= 10){
      for(int i = 0; i < 5; i++){
        data[i] = mlx.readObjectTempC();
        }

      dataTemp = (data[0] + data[1] + data[2] + data[3] + data[4])/ 5;
      }
     
     lcd.setCursor(2, 1);
     lcd.print(dataTemp);
     lcd.setCursor(2, 1);
     lcd.print("      ");
     var = 3;
     state = 0;
    break;
  case 3:
     if (state == 0){
       digitalWrite(in1, LOW);
       digitalWrite(in2, HIGH);
       state = 1;
      }
    irsensorvalue = digitalRead (irsensor);

    if (irsensorvalue == LOW){
       digitalWrite(in1, LOW);
       digitalWrite(in2, LOW);
       var = 4;
      }
    break;
}
  // Look for new cards
  
} 
