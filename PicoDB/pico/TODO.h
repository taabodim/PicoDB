/*
 * TODO.h
 *
 *  Created on: Mar 22, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef TODO_H_
#define TODO_H_
//create a method to convert pico_message to
//a list of pico_buffers each 128 bytes

//create a method to convert a list of pico_buffers
//to a pico_message


//create a method to convert pico_record to
//a list of pico_buffers each 128 bytes (done)

//create a method to convert a list of pico_buffers
//to a pico_record which is 16 mg or more (done)


////
//add fields to pico_buffer to point to the next right pico_buffer_id
//each pico_buffer should have a pico_buffer_id which is unique based on
//time and hash of its content


//zip the content of the file that contains all the data.using snappy
//

//create a index btree for collection and index it on the key of the
//data , and each record should have the unique id for the pico_record
//and the offset of its pico_record in databases file (done)

//whenever you add or change something, make the change in the index
//tree too

//create a thread-safe list class and use it every where (done)

//create a thread-safe set and map wrapper and use it every where

//create a replicator engine

//send acknowledge when you recieve a data fine (to be completed)
//specially when you are sending huge data

//create jms provider





#endif /* TODO_H_ */
