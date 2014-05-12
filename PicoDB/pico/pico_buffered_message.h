//
//  pico_buffered_message.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/29/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_buffered_message_h
#define PicoDB_pico_buffered_message_h

#include <pico/pico_concurrent_list.h>
#include <pico/pico_utils.h>
#include <logger.h>
#include <list>
//this class contains a message thats has been transformed into a list of buffers
using namespace std;
namespace pico
{
//    class pico_buffered_message;
//    typedef std::shared_ptr<pico_buffered_message> msgPtr;
    template <typename type>
    class pico_buffered_message{
    public:
        pico_buffered_message():msg_in_buffers(new pico_concurrent_list<type>())
        {
            //    mylogger<<("empty pico_buffered_message being constructed.....\n");
            
        }
        pico_buffered_message(std::shared_ptr<pico_concurrent_list<type>> list):msg_in_buffers(new pico_concurrent_list<type>()){
            //     mylogger<<("pico_buffered_message being constructed....\n");
            msg_in_buffers = list;
            
        }
        pico_buffered_message(const pico_buffered_message& copy){
            //mylogger<<("pico_buffered_message copy constructed....\n");
            msg_in_buffers = copy.msg_in_buffers;
            
        }
        pico_buffered_message operator=(const pico_buffered_message& copy){
            msg_in_buffers = copy.msg_in_buffers;
            return *this;
        }
        virtual ~pico_buffered_message()
        {
            // mylogger<<("pico_buffered_message being desstructed....\n");
            
        }
        void append(type buf)
        {
        	msg_in_buffers->append(buf);
        }
        type pop()
        {
            return msg_in_buffers->pop();
        }
        type peek()
        {
         return msg_in_buffers->peek();
        }
        bool empty()
        {
        	return  msg_in_buffers->empty();
        }
        size_t size()
        {

        	return msg_in_buffers->size();
        }
        void clear()
        {
            
            msg_in_buffers->clear();
        }
        
        void print()
        {
            msg_in_buffers->printAll();
        }
        string toString()
        {
            
            return msg_in_buffers->toString();
        }
        
        typename list<type>::iterator getLastBuffer()
        {
            return  msg_in_buffers->getLastBuffer();
        }
        
        typename list<type>::iterator getFirstBuffer()
        {
            return msg_in_buffers->getFirstBuffer();
        }
        std::shared_ptr<pico_concurrent_list<type>> msg_in_buffers;
        //since, a list cannot be copied, I create the list on the heap and copy the pointer to it
        //every time this object wants to copy
    };
}


#endif
