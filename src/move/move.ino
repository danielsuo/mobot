#include <SoftwareSerial.h>
#include <Kangaroo.h>
#include "Mobot.h"

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
Mobot Mobot();

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
  
  checkError(__LINE__, Drive.start());
  checkError(__LINE__, Turn.start());
  
  Drive.si(0);
  Turn.si(0);
}

void loop()
{
  if (stringComplete) {
    printMsg("received command: ", inputString);

    switch(inputString[0]) {
      case 'd':
        printMsg("driving ", inputString.substring(1));
        monitorChannel(Drive, Drive.pi(inputString.substring(1).toInt()));
        break;
      case 't':
        printMsg("turning ", inputString.substring(1));
        monitorChannel(Turn, Turn.pi(inputString.substring(1).toInt()));
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

void monitorChannel(KangarooChannel &ch, KangarooMonitor monitor) {
//  int count = 0;
//  while (monitor.valid() && monitor.status().busy()) {
//    count++;
//    printMsg("update: ", "still working!");
//    printMsg("update # ", String(count));
//    delay(500);
//  }
  debugMonitor(monitor);
  while (ch.getp().value() < 500) {
    Serial.print(ch.getp().value());
    Serial.print(" ");
    delay(50);
  }
//  delay(2000);
//  if (monitor.status().done()) Serial.println("Done!");
//  else Serial.println(ch.getp().value());
}

void printMsg(String msg, String data) {
  Serial.print("ARDUINO: ");
  Serial.print(msg);
  Serial.print(data);
  
  if (!data.endsWith("\n") && !data.endsWith("\r")) {
    Serial.println();
  }
}

void checkError(const int line, KangarooError err) {
  if (err != KANGAROO_NO_ERROR) {
    Serial.print("ARDUINO: Found error: ");
    
    switch(err) {
      case KANGAROO_NO_ERROR:
        Serial.print("KANGAROO_NO_ERROR");
      break;
      case KANGAROO_NOT_STARTED:
        Serial.print("KANGAROO_NOT_STARTED");
      break;
      case KANGAROO_NOT_HOMED:
        Serial.print("KANGAROO_NOT_HOMED");
      break;
      case KANGAROO_CONTROL_ERROR:
        Serial.print("KANGAROO_CONTROL_ERROR");
      break;
      case KANGAROO_WRONG_MODE:
        Serial.print("KANGAROO_WRONG_MODE");
      break;
      case KANGAROO_UNRECOGNIZED_CODE:
        Serial.print("KANGAROO_UNRECOGNIZED_CODE");
      break;
      case KANGAROO_SERIAL_TIMEOUT:
        Serial.print("KANGAROO_SERIAL_TIMEOUT");
      break;
      case KANGAROO_INVALID_STATUS:
        Serial.print("KANGAROO_INVALID_STATUS");
      break;
      case KANGAROO_TIMED_OUT:
        Serial.print("KANGAROO_TIMED_OUT");
      break;
      case KANGAROO_PORT_NOT_OPEN:
        Serial.print("KANGAROO_PORT_NOT_OPEN");
      break;
    }
    
    Serial.print(" on line ");
    Serial.println(line);
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
}
