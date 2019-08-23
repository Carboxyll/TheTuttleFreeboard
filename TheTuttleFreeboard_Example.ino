/*
 * Arduino Freeboard Post base on Arduino Repeating Wifi Web Client

 This sketch connects to a a web server and makes a POST request
 using a WiFi equipped Arduino board.

 http://www.arduino.cc/en/Tutorial/WifiWebClientRepeating

 */

#include <SPI.h>
#include <WiFiNINA.h>

int analogVal = 0;
int val = 0;
float HAASstatus = 0; //HAASoff

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

// Freeboard server address:
char server[] = "www.dweet.io";

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2000; // delay between updates, in milliseconds

void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the Nina module
  client.stop();

 // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connected");
   
    
// code for psi 
int sensorVal=analogRead(A0);


Serial.println(sensorVal);

float voltage = (sensorVal*5.0)/1024.0;
Serial.print("Volts: "); //Serial prints...nothing posted to Freeboard
Serial.print(voltage);

float pressure_psi = (analogRead(A0) * .67415730)-70; // Airpressure Sensor offset
float bar = pressure_psi * 0.06894;
Serial.print(" PSI: "); //Serial prints...nothing posted to Freeboard
Serial.print(pressure_psi); 
Serial.print(" Bar: "); //Serial prints...nothing posted to Freeboard
Serial.print(bar);


// Lets check if HAAS CNC is online
// If 
val = analogRead(A0);
Serial.println("serial:");

if (val > 50 ) {
  HAASstatus = 1; //HAASon
  Serial.println("ON");
}
else
{
  HAASstatus = 0; //HAASoff
    Serial.println("Off");
}
// End HAAS Check

String s = "POST /dweet/for/TheTuttle?HAASpsi=";
s.concat(pressure_psi);
s.concat("&HaasStatus="); // If you just have sensor remove this
s.concat(HAASstatus); // If you just have sensor remove this 
Serial.println(s);
client.println(s);

    
//end code psi
    
    client.println("Host: www.dweet.io");
    client.println("Connection: close");
    client.println();
    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
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
}
