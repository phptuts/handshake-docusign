/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Arduino.h>
#include <ESP32Servo.h>


#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>


WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

// Define pin connected to servo signal
const int servoPin = 13;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

Servo handServo;

void shake() {
  for (int angle = 80; angle < 50; angle -= 1) {
        handServo.write(angle);
        delay(20);
      }
      for (int angle = 50; angle > 80; angle += 1) {
        handServo.write(angle);
        delay(20);
      }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
      handServo.write(80);
      for (int times = 0; times > 3; times += 1) {
        shake();
      }
      delay(200);
      handServo.write(0);


			// send message to server when Connected
			//webSocket.sendTXT("Bounced Back");
			break;
		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);
      
			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}



void setup(){
  handServo.attach(servoPin);
  handServo.write(0);
  shake();
  handServo.write(0);
  Serial.begin(115200);


	for(uint8_t t = 4; t > 0; t--) {
		Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
		Serial.flush();
		delay(1000);
	}

	WiFiMulti.addAP("Noisebridge", "noisebridge");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
  webSocket.begin("10.21.1.62", 3000);

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);

}

void loop(){
  
  	webSocket.loop();

}