#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
Adafruit_SSD1306 display(OLED_RESET);

#define DRAW_DELAY 118
#define D_NUM 47

void setup()   {

	// initialize with the I2C addr 0x3C / mit I2C-Adresse 0x3c initialisieren
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

	// random start seed / zufälligen Startwert für Random-Funtionen initialisieren
	randomSeed(analogRead(0));
}


int i;

void examples() {
		// invert the display / Display invertieren
	display.invertDisplay(true);
	delay(2000);
	display.invertDisplay(false);
	delay(1000);

	// draw some random pixel / zufaellige Pixel anzeigen
	for(i=0;i<380;i++){
		display.drawPixel(random(128),random(64), WHITE);
		display.display();
	}
	delay(DRAW_DELAY);
	display.clearDisplay();

	// draw some random lines / zufaellige Linien anzeigen
	for(i=0;i<D_NUM;i++){
		display.drawLine(random(128),random(64),random(128),random(64), WHITE);
		display.display();
		delay(DRAW_DELAY);
		display.clearDisplay();
	}

	// draw some random triangles / zufaellige Dreiecke anzeigen
	for(i=0;i<D_NUM;i++){
		if(random(2))display.drawTriangle(random(128),random(64),random(128),random(64), random(128),random(64), WHITE); // normal
		else display.fillTriangle(random(128),random(64),random(128),random(64), random(128),random(64), WHITE);		 // filled / ausgefuellt
		display.display();
		delay(DRAW_DELAY);
		display.clearDisplay();
	}

	// draw some random rectangles / zufaellige Rechtecke anzeigen
	for(i=0;i<D_NUM;i++){
		int rnd=random(4);
		if(rnd==0)display.drawRect(random(88),random(44),random(40),random(20), WHITE); 							// normal
		else if(rnd==1)display.fillRect(random(88),random(44),random(40),random(20), WHITE);						// filled / ausgefuellt
		else if(rnd==2)display.drawRoundRect(random(88),random(44),random(30)+10,random(15)+5,random(5), WHITE);	// normal with rounded edges / normal mit abgerundeten Ecken
		else display.fillRoundRect(random(88),random(44),random(30)+10,random(15)+5,random(5), WHITE);				// filled with rounded edges / ausgefuellt mit  mit abgerundeten Ecken
		display.display();
		delay(DRAW_DELAY);
		display.clearDisplay();
	}

	// draw some random circles / zufaellige Kreise anzeigen
	for(i=0;i<D_NUM;i++){
		if(random(2))display.drawCircle(random(88)+20,random(44)+20,random(10), WHITE);	// normal
		else display.fillCircle(random(88)+20,random(44)+20,random(10), WHITE);			// filled / ausgefuellt
		display.display();
		delay(DRAW_DELAY);
		display.clearDisplay();
	}

}

unsigned long lastAnimationFrame;
unsigned long nextAnimationFrame;

void loop() {

	display.clearDisplay();

	display.setTextColor(WHITE);

	display.setTextSize(2);
	display.setCursor(0,0);
	display.print("12:34 43:21");

	display.setTextSize(1);
	display.setCursor(10,16);
	display.println("> 12:12,1");
	display.println("  34:34,3");

	// int rssiA = 79;
	// int height_A = map(rssiA, 0, 100, 0, 32);
	// display.drawRect(0, 0, 5, 32 - height_A, WHITE);
	// display.fillRect(0, 32 - height_A, 5, height_A, WHITE);

	// int rssi_B = 60;
	// int height_B = map(rssi_B, 0, 100, 0, 32);
	// display.drawRect(123, 0, 5, 32 - height_B, WHITE);
	// display.fillRect(123, 32 - height_B, 5, height_B, WHITE);


	display.display();
	delay(8000);
}
