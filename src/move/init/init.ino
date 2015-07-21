#define ROBOT_NAME "Mobot"

#define SERIAL_BAUD 9600
#define BLUETOOTH_BAUD 57600

#include <SoftwareSerial.h>

// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
SoftwareSerial bluetoothSSP(10, 11); // RX, TX

/*
  The posible baudrates are:
    AT+BAUD1-------1200
    AT+BAUD2-------2400
    AT+BAUD3-------4800
    AT+BAUD4-------9600 - Default for hc-06
    AT+BAUD5------19200
    AT+BAUD6------38400
    AT+BAUD7------57600 - Firmata
    AT+BAUD8-----115200
    AT+BAUD9-----230400
    AT+BAUDA-----460800
    AT+BAUDB-----921600
    AT+BAUDC----1382400
*/

void setup()
{
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Starting config");
  bluetoothSSP.begin(BLUETOOTH_BAUD);
  delay(1000);

  // Should respond with OK
  bluetoothSSP.print("AT");
  waitForResponse();

  // Should respond with its version
  bluetoothSSP.print("AT+VERSION");
  waitForResponse();

  // Set pin to 0000
  bluetoothSSP.print("AT+PIN0000");
  waitForResponse();

  // Set the name to ROBOT_NAME
  bluetoothSSP.print("AT+NAME");
  bluetoothSSP.print(ROBOT_NAME);
  waitForResponse();

  // Set baudrate to 57600
  bluetoothSSP.print("AT+BAUD7");
  waitForResponse();

  Serial.println("Done!");
}

void waitForResponse() {
    delay(1000);
    while (bluetoothSSP.available()) {
      Serial.write(bluetoothSSP.read());
    }
    Serial.write("\n");
}

void loop() {}
