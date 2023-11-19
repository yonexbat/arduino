#include <Servo.h>
#include "WiFiS3.h"
#include "arduino_secrets.h" 

Servo servo;
Servo speed;
int servoPos = 1500;
int newServoPos = 1500;


int speedPosNeutral = 1580;
int speedPos = speedPosNeutral;

unsigned long servoLastTime;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  setupServoAndEsc();
  setupWifiAndWebServer();
}

// Servo and electronic speed controller.
void setupServoAndEsc()
{
  servo.attach(5); 
  speed.attach(3);
  servoLastTime = millis(); 
}

// Setup wifi
void setupWifiAndWebServer() {

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


// the loop function runs over and over again forever
void loop() {
  int command = readSerial();
  handleCommand(command);
  setServoPosition();
  wifiLoop();
}

void wifiLoop()
{
  WiFiClient client = server.available();

  if (client) {                            
    Serial.println("new client");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            printClientState(client);                   
            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        
        if (currentLine.endsWith("GET /a")) {
          handleCommand(97);
        }
        if (currentLine.endsWith("GET /b")) {
          handleCommand(98);
        }
        if (currentLine.endsWith("GET /c")) {
          handleCommand(99);
        }
        if (currentLine.endsWith("GET /d")) {
          handleCommand(100);
        }
        if (currentLine.endsWith("GET /e")) {
          handleCommand(101);
        }
        if (currentLine.endsWith("GET /f")) {
          handleCommand(102);
        }
      }
      
    }
    // close the connection:
    printClientState(client); 
    client.stop();
    Serial.println("client disconnected");
  } 
}

void printClientState(WiFiClient& client){
  client.print("<p>");
  client.print("Speed: ");
  client.print(speedPos, DEC);
  client.print("  ");
  client.print("servo: ");
  client.print(servoPos, DEC);
  client.print("</p>");        
  client.println();   
}

void handleCommand(int command)
{
  if(command >= 97 && command <= 110) 
  {
   Serial.print("Handling command: ");
   Serial.println(command, DEC);
  }

  switch(command)
  {
    //a
    case 97: 
      digitalWrite(LED_BUILTIN, LOW);
      newServoPos = 1500; 
      break;

    //b
    case 98:
      digitalWrite(LED_BUILTIN, HIGH);
      newServoPos = 1000;
      break;
    
    //c
    case 99:
      digitalWrite(LED_BUILTIN, HIGH);
      newServoPos = 1800;
      break;    

    //d
    case 100:
      speedPos += 10;
      speed.writeMicroseconds(speedPos);
      break;
    
    //e
    case 101:
      speedPos += -10;
      speed.writeMicroseconds(speedPos);
      break;

    //f
    case 102:
      speedPos = speedPosNeutral;
      speed.writeMicroseconds(speedPos);
      break;
  }
}

void setServoPosition()
{  
  unsigned long currentTime = millis();

  if((currentTime - servoLastTime) <= 100) {
    return;
  }

  if(newServoPos == servoPos)
  {
    return;
  }

  int diff = abs(newServoPos - servoPos);

  if(diff > 30)
  {
    if(newServoPos > servoPos)
    {
      servoPos += 30;
    } 
    else if (newServoPos < servoPos) {
      servoPos -= 30;   
    }
  }
  else {
    servoPos = newServoPos;
  }

  // prevent overflow
  if(servoPos > 2000) {
    servoPos = 2000;
  }
  if(servoPos < 1000)
  {
    servoPos = 1000;
  }

  servo.writeMicroseconds(servoPos);
  
  servoLastTime = millis();
}

int readSerial() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    Serial.print("Servo current: ");
    Serial.println(servoPos, DEC);
    Serial.print("Speed current: ");
    Serial.println(speedPos, DEC);
    return incomingByte;
  }
  return 0;  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
