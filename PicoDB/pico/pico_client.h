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
#include <pico/pico_session.h> //for the checking if appended function
using boost::asio::ip::tcp;
using namespace std;


namespace pico {
    
    class DBClient;
    typedef DBClient clientType;
    typedef pico_message queueType;
    class DBClient: public std::enable_shared_from_this<DBClient> {
    private:
        
        
        socketType socket_;
        pico_concurrent_list <queueType> commandQueue_;
    public:
        static string logFileName;
        logger mylogger;
        DBClient(socketType socket) :
        writeOneBufferLock(sessionMutex),allowedToWriteLock(allowedToWriteLockMutext) ,mylogger(logFileName){
            
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
                    for(int  i=0;i<1;i++)
                        insert();
                    
                    writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
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
            
            writeOneBuffer();
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
        void read_one_buffer() {
            
            auto self(shared_from_this());
            std::cout<<"client is trying to read one buffer\n" ;
            bufferTypePtr currentBuffer = asyncReader_.getOneBuffer();
            
            boost::asio::async_read(*socket_,
                                    boost::asio::buffer(currentBuffer->getData(),
                                                        pico_buffer::max_size),
                                    [this,self,currentBuffer](const boost::system::error_code& error,
                                                              std::size_t t ) {
                                        
                                        processTheMessageJustRead(currentBuffer,t);
                                        //                                         clientIsAllowedToWrite.notify_all();
                                        
                                        
                                    });
            //		boost::asio::async_read(*socket_,
            //                                boost::asio::buffer(currentBuffer->getData(),
            //                                                    pico_buffer::max_size),
            //                                [this,self,currentBuffer](const boost::system::error_code& error,
            //                                                          std::size_t t ) {
            //                                    string app("append");
            //                                    string str =currentBuffer->toString();
            //                                    append_to_last_message(*currentBuffer);
            //                                    if(str.find_last_of(app)== string::npos)
            //                                    {
            //                                        std::cout<<("client : this buffer is an add on to the last message..dont process anything..read the next buffer");
            //
            //                                    }
            //                                    else {
            //                                        std::cout<<("client : message was read completely..process the last message ");
            //                                        str =last_read_message.toString();
            //                                        print(error,t,str);
            //
            //
            //                                        processDataFromServer(str);
            //                                        last_read_message.clear();
            //
            //                                    }
            //                                    clientIsAllowedToWrite.notify_all();
            //                                });
            
        }
        
        void processTheMessageJustRead(bufferTypePtr currentBuffer,std::size_t t){
            
            string str =currentBuffer->toString();
            append_to_last_message(*currentBuffer);
            std::cout<<"\nthis is the message that client read just now \n "<<str<<endl;
            //log to file the string that was just read.
            mylogger.log(str);
            if(pico_session::find_last_of_string(currentBuffer))
            {
                std::cout<<("client: this buffer is an add on to the last message..dont process anything..read the next buffer\n");
                processIncompleteData();
            }
            else {
                std::cout<<"client : message was read completely..process the last message\n ";
                str =last_read_message.toString();
                // print(error,t,str);
                processDataFromOtherSide(str);
                last_read_message.clear();
                //reading from server is done
                //now we go to writing mode
            }
        }
        void processDataFromOtherSide(std::string msg) {
            
            try {
                
                //                pico_message reply = requestProcessor_.processRequest(msg);
                //
                //                messageToClientQueue_.push(reply);
                //                messageClientQueueIsEmpty.notify_all();
                //
                std::cout<<"this is the complete message from server : "<<msg<<endl;
                writeOneBuffer();
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
        }
        void  processIncompleteData()
        {
            string msg("1");
            
            pico_message reply = pico_message::build_message_from_string(msg);
          	queueMessages(reply);
            
            
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
            key="They've";
            value="They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot";
            value="gone there";
            string command("insert");
            string database("currencyDB");
            string user("currencyUser");
            string col("currencyCollection");
            
            queueType msg (key,value,command,database,user,col );
            queueMessages(msg);
            
            //            queueType msgReadFromQueue = commandQueue_.pop();
            //            std::cout<<"this is to test if queue works fine"<<endl<<"queue item is "<<msgReadFromQueue.toString()<<endl<<msgReadFromQueue.key_of_message<<" " <<msgReadFromQueue.value_of_message<<endl<<msgReadFromQueue.command<<endl<<msgReadFromQueue.collection<<endl;
            
        }
             void writeOneBuffer()
        {
            if(bufferQueue_.empty())
            {
                cout<<"client : bufferQueue is empty..waiting ..."<<endl;
                bufferQueueIsEmpty.wait(writeOneBufferLock);
            }
            bufferTypePtr currentBuffer = bufferQueue_.pop();
            char* data = currentBuffer->getData();
            std::size_t dataSize = currentBuffer->getSize();
            std::cout<<"client is writing one buffer with this size "<<dataSize<<endl;
            
            auto self(shared_from_this());
            
            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                     [this,self,currentBuffer](const boost::system::error_code& error,
                                                               std::size_t t) {
                                         string str = currentBuffer->toString();
                                         std::cout<<"Client Sent :  \n";
                                         std::cout<<t<<" bytes to server "<<std::endl;
                                         if(error)
                                             std::cout<<" error msg : "<<error.message()<<std::endl;
                                         
                                         std::cout<< " data sent to server is \n";
                                         std::cout<<str<<endl;
                                         std::cout<<"-------------------------"<<endl;
                                         string logMsg("data sent to server is ");
                                         logMsg.append(str);
                                         mylogger.log(logMsg);
                                         read_one_buffer();
                                     });
            
        }
        void queueMessages(queueType message) {
          //TODO put a lock here to make the all the buffers in a message go after each other.
        boost::interprocess::scoped_lock<boost::mutex> queueMessagesLock(queueMessagesMutext);
            
            //put all the buffers in the message in the buffer queue
            while(!message.buffered_message.msg_in_buffers->empty())
            {
                
                std::cout<<"pico_client : popping current Buffer ";
                bufferType buf = message.buffered_message.msg_in_buffers->pop();
                //                    std::cout<<"pico_client : popping current Buffer this is current buffer ";
                
                std::shared_ptr<pico_buffer> curBufPtr(new pico_buffer(buf));
                bufferQueue_.push(curBufPtr);
                
            }
           bufferQueueIsEmpty.notify_all();
        }
        
        //        writer_buffer_container writer_buffer_container_;
        pico_concurrent_list<bufferTypePtr> bufferQueue_;
        asyncReader asyncReader_;
        boost::mutex sessionMutex;   // mutex for the condition variable
        boost::mutex allowedToWriteLockMutext;
        boost::mutex queueMessagesMutext;
        boost::condition_variable bufferQueueIsEmpty;
        boost::condition_variable clientIsAllowedToWrite;
        boost::unique_lock<boost::mutex> allowedToWriteLock;
        boost::unique_lock<boost::mutex> writeOneBufferLock;
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
