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
#include <pico/pico_buffer.h>
#include <pico/writer_buffer_container.h>
#include <pico/asyncReader.h>
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
#include <pico/pico_session.h> //for the checking if appended function
#include <pico/pico_test.h>
#include <pico/pico_logger_wrapper.h>
#include <ClientResponseProcessor.h>

using boost::asio::ip::tcp;
using namespace std;
using namespace std::chrono;



namespace pico {
    
    class PonocoDriver;
    typedef PonocoDriver DriverType;
    typedef pico_message queueType;
    class PonocoDriver: public std::enable_shared_from_this<PonocoDriver>, public pico_logger_wrapper {
    private:
        
        
        socketType socket_;
        pico_concurrent_list <queueType> commandQueue_;
        pico_concurrent_list <queueType> responseQueue_;
        ClientResponseProcessor responseProcessor;
    public:
        static string logFileName;
        
        PonocoDriver(socketType socket) :
        writeOneBufferLock(sessionMutex),allowedToWriteLock(allowedToWriteLockMutex),
        responseQueueIsEmptyLock(responseQueueMutex){
            
            socket_ = socket;
            mylogger<<( "client initializing ");
            
        }
        
        void start_connect(tcp::resolver::iterator endpoint_iter) {
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
                start_connect(++endpoint_iter);
            }
        }
        
