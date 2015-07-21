#include <Kangaroo.h>
#include <AltSoftSerial.h>

// AltSoftSerial default pins for Arduino Uno
// Pin 9 (TX) -> Bluetooth RX
// Pin 8 (RX) -> Bluetooth TX
AltSoftSerial SerialPort;

// Pin 1 (TX) -> Kangaroo S1
// Pin 0 (RX) -> Kangaroo S2
KangarooSerial KangarooController(Serial);

KangarooChannel K1(KangarooController, '1', 135);
KangarooChannel K2(KangarooController, '2', 135);

void setup() {
  // Start Kangaroo serial
  Serial.begin(115200);
  SerialPort.begin(57600);

  delay(1000);

  K1.streaming(true);
  SerialPort.print(K1.start());

  KangarooMonitor k1_home = K1.home().wait();
  debugMonitor(k1_home);

  K2.streaming(true);
  SerialPort.print(K2.start());
  KangarooMonitor k2_home = K2.home().wait();
  debugMonitor(k2_home);

  debugMonitor(K1.s(100));
  
}

void debugMonitor(KangarooMonitor monitor) {
  KangarooStatus status = monitor.status();

  SerialPort.print("Monitoring channel: ");
  SerialPort.println(monitor.name());
  
  SerialPort.print("Monitor status flags: ");
  SerialPort.println(status.flags());
  
  SerialPort.print("Monitor status type: ");
  SerialPort.println(status.type());

  SerialPort.print("Monitor status value: ");
  SerialPort.println(status.value());

  SerialPort.println();
}

void loop() {
  if (SerialPort.available()) {
    char incoming = SerialPort.read();
    Serial.print(incoming);
  }

  if (Serial.available()) {
    SerialPort.print(Serial.read());
  }


  // long minimum = K1.getMin().value();
  // K1.p(minimum).wait();
  
  // delay(250);
  
  // long maximum = K1.getMax().value();
  // K1.p(maximum).wait();

  // delay(250);
  // K1.pi(500).wait();
  // delay(1000);
}
// #define ROBOT_NAME "MOBOT"

// // If you haven't configured your device before use this
// #define BLUETOOTH_SPEED 57600
// // If you are modifying your existing configuration, use this:
// // #define BLUETOOTH_SPEED 57600

// #include <SoftwareSerial.h>

// // Swap RX/TX connections on bluetooth chip
// //   Pin 10 --> Bluetooth TX
// //   Pin 11 --> Bluetooth RX
// SoftwareSerial mySerial(10, 11); // RX, TX


// /*
//   The posible baudrates are:
//     AT+BAUD1-------1200
//     AT+BAUD2-------2400
//     AT+BAUD3-------4800
//     AT+BAUD4-------9600 - Default for hc-06
//     AT+BAUD5------19200
//     AT+BAUD6------38400
//     AT+BAUD7------57600 - Johnny-five speed
//     AT+BAUD8-----115200
//     AT+BAUD9-----230400
//     AT+BAUDA-----460800
//     AT+BAUDB-----921600
//     AT+BAUDC----1382400
// */


// void setup()
// {
//   Serial.begin(9600);
//   while (!Serial) {
//     ; // wait for serial port to connect. Needed for Leonardo only
//   }
//   Serial.println("Starting config");
//   mySerial.begin(BLUETOOTH_SPEED);
//   delay(1000);

//   // Should respond with OK
//   mySerial.print("AT");
//   waitForResponse();

//   // Should respond with its version
//   mySerial.print("AT+VERSION");
//   waitForResponse();

//   // Set pin to 0000
//   mySerial.print("AT+PIN0000");
//   waitForResponse();

//   // Set the name to ROBOT_NAME
//   mySerial.print("AT+NAME");
//   mySerial.print(ROBOT_NAME);
//   waitForResponse();

//   // Set baudrate to 57600
//   mySerial.print("AT+BAUD7");
//   waitForResponse();

//   Serial.println("Done!");
// }

// void waitForResponse() {
//   delay(1000);
//   while (mySerial.available()) {
//     Serial.write(mySerial.read());
//   }
//   Serial.write("\n");
// }

// void loop() {}
// Movement Sample for Kangaroo
// Copyright (c) 2013 Dimension Engineering LLC
// See license.txt for license details.

// #include <SoftwareSerial.h>
// #include <Kangaroo.h>

// // Arduino TX (pin 11) goes to Kangaroo S1
// // Arduino RX (pin 10) goes to Kangaroo S2
// // Arduino GND         goes to Kangaroo 0V
// // Arduino 5V          goes to Kangaroo 5V (OPTIONAL, if you want Kangaroo to power the Arduino)
// #define TX_PIN 9
// #define RX_PIN 8

// // Independent mode channels on Kangaroo are, by default, '1' and '2'.
// SoftwareSerial  K_SERIAL(RX_PIN, TX_PIN);
// KangarooSerial  K(K_SERIAL);
// KangarooChannel K1(K, '1');
// KangarooChannel K2(K, '2');

// void setup()
// {
//   Serial.begin(9600);
//   Serial.println("Starting!");

//   K_SERIAL.begin(9600);
//   K_SERIAL.listen();
//   Serial.println("Listening on K_SERIAL!");

//   Serial.println("Starting on K_SERIAL!");
//   K_SERIAL.println("1,start");
//   Serial.println("units on K_SERIAL!");
//   K_SERIAL.println("1,units 1000 thousandths = 1024 lines");
//   Serial.println("Homing on K_SERIAL!");
//   K_SERIAL.println("1,home");
//   Serial.println("Getting pos on K_SERIAL!");
//   K_SERIAL.println("1,getp");
//   waitForResponse();

//   // delay(2000);

//   // K1.start();
//   // Serial.println("Kangaroo started!");

//   // K1.home().wait();
// }

// void waitForResponse() {
//   delay(1000);
//   while (K_SERIAL.available()) {
//     Serial.write(K_SERIAL.read());
//   }
//   Serial.write("\n");
// }

// // .wait() waits until the command is 'finished'. For position, this means it is within the deadband
// // distance from the requested position. You can also call K1.p(position); without .wait() if you want to command it
// // but not wait until it gets to the destination. If you do this, you may want to use K1.getP().value()
// // to check progress.
// void loop()
// {
//   if (Serial.available()) {
//     char byte = Serial.read();
//     Serial.write(byte);
//     K_SERIAL.write(byte);
//   }
//   if (K_SERIAL.available()) {
//     Serial.write(K_SERIAL.read());
//   }
//   // Go to the minimum side at whatever speed limit is set on the potentiometers.
//   // long minimum = K1.getMin().value();
//   // K1.p(minimum).wait();
//   // Serial.println("Starting kangaroo!");
//   // K1.start();
//   // Serial.println("Kangaroo started!");
//   // delay(2000);
  
//   // // Going to the maximum side, limit speed to 1/10th of the range per second
//   // // (at least 10 seconds of travel time).
//   // long maximum = K1.getMax().value();
//   // long speedLimit = (maximum - minimum) / 10;
//   // K1.p(maximum, speedLimit).wait();

//   // delay(2000);
// }
