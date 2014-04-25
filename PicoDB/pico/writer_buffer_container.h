#ifndef ASYNC_WRITER_H
#define ASYNC_WRITER_H
#include <pico/pico_buffer.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
using namespace std;
namespace pico {
    class writer_buffer_container
    {
    
    public:
        //this has to be a list of pointer to buffers because we want to keep the buffers in the heap until
        // all the buffer is fully written to the other side...also the list should be kept in heap because
        // we cant copy a list to another list in c++, so we copy the pointers to it.
        std::shared_ptr<pico_concurrent_list<std::shared_ptr<pico_buffer>>> writerMessageList;
        writer_buffer_container():   writerMessageList ( new pico_concurrent_list<std::shared_ptr<pico_buffer>>())
        {
            //std::cout<<("empty writer_buffer_container being constructed....");
           }
        writer_buffer_container(std::shared_ptr<pico_concurrent_list<std::shared_ptr<pico_buffer>>> list)
        
        {
            //std::cout<<("writer_buffer_container being constructed....");
             writerMessageList = list;
        }
        ~writer_buffer_container(){
            //std::cout<<("writer_buffer_container being destructed....");
        }
        void addToAllBuffers(std::shared_ptr<pico_buffer> msg)
        {
            writerMessageList->push(msg);
        }
        
        
    };
}
#endif
