/*
  Simple queue implementation
*/
#include "bqueue.h"

bQueue::bQueue( void ) {
  start_ind = 0; //index of the element at the start of the queue (the index to be read next)
  end_ind = 0; //index of the first free space after the end of the queue (the index to be written next)
  byte_cnt = 0; //end_ind-start_ind=byte_cnt (unless it's full, then end_ind==start_ind)
}

uint8_t bQueue::available( void ) {
  return byte_cnt;
}

bool bQueue::isempty( void ) {
  return ( byte_cnt == 0 );
}

// Check if we were to add another byte if our start and end inds collide
bool bQueue::isfull( void ) {  
  return ( byte_cnt == QUEUE_SIZE );
}

void bQueue::flush(){
  start_ind = 0;
  end_ind = 0;
  byte_cnt = 0;
}

// Enqueue an array of bytes 
bool bQueue::enqueue( uint8_t* arr, uint8_t arr_len ) {
    for ( uint8_t i=0; i<arr_len; i++ ) {      
      // Try to enqueue byte, if we can't then fail 
      if ( !enqueue( arr[i] ) ) {
        return false; 
      }
    }
    return true;
}

bool bQueue::enqueue( uint8_t byte ) {
  // Check if we are full
  if ( isfull() ) {
    return false;
  }
  
  //increment byte_cnt and add byte to buffer
  byte_cnt++;
  buf[end_ind] = byte;

  // increment the end index of the queue
  // end_ind = incr(end_ind);
  incr(end_ind);
  return true;
}   

// Show the most recent byte in the queue, if there are any 
bool bQueue::peek( uint8_t* byte ) {
  // Check if we are empty 
  if ( isempty() ) {
    return false;
  }
  *byte = buf[start_ind];
  return true;
}

bool bQueue::dequeue( uint8_t* byte ) {
  // Store byte and decrement byte count
  if ( !peek(byte) ) {
    return false;
  }
  byte_cnt--;

  //increment the start_ind
  // start_ind = incr(start_ind);
  incr(start_ind);
  return true;
}

bool bQueue::dequeue(uint8_t* arr,uint8_t arr_len){
  //dequeue arr_len elements into the arr buffer
  for(int i=0; i<arr_len; i++){
    if ( !dequeue(arr+i) ){
      return false;
    }
  }
  return true;
}

// Private Increment index with wraparound
inline void bQueue::incr( uint8_t &ind ) {
  // Increment index wrapping around the QUEUE_SIZE
  ++ind %= QUEUE_SIZE;
}
