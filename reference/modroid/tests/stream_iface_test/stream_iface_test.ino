#include "Timer.h"
#include <bqueue.h>
#include <stream_iface.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

// initialize queues and stream_iface ... TODO: I need to create proper init functions
bQueue out_q = bQueue();
bQueue in_q = bQueue();
StreamIface host_iface = StreamIface( &Serial, &in_q, &out_q );

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
const char* buf = "hello world\n\r";
uint8_t buf_len;

// Print hello_world timer
Timer t;

// the setup routine runs once when you press reset:
void setup() {
  // Initialize serial
  Serial.begin(115200);
  
  
  // Output string
  buf_len = strlen(buf);
  
   t.every(1000, write_string);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
    // Update timer
    t.update();
  
    // Write bytes from output queue
    host_iface.writeBytes();
}

void write_string() {
  // Write buf
  /*
  for ( uint8_t i=0; i<buf_len; i++ ) {
    out_q.enqueue(buf[i]);
  } 
 */
  out_q.enqueue((uint8_t*)buf, buf_len); 
  digitalWrite(led, !digitalRead(led));   // toggle LED
}
