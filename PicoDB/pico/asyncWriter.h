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
	
    std::shared_ptr<pico_concurrent_list<msgPtr>> writerMessageList;
    
    asyncWriter():   writerMessageList ( new pico_concurrent_list<msgPtr>())
    {
        cout<<"empty asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
    }
    asyncWriter(std::shared_ptr<pico_concurrent_list<msgPtr>> list)
    
    {
        cout<<"asyncWriter being constructed...."<<endl;
        currentBufferIndex=0;
        writerMessageList = list;
    }
	~asyncWriter(){
        cout<<"asyncWriter being destructed...."<<endl;
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
