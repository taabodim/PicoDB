#ifndef ASYNC_WRITER_H
#define ASYNC_WRITER_H
#include <pico/pico_buffer.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
using namespace std;
namespace pico {
class asyncWriter
{
private:
    //none as of now
    int currentBufferIndex;

public:
	
    std::shared_ptr<pico_concurrent_list<msgPtr>> writerMessageList;
       logger mylogger;
    asyncWriter():   writerMessageList ( new pico_concurrent_list<msgPtr>())
    {
        std::cout<<("empty asyncWriter being constructed....");
        currentBufferIndex=0;
    }
    asyncWriter(std::shared_ptr<pico_concurrent_list<msgPtr>> list)
    
    {
        std::cout<<("asyncWriter being constructed....");
        currentBufferIndex=0;
        writerMessageList = list;
    }
	~asyncWriter(){
        std::cout<<("asyncWriter being destructed....");
    }
	void addToAllMessages(msgPtr& msg)
    {
        writerMessageList->push(msg);
    }
    msgPtr get_new_pico_buffered_message(){
		msgPtr msgPtr(new pico_buffered_message());
		writerMessageList->push(msgPtr);
		return msgPtr;
	}
    
    msgPtr getMessage()
    
    {
        msgPtr curMessage = writerMessageList->get(currentBufferIndex);
      
        currentBufferIndex++;
        return curMessage;
    }

    
};
}
#endif
