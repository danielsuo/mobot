#include "motion.h"

#define MIN(x,y) ( (x) < (y) ? (x) : (y) )
#define MAX(x,y) ( (x) > (y) ? (x) : (y) )
#define ABS(x) ( (x) < 0 ? (-x) : (x) )

Motion::Motion(){ }

Motion::Motion( Serial_* serial ){
	driver_iface = &StreamIface(serial);
	cmd = new uint8_t[2];
}

Motion::~Motion(){ }

void Motion::init(long baud_rate){
	//maybe we should do things like configure the serial here.
	driver_iface->init(baud_rate);
};

uint8_t Motion::getFirmwareVersion(){
	uint8_t rcv;

	driver_iface->syncWriteRead(QIK_GET_FIRMWARE_VERSION,&rcv);

	return rcv;
}

uint8_t Motion::getErrors(){
	uint8_t rcv;

	driver_iface->syncWriteRead(QIK_GET_ERROR_BYTE,&rcv);

	return rcv;
}

uint8_t Motion::getConfigurationParameter(uint8_t parameter){
	uint8_t* snd = new uint8_t[2];
	uint8_t rcv;

	snd[0] = QIK_GET_CONFIGURATION_PARAMETER;
	snd[1] = parameter;

	driver_iface->syncWriteRead(snd,2,&rcv,1);

	return rcv;
}

uint8_t Motion::setConfigurationParameter(uint8_t parameter, uint8_t value){
	uint8_t* snd = new uint8_t[5];
	uint8_t rcv;

	snd[0] = QIK_SET_CONFIGURATION_PARAMETER;
	snd[1] = parameter;
	snd[2] = value;
	snd[3] = 0x55;
	snd[4] = 0x2A;

	driver_iface->syncWriteRead(snd,5,&rcv,1);

	return rcv;
}

void Motion::setM0Brake(uint8_t brake){

    brake = MAX(brake,127);
  
	cmd[0] = QIK_2S12V10_MOTOR_M0_BRAKE;
	cmd[1] = brake;

	driver_iface->writeNow(cmd,2);
}

void Motion::setM1Brake(uint8_t brake){

    brake = MAX(brake,127);
  
	cmd[0] = QIK_2S12V10_MOTOR_M1_BRAKE;
	cmd[1] = brake;

	driver_iface->writeNow(cmd,2);
}

void Motion::setBrakes(uint8_t m0Brake, uint8_t m1Brake){
	setM0Brake(m0Brake);
	setM1Brake(m1Brake);
}

void Motion::setM0Speed(int speed){
	boolean reverse = (speed<0);

	speed = MAX(ABS(speed),255);

	if (speed > 127){
		// 8-bit mode: actual speed is (cmd[1] + 128)
		cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE_8_BIT : QIK_MOTOR_M0_FORWARD_8_BIT;
		cmd[1] = speed - 128;
	}else{
		cmd[0] = reverse ? QIK_MOTOR_M0_REVERSE : QIK_MOTOR_M0_FORWARD;
		cmd[1] = speed;
	}

	driver_iface->writeNow(cmd,2);
}

void Motion::setM1Speed(int speed){
	boolean reverse = 0;

	speed = MAX(ABS(speed),255);

	if (speed > 127){
		// 8-bit mode: actual speed is (cmd[1] + 128)
		cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE_8_BIT : QIK_MOTOR_M1_FORWARD_8_BIT;
		cmd[1] = speed - 128;
	}else{
		cmd[0] = reverse ? QIK_MOTOR_M1_REVERSE : QIK_MOTOR_M1_FORWARD;
		cmd[1] = speed;
	}

	driver_iface->writeNow(cmd,2);
}

void Motion::setSpeeds(int m0Speed, int m1Speed){
	setM0Speed(m0Speed);
	setM1Speed(m1Speed);
}

unsigned int Motion::getM0Current(){
	// These commands return the average motor currents over the last 5 ms in mA. 
	// Please note that the actual current could differ from this value by as much as 20%.	
	uint8_t rcv;
	int rcv_mA;

	driver_iface->syncWriteRead(QIK_2S12V10_GET_MOTOR_M0_CURRENT,&rcv);
	rcv_mA = rcv * 150;

	return rcv_mA;
}

unsigned int Motion::getM1Current(){
	// These commands return the average motor currents over the last 5 ms in mA. 
	// Please note that the actual current could differ from this value by as much as 20%.
	uint8_t rcv;
	int rcv_mA;

	driver_iface->syncWriteRead(QIK_2S12V10_GET_MOTOR_M1_CURRENT,&rcv);
	rcv_mA = rcv * 150;

	return rcv_mA;
}

uint8_t Motion::getM0Speed(){
	uint8_t rcv;
	driver_iface->syncWriteRead(QIK_2S12V10_GET_MOTOR_M0_SPEED,&rcv);
}

uint8_t Motion::getM1Speed(){
	uint8_t rcv;
	driver_iface->syncWriteRead(QIK_2S12V10_GET_MOTOR_M1_SPEED,&rcv);
}
