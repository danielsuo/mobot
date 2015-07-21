#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>
#include "stream_iface.h"

#define QIK_GET_FIRMWARE_VERSION         0x81
#define QIK_GET_ERROR_BYTE               0x82
#define QIK_GET_CONFIGURATION_PARAMETER  0x83
#define QIK_SET_CONFIGURATION_PARAMETER  0x84

#define QIK_MOTOR_M0_FORWARD             0x88
#define QIK_MOTOR_M0_FORWARD_8_BIT       0x89
#define QIK_MOTOR_M0_REVERSE             0x8A
#define QIK_MOTOR_M0_REVERSE_8_BIT       0x8B
#define QIK_MOTOR_M1_FORWARD             0x8C
#define QIK_MOTOR_M1_FORWARD_8_BIT       0x8D
#define QIK_MOTOR_M1_REVERSE             0x8E
#define QIK_MOTOR_M1_REVERSE_8_BIT       0x8F

// 2s12v10 only
#define QIK_2S12V10_MOTOR_M0_BRAKE       0x86
#define QIK_2S12V10_MOTOR_M1_BRAKE       0x87
#define QIK_2S12V10_GET_MOTOR_M0_CURRENT 0x90
#define QIK_2S12V10_GET_MOTOR_M1_CURRENT 0x91
#define QIK_2S12V10_GET_MOTOR_M0_SPEED   0x92
#define QIK_2S12V10_GET_MOTOR_M1_SPEED   0x93

// Configuration parameters

#define QIK_CONFIG_DEVICE_ID                        0
#define QIK_CONFIG_PWM_PARAMETER                    1
#define QIK_CONFIG_SHUT_DOWN_MOTORS_ON_ERROR        2
#define QIK_CONFIG_SERIAL_TIMEOUT                   3
#define QIK_CONFIG_MOTOR_M0_ACCELERATION            4
#define QIK_CONFIG_MOTOR_M1_ACCELERATION            5
#define QIK_CONFIG_MOTOR_M0_BRAKE_DURATION          6
#define QIK_CONFIG_MOTOR_M1_BRAKE_DURATION          7
#define QIK_CONFIG_MOTOR_M0_CURRENT_LIMIT_DIV_2     8
#define QIK_CONFIG_MOTOR_M1_CURRENT_LIMIT_DIV_2     9
#define QIK_CONFIG_MOTOR_M0_CURRENT_LIMIT_RESPONSE 10
#define QIK_CONFIG_MOTOR_M1_CURRENT_LIMIT_RESPONSE 11

class Motion
{
public:
    Motion();
	Motion(Serial_* serial);
	~Motion();

	void init(long speed = 9600);

    uint8_t getFirmwareVersion();
    uint8_t getErrors();

    uint8_t getConfigurationParameter(uint8_t parameter);
    uint8_t setConfigurationParameter(uint8_t parameter, uint8_t value);

    void setM0Brake(uint8_t brake);
    void setM1Brake(uint8_t brake);
    void setBrakes(uint8_t m0Brake, uint8_t m1Brake);

    void setM0Speed(int speed);
    void setM1Speed(int speed);
    void setSpeeds(int m0Speed, int m1Speed);

	unsigned int getM0Current();
    unsigned int getM1Current();
    uint8_t getM0Speed();
    uint8_t getM1Speed();
private:
	StreamIface* driver_iface;
	uint8_t* cmd;
};

#endif