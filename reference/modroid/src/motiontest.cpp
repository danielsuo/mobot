#include "motiontest.h"

MotionTest::MotionTest(){ }

MotionTest::MotionTest( Serial_* serial ){
	// in_q = bQueue();
	// out_q = bQueue();
	driver_iface = &StreamIface(serial);//,&in_q,&out_q);
	// m_serial = serial;
}

MotionTest::~MotionTest(){ }

void MotionTest::init(long baudrate){
	// m_serial->begin(baudrate);
	driver_iface->init(baudrate);
}