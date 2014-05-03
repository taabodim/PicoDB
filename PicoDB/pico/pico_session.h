  /*
 * ClientHandler.h
 *
 *  Created on: Mar 11, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_

#include "pico/pico_record.h"
#include "pico/pico_collection.h"
#include "pico/writer_buffer_container.h"
#include "pico/asyncReader.h"
#include "pico/request_processor.h"
#include <pico/pico_utils.h>
#include <pico_logger_wrapper.h>
#include <pico_buffered_message.h>
using boost::asio::ip::tcp;
using namespace std;
namespace pico {
    
    class pico_session: public std::enable_shared_from_this<pico_session> , public pico_logger_wrapper {
        
    private:
        std::shared_ptr<tcp::socket> socket_;
        typedef pico_message queueType;
    public:
        static string logFileName;
        pico_session(std::shared_ptr<tcp::socket> r_socket)
         {
            socket_ = r_socket;
        }
        
        
        void start() {
            
            try {
                cout << " session started already..\n";
                readOneBuffer();
            } catch (const std::exception& e) {
                cout << " exception : " << e.what() << endl;
            } catch (...) {
                
                cout << "<----->unknown exception thrown.<------>";
                raise(SIGABRT);
            }
        }
        void readOneBuffer() {
            //we read until the whole message is read
            //then we write until the whole message is written
            auto self(shared_from_this());
          //  cout << "session is trying to read messages" << endl;
            std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();
            boost::asio::async_read(*socket_,
                                    boost::asio::buffer(currentBuffer->getDataForRead(),
                                                        pico_record::max_size),
                                    [this,self,currentBuffer](const boost::system::error_code& error,
                                                              std::size_t t ) {
                                        
                                        processTheBufferJustRead(currentBuffer,t);
                                        writeOneBuffer();
                                        
                                    });
        }
        void queueMessages(queueType message) {
            //TODO put a lock here to make the all the buffers in a message go after each other.
            while(true)
            {
                std::unique_lock<std::mutex> queueMessagesLock(queueMessagesMutext);
            //put all the buffers in the message in the buffer queue
            while(!message.recorded_message.msg_in_buffers->empty())
            {
                
                mylogger<<"pico_session : popping current Buffer ";
                pico_record buf = message.recorded_message.msg_in_buffers->pop();
                //                    mylogger<<"PonocoDriver : popping current Buffer this is current buffer ";
                
                std::shared_ptr<pico_record> curBufPtr(new pico_record(buf));
                bufferQueue_.push(curBufPtr);
                
            }
            bufferQueueIsEmpty.notify_all();
                break;
            }
            
            }
        
        void writeOneBuffer()
        {
            
            std::unique_lock<std::mutex> writeOneBufferLock(bufferQueueIsEmptyMutex);
            while(bufferQueue_.empty())
            {
             //   cout<<"session : bufferQueue is empty..waiting ..."<<endl;
                bufferQueueIsEmpty.wait(writeOneBufferLock);
            }
            
            
            std::shared_ptr<pico_record> currentBuffer = bufferQueue_.pop();
           // cout << " session is writing one buffer to client : " <<currentBuffer->toString()//<< std::endl;
            char* data = currentBuffer->getDataForWrite();
            std::size_t dataSize = currentBuffer->getSize();
            auto self(shared_from_this());
            
            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                     [this,self,currentBuffer](const boost::system::error_code& error,
                                                               std::size_t t) {
                                         string str = currentBuffer->toString();
                                         mylogger << "\nSession Sent :  "<<t<<" bytes from Client ";
                                         //                                         if(error) mylogger<<" error msg : "<<error.message()<<std::endl;
                                         //                                         mylogger<<" data sent to client is "<<str<<endl;
                                         mylogger<<("-------------------------");
                                         
                                         readOneBuffer();
                                     });
            
        }
        
       
        void readingAndWritingRecordData() {
            
            string key1 = "keyfromkey1";
            string value1 = "valuefromvalue1";
            string key2 = "key2";
            string value2 = "value2";
            pico_record x1(key1, value1);
            
            pico_record x2(key2, value2);
            pico_collection optionCollection("options-collection");
            
            for (int i = 0; i < 10; i++) {
                optionCollection.insert(x1);
            }
            cout << " number of records are : "
            << optionCollection.getNumberOfMessages() << " \n";
//            cout << " record 4 : " << optionCollection.get(3).toString() << " \n";
//            cout << " record 4 : " << optionCollection.get(3).toString() << " \n";
//            optionCollection.update(x1, x2);
            //optionCollection.deleteRecord(x1);
            
            mylogger << "\nend of function readingAndWritingRecordData() "
            ;
            
        }
        void processDataFromClient(pico_message messageFromClient) {
            
            try {
                
                
                pico_message reply = requestProcessor_.processRequest(messageFromClient);
                
                queueMessages(reply);
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
            
        }
        
        void processTheBufferJustRead(std::shared_ptr<pico_record> currentBuffer,std::size_t t){
//            currentBuffer->loadTheKeyValueFromData();
            string str =currentBuffer->toString();
            
            mylogger<<"\nsession : this is the message that server read just now "<<str;
            
            if(sendmetherestofdata(str))
                ignoreThisMessageAndWriterNextBuffer();
            
            else
                if(find_last_of_string(currentBuffer))
            {
                mylogger<<"\nsession: this buffer is an add on to the last message..dont process anything..read the next buffer\n";
//                pico_message::removeTheAppendMarker(currentBuffer);
//                string strWithoutJunk =currentBuffer->toString();
//                append_to_last_message(strWithoutJunk);
                allBuffersReadFromClient.append(*currentBuffer);
                tellHimSendTheRestOfData();
            }
            else {
                
//                pico_message::removeTheAppendMarker(currentBuffer);
//                string strWithoutJunk =currentBuffer->toString();
//                append_to_last_message(strWithoutJunk);
                  allBuffersReadFromClient.append(*currentBuffer);
                
                
                    pico_message last_read_message = pico_message::convertBuffersToMessage(allBuffersReadFromClient);
                mylogger<<"\nsever : this is the complete message read from  "<<last_read_message.toString();
              
                processDataFromClient(last_read_message);
                
            }
        }
        bool sendmetherestofdata(string comparedTo)
        {
            string ignore("sendmetherestofdata");
            
            if(comparedTo.compare(ignore)==0 || comparedTo.empty())
                return true;
            return false;
        }
        static bool find_last_of_string(std::shared_ptr<pico_record> currentBuffer)
        {
            
            
            int pos = pico_record::max_size-1;
            if(currentBuffer->data_[pos] != 'd' ||
               currentBuffer->data_[--pos] != 'n' ||
               currentBuffer->data_[--pos] != 'e' ||
               currentBuffer->data_[--pos] != 'p' ||
               currentBuffer->data_[--pos] != 'p' ||
               currentBuffer->data_[--pos] != 'a' )
                return false;
            
            
            return true;
            
        }
        void tellHimSendTheRestOfData()
        {
            string msg("sendmetherestofdata");
            
            pico_message reply = pico_message::build_message_from_string(msg);
          	queueMessages(reply);
            
        }
        void  ignoreThisMessageAndWriterNextBuffer()
        {
            writeOneBuffer();
            
        }
       
        void print(const boost::system::error_code& error,std::size_t t,string& str)
        {
          //  if(error) mylogger<<" error msg : "<<error.message()<<std::endl;
            mylogger << "\nServer received "<<t<<" bytes read from Client  data read from client is "<<str;
            
        }
//        void append_to_last_message(string str) {
//            last_read_message.append(str);
//            
//        }
        
        //        writer_buffer_container writer_buffer_container_;
        asyncReader asyncReader_;
        request_processor requestProcessor_;
        pico_concurrent_list<queueType> messageToClientQueue_;
        pico_concurrent_list<std::shared_ptr<pico_record>> bufferQueue_; //bufferQueue should containt pointer because each data should be in heap until the data is read completely and it should be raw pointer because shared pointer will go out of scope
      
        std::mutex sessionMutex;   // mutex for the condition variable
        std::mutex bufferQueueIsEmptyMutex;
        std::mutex queueMessagesMutext;
        std::condition_variable bufferQueueIsEmpty;
        
        std::condition_variable clientIsAllowedToWrite;
        
        
        pico_buffered_message<pico_record> allBuffersReadFromClient;
        
        
        
        
        
        
        //        void readWriteSync() {
        //
        //            string msgFromClient = read_messages_sync();
        //            if (msgFromClient.compare("insert") == 0) {
        //                cout << "server recienved insert message from client" << endl;
        //                //insertData to client
        //
        //                mylogger << "server reading message : " << msgFromClient
        //                //<< std::endl;
        //                write_messages_sync();
        //                boost::this_thread::sleep(boost::posix_time::seconds(4));
        //            }
        //        }
        //
        //        void write_messages_sync() {
        //
        //        }
        //        string read_messages_sync() {
        //
        //            //		char* data = buffer.getData();
        //            //		std::size_t dataSize = buffer.getSize();
        //            //
        //            //		mylogger << "data to send is : " << data //<< std::endl;
        //            //		mylogger << "dataSize to send is : " << dataSize //<< std::endl;
        //            //
        //            //		boost::array<char, 128> buf;
        //            //		boost::system::error_code error;
        //            //
        //            //		size_t len = socket_->read_some(boost::asio::buffer(buf), error);
        //            //		return buffer.getString();
        //            string empty;
        //            return empty;
        //        }
        
        //        write_messages_async_normal(){
        //		if (messageToClientQueue_.empty())
        //			messageClientQueueIsEmpty.wait(writeMessageLock);
        //
        //		cout << " session : messageToClientQueue_ size is "
        //				<< messageToClientQueue_.size() << endl;
        //		string messageToClient = messageToClientQueue_.front();
        //		messageToClientQueue_.pop_front();
        //		msgPtr currentBuffer = writer_buffer_container_.getWriteBuffer();
        //		currentBuffer->setData(messageToClient);
        //		auto self(shared_from_this());
        //		boost::asio::async_write(*socket_,
        //				boost::asio::buffer(currentBuffer->getData(),
        //						currentBuffer->getSize()),
        //				[this,self,currentBuffer](const boost::system::error_code& error,
        //						std::size_t t) {
        //
        //					string str = currentBuffer->getString();
        //					mylogger << "Server sent "<<std::endl;
        //					mylogger<<(t<<" bytes sent to client "<<std::endl;
        //					if(error) mylogger<<(" error msg : "<<error.message()<<std::endl;
        //					mylogger<<( " data sent to client is "<<str<<std::endl;
        //					mylogger << "-------------------------"<<std::endl;
        //					read_messages();
        //				});
        
        //        }
    };
}

#endif /* CLIENTHANDLER_H_ */
