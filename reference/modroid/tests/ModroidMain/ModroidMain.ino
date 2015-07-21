#include <Bridge.h> //this is just a datastore that the cpus share
#include <motion.h>

const int ledPin = 13;
char bridgeFlag[2]; //needs to be length 2 because a tring terminator is always added
int bridgeFlagInt;
char bridgeVal[4];
Motion motors(&Serial);

void setup() {
  //Serial startup
  pinMode(ledPin,OUTPUT);
  
  //Bridge startup
  digitalWrite(ledPin,HIGH);
  Bridge.begin();
  digitalWrite(ledPin,LOW);
  
  delay(1000);
  
  //Console startup
  digitalWrite(ledPin,HIGH);
  Console.begin(); //for debugging purposes
  while(!Console){
    ; //wait for console port to connect
  }
  Console.println("You're connected to the Console!!!");
  digitalWrite(ledPin,LOW);
  
  delay(500);
  
  digitalWrite(ledPin,HIGH);
  //Motors startup
  motors.init(9600);
  digitalWrite(ledPin,LOW);
  
  Bridge.put("FLAG","0");
  Bridge.put("SPEED","100");
  Bridge.put("BRAKE","127");
  digitalWrite(ledPin,HIGH);
}

void loop() {
  // Check the bridge FLAG, this means we have an incoming UDP command
  Bridge.get("FLAG",bridgeFlag,2);
  if(bridgeFlag[0] != '0'){
    process_bridge(bridgeFlag[0]);
    Bridge.put("FLAG","0");
  }
  
}

void process_bridge(char cmd){
  int speed,brake;
  Console.print("received: ");
  Console.println(cmd);
  switch (cmd) {
    case 'U': //up key
      Bridge.get("SPEED",bridgeVal,4);
      speed = atoi(bridgeVal);
      motors.setSpeeds(speed,speed);
      Console.print("cmd speeds ");
      Console.print(speed);
      Console.println(speed);
      digitalWrite(ledPin,HIGH);
      break;
    case 'D': //down key
      Bridge.get("SPEED",bridgeVal,4);
      speed = atoi(bridgeVal);
      motors.setSpeeds(-speed,-speed);      
      Console.print("cmd speeds ");
      Console.print(-speed);
      Console.println(-speed);
      digitalWrite(ledPin,LOW);
      break;
    case 'L': //left key
      Bridge.get("SPEED",bridgeVal,4);
      speed = atoi(bridgeVal);
      motors.setSpeeds(speed,-speed);
      Console.print("cmd speeds ");
      Console.print(speed);
      Console.println(-speed);
      break;
    case 'R': //right key
      Bridge.get("SPEED",bridgeVal,4);
      speed = atoi(bridgeVal);
      //motors.setSpeeds(-speed,speed);
      Console.print("cmd speeds ");
      Console.print(-speed);
      Console.println(speed);
      break;
    case 'S': //space key
      Bridge.get("BRAKE",bridgeVal,4);
      brake = atoi(bridgeVal);
      motors.setBrakes(brake,brake);
      Console.print("cmd brakes ");
      Console.print(brake);
      Console.println(brake);
      break;
    default:
      break;
  }

}
