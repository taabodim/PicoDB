//
//  pico_buffered_message.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/29/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_buffered_message_h
#define PicoDB_pico_buffered_message_h
#include <pico/pico_buffer.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_utils.h>
#include <logger.h>
//this class contains a message thats has been transformed into a list of buffers
using namespace std;
 namespace pico
{
    class pico_buffered_message;
    typedef std::shared_ptr<pico_buffered_message> msgPtr;

    class pico_buffered_message{
    public:
          logger mylogger;
        pico_buffered_message()
        {
        std::cout<<("empty pico_buffered_message being constructed.....\n");
        }
        pico_buffered_message(std::shared_ptr<pico_concurrent_list<bufferType>> list){
        std::cout<<("pico_buffered_message being constructed....\n");
            msg_in_buffers = list;
        }
        pico_buffered_message(pico_buffered_message& copy){
            std::cout<<("pico_buffered_message copy constructed....\n");
            msg_in_buffers = copy.msg_in_buffers;
            
        }
        
        virtual ~pico_buffered_message()
        {
            std::cout<<("pico_buffered_message being desstructed....\n");
            
        }
        void append(bufferType buf)
        {
        	msg_in_buffers->append(buf);
        }
        
        void clear()
        {
            
            msg_in_buffers->clear();
        }


        string toString()
        {

            return msg_in_buffers->toString();
        }
        std::shared_ptr<pico_concurrent_list<bufferType>> msg_in_buffers;
        //since, a list cannot be copied, I create the list on the heap and copy the pointer to it
        //every time this object wants to copy
    };
}


#endif
