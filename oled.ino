#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, 4);


void setup() {

display.begin(SSD1306_SWITCHCAPVCC,0x3C);

display.clearDisplay();

}

void loop() {
  
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
//display.drawLine(0, 0, 120, 32, WHITE);
display.drawRect(1, 1, 126, 62, WHITE);
display.setCursor(40,4);
display.println("Bejli");
display.display();
for(int i =1;i<30;i++){
 
kolko(i);
display.display();
display.clearDisplay();
}

delay(2000);
display.setCursor(10,16);

delay(3000);
display.setCursor(4,16);
display.println("nie wiem");
display.display();
delay(3000);
display.clearDisplay();
display.println("Bejli");
display.display();
delay(3000);
display.println("i");
  
display.println("Listek");
display.display();
delay(6000);

display.drawPixel(120, 30, WHITE);
display.display(); 

}


void kolko(int promien){
//void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
//void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
display.drawCircle(10, 10, promien, WHITE);
}
