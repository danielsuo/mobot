#ifndef MOTIONTEST_H
#define MOTIONTEST_H

#include <Arduino.h>
#include "stream_iface.h"
// #include "bqueue.h"

class MotionTest
{
	// Serial_* m_serial; //this works!
	StreamIface* driver_iface; //this works as long as you initialize it as driver_iface = &StreamIface(...)
	// bQueue in_q;
	// bQueue out_q;
public:
    MotionTest();
	MotionTest(Serial_* serial);
	~MotionTest();

	void init(long baudrate);
};

#endif