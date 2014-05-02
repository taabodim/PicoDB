//
//  ClientResponseProcessor.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/28/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//
#include <pico/pico_logger_wrapper.h>

#ifndef PicoDB_ClientResponseProcessor_h
#define PicoDB_ClientResponseProcessor_h
namespace pico {

    class ClientResponseProcessor :  public pico_logger_wrapper {
        
        private :
        
        public :
        
        ClientResponseProcessor(){
        
        }
        void processResponse(string messageFromServer)
        {
        
            //convert the string to pico_message
            mylogger<<"ClientResponseProcessor : this is the message that is going to be processed by the client "<<messageFromServer;
            
            
          //  pico_message picoMessage(messageFromServer);
 
           // mylogger<<"this response is for this request .."<<picoMessage.requestId<<"\n";
            //put the message in responseQueue, so the get method that checks it,
            //fetches the message
            
            //if there is client sent a key to get a value
           // TODO..
            //the message has to have a specific 
            
        }
        virtual ~ClientResponseProcessor(){
        
        }
    };

}


#endif
