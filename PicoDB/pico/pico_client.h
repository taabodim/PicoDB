//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PICO_CLIENT_H
#define PICO_CLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <array>
#include "AsyncTcpClient.h"
#include <boost/thread.hpp>
#include <pico/pico_buffer.h>
#include <pico/writer_buffer_container.h>
#include <pico/asyncReader.h>
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
using boost::asio::ip::tcp;
using namespace std;


namespace pico {
    
    class DBClient;
 typedef DBClient clientType;
    typedef std::shared_ptr<pico_message> queueType;
    class DBClient: public std::enable_shared_from_this<DBClient> {
private:
   
	
	socketType socket_;
	pico_concurrent_list <queueType> commandQueue_;
public:
    logger mylogger;
	DBClient(socketType socket) :
        writeMessageLock(sessionMutex),allowedToWriteLock(allowedToWriteLockMutext) {

		socket_ = socket;
		std::cout<<( "client initializing ");

	}
    
	void start_connect(tcp::resolver::iterator endpoint_iter) {
		std::cout<<" start_connect(tcp::resolver::iterator endpoint_iter) ";
		if (endpoint_iter != tcp::resolver::iterator()) {
			std::cout<<  "Trying "<<endpoint_iter->endpoint() << "...\n";

			// Start the asynchronous connect operation.
			socket_->async_connect(endpoint_iter->endpoint(),
					boost::bind(&DBClient::start, this, _1, endpoint_iter));
		} else {
			// There are no more endpoints to try. Shut down the client.
//				stop();
		}
		std::cout<<(  " start_connect ends" );
	}
	void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
//			if (stopped_)
//				return;

// The async_connect() function automatically opens the socket at the start
// of the asynchronous operation. If the socket is closed at this time then
// the timeout handler must have run first.
		if (!socket_->is_open()) {
			std::cout<<(  "Connect timed out\n");

			// Try the next available endpoint.
			start_connect(++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
        std::cout<<"\nclient starting the process..going to write_message to server\n";
       
		try {
            if(!ec)
            {
            for(int  i=0;i<10;i++)
                insert();
            
			write_messages();
//			read_messages();
            }
            else{
                std::cout<<"client : start : error is "<<ec.value()<<" error message is "<<ec.message()<<std::endl;
                std::cout <<"error name is "<< ec.category().name() << std::endl;
            }
            
		} catch (const std::exception& e) {
			std::cout<<" exception : "<<e.what();
		} catch (...) {
			std::cout<< "<----->unknown exception thrown.<------>\n";
		}
	}
//	void readAsync() {
////		while (true) {
//		std::cout<<(" client is going to read asynchronously..\n";
//		read_messages();
//
//		//boost::this_thread::sleep(boost::posix_time::seconds(4));
//
//	}

	void processDataFromServer(string messageFromServer) {

		write_messages();
	}
	void readSynchronously() {
		for (;;) {
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket_->read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			std::cout << "client got this " << buf.data() << std::endl;
		}
	}
	void read_messages() {

        auto self(shared_from_this());
		std::cout<<( "client is trying to read messages" );
		bufferTypePtr currentBuffer = asyncReader_.getOneBuffer();
		//get a message in pico_buffer
        //convert it to string , if at the end of it is append.. add it to the list of buffers and create a
		//buffered_message out of all the pico_buffers and then convert the buffere_message to string and process
        
		//if the append is not at the end..convert the pico_buffer to string and process
		//
		boost::asio::async_read(*socket_,
                                boost::asio::buffer(currentBuffer->getData(),
                                                    pico_buffer::max_size),
                                [this,self,currentBuffer](const boost::system::error_code& error,
                                                          std::size_t t ) {
                                    string app("append");
                                    string str =currentBuffer->toString();
                                    append_to_last_message(*currentBuffer);
                                    if(str.find_last_of(app)== string::npos)
                                    {
                                        std::cout<<("client : this buffer is an add on to the last message..dont process anything..read the next buffer");
                                        
                                    }
                                    else {
                                        std::cout<<("client : message was read completely..process the last message ");
                                        str =last_read_message.toString();
                                        print(error,t,str);
                                        
                                        
                                        processDataFromServer(str);
                                        last_read_message.clear();
                                        
                                    }
                                    clientIsAllowedToWrite.notify_all();
                                });

	}
    void print(const boost::system::error_code& error,
               std::size_t t,string& str)
    {
        std::cout<<"Client Received :  "<<t<<" bytes from server "<<std::endl;
        if(error) std::cout<<" error msg : "<<error.message()<<" data  read from server is "<<str<<"-------------------------"<<std::endl;
    }
    void append_to_last_message(bufferType currentBuffer) {
		last_read_message.append(currentBuffer);
        
	}
//    void insert(const std::string& key,const std::string& value){
        void insert(){
            std::string key;
            std::string value;
        key="1";
        value="They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot ";
        value="adasldj";
        key="asd";
        string command("insert");
        string database("currencyDB");
        string user("currencyUser");
        string col("currencyCollection");
        
        queueType msg (  new pico_message(key,value,command,database,user,col) );
        queueCommand(msg);
        
    }
	void write_messages() {
        
		
		if (commandQueue_.empty())
        {
            std::cout<<"client queue of messages is empty...going to wait on the lock"<<endl;
			messageClientQueueIsEmpty.wait(writeMessageLock);
        }
		std::cout<< "client is writing async now"<<endl;

		if (!commandQueue_.empty()) {
			queueType message = commandQueue_.pop();

		
		// std::cout<<( " client is going to send this message to server : ");
         //std::cout<<(message->json_form_of_message); //this line throws exception , check why

        while(! message->buffered_message.msg_in_buffers->empty())
        {
            std::cout<<"pico_client : popping current Buffer ";
            bufferType buf = message->buffered_message.msg_in_buffers->pop();
            std::cout<<"pico_client : popping current Buffer this is current buffer ";
           
            std::shared_ptr<pico_buffer> bufPtr(new pico_buffer(buf));
            writer_buffer_container_.addToAllBuffers(bufPtr);
            writeOneBuffer(bufPtr);
            clientIsAllowedToWrite.wait(allowedToWriteLock);
        
          
      	}
        }
    }
    void writeOneBuffer(bufferTypePtr currentBuffer)
    {
        char* data = currentBuffer->getData();
		std::size_t dataSize = currentBuffer->getSize();
        std::cout<<"client is writing one buffer with this size "<<dataSize<<endl;
        
        auto self(shared_from_this());
        
		boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                 [this,self,currentBuffer](const boost::system::error_code& error,
                                                           std::size_t t) {
                                     string str = currentBuffer->toString();
                                      std::cout<<( "Client Sent :  ");
                                     std::cout<<t<<" bytes to server "<<std::endl;
                                     if(error)
                                         std::cout<<" error msg : "<<error.message()<<std::endl;
                                     
                                     std::cout<< " data sent to server is ";
                                     std::cout<<str;
                                     std::cout<<"-------------------------";
                                     read_messages();
                                 });
 
    }
	void queueCommand(queueType queueValue) {
		
        commandQueue_.push(queueValue);
		messageClientQueueIsEmpty.notify_all();
	}
//	pico_client_server::pico_buffer buffer;

	writer_buffer_container writer_buffer_container_;
	asyncReader asyncReader_;
    boost::mutex sessionMutex;   // mutex for the condition variable
    boost::mutex allowedToWriteLockMutext;
	boost::condition_variable messageClientQueueIsEmpty;
    boost::condition_variable clientIsAllowedToWrite;
    boost::unique_lock<boost::mutex> allowedToWriteLock;
	boost::unique_lock<boost::mutex> writeMessageLock;
	pico_buffered_message last_read_message;

};


void startTheShell(std::shared_ptr<clientType> ptr) {
	std::string quitCmd("quit");

	cout << "Wlecome to the Pico Shell  " << endl;
	std::string shellCommand; // declare one variable
	do {

		cin >> shellCommand;
		try{
//		queueType msg ( new pico_message(shellCommand));
//		ptr->queueCommand(msg);
		}catch(...)
		{
			cout<<("error in parsing command");
		}

	} while (shellCommand.compare(quitCmd) != 0);
}
    

    


} //end of namespace
#endif
