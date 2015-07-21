#include <Bridge.h>

char D13char[2];
int D13int = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  
  digitalWrite(13,LOW);
  Bridge.begin();
  digitalWrite(13,HIGH);
  
  delay(1000);
  
  digitalWrite(13,LOW);
  Console.begin();
  while(!Console){
    ; //wait for console port to connect.
  }
  Console.println("You're connected to the Console!!!");
  digitalWrite(13,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Bridge.get("D13",D13char,2);
  D13int = atoi(D13char);
  if(D13int==0 || D13int==1){
    Console.println(D13char);
    digitalWrite(13,D13int);
    Bridge.put("D13","2");
  }
  delay(50);
}
