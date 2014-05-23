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
#include <atomic>
#include <pico/ConcurrentVector.h>

using boost::asio::ip::tcp;
using namespace std;
namespace pico {
    
    class pico_session: public std::enable_shared_from_this<pico_session>,
    public pico_logger_wrapper {
        
    private:
        std::shared_ptr<tcp::socket> socket_;
        typedef msgPtr queueType;
        std::atomic<long> messageNumber;
    public:
        static string logFileName;
        
        
        
        //constructor
        pico_session(std::shared_ptr<tcp::socket> r_socket) :shutDownNormally(false),messageNumber(0){
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
                raise (SIGABRT);
            }
        }
        void readOneBuffer() {
            if (sessionLogger->isTraceEnabled()) {
                *sessionLogger<< "\n Session is going to read a buffer from client...\n";
            }
            
            //we read until the whole message is read
            //then we write until the whole message is written
            auto self(shared_from_this());
            assert(self);
            //  cout << "session is trying to read messages" << endl;
            std::shared_ptr<pico_record> currentBuffer =
            asyncReader_.getOneBuffer();
            if (sessionLogger->isTraceEnabled()) {
                *sessionLogger<< "\n Session is going to call the async read...\n";
            }
            boost::asio::async_read(*socket_,
                                    boost::asio::buffer(currentBuffer->getDataForRead(),
                                                        pico_record::max_size),
                                    [this,self,currentBuffer](const boost::system::error_code& error,
                                                              std::size_t t ) {
                                        if (sessionLogger->isTraceEnabled()) {
                                            *sessionLogger<< "\n Session is done reading the buffer...\n";
                                        }
                                        processTheBufferJustRead(currentBuffer,t);
                                        
                                    });
        }
        
