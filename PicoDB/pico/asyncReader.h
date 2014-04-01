/*
 * asyncReader.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef ASYNCREADER_H_
#define ASYNCREADER_H_

#include <pico/pico_buffer.h>
#include <pico/asyncWriter.h> //for typedef only
#include <pico/pico_concurrent_list.h>
#include <pico/pico_utils.h>
using namespace std;
namespace pico {

class asyncReader
{

public:
	asyncReader(){}
	virtual ~asyncReader(){}
	
    bufferPtrType getReadBuffer(){
		bufferPtrType  bufferPtr (new pico_buffered_message());
		readerBufferList.push(bufferPtr);
		return bufferPtr;
	}
	pico_concurrent_list<bufferPtrType> readerBufferList;

private:
//none as of now

};
}



#endif /* ASYNCREADER_H_ */