/*
 * asyncReader.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef ASYNCREADER_H_
#define ASYNCREADER_H_

#include <pico/pico_record.h>
#include <pico/writer_buffer_container.h> //for typedef only
#include <pico/ConcurrentVector.h>
#include <pico/pico_utils.h>

#include  <pico_logger_wrapper.h>
using namespace std;
namespace pico {

    class asyncReader : public pico_logger_wrapper{

public:
    
	asyncReader() {
		mylogger<<"asyncReader is being constructed....\n";
	}
	virtual ~asyncReader() {
		mylogger<<("asyncReader is being destructed....\n");
	}
//
//	msgPtr getReadBuffer() {
//		msgPtr bufferPtr(new pico_recorded_message());
//		readerBufferList.push(bufferPtr);
//		return bufferPtr;
//	}
        std::shared_ptr<pico_record> getOneBuffer() {

		std::shared_ptr<pico_record> buf (new pico_record());
		singleBufferList.push_back(buf);
		return buf;
	}

//	ConcurrentVector<msgPtr> readerBufferList;
	ConcurrentVector<std::shared_ptr<pico_record>,VectorTraits<pico_record>  > singleBufferList;

private:
//none as of now

};
}

#endif /* ASYNCREADER_H_ */
