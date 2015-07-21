/*
  Simple queue implementation
*/
#ifndef BQUEUE_H
#define BQUEUE_H

#include "stdint.h"

#define QUEUE_SIZE 100

class bQueue {
public:
  bQueue( void );
  uint8_t available( void );
  bool isempty( void );
  bool isfull( void );    
  bool peek( uint8_t* byte );
  bool dequeue( uint8_t* byte );
  bool dequeue( uint8_t* buf, uint8_t buf_len );
  bool enqueue( uint8_t byte );
  bool enqueue( uint8_t* buf, uint8_t buf_len );
  void flush();
private:
  inline void incr( uint8_t &ind );
  uint8_t start_ind;
  uint8_t end_ind;
  uint8_t byte_cnt;
  uint8_t buf[QUEUE_SIZE];
};

#endif 
