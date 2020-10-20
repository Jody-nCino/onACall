#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#define BUZZER_PIN  D2

String marqueeMessage = "";
// Display Settings
// CLK -> D5 (SCK)
// CS  -> D6
// DIN -> D7 (MOSI)
const int pinCS = D6; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int displayIntensity = 4;  //(This can be set from 0 - 15)
const int numberOfHorizontalDisplays = 4; // default 4 for standard 4 x 1 display Max size of 16
const int numberOfVerticalDisplays = 1; // default 1 for a single row height
/* set ledRotation for LED Display panels (3 is default)
0: no rotation
1: 90 degrees clockwise
2: 180 degrees
3: 90 degrees counter clockwise (default)
*/
int ledRotation = 3;
int displayScrollSpeed = 15; // In milliseconds -- Configurable by the web UI (slow = 35, normal = 25, fast = 15, very fast = 5)

// LED Settings
const int offset = 1;
int refresh = 0;
int spacer = 1;  // dots between letters
int width = 5 + spacer; // The font width is 5 pixels + spacer
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);



ESP8266WebServer server;
char* ssid = "networkName";
char* password = "networkPassword";
// 192.168.86.78

void setup() {
	WiFi.begin(ssid, password);
	Serial.begin(115200);

	// Initialize digital pin for LED
	pinMode(LED_BUILTIN, OUTPUT);

	// New Line to clear from start garbage
	Serial.println();

	// initialize display
	matrix.setIntensity(0);

	while(WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(500);
	}

	Serial.println(WiFi.localIP());

	server.on("/", home);
	server.on("/onACall", onACall);
	server.on("/offACall", offACall);

	server.begin();

	int maxPos = numberOfHorizontalDisplays * numberOfVerticalDisplays;
	for (int i = 0; i < maxPos; i++) {
		matrix.setRotation(i, ledRotation);
		matrix.setPosition(i, maxPos - i - 1, 0);
	}

	Serial.println("matrix created");
	matrix.fillScreen(LOW); // show black
	scrollMessage(WiFi.localIP().toString());

	for (int inx = 0; inx <= 15; inx++) {
		matrix.setIntensity(inx);
		delay(100);
	}
	for (int inx = 15; inx >= 0; inx--) {
		matrix.setIntensity(inx);
		delay(60);
	}
	delay(1000);
	matrix.setIntensity(displayIntensity);
}

void loop() {
	server.handleClient();
	matrix.fillScreen(LOW); // show black

	if (marqueeMessage != "") {
		scrollMessage(marqueeMessage);
	} else {
		centerPrint("hi");
	}
}

void home() {
	server.send(200, "text/plain", "Velcro is such a rip off");
	Serial.println();
	centerPrint("howdy");
	delay(5000);
}

void onACall() {
	server.send(200);
	marqueeMessage = "On A Call...";
}

void offACall() {
	server.send(200);
	centerPrint("done");
	delay(5000);
	marqueeMessage = "";
}


void centerPrint(String msg) {
  int x = (matrix.width() - (msg.length() * width)) / 2;

  matrix.setCursor(0, 0);
  matrix.print(msg);

  matrix.write();
}

void scrollMessage(String msg) {
  msg += " "; // add a space at the end
  for ( int i = 0 ; i < width * msg.length() + matrix.width() - 1 - spacer; i++ ) {

    if (refresh == 1) i = 0;
    refresh = 0;
    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < msg.length() ) {
        matrix.drawChar(x, y, msg[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display
    delay(displayScrollSpeed);
  }
  matrix.setCursor(0, 0);
}