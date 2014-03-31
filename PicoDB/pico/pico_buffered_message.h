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
//this class contains a message thats has been transformed into a list of buffers
using namespace std;
 namespace pico
{
    
    class pico_buffered_message{
    public:
        pico_buffered_message()
        {
        cout<<"empty pico_buffered_message being constructed.....\n";
        }
        pico_buffered_message(std::shared_ptr<pico_concurrent_list<pico_buffer>> list){
        cout<<"pico_buffered_message being constructed....\n";
            msg_in_buffers = list;
        }
        
        
        virtual ~pico_buffered_message()
        {
            cout<<"pico_buffered_message being desstructed....\n";
            
        }
        
        std::shared_ptr<pico_concurrent_list<pico_buffer>> msg_in_buffers;
    };
}


#endif
