#ifndef ASYNC_WRITER_H
#define ASYNC_WRITER_H
#include <pico/pico_buffer.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
using namespace std;
namespace pico {
class asyncWriter
{
private:
    //none as of now
    int currentBufferIndex;

public:
	
    std::shared_ptr<pico_concurrent_list<bufferPtrType>> writerMessageList;
    
    asyncWriter():   writerMessageList ( new pico_concurrent_list<bufferPtrType>())
    {
        cout<<"empty asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
    }
    asyncWriter(std::shared_ptr<pico_concurrent_list<bufferPtrType>> list)
    
    {
        cout<<"asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
        writerMessageList = list;
    }
	~asyncWriter(){
        cout<<"asyncWriter being destructed...."<<endl;
    }
	void addToAllMessages(bufferPtrType& msg)
    {
        writerMessageList->push(msg);
    }
    bufferPtrType get_new_pico_buffered_message(){
		bufferPtrType msgPtr(new pico_buffered_message());
		writerMessageList->push(msgPtr);
		return msgPtr;
	}
    
    bufferPtrType getCurrentBuffer()
    
    {
        bufferPtrType curBuf = writerMessageList->get(currentBufferIndex);
        
        currentBufferIndex++;
        return curBuf;
    }

    
};
}
#endif
