// Mixed Mode Sample for Kangaroo
// Copyright (c) 2013 Dimension Engineering LLC
// See license.txt for license details.
#include <SoftwareSerial.h>
#include <Kangaroo.h>
// Arduino TX (pin 11) goes to Kangaroo S1
// Arduino RX (pin 10) goes to Kangaroo S2
// Arduino GND         goes to Kangaroo 0V
// Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
#define TX_PIN 11
#define RX_PIN 10
// Mixed mode channels on Kangaroo are, by default, 'D' and 'T'.
SoftwareSerial  SerialPort(RX_PIN, TX_PIN);
KangarooSerial  K(SerialPort);
KangarooChannel Drive(K, 'D');
KangarooChannel Turn(K, 'T');

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup()
{
  SerialPort.begin(9600);
  SerialPort.listen();

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  
  Drive.start();
  Turn.start();

  Serial.write(SerialPort.read());
  
  Drive.si(0);
  Turn.si(0);
}
void loop()
{
  if (stringComplete) {
    Serial.println(inputString);

    switch(inputString[0]) {
      case 'd':
        Serial.println(inputString.substring(1));
        Drive.pi(inputString.substring(1).toInt());
        break;
      case 't':
        Serial.println(inputString.substring(1));
        Turn.pi(inputString.substring(1).toInt());
        break;
      default:
        Serial.println("Command not recognized!");
        break;
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void debugMonitor(KangarooMonitor monitor) {
  debugStatus(monitor.status());
}

void debugStatus(KangarooStatus status) {
  Serial.print("Monitoring channel: ");
  Serial.println(status.channel());
  
  Serial.print("Monitor status flags: ");
  Serial.println(status.flags());
  
  Serial.print("Monitor status type: ");
  Serial.println(status.type());

  Serial.print("Monitor status value: ");
  Serial.println(status.value());

  Serial.print("Monitor status valid: ");
  Serial.println(status.valid());

  Serial.print("Monitor status error: ");
  Serial.println(status.error());

  Serial.println();
}