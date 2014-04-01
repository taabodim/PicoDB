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

//this class contains a message thats has been transformed into a list of buffers
using namespace std;
 namespace pico
{
    class pico_buffered_message;
    typedef std::shared_ptr<pico_buffered_message> msgPtr;

    class pico_buffered_message{
    public:
        pico_buffered_message()
        {
        log("empty pico_buffered_message being constructed.....\n");
        }
        pico_buffered_message(std::shared_ptr<pico_concurrent_list<pico_buffer>> list){
        log("pico_buffered_message being constructed....\n");
            msg_in_buffers = list;
        }
        
        
        virtual ~pico_buffered_message()
        {
            log("pico_buffered_message being desstructed....\n");
            
        }
        void append(pico_buffer buf)
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
        std::shared_ptr<pico_concurrent_list<pico_buffer>> msg_in_buffers;
    };
}


#endif
