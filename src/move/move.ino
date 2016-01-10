#include <SoftwareSerial.h>
#include <Kangaroo.h>
#include "Mobot.h"

// Arduino TX (pin 11) goes to Kangaroo S1
// Arduino RX (pin 10) goes to Kangaroo S2
// Arduino GND         goes to Kangaroo 0V
// Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
#define TX_PIN 11
#define RX_PIN 10

#define DEFAULT_SPEED_UNITS_PER_SECOND 1000

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
    printMsg("Received command: ", inputString);
    
    int magnitude = inputString.substring(1).toInt();

    switch(inputString[0]) {
      case 'd':
        printMsg("Driving ", String(magnitude));
        pi(Drive, magnitude);
        break;
      case 't':
        printMsg("Turning ", String(magnitude));
        pi(Turn, magnitude);
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

void pi(KangarooChannel &ch, int magnitude) {
  monitorChannel(ch, ch.pi(magnitude, DEFAULT_SPEED_UNITS_PER_SECOND, KANGAROO_MOVE_RAW_UNITS), magnitude);
}

void monitorChannel(KangarooChannel &ch, KangarooMonitor monitor, int magnitude) {
  debugMonitor(monitor);
  while (abs(magnitude - ch.getpi(KANGAROO_GET_RAW_UNITS).value()) > 5) {
    printMsg("m", String(int(ceil(ch.getpi(KANGAROO_GET_RAW_UNITS).value() / float(magnitude) * 100))));
    // delay(50);
  }
  printMsg("m", String(int(ceil(ch.getpi(KANGAROO_GET_RAW_UNITS).value() / float(magnitude) * 100))));
}

void printMsg(String msg, String data) {
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
  Serial.println(status.flags(), HEX);
  
  Serial.print("Monitor status type: ");
  Serial.println(status.type(), HEX);

  Serial.print("Monitor status value: ");
  Serial.println(status.value());

  Serial.print("Monitor status valid: ");
  Serial.println(status.valid());

  Serial.print("Monitor status error: ");
  Serial.println(status.error(), HEX);
}
