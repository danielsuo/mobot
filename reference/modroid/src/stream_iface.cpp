/*
	Stream interface implementation
*/
#include "stream_iface.h"

StreamIface::StreamIface( Serial_* serial){//, bQueue* in_queue, bQueue* out_queue ) {
	m_serial = serial;
	m_in = &bQueue();
	m_out = &bQueue();
}

void StreamIface::init(long baud_rate) {
	m_serial->begin(baud_rate);
}

bool StreamIface::readInQueue( void ) {
	uint8_t byte;
	for ( uint8_t i=0; i < m_serial->available(); i++ ) {
		byte = m_serial->read();
		if ( !m_in->enqueue(byte) ) { //fails if m_in is full
			return false;
		}
	}
	return true;
}

bool StreamIface::writeOutQueue( void ) {
	uint8_t byte;
	while( m_out->available() ) {
		// Look at the next byte to dequeue
		m_out->peek( &byte );
		// Try to write the byte, if failed then return
		if ( m_serial->write( byte ) == 0 ) {
			return false;
		}
		// If we succeeded in writing the byte dequeue
		m_out->dequeue( &byte );
	}
	return true;
}

bool StreamIface::enqueue(uint8_t* arr, uint8_t arr_len ) {
	return m_out->enqueue(arr,arr_len);
}

bool StreamIface::enqueue(uint8_t byte) {
	return m_out->enqueue(byte);
}

bool StreamIface::dequeue(uint8_t* arr, uint8_t arr_len) {
	return m_in->dequeue(arr,arr_len);
}

bool StreamIface::dequeue(uint8_t* byte) {
	return m_in->dequeue(byte);
}

bool StreamIface::readNow( uint8_t* arr, uint8_t arr_len) {
	m_in->flush();
	if ( !readInQueue() ){ //fails if m_in fills up, 
		return false;
	}
	if ( !dequeue(arr,arr_len) ){ //fails if arr_len is bigger than m_in->byte_cnt
		return false;
	}

	return true;
}

bool StreamIface::writeNow( uint8_t* arr, uint8_t arr_len) {
	m_out->flush();
	if ( !enqueue(arr,arr_len) ){ //fails if arr_len is bigger than m_out->QUEUE_SIZE
		return false;
	}
	if ( !writeOutQueue() ){ //fails if the serial fails to write one of the m_out bytes
		return false;
	}

	return true;
}

bool StreamIface::syncWriteRead(uint8_t* snd, uint8_t snd_len, uint8_t* rcv, uint8_t rcv_len){

	//send the request
	if ( !writeNow(snd,snd_len) ){ //fails if the m_out queue isn't big enough or if the serial write fails somehow
		return false;
	}

	//recieve the response
	if ( !readNow(rcv,rcv_len) ){ //fails if the m_in queue isn't big enough
		return false;
	}

	return true;
}

bool StreamIface::syncWriteRead(uint8_t snd, uint8_t* rcv){
	return syncWriteRead(&snd,1,rcv,1);
}