        void writeOneBuffer() {
            if (sessionLogger->isTraceEnabled()) {
                *sessionLogger<< "\nSession is going to send a buffer to client..going to acquire lock \n";
            }
            
            std::unique_lock < std::mutex
            > writeOneBufferLock(bufferQueueIsEmptyMutex);
            while (bufferQueue_.empty()) {
                if (sessionLogger->isTraceEnabled()) {
                    *sessionLogger<< "\nSession bufferQueue_ is empty...waiting for a message to come to queue. \n";
                }
                bufferQueueIsEmpty.wait(writeOneBufferLock);
            }
            if (sessionLogger->isTraceEnabled()) {
                *sessionLogger<< "\nSession is going to send a buffer to client.. lock obtained \n";
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
                                         if(sessionLogger->isTraceEnabled())
                                         {
                                             *sessionLogger<< "\nSession Sent :  "<<t<<" bytes from Client ";
                                             if(error) *sessionLogger<<" \nSession : a communication error happend...\n msg : "<<error.message();
                                             *sessionLogger<<" nSession : data sent to client is "<<str<<"\n";
                                             *sessionLogger<<("-------------------------");
                                         }
                                         
                                         assert(t!=0);
                                         
                                         if(pico_record::IsThisRecordAnAddOn(*currentBuffer)) //write to client until
                                             //buffer is an addon,if its not, go to reading mode to and wait for other requests
                                         {
                                             writeOneBuffer();
                                             if(sessionLogger->isTraceEnabled())
                                             {
                                                 *sessionLogger<<"\n nSession :I am going to send the next incomplete buffer \n";
                                             }
                                         } else {
                                             *sessionLogger<<"\n nSession :I am done sending a complete message now going to wait for the next requests \n";
                                             readOneBuffer();
                                         }
                                     });
            
        }
        
        void queueMessages(queueType message) {
            //TODO put a lock here to make the all the buffers in a message go after each other.
            while (true) {
                std::unique_lock < std::mutex
                > queueMessagesLock(queueMessagesMutext);
                //put all the buffers in the message in the buffer queue
                pico_buffered_message<pico_record> msg_in_buffers =
                message->getCompleteMessageInJsonAsBuffers();
                while (!msg_in_buffers.empty()) {
                    
                    *sessionLogger << "pico_session : popping current Buffer ";
                    pico_record buf = msg_in_buffers.pop();
                    //                    sessionLogger<<"PonocoDriver : popping current Buffer this is current buffer ";
                    
                    std::shared_ptr<pico_record> curBufPtr(new pico_record(buf));
                    bufferQueue_.push_back(curBufPtr);
                    
                }
                bufferQueueIsEmpty.notify_all();
                break;
            }
            
        }
        
        void readingAndWritingRecordData() {
            
            string key1 = "keyfromkey1";
            string value1 = "valuefromvalue1";
            string key2 = "key2";
            string value2 = "value2";
            //            pico_record x1(key1, value1);
            //
            //            pico_record x2(key2, value2);
            pico_collection optionCollection("options-collection");
            
            //            for (int i = 0; i < 10; i++) {
            //                optionCollection.insert(x1);
            //            }
            cout << " number of records are : "
            << optionCollection.getNumberOfMessages() << " \n";
            //            cout << " record 4 : " << optionCollection.get(3).toString() << " \n";
            //            cout << " record 4 : " << optionCollection.get(3).toString() << " \n";
            //            optionCollection.update(x1, x2);
            //optionCollection.deleteRecord(x1);
            
            *sessionLogger << "\nend of function readingAndWritingRecordData() ";
            
        }
        void processDataFromOtherSide(msgPtr messageFromOtherSide) {
            
            try {
                if (sessionLogger->isTraceEnabled()) {
                    *sessionLogger << "\nSession read this message from client "
                    << messageFromOtherSide->toString() << "...\n";
                }
                
                msgPtr reply = requestProcessor_.processRequest(
                                                                messageFromOtherSide);
                if (sessionLogger->isTraceEnabled()) {
                    
                    *sessionLogger
                    << "\nSession : putting reply to the queue this is the message that Session read just now"
                    << reply->toString();
                }
                
                assert(!reply->toString().empty());
                
                queueMessages(reply);
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
            
        }
        
        void processTheBufferJustRead(std::shared_ptr<pico_record> currentBuffer,
                                      std::size_t t) {
            messageNumber++;
            string str = currentBuffer->toString();
            *sessionLogger << "\nsession : allBuffersReadFromTheOtherSide is  \n"<<allBuffersReadFromTheOtherSide.toString();
            
            allBuffersReadFromTheOtherSide.append(*currentBuffer);
            
            *sessionLogger << "\nsession : this is the "<<messageNumber.load()<<"th message that Session read just now "<< str;
            
            if (pico_record::IsThisRecordAnAddOn(*currentBuffer)) {
                *sessionLogger
                << "\nsession: this buffer is an add on to the last message.."
                << "dont process anything..read the next buffer\n";
                
                
                
                readOneBuffer();		//read the next addon buffer
            } else if(!str.empty() && str.compare("")!=0 ) {
                
                *sessionLogger << "\nsession : is going to call the convert_records_to_message \n";
                assert(allBuffersReadFromTheOtherSide.size()>0);
                *sessionLogger << "\nsession : These are the input for  convert_records_to_message  function \n";
                allBuffersReadFromTheOtherSide.print();
                pico_message util;
                std::shared_ptr<pico_message> last_read_message = util.convert_records_to_message(
                                                                                                  allBuffersReadFromTheOtherSide,
                                                                                                  currentBuffer->getMessageIdAsString(),
                                                                                                  COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY,sessionLogger);
                assert(!last_read_message->toString().empty());
                *sessionLogger << "\nsession : this is the complete message read from Client "
                << last_read_message->toString();
                
                processDataFromOtherSide(last_read_message);
                allBuffersReadFromTheOtherSide.clear();
                writeOneBuffer(); //going to writing mode to write the reply for this complete message
                
            }else
            {
                *sessionLogger << "\nsession : ERROR : reading empty message  \n";
                readOneBuffer();		//read the next addon buffer
                
            }
            
        }
        
        void print(const boost::system::error_code& error, std::size_t t,
                   string& str) {
            //  if(error) sessionLogger<<" error msg : "<<error.message()<<std::endl;
            if (sessionLogger->isTraceEnabled()) {
                
                *sessionLogger << "\nSession received " << t
                << " bytes read from Client  data read from client is "
                << str;
            }
        }
        virtual ~pico_session()
        {
            
            *sessionLogger << "\n Session going out of scope "
            ;
            assert(shutDownNormally);
            
        }
        bool shutDownNormally;
        asyncReader asyncReader_;
        request_processor requestProcessor_;
        ConcurrentVector<queueType,list_traits<pico_message>> messageToClientQueue_;
        ConcurrentVector<std::shared_ptr<pico_record>,list_traits<pico_record>> bufferQueue_; //bufferQueue should containt pointer because each data should be in heap until the data is read completely and it should be raw pointer because shared pointer will go out of scope
        
        std::mutex sessionMutex;   // mutex for the condition variable
        std::mutex bufferQueueIsEmptyMutex;
        std::mutex queueMessagesMutext;
        std::condition_variable bufferQueueIsEmpty;
        
        std::condition_variable clientIsAllowedToWrite;
        
        pico_buffered_message<pico_record> allBuffersReadFromTheOtherSide;
        
        
    };
}

#endif /* CLIENTHANDLER_H_ */
