/*
 * asyncReader.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef ASYNCREADER_H_
#define ASYNCREADER_H_

#include <pico/pico_buffer.h>
#include <pico/writer_buffer_container.h> //for typedef only
#include <pico/pico_concurrent_list.h>
#include <pico/pico_utils.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
using namespace std;
namespace pico {

class asyncReader {

public:
       //logger mylogger;
	asyncReader() {
		//std::cout<<"asyncReader is being constructed....\n";
	}
	virtual ~asyncReader() {
		//std::cout<<("asyncReader is being destructed....\n");
	}
//
//	msgPtr getReadBuffer() {
//		msgPtr bufferPtr(new pico_buffered_message());
//		readerBufferList.push(bufferPtr);
//		return bufferPtr;
//	}
	bufferTypePtr getOneBuffer() {

		bufferTypePtr buf (new bufferType());
		singleBufferList.push(buf);
		return buf;
	}

//	pico_concurrent_list<msgPtr> readerBufferList;
	pico_concurrent_list<bufferTypePtr> singleBufferList;

private:
//none as of now

};
}

#endif /* ASYNCREADER_H_ */
