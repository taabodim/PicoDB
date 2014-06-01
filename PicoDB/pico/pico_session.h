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
#include <pico/MessageSender.h>
#include <boost/format.hpp>
#include <pico/AtomicCounter.h>
using boost::asio::ip::tcp;
using namespace std;
namespace pico {
    
    class pico_session: public std::enable_shared_from_this<pico_session>,
    public pico_logger_wrapper {
        
    private:
        std::shared_ptr<tcp::socket> socket_;
        typedef msgPtr queueType;
        std::atomic<long> messageNumber;
        MessageSender* messageSender;
        bool shutDownNormally;
        asyncReader asyncReader_;
        request_processor requestProcessor_;
        ConcurrentVector<queueType,VectorTraits<pico_message>> messageToClientQueue_;
        ConcurrentVector<std::shared_ptr<pico_record>,VectorTraits<pico_record>> bufferQueue_;
        ConcurrentVector<string,VectorTraits<string>> listOfAllMessagesReadInOrder;
        //bufferQueue should containt pointer because each data should be in heap until the data is read completely and it should be raw pointer because shared pointer will go out of scope
        
        std::mutex sessionMutex;   // mutex for the condition variable
        std::mutex bufferQueueIsEmptyMutex;
        std::mutex queueMessagesMutext;
        std::condition_variable bufferQueueIsEmpty;
        
        std::condition_variable clientIsAllowedToWrite;
        
        pico_buffered_message<pico_record> allBuffersReadFromTheOtherSide;
        AtomicCounter numberOfMessageRead;
        AtomicCounter numberOfMessageSent;
        
    public:
        static string logFileName;
        
        
        
        //constructor
        pico_session(std::shared_ptr<tcp::socket> r_socket) :shutDownNormally(false),messageNumber(0),messageSender(new MessageSender()){
            socket_ = r_socket;
            
        }
        int numberOfCharsToRead=10;
        void start() {
            
            try {
                cout << " session started already..\n";
                
                while(true)
                {
                    readOneBuffer(10);
                    writeOneBuffer();//write the reply

                }
            } catch (const std::exception& e) {
                cout << " exception : " << e.what() << endl;
            } catch (...) {
                
                cout << "<----->unknown exception thrown.<------>";
                raise (SIGABRT);
            }
        }
        void readOneBuffer(long dataSizeToReadNext) {
            assert(dataSizeToReadNext>0);
            //we read until the whole message is read
            //then we write until the whole message is written
            
            
            if (sessionLogger->isTraceEnabled()) {
                string logMsg( "\n Session is going to read ");
                logMsg.append(toStr(dataSizeToReadNext));
                logMsg.append(" chars into buffer from client...\n");
                sessionLogger->log(logMsg);
            }
           
            
            readSync(dataSizeToReadNext);
        }
        void readSync(long dataSizeToReadNext) {
            std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();
            numberOfCharsToRead = dataSizeToReadNext;
            
            boost::system::error_code error;
            size_t length = socket_->read_some(boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead), numberOfCharsToRead), error);
             if (sessionLogger->isTraceEnabled()) {
                 string logMsg( "\n Session is done reading  ");
                 logMsg.append(toStr(numberOfCharsToRead));
                 logMsg.append(" chars from client...\n");
                 sessionLogger->log(logMsg);
               }
             processTheBufferJustRead(currentBuffer,length);
             numberOfCharsToRead=10;
             //printHistoryOfMessages();//print all the messages after you are done with processing it
        }
        
        void readAsync() {
            
//            auto self(shared_from_this());
//            assert(self);
            //  cout << "session is trying to read messages" << endl;
            
            
//            boost::asio::read(*socket_,boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead),
//                                                           numberOfCharsToRead),
//                              [this,self,currentBuffer](const boost::system::error_code& error,
//                                                        std::size_t t ) {
//                                  if (sessionLogger->isTraceEnabled()) {
//                                      sessionLogger->log( "\n Session is done reading  "<<numberOfCharsToRead<<" chars from client...\n";
//                                  }
//                                  processTheBufferJustRead(currentBuffer,t);
//                                  //printHistoryOfMessages();//print all the messages after you are done with processing it
//                              });
        }
        void writeOneBuffer() {
            if (sessionLogger->isTraceEnabled()) {
                sessionLogger->log( "\nSession is going to send a buffer to client..going to acquire lock \n");
            }
            
            
            while (bufferQueue_.empty()) {
                if (sessionLogger->isTraceEnabled()) {
                    sessionLogger->log( "\nSession bufferQueue_ is empty...waiting for a message to come to queue. \n");
                }
                std::unique_lock <std::mutex> writeOneBufferLock(bufferQueueIsEmptyMutex);
                bufferQueueIsEmpty.wait(writeOneBufferLock);
                
            }
            if (sessionLogger->isTraceEnabled()) {
                sessionLogger->log( "\nSession is going to send a buffer to client.. lock obtained \n");
            }
            
            std::shared_ptr<pico_record> currentBuffer = bufferQueue_.pop();
            // cout << " session is writing one buffer to client : " <<currentBuffer->toString()//<< std::endl;
            string data = currentBuffer->getDataForWrite();
            std::size_t dataSize = currentBuffer->getSize();
            
            const char* dataSizeAsStr = convertToString(dataSize).c_str();
            
            string properMessageAboutSize;
            getProperMessageAboutSize(dataSizeAsStr,properMessageAboutSize);
            writeOneMessageToOtherSide(properMessageAboutSize.c_str(),10,true,data,dataSize);
            
            
            
        }
        
        
        void queueMessages(queueType message) {
            //TODO put a lock here to make the all the buffers in a message go after each other.
            
            std::unique_lock < std::mutex> queueMessagesLock(queueMessagesMutext);
            std::shared_ptr<pico_record> curBufPtr =  message->getCompleteMessageInJsonAsOnBuffer();
            bufferQueue_.push_back(curBufPtr);
            bufferQueueIsEmpty.notify_all();
            
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
            
            sessionLogger->log( "\nend of function readingAndWritingRecordData() ");
            
        }
        void processDataFromOtherSide(msgPtr messageFromOtherSide) {
            
            try {
                numberOfMessageRead.increment();
                
                if (sessionLogger->isTraceEnabled()) {
                   
                    sessionLogger->log(toStr(format( "\nSession read the %1%th message from client : %2% ") %numberOfMessageRead.get() %messageFromOtherSide->toString()));
                }
                //the requestProcessor part will be done after multi threadin
                //socket part is done and tested
//              msgPtr reply = requestProcessor_.processRequest(messageFromOtherSide);
                msgPtr reply =messageFromOtherSide;
                if(sessionLogger->isTraceEnabled()) {
                    
                    string logMsg( "\nSession : putting reply to the queue this is the message that Session read just now");
                    logMsg.append(reply->toString());
                    sessionLogger->log(logMsg);
                    
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
            if (sessionLogger->isTraceEnabled()) {
            string logMsg( "\nsession : data read just now is  \n");
            logMsg.append(str);
            sessionLogger->log(logMsg);
            }
            
            listOfAllMessagesReadInOrder.push_back(str);
            if(pico_record::IsThisRecordASizeInfo(currentBuffer))
            {
                string sizeOfNextBufferToReadStr;
                for(int i=0;i<currentBuffer->getSize();i++)
                {
                    if(currentBuffer->getChar(i)!='#')
                        sizeOfNextBufferToReadStr.push_back(currentBuffer->getChar(i));
                }
                long nextDataSize = convertToSomething<long>(sizeOfNextBufferToReadStr.c_str());
                
                if (sessionLogger->isTraceEnabled()) {
                    string logMsg( "\nsession : dataSize read just now says that next data size is \n");
                    logMsg.append(toStr(nextDataSize));
                    sessionLogger->log(logMsg);
                }
                readOneBuffer(nextDataSize);
                
            }
            else{
                sessionLogger->log( "\nsession : data read just now is not a dataSize Info   \n");
                sessionLogger->log( "\nsession : this is the complete message read from Client ");
                msgPtr last_read_message(new pico_message(str));
                processDataFromOtherSide(last_read_message);
               // writeOneBuffer(); //going to writing mode to write the reply for this complete message
                }
//            else
//                            {
//                                sessionLogger->log( "\nsession : ERROR : reading empty message  \n";
//                                readOneBuffer();		//read the next addon buffer
//                
//                            }
            
            }
        void writeOneMessageToOtherSide(const char* data,std::size_t dataSize,bool sendTheRealData,const string& realData,std::size_t realDataSize)
        {
            numberOfMessageSent.increment();
            boost::asio::write(*socket_, boost::asio::buffer(data, dataSize));
           
            if(sendTheRealData)
            {
                writeOneMessageToOtherSide(realData.c_str(),realDataSize,false,"",-1);//this is the real data
            }
            if(sessionLogger->isTraceEnabled())
            {
                sessionLogger->log(toStr(format("Session : sent the %2%th message to client %1% \n") % data %numberOfMessageSent.get()));
            }

        }
        
//        void writeOneMessageToOtherSideAsync(const char* data,std::size_t dataSize,bool sendTheRealData,const string& realData,std::size_t realDataSize)
//        {
//            auto self(shared_from_this());
//            
//            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
//                                     [this,self,data,sendTheRealData,realData,realDataSize](const boost::system::error_code& error,
//                                                                                            std::size_t t) {
//                                         
//                                         print(error,t,data);
//                                         if(sendTheRealData)
//                                         {
//                                             writeOneMessageToOtherSide(realData.c_str(),realDataSize,false,"",-1);//this is the real data
//                                         }
//                                         
//                                     });
//            
//        }
        
    
        void print(const boost::system::error_code& error, std::size_t t,
                   const char* data) {
            if (sessionLogger->isTraceEnabled()) {

                {
                    if (error) {
                    string logMsg(" error msg : ");
                    logMsg.append(error.message());
                    logMsg.append("\n");
                    sessionLogger->log(logMsg);
                    }
                }
                string str(data);
               
                string logMsg( "\nSession received ");
                logMsg.append(toStr(t));
                logMsg.append(" bytes read from Client \n data read from client is ");
                logMsg.append(str);
                sessionLogger->log(logMsg);
            
            }
        }
        virtual ~pico_session()
        {
            
            sessionLogger->log( "\n Session going out of scope ");
            printHistoryOfMessages();
            assert(shutDownNormally);
            
        }
    void printHistoryOfMessages() {
        sessionLogger->log( "\n Session These are all the messages that session read from client");
        listOfAllMessagesReadInOrder.printAll();
        }
        
    };
}

#endif /* CLIENTHANDLER_H_ */
