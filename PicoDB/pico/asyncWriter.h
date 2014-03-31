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
	
    std::shared_ptr<pico_concurrent_list<pico_buffered_message>> writerMessageList;
    
    asyncWriter():   writerMessageList ( new pico_concurrent_list<pico_buffered_message>())
    {
        cout<<"empty asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
    }
    asyncWriter(std::shared_ptr<pico_concurrent_list<pico_buffered_message>> list)
    
    {
        cout<<"asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
        writerMessageList = list;
    }
	~asyncWriter(){
        cout<<"asyncWriter being destructed...."<<endl;
    }
	void addToAllMessages(pico_buffered_message& msg)
    {
        writerMessageList->push(msg);
    }
    pico_buffered_message get_new_pico_buffered_message(){
		pico_buffered_message  msgPtr;
		writerMessageList->push(msgPtr);
		return msgPtr;
	}
    
    pico_buffered_message getCurrentBuffer()
    
    {
        pico_buffered_message curBuf = writerMessageList->get(currentBufferIndex).msg_in_buffers;
        
        currentBufferIndex++;
        return curBuf;
    }

    
};
}
#endif
