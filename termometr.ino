/***************************************************************
 Insturciton:
 Connection:
 1.Plugable Terminal Sensor Adapter & Waterproof DS18B20 Digital Temperature Sensor
                 A   ---->     Blue(DATA SIGNAL)
                 B   ---->     RED   (VIN)
                 C   ---->     Black (GND)

 2.Waterproof DS18B20 Digital Temperature Sensor & Arduino board
              1(A)   ---->     Digital Pin2
              2(B)   ---->     5V/3.3V
              3(C)   ---->     GND

 Setting for the Pull-up Register/Pull-down Register Selection Jumpers
     When connect DS18B20 with the adapter,please choose to use the
     Pull-up Register Jumper
 ***************************************************************/
#include  <ESP8266WiFi.h>
#include <WiFiManager.h> 
#include <OneWire.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid     ="AutoConnectAP"; // Tu wpisz nazwę swojego wifi
const char* password = "password"; // Tu wpisz hasło do swojego wifi

// Pin Definitions
int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2
WiFiServer server(80);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float temperature = 0;


void setup(void) {
  Serial.begin(9600);    
  
  WiFi.mode(WIFI_STA); 
  WiFiManager wm;
  bool res = wm.autoConnect(ssid, password); // password protected ap

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");


    Serial.print("Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    display.begin(SSD1306_SWITCHCAPVCC,0x3C);
    display.clearDisplay();
  }
   
  // Start the server
  server.begin();
  Serial.println("Server started");
   
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/"); 
  
}

void loop(void) {
  
  temperature = getTemp();
  Serial.print(F("DS18S20: ")); Serial.println(temperature);
  oled();
  delay(2000); //just here to slow down the output so it is easier to read

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  int timewate = 0;
  while(!client.available()){
    delay(1);
    timewate = timewate +1;
    if(timewate>1800)
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  String request = parseRequest(client);

  // Return the response
  homePage(client);

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  
}


float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      Serial.println("no more sensors on chain, reset search!");
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}

String parseRequest(WiFiClient client){
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  return request;
}

void homePage(WiFiClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("");
  client.println("");
  
  String localIp = WiFi.localIP().toString();
 
  client.println(temperature);
}



void oled(){
  
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE); 
display.drawRect(1, 1, 126, 62, WHITE);
display.setCursor(10,8);
display.println("Temp");
display.setCursor(40,25);
display.setTextSize(2);
display.println(temperature);
display.display();
  
  }
