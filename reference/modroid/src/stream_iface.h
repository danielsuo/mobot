/*
	Stream interface wrapping arduino serial and using byte queue
*/

#ifndef STREAM_IFACE_H
#define STREAM_IFACE_H

#include <Arduino.h>
#include "bqueue.h"

class StreamIface {
public:
	StreamIface( Serial_* serial);//, bQueue* in_queue, bQueue* out_queue );

	// Read and write bytes from queues
	void init( long baud_rate );
	bool readNow( uint8_t* buf, uint8_t buf_len );
	bool readInQueue( void );
	bool writeNow( uint8_t* buf, uint8_t buf_len );
	bool writeOutQueue( void );
	bool dequeue( uint8_t* byte );
	bool dequeue( uint8_t* buf, uint8_t buf_len );
	bool enqueue( uint8_t byte );
	bool enqueue( uint8_t* buf, uint8_t buf_len );
	bool syncWriteRead( uint8_t* snd, uint8_t snd_len, uint8_t* rcv, uint8_t rcv_len );
	bool syncWriteRead( uint8_t snd, uint8_t* rcv );

private:
	Serial_* m_serial;
	bQueue* m_in;
	bQueue* m_out;	
};

#endif