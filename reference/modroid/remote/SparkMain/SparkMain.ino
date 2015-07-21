#define QIK_GET_CONFIGURATION_PARAMETER  0x83
#define QIK_SET_CONFIGURATION_PARAMETER  0x84
#define QIK_SET_CONFIGURATION_CONFIRM1 0x55
#define QIK_SET_CONFIGURATION_CONFIRM2 0x2A

#define QIK_MOTOR_M0_FORWARD             0x88
#define QIK_MOTOR_M0_REVERSE             0x8A
#define QIK_MOTOR_M1_FORWARD             0x8C
#define QIK_MOTOR_M1_REVERSE             0x8E


// UDP Port used for two way communication
unsigned int localPort = 5555;

//variables to show the IP address
int showIP(String unused);
char devIP_chr[20];
IPAddress devIP;

// An UDP instance to let us send and receive packets over UDP
UDP Udp;

//global variables
int motor_speed = 0;
int motor_bias = 0;

void setup() {
    // start the UDP
    Udp.begin(localPort);
    devIP = WiFi.localIP();
    String devIP_str = String(devIP[0])+"."+String(devIP[1])+"."+String(devIP[2])+"."+String(devIP[3]+": "+String(WiFi.RSSI()));
    devIP_str.toCharArray(devIP_chr,20);
    
    
    //register the ShowIP function
    Spark.function("showIP",showIP);
    //Spark.variable("devIP",devIP_chr,STRING);

    //set pin modes
    pinMode(D7,OUTPUT);

    // Print your device IP Address via serial 
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println(devIP_str);
    
    //Set motor configuration parameters
    digitalWrite(D7, HIGH);
    write_config_param_to_serial(1,0); //0 = 7-bit resolution, high-frequency (PWM frequency of 19.7 kHz, which is ultrasonic)
    write_config_param_to_serial(2,6); //stop motors when either any motor-over-current error or motor-fault error occurs, but not any serial error
    write_config_param_to_serial(3,0); //no serial timeout
    write_config_param_to_serial(4,10); //When accelerating M0, it can add 10 to the motor speed every period (40ms).
    write_config_param_to_serial(5,10); //same as above, but for M1
    write_config_param_to_serial(6,50); //M0 brakes for 50 ms before switching motor direction
    write_config_param_to_serial(7,50); //same as above, but for M1
    write_config_param_to_serial(8,0); //current limit disabled
    write_config_param_to_serial(9,0); //current limit disabled
    digitalWrite(D7, LOW);
    
}

void loop() {
    digitalWrite(D7, HIGH);
    if (Udp.parsePacket() > 0) {
        Serial.println("Hey! I got something!");
        char c = Udp.read();
        Serial.println(c);
        switch (c)
        {
            case 'U':
                //increase forward speed
                motor_speed = min(motor_speed+16,128); //we limit this to 127 before we write it, I just want to keep things divisible by 16 :)
                break;
            case 'D':
                //decrease forward speed
                motor_speed = max(motor_speed-16,-128);
                break;
            case 'L':
                //increase the left bias
                motor_bias = min(motor_bias+16,128);
                break;
            case 'R':
                //decrease the left bias
                motor_bias = max(motor_bias-16,-128);
                break;
            case 'S':
                //stop
                motor_speed = 0;
                break;
            case 'I':
                Serial.print("IP: ");
                Serial.println(WiFi.localIP());
                Serial.print("RSSI: ");
                Serial.println(WiFi.RSSI());
                String devIP_str = String(devIP[0])+"."+String(devIP[1])+"."+String(devIP[2])+"."+String(devIP[3]+": "+String(WiFi.RSSI()));
                devIP_str.toCharArray(devIP_chr,20);
                break;
        }
        update_speeds();
    }
    delay(100);
    //Serial.println(WiFi.localIP());
    digitalWrite(D7, LOW);
    delay(100);
}

void update_speeds()
{
    int m0_speed = motor_speed + motor_bias;
    int m1_speed = motor_speed - motor_bias;
    int m0_clamped_speed = min(max(m0_speed,-127),127);
    int m1_clamped_speed = min(max(m1_speed,-127),127);
    
    int m0_scaled_speed = m1_clamped_speed * m0_speed / m1_speed;
    int m1_scaled_speed = m0_clamped_speed * m1_speed / m0_speed;
    
    write_speeds_to_serial(m0_scaled_speed, m1_scaled_speed);
    Serial.print("motor_speed, motor_bias = ");
    Serial.print(motor_speed);
    Serial.print(", ");
    Serial.println(motor_bias);
}

void write_speeds_to_serial(int8_t m0_speed, int8_t m1_speed)
{
    byte speed_cmd[2];
    if (m0_speed < 0){
        speed_cmd[0] = QIK_MOTOR_M0_REVERSE;
    }else{
        speed_cmd[0] = QIK_MOTOR_M0_FORWARD;
    }
    speed_cmd[1] = min(abs(m0_speed),127);

    Serial1.write(speed_cmd,2);
    Serial.print(speed_cmd[0]);
    Serial.print(" ");
    Serial.println(speed_cmd[1]);
    
    if (m1_speed < 0){
        speed_cmd[0] = QIK_MOTOR_M1_REVERSE;
    }else{
        speed_cmd[0] = QIK_MOTOR_M1_FORWARD;
    }
    speed_cmd[1] = min(abs(m1_speed),127);
    
    Serial1.write(speed_cmd,2);
    Serial.print(speed_cmd[0]);
    Serial.print(" ");
    Serial.println(speed_cmd[1]);
}

void write_config_param_to_serial(byte number, byte value)
{
    //Compact protocol: 0x84, parameter number, parameter value, 0x55, 0x2A
    byte config_cmd[5];
    config_cmd[0] = QIK_SET_CONFIGURATION_PARAMETER;
    config_cmd[1] = number;
    config_cmd[2] = value;
    config_cmd[3] = QIK_SET_CONFIGURATION_CONFIRM1;
    config_cmd[4] = QIK_SET_CONFIGURATION_CONFIRM2;
    Serial1.write(config_cmd,5);
}

int showIP(String unused)
{
    IPAddress localIP = WiFi.localIP();
    return 1000*localIP[2]+localIP[3];
}