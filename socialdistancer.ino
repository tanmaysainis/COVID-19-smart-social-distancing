#include <LiquidCrystal.h>
#include<SoftwareSerial.h>
int Contrast=125;
LiquidCrystal lcd(12,11,5,4,3,2);
#include <Servo.h>

#define     RX    10
#define     TX    13
#define ULTRASONIC_TRIG_PIN     9   // pin TRIG 
#define ULTRASONIC_ECHO_PIN     8   // pin ECHO 
int red=A4;
int green=A3;
int yellow=A5;

int val;
Servo myservo;
float dist;

//asksensors
SoftwareSerial AT(RX,TX); 

String ssid     = "*******"; //Wifi SSID
String password = "******"; //Wifi Password
String apiKeyIn = "**************"; // API Key
const unsigned int writeInterval = 25000; // write interval (in ms)

// ASKSENSORS API host config
String host = "api.thingspeak.com";  // API host name
String port = "80";      // port

int AT_cmd_time; 
boolean AT_cmd_result = false; 

//

void setup ()
{
//asksensors set up
  Serial.begin(9600);
  // open serial 
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect Ultrasonic HC-SR04 + Arduino WiFi to Thingspeak");
  AT.begin(115200);
  Serial.println("Initiate AT commands with ESP8266");
  sendATcmd("AT",5,"OK");
  sendATcmd("AT+CWMODE=1",5,"OK");
  Serial.print("Connecting to WiFi:");
  Serial.println(ssid);
  sendATcmd("AT+CWJAP=\""+ ssid +"\",\""+ password +"\"",20,"OK");

//pinmode
pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
pinMode(ULTRASONIC_ECHO_PIN, INPUT);
pinMode(red, OUTPUT);
pinMode(green, OUTPUT);

//lcd setup
analogWrite(6,Contrast);
lcd.begin(16,2);
lcd.clear(); 
lcd.setCursor(0,0);;
lcd.print("POWER ON");
delay(1000);
lcd.setCursor(0,1);
lcd.print("SOCIAL DISTANCER");
delay(1000);
delay(3000);

myservo.attach(7);   /*attaches myservo object to pin 7*/

}


void loop()

{
 // measure distance
  long duration, distance;
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  dist=distance*0.032848;

  Serial.print("********** Ultrasonic Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

 // Create the URL for the request
  String url = "GET /write/";
  url += apiKeyIn;
  url += "?module1=";
  url += distance;
  
  Serial.println("*****************************************************");
  Serial.println("********** Open TCP connection");
  sendATcmd("AT+CIPMUX=1", 10, "OK");
  sendATcmd("AT+CIPSTART=0, \"TCP\",\"" + host +"\"," + port, 20, "OK");
  sendATcmd("AT+CIPSEND=0," + String(url.length() + 4), 10, ">");
  
  Serial.print("********** requesting URL: ");
  Serial.println(url);
  AT.println(url);
  delay(2000);
  sendATcmd("AT+CIPCLOSE=0", 10, "OK");
  
  Serial.println("********** Close TCP Connection ");
  Serial.println("*****************************************************");
  
  delay(writeInterval);   // delay
     
    
  if(distance<180)
  {
  digitalWrite(red,HIGH);
  digitalWrite(green,LOW);
  lcd.begin(16,2);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("TOO CLOSE!");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("DISTANCE:");
  lcd.print(dist);
  lcd.print("ft");
  delay(1000);
  delay(1000);
   val = map(distance, 0, 360, 0, 180);     /* scale it to use it with the servo (value between 0 and 180) */
   myservo.write(val);                  /* sets the servo position according to the scaled value of distance */
   delay(2000);;
}
 
else if(distance>180){
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);
  lcd.begin(16,2);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("MAINTAIN 6ft");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("DISTANCE:");
  lcd.print(dist);
  lcd.print("ft");
  delay(1000);
  delay(1000);
   val = map(distance, 0, 360, 0, 180);     /* scale it to use it with the servo (value between 0 and 180) */
   myservo.write(val);                  /* sets the servo position according to the scaled value of distance */
   delay(2000);;
}
}

// sendATcmd
void sendATcmd(String AT_cmd, int AT_cmd_maxTime, char readReplay[]) {
  Serial.print("AT command:");
  Serial.println(AT_cmd);

  while(AT_cmd_time < (AT_cmd_maxTime)) {
    AT.println(AT_cmd);
    if(AT.find(readReplay)) {
      AT_cmd_result = true;
      break;
    }
  
    AT_cmd_time++;
  }
  Serial.print("...Result:");
  if(AT_cmd_result == true) {
    Serial.println("DONE");
    AT_cmd_time = 0;
  }
  
  if(AT_cmd_result == false) {
    Serial.println("FAILED");
    AT_cmd_time = 0;
  }
  
  AT_cmd_result = false;
}
