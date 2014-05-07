//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PonocoDriver_H
#define PonocoDriver_H

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

#include <pico/writer_buffer_container.h>
#include <pico/asyncReader.h>
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
#include <pico/pico_session.h> //for the checking if appended function and sendmetherestofdata function
#include <pico/pico_test.h>
#include <pico/pico_logger_wrapper.h>
#include <ClientResponseProcessor.h>
#include <PonocoDriverHelper.h>

using boost::asio::ip::tcp;
using namespace std;
using namespace std::chrono;



namespace pico {
    
    class PonocoDriver;
    typedef PonocoDriver DriverType;
    typedef pico_message queueType;
    class PonocoDriver: public std::enable_shared_from_this<PonocoDriver>, public pico_logger_wrapper
    //,//public  PonocoDriverHelper
    {
    private:
        std::shared_ptr<tcp::socket> socket_;
        ResponseProcessor responseProcessor;
      //  typedef  std::shared_ptr<PonocoDriverHelper> helperType;
       typedef  PonocoDriverHelper* helperType;
      // helperType syncHelper;
        pico_concurrent_list <queueType> responseQueue_;
        std::shared_ptr<pico_concurrent_list<std::shared_ptr<pico_record>>> bufferQueuePtr_;
        
        bool clientIsConnected;

       
        std::mutex writeOneBufferMutex;   // mutex for the condition variable
        std::mutex allowedToWriteLockMutex;
        std::mutex queueRequestMessagesMutex;
        std::mutex responseQueueMutex;
        std::mutex  waitForClientToConnectMutex;
        
        std::condition_variable clientIsConnectedCV;
        std::condition_variable clientIsAllowedToWrite;
        std::condition_variable bufferQueueIsEmpty;
        std::condition_variable responseQueueIsEmpty;
        
      
    public:
        PonocoDriver(helperType syncHelperArg )
       // syncHelper ( syncHelperArg),
        :bufferQueuePtr_(new pico_concurrent_list<std::shared_ptr<pico_record>>)
          {
//            syncHelper = syncHelperArg;
              boost::unique_lock<std::mutex> waitForClientToConnectLock(waitForClientToConnectMutex);
            //clientIsConnectedCV.wait(waitForClientToConnectLock);
//            while(!clientIsConnected)
//            {
//                //wait until client is connected
//            }
            mylogger<< "Ponoco Instance is initializing  ";
        
        }
//        PonocoDriver(std::shared_ptr<PonocoDriverHelper> syncHelperArg) {
//            
//           
//            syncHelper = syncHelperArg;
//            mylogger<< " Ponoco Driver is initializing ";
//            
//        }
        
        void start_connect(std::shared_ptr<tcp::socket> socket,tcp::resolver::iterator endpoint_iter) {
            try{
                 socket_ = socket;
                mylogger<<" start_connect(tcp::resolver::iterator endpoint_iter) ";
                if (endpoint_iter != tcp::resolver::iterator()) {
                    mylogger<<  "Trying "<<endpoint_iter->endpoint() << "...\n";
                    
                    // Start the asynchronous connect operation.
                    socket_->async_connect(endpoint_iter->endpoint(),
                                           boost::bind(&PonocoDriver::start, this, _1, endpoint_iter));
                } else {
                    // There are no more endpoints to try. Shut down the client.
                    //				stop();
                }
                mylogger<<(  " start_connect ends" );
            }catch(...)
            {
                std::cerr << "client start_connect : Exception: unknown thrown" << "\n";
                raise(SIGABRT);
                
            }
        }
        void handle_connect(const boost::system::error_code& ec,
                            tcp::resolver::iterator endpoint_iter) {
            //			if (stopped_)
            //				return;
            
            // The async_connect() function automatically opens the socket at the start
            // of the asynchronous operation. If the socket is closed at this time then
            // the timeout handler must have run first.
            if (!socket_->is_open()) {
                mylogger<<(  "Connect timed out\n");
                
                // Try the next available endpoint.
                start_connect(this->socket_,++endpoint_iter);
            }
        }
        
        void start(const boost::system::error_code& ec,
                   tcp::resolver::iterator endpoint_iter) {
            //            mylogger<<"\nclient starting the process..going to write_message to server\n";
            
            try {
                if(!ec)
                {
                    //clientIsConnectedCV.notify_all();
                    clientIsConnected=true;
                    writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
                }
                else{
                    mylogger<<"\nclient : start : error is "<<ec.value()<<" error message is "<<ec.message();
                    mylogger <<"\n error name is "<< ec.category().name();}
                
            } catch (const std::exception& e) {
                mylogger<<" exception : "<<e.what();
                raise(SIGABRT);
                
            } catch (...) {
                mylogger<< "<----->unknown exception thrown.<------>\n";
                raise(SIGABRT);
                
            }
        }
        //	void readAsync() {
        ////		while (true) {
        //		mylogger<<(" client is going to read asynchronously..\n";
        //		read_messages();
        //
        //		//boost::this_thread::sleep(boost::posix_time::seconds(4));
        //
        //	}
        
        
        void readSynchronously() {
            for (;;) {
                boost::array<char, 128> buf;
                boost::system::error_code error;
                
                size_t len = socket_->read_some(boost::asio::buffer(buf), error);
                
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.
                
                mylogger << "\nclient got this " << buf.data();
            }
        }
        void readOneBuffer() {
            
            auto self(shared_from_this());
            // mylogger<<"client is trying to read one buffer\n" ;
            std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();
            
            boost::asio::async_read(*socket_,
                                    boost::asio::buffer(currentBuffer->getDataForRead(),
                                                        pico_record::max_size),
                                    [this,self,currentBuffer](const boost::system::error_code& error,
                                                              std::size_t t ) {
                                        
                                        self->processTheMessageJustRead(currentBuffer,t);
                                    });
            
        }
        
        void processTheMessageJustRead(std::shared_ptr<pico_record> currentBuffer,std::size_t t){
//            currentBuffer->loadTheKeyValueFromData();
            string str =currentBuffer->toString();
            
            mylogger<<"\n client : this is the message that client read just now "<<str;
            
            if(sendmetherestofdata(str))
                ignoreThisMessageAndWriterNextBuffer();
            else{
                if(pico_record::find_last_of_string(currentBuffer))
                {
                    mylogger<<"\Client : this buffer is an add on to the last message..dont process anything..read the next buffer\n";
                    allBuffersReadFromTheOtherSide.append(*currentBuffer);
                    tellHimSendTheRestOfData(currentBuffer->messageId);
                }
                else {
                    

                    
                    allBuffersReadFromTheOtherSide.append(*currentBuffer);
                    
                    
                    pico_message util;
                    pico_message last_read_message = util.convertBuffersToMessage(allBuffersReadFromTheOtherSide);
                    mylogger<<"\nsever : this is the complete message read from server "<<last_read_message.toString();
                    processDataFromOtherSide(last_read_message);
                    allBuffersReadFromTheOtherSide.clear();
                    
                }
                
            }
        }
        
        bool sendmetherestofdata( string comparedTo)
        {
            string ignore("sendmetherestofdata");
            
            if(comparedTo.compare(ignore)==0 || comparedTo.empty())
                return true;
            return false;
        }
        void processDataFromOtherSide(pico_message messageFromOtherSide) {
            
            try {
                mylogger<<"\nthis is the complete message from server : "<<messageFromOtherSide.toString();
                //process the data from server and queue the right message or dont
                
                // TODO
                responseProcessor.processResponse(messageFromOtherSide);
                queueTheResponse(messageFromOtherSide);
              
                  mylogger<<"\n after queueing response  : "<<messageFromOtherSide.toString();
                 writeOneBuffer();
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
        }
        void tellHimSendTheRestOfData(string messageId)
        {
            mylogger<<"\nClient is telling send the rest of data for this message Id  "<<messageId<<" \n";
            string msg("sendmetherestofdata");
            
            pico_message reply = pico_message::build_message_from_string(msg,messageId);
          	queueRequestMessages(reply);
            writeOneBuffer(); //go to writing mode
           
        }
        void  ignoreThisMessageAndWriterNextBuffer()
        {
            writeOneBuffer();
            
        }
        void print(const boost::system::error_code& error,
                   std::size_t t,string& str)
        {
            mylogger<<"\nClient Received :  "<<t<<" bytes from server ";
              if(error) mylogger<<" error msg : "<<error.message()<<" data  read from server is "<<str<<"-------------------------\n";
        }
       
        //this is the driver function thats part of driver api
        void insert(std::string key,std::string value){
            
            string command("insert");
            string database("currencyDB");
            string user("currencyUser");
            string col("currencyCollection");
            
            queueType msg (key,value,command,database,user,col );
            queueRequestMessages(msg);
            //            queueType msgReadFromQueue = commandQueue_.pop();
            //            mylogger<<"this is to test if queue works fine"<<endl<<"queue item is "<<msgReadFromQueue.toString()<<endl<<msgReadFromQueue.key_of_message<<" " <<msgReadFromQueue.value_of_message<<endl<<msgReadFromQueue.command<<endl<<msgReadFromQueue.collection<<endl;
            
        }
        
        void deleteTest(std::string key,std::string value){
            
            string command("delete");
            string database("currencyDB");
            string user("currencyUser");
            string col("currencyCollection");
            
            queueType msg (key,value,command,database,user,col );
            queueRequestMessages(msg);
            //            queueType msgReadFromQueue = commandQueue_.pop();
            //            mylogger<<"this is to test if queue works fine"<<endl<<"queue item is "<<msgReadFromQueue.toString()<<endl<<msgReadFromQueue.key_of_message<<" " <<msgReadFromQueue.value_of_message<<endl<<msgReadFromQueue.command<<endl<<msgReadFromQueue.collection<<endl;
            
        }
        
        void update(std::string key,std::string oldValue,std::string newValue){
            
            string command("update");
            string database("currencyDB");
            string user("currencyUser");
            string col("currencyCollection");
            
            queueType msg (key,oldValue,newValue,command,database,user,col );
            queueRequestMessages(msg);
            
            //            queueType msgReadFromQueue = commandQueue_.pop();
            //            mylogger<<"this is to test if queue works fine"<<endl<<"queue item is "<<msgReadFromQueue.toString()<<endl<<msgReadFromQueue.key_of_message<<" " <<msgReadFromQueue.value_of_message<<endl<<msgReadFromQueue.command<<endl<<msgReadFromQueue.collection<<endl;
            
        }
        std::string get(std::string key,double userTimeOut=10){
            
            string command("get");
            string database("currencyDB");
            string user("currencyUser");
            string col("currencyCollection");
            string oldValue("");
            string newValue("");
            queueType msg (key,oldValue,newValue,command,database,user,col );
            queueRequestMessages(msg);
            
            steady_clock::time_point t1 = steady_clock::now(); //time that we started waiting for result
            mylogger<<"Client : waiting for our response from server...msg.messageId = "<< msg.messageId<< " \n";
            std::unique_lock<std::mutex> responseQueueIsEmptyLock(responseQueueMutex);
            while(true)
            {
                
                while(responseQueue_.empty())
                {
                    mylogger<<"Client : waiting for our responseQueue_ to be filled again 1 !\n";
                    responseQueueIsEmpty.wait(responseQueueIsEmptyLock);
                }
                queueType response = responseQueue_.pop();
                mylogger<<"Client : response.requestId"<<response.messageId<<"\n"<<
                "msg.requestId is "<<response.messageId<<"\n";
                if(response.messageId==msg.messageId)
                {
                    //this is our response
                    mylogger<<"Client : got our response"<<response.messageId<<"\n"<<
                    "this is our response "<<response.value;
                   
                    return response.value;
                }
                else
                {
                    steady_clock::time_point t2 = steady_clock::now();//time that we are going to check to determine timeout
                    
                    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
                    double timeoutInSeconds =  time_span.count();
                    if(timeoutInSeconds>=userTimeOut)
                    {
                        //we ran out of time, get failed....
                        mylogger<<"Client : get Operation TIMED OUT!!\n";
                        break;
                      
                    }
                    else{
                        
                    }
                    
                }
                
            }//while
        
            std::string timeout("OPERATION TIMED OUT!");
            return timeout;
            
            
        }
        void writeOneBuffer()
        //boost::unique_lock<std::mutex> writeOneBufferLock)
        {

              mylogger<<"client : writeOneBuffer BEFORE getting the lock ...\n";
             std::unique_lock<std::mutex> writeOneBufferLock(writeOneBufferMutex);
            
            while(bufferQueuePtr_->empty())
            {
                
                mylogger<<"client : bufferQueue is empty..waiting ...\n";
       
                    bufferQueueIsEmpty.wait(writeOneBufferLock);
                    mylogger<<"client : bufferQueue waking up because there is some data in the queue ...\n";
            }
            
             mylogger<<"client : is going to send some data over ...\n";
            std::shared_ptr<pico_record> currentBuffer =bufferQueuePtr_->pop();
            
            char* data = currentBuffer->getDataForWrite();
            std::size_t dataSize = currentBuffer->getSize();
            
            auto self(shared_from_this());
            
            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                     [this,self,currentBuffer](const boost::system::error_code& error,
                                                               std::size_t t) {
                                         string str = currentBuffer->toString();
                                         
                                                                                  mylogger<<t<<" bytes to server \n";
                                         if(error)
                                             mylogger<<"\n error msg : "<<error.message();
                                         
                                         mylogger<<"\ndata sent to server is "<<str;
                                         readOneBuffer();
                                     });
            
            
            
            
        }
        void queueTheResponse(pico_message msg)
        
        {
            mylogger<<"client : putting the response in the queue "<<msg.toString();
            responseQueue_.push(msg);
            mylogger<<"\n client : response pushed to responseQUEUE \n";

            responseQueueIsEmpty.notify_all();
            
        }
        void queueRequestMessages(queueType message) {
            //TODO put a lock here to make the all the buffers in a message go after each other.
            try{
                
                    boost::unique_lock<std::mutex> writeOneBufferMutexLock(writeOneBufferMutex);
                        while(!message.recorded_message.msg_in_buffers->empty())
                        {
                            
                            mylogger<<"\nPonocoDriver : queueRequestMessages : : popping current Buffer \n";
                            pico_record buf = message.recorded_message.msg_in_buffers->pop();
                            mylogger<<"PonocoDriver : popping current Buffer this is current buffer a and pushing it to the bufferQueue to send \n "<<buf.toString();
                            
                            std::shared_ptr<pico_record> curBufPtr(new pico_record(buf));
                            bufferQueuePtr_->push(curBufPtr);
                        }

                bufferQueueIsEmpty.notify_all();
            }catch (...) {
                std::cerr << "Exception: queueRequestMessages  message : unknown thrown" << "\n";
                raise(SIGABRT);
            
            }
        }
        pico_buffered_message<pico_record> allBuffersReadFromTheOtherSide;
        
        asyncReader asyncReader_;
    };//end of class
} //end of namespace
#endif
