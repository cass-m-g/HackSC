#include <b64.h>
#include <HttpClient.h>



#include <Wire.h>
#include <rgb_lcd.h>


/*
  Web client
 
 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 This example is written for a network using WPA encryption. For 
 WEP or WPA, change the Wifi.begin() call accordingly.
 
 Circuit:
 * WiFi shield attached
 
 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
 
#include <SPI.h>
#include <WiFi.h>

char ssid[] = "NETGEAR"; //  your network SSID (name) 
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char to_server[] = "hgaquaponics.azurewebsites.net";    // name address for Google (using DNS)




char from_server[] = "hgaquaponics.azurewebsites.net";    // name address for Google (using DNS)
const int NUMDATA = 1;
int data[NUMDATA];
//*********needs to be defined
unsigned char something_wrong = 0;
  int fishFed = 1;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
WiFiClient client2;
  HttpClient http(client2);
const int pinTemp = A0;      // pin of temperature sensor

float temperature_cel, temperature_fer;
int B=3975;                  // B value of the thermistor
float resistance;

int colorR = 0;
int colorG = 0;
int colorB = 0;
rgb_lcd lcd;
void setup() {
    lcd.begin(16, 2);
    colorB = 100;
    lcd.setRGB(colorR, colorG, colorB);
    lcd.print("Working.");
    delay(100);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 

  String fv = WiFi.firmwareVersion();
  if( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    lcd.clear();
    lcd.print("Connecting to ");
    lcd.setCursor(0, 1);
    lcd.print("network...");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
    lcd.clear();
  lcd.print("Connected to ");
  lcd.setCursor(0, 1);
  lcd.print("wifi.");
  printWifiStatus();
  
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  
  
    pinMode(7, OUTPUT);  
    pinMode(5, INPUT);    
    pinMode(8, INPUT);
    
  /*if (client2.connect(from_server, 80))
  {
    // Make a HTTP request:
    client2.print("GET ");
    client2.print("/latest.txt");
    client2.println(" HTTP/1.1");
    client2.print("Host: ");
    client2.println(from_server);
      client2.println( "Content-Type: text/html" );
      client2.println( "Connection: close" );

    client2.println();
    
  }*/
 
  
}

int lcdcnt = 0;
int wrong_prev = 1;

void displayErrorMsg(){
  if(wrong_prev != something_wrong){
 switch(something_wrong){
    case 0:
     //display nothing wrong
     colorR = 0; colorG = 0; colorB = 0;
     lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
     lcd.print("Working.");
    break;
    case 1:
     colorR = 100; colorG = 0; colorB = 0;
     lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
     lcd.print("Needs water.");
     break;
    case 2:
     colorR = 100; colorG = 0; colorB = 0;
     lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
     lcd.print("Need to feed");
     lcd.setCursor(0,1);
     lcd.print(" fish.");
     break;
    case 3:
     colorR = 100; colorG = 0; colorB = 0;
     lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
     lcd.print("Needs water and need");
     lcd.setCursor(0, 1);
     lcd.print("to feed fish.");
     break;
     default:
     //display nothing wrong
     colorR = 0; colorG = 0; colorB = 0;
     lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
     lcd.print("Working.");
    
 } 
  }
}

int water_full = 1;

void sendData(){
    if(!fishFed && digitalRead(8))
      fishFed = 1;
      wrong_prev = something_wrong;
    if(!water_full && !fishFed){
      //Serial.println("testing");
      something_wrong = 3;
    }
    else if(!water_full){
      something_wrong = 1;  //water is empty
      //Serial.println("something wrong ");
    }
    else if(!fishFed){
      something_wrong = 2;
     //Serial.println("something wrong : feed fish");
    }
    else
      something_wrong = 0;
  
    int val = analogRead(pinTemp);                               // get analog value
    resistance=(float)(1023-val)*10000/val;                      // get resistance
    temperature_cel=1/(log(resistance/10000)/B+1/298.15)-273.15;     // calc temperature
    temperature_fer = (temperature_cel*(9/5)) + 32;
  
      //Serial.println("connected");
    if (client.connect(to_server, 80)) {
      Serial.println("connected");
      client.print( "GET /Input.php?");
      client.print("tempF=");
      client.print(temperature_fer);
      client.print("&");
      client.print("tempC=");
      client.print(temperature_cel);
      client.print("&");
      client.print("waterLevel=");
      client.print(water_full);
      client.print("&");
      client.print("fishFed=");
      client.print(fishFed);
      client.println( " HTTP/1.1");
      client.print( "Host: " );
      client.println( to_server );
      client.println( "Content-Type: application/x-www-form-urlencoded" );
      client.println( "Connection: close" );
      client.println();
      client.println();
      client.stop();
  }
}
/*
void recieveData(){
    int temp = 0;
     if (client2.available()) {
        char c = client2.read();
        
        Serial.write(c);
    }
    if (!client2.available() && !client2.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client2.stop();
  }
}*/

unsigned char ledon = 0;

void displayErrorLight(){
  if(something_wrong && !ledon){
    digitalWrite(7, HIGH);
    ledon = 1;
  }
  else{
    digitalWrite(7, LOW);
    ledon = 0;
  }
}
  

unsigned long int ledcnt = 0;
int fullcnt = 0;

void loop() {
  
  
  http.get(from_server, "/fishFed.txt");
  http.skipResponseHeaders(); 
  //read sensors //set to array var
  if(digitalRead(5)){
    fullcnt = 0;
    water_full = 1;
  }
  else if(fullcnt>=10){
    fullcnt = 0;
    water_full = 0;
  }
  else
    fullcnt = fullcnt + 1;
  
  sendData();
  //recieveData();
  //if(http.available()){
  char c = http.read();
  fishFed = c - 48;
  Serial.print(c);
  http.stop();
  //}
    displayErrorMsg();
  if(ledcnt >= 5){
    displayErrorLight();
    ledcnt = 0;
  }
    ledcnt= ledcnt +1;
    delay(10);
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