        void start(const boost::system::error_code& ec,
                   tcp::resolver::iterator endpoint_iter) {
            //            mylogger<<"\nclient starting the process..going to write_message to server\n";
            
            try {
                if(!ec)
                {
                    runTestCase();
                    writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
                }
                else{
                    mylogger<<"\nclient : start : error is "<<ec.value()<<" error message is "<<ec.message();
                    mylogger <<"\n error name is "<< ec.category().name();}
                
            } catch (const std::exception& e) {
                mylogger<<" exception : "<<e.what();
            } catch (...) {
                mylogger<< "<----->unknown exception thrown.<------>\n";
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
            bufferTypePtr currentBuffer = asyncReader_.getOneBuffer();
            
            boost::asio::async_read(*socket_,
                                    boost::asio::buffer(currentBuffer->getData(),
                                                        pico_buffer::max_size),
                                    [this,self,currentBuffer](const boost::system::error_code& error,
                                                              std::size_t t ) {
                                        
                                        self->processTheMessageJustRead(currentBuffer,t);
                                    });
            
        }
        
        void processTheMessageJustRead(bufferTypePtr currentBuffer,std::size_t t){
            
            string str =currentBuffer->toString();
            
            mylogger<<"\n client : this is the message that client read just now "<<str;
            
            if(sendmetherestofdata(str))
                ignoreThisMessageAndWriterNextBuffer();
            else
                if(pico_session::find_last_of_string(currentBuffer))
                {
                    mylogger<<("\nsession: this buffer is an add on to the last message..dont process anything..read the next buffer\n");
                    pico_message::removeTheEndingTags(currentBuffer);
                    string strWithoutJunk =currentBuffer->toString();
                    append_to_last_message(strWithoutJunk);
                    tellHimSendTheRestOfData();
                }
                else {
                    
                    pico_message::removeTheEndingTags(currentBuffer);
                    string strWithoutJunk =currentBuffer->toString();
                    append_to_last_message(strWithoutJunk);
                    
                    
                    mylogger<<"\nthis is the complete message read from session :"<<last_read_message;
                    
                    processDataFromOtherSide(last_read_message);
                    last_read_message.clear();
                    
                }
        }
        
        bool sendmetherestofdata( string comparedTo)
        {
            string ignore("sendmetherestofdata");
            
            if(comparedTo.compare(ignore)==0 || comparedTo.empty())
                return true;
            return false;
        }
        void processDataFromOtherSide(std::string msg) {
            
            try {
                mylogger<<"\nthis is the complete message from server : "<<msg;
                //process the data from server and queue the right message or dont
                
                // TODO
                responseProcessor.processResponse(msg);
                queueTheResponse(msg);
                writeOneBuffer();
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
        }
        void tellHimSendTheRestOfData()
        {
            string msg("sendmetherestofdata");
            
            pico_message reply = pico_message::build_message_from_string(msg);
          	queueRequestMessages(reply);
        }
        void  ignoreThisMessageAndWriterNextBuffer()
        {
            writeOneBuffer();
            
        }
        void print(const boost::system::error_code& error,
                   std::size_t t,string& str)
        {
            mylogger<<"\nClient Received :  "<<t<<" bytes from server ";
            //  if(error) mylogger<<" error msg : "<<error.message()<<" data  read from server is "<<str<<"-------------------------"<<std::endl;
        }
        void append_to_last_message(string str) {
            last_read_message.append(str);
            
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
            mylogger<<"Client : waiting for our response from server !\n";
            
            while(true)
            {
                while(responseQueue_.empty())
                {
                    responseQueueIsEmpty.wait(responseQueueIsEmptyLock);
                }
                queueType response = responseQueue_.peek();
                mylogger<<"Client : response.requestId"<<response.requestId<<"\n"<<
                "msg.requestId is "<<msg.requestId<<"\n";
                if(response.requestId==msg.requestId)
                {
                    //this is our response
                    mylogger<<"Client : got our response"<<response.requestId<<"\n"<<
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
        {
            
            if(bufferQueue_.empty())
            {
                
                mylogger<<"client : bufferQueue is empty..waiting ...\n";
                
                bufferQueueIsEmpty.wait(writeOneBufferLock);
            }
            bufferTypePtr currentBuffer = bufferQueue_.pop();
            char* data = currentBuffer->getData();
            std::size_t dataSize = currentBuffer->getSize();
            
            auto self(shared_from_this());
            
            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                     [this,self,currentBuffer](const boost::system::error_code& error,
                                                               std::size_t t) {
                                         string str = currentBuffer->toString();
                                         
                                         //                                         mylogger<<t<<" bytes to server "<<std::endl;
                                         if(error)
                                             mylogger<<"\n error msg : "<<error.message();
                                         
                                         mylogger<<"\ndata sent to server is "<<str;
                                         readOneBuffer();
                                     });
            
        }
        void queueTheResponse(queueType message)
        
        {
            mylogger<<"client : putting the response in the queue\n";
            responseQueue_.push(message);
            responseQueueIsEmpty.notify_all();
            
        }
        void queueRequestMessages(queueType message) {
            //TODO put a lock here to make the all the buffers in a message go after each other.
            while(true)
            {
                boost::interprocess::scoped_lock<boost::mutex> queueRequestMessagesLock(queueRequestMessagesMutex, boost::interprocess::try_to_lock);
                
                if (queueRequestMessagesLock)
                {
                    //put all the buffers in the message in the buffer queue
                    while(!message.buffered_message.msg_in_buffers->empty())
                    {
                        
                        mylogger<<"PonocoDriver : popping current Buffer ";
                        bufferType buf = message.buffered_message.msg_in_buffers->pop();
                        //                    mylogger<<"PonocoDriver : popping current Buffer this is current buffer ";
                        
                        std::shared_ptr<pico_buffer> curBufPtr(new pico_buffer(buf));
                        bufferQueue_.push(curBufPtr);
                        
                    }
                    bufferQueueIsEmpty.notify_all();
                    break;
                }//lock obtained
                else
                {
                    mylogger<<"queueRequestMessages was called but unable to get the lock\n";
                    //    return false;
                }
            }
        }
        
        
        
        //dont delete these declarations, later they have to be moved to pico_test class
        //        void insert1SmallKeyBigValueAndGetItAndUpdateItAndDeleteIt();
        //        void getTest(std::string key);
        //        void write1000smallRandomData();
        //        void writeOneDeleteOne();
        //        void writeThe_same_record_to_check_if_we_update_or_insert_multiple();
        //
        //        void writeTenKEY0KEY1KEY2DeleteAllKEY2();
        //        void write1000SmallKeysBigValues_and_deleteAll();
        //        void write1000SmallKeysValues_and_deleteAll();
        
        
        //        writer_buffer_container writer_buffer_container_;
        pico_concurrent_list<bufferTypePtr> bufferQueue_;
        asyncReader asyncReader_;
        boost::mutex sessionMutex;   // mutex for the condition variable
        boost::mutex allowedToWriteLockMutex;
        boost::mutex queueRequestMessagesMutex;
        boost::mutex responseQueueMutex;
        boost::condition_variable bufferQueueIsEmpty;
        boost::condition_variable responseQueueIsEmpty;
        
        boost::condition_variable clientIsAllowedToWrite;
        boost::unique_lock<boost::mutex> allowedToWriteLock;
        boost::unique_lock<boost::mutex> writeOneBufferLock;
        boost::unique_lock<boost::mutex> responseQueueIsEmptyLock;
        
        string last_read_message;
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        /*******************************************************************/
        /*******************************************************************/
        /*******************************************************************/
        /**********************Test Functions**********************************/
        /*******************************************************************/
        /*******************************************************************/
        /*******************************************************************/
        
        
        void insert1SmallKeyBigValue_And_GetIt()
        {
            
            std::string key(pico_test::smallKey0);
            insert(key,pico_test::bigValue0);
            get(key);
            
        }
        void getTest(std::string key)
        {
            
            std::size_t hashcodeOfSentKey =   calc_hash_code(key);
            
        }
        void write1000smallRandomData()
        {
            
            
            for(int  i=0;i<1000;i++)
                insert(random_string(20),random_string(20));
            
        }
        void writeOneDeleteOne()
        {
            for(int  i=0;i<1;i++)
                insert(pico_test::smallKey0,pico_test::smallValue0);
            for(int  i=0;i<1;i++)
                deleteTest(pico_test::smallKey0,pico_test::smallValue0);
            
        }
        void writeThe_same_record_to_check_if_we_update_or_insert_multiple()
        {
            for(int  i=0;i<2;i++)
            {
                insert(pico_test::smallKey0,pico_test::smallValue0);
            }
        }
        
        void writeTenKEY0KEY1KEY2DeleteAllKEY2()
        {
            for(int  i=0;i<1;i++)
            {
                
                insert(pico_test::smallKey0,pico_test::smallValue0);
                insert(pico_test::smallKey1,pico_test::smallValue1);
                insert(pico_test::smallKey2,pico_test::smallValue2);
                insert(pico_test::smallKey2,pico_test::smallValue2);
                insert(pico_test::smallKey2,pico_test::smallValue2);
                insert(pico_test::smallKey0,pico_test::smallValue0);
                insert(pico_test::smallKey1,pico_test::smallValue1);
            }
            for(int  i=0;i<1;i++)
                deleteTest(pico_test::smallKey2,pico_test::smallKey2);
            //
        }
        void write1000SmallKeysBigValues_and_deleteAll()
        {
            for(int  i=0;i<1000;i++)
            {
                std::string key(pico_test::smallKey0);
                key.append(convertToString(i));
                insert(key,pico_test::bigValue0);
            }
            
            //            for(int  i=0;i<1000;i++)
            //            {
            //                std::string key(pico_test::smallKey0);
            //                key.append(convertToString(i));
            //
            //                deleteTest(key,pico_test::smallKey2);
            //            }
            
            
        }
        
        void write1000SmallKeysValues_and_deleteAll()
        {
            for(int  i=0;i<1000;i++)
            {
                std::string key(pico_test::smallKey0);
                key.append(convertToString(i));
                insert(key,pico_test::smallValue0);
            }
            
            for(int  i=0;i<1000;i++)
            {
                std::string key(pico_test::smallKey0);
                key.append(convertToString(i));
                
                deleteTest(key,pico_test::smallKey2);
            }
            
            
        }
        
        
        void currentTestCase()
        {
            
            
            steady_clock::time_point t1 = steady_clock::now();
            
            
            
            // write1000smallRandomData();
            //            writeOneDeleteOne();
            insert1SmallKeyBigValue_And_GetIt();
            //write1000SmallKeysBigValues_and_deleteAll();
            
            
            steady_clock::time_point t2 = steady_clock::now();
            
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            
            std::cout << "****************************************\n";
            std::cout << "\nIt took me " << time_span.count() << " seconds.";
            std::cout << std::endl;
        }
        void runTestCase()
        {
            
            //boost::thread poncoClientThread(boost::bind(&PonocoDriver::currentTestCase,this));
            
        }
        
        
    };//end of class
} //end of namespace
#endif
