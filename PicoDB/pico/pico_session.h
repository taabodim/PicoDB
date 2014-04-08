/*
 * ClientHandler.h
 *
 *  Created on: Mar 11, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_
#include "pico/pico_server.h"
#include "pico/pico_record.h"
#include "pico/pico_collection.h"
#include "pico/writer_buffer_container.h"
#include "pico/asyncReader.h"
#include "pico/request_processor.h"
#include <pico/pico_utils.h>
#include <logger.h>
using boost::asio::ip::tcp;
using namespace std;
namespace pico {
    
    class pico_session: public std::enable_shared_from_this<pico_session> {
        
    private:
        socketType socket_;
        typedef pico_message queueType;
    public:
        logger mylogger;
        pico_session(socketType r_socket) :
        writeMessageLock(sessionMutex) ,allowedToWriteLock(allowedToWriteLockMutext){
            socket_ = r_socket;
            
            
            
        }
        void start() {
            
            try {
                cout << " session started already..\n";
                read_messages();
            } catch (const std::exception& e) {
                cout << " exception : " << e.what() << endl;
            } catch (...) {
                cout << "<----->unknown exception thrown.<------>";
            }
        }
        void readWriteSync() {
            
            string msgFromClient = read_messages_sync();
            if (msgFromClient.compare("insert") == 0) {
                cout << "server recienved insert message from client" << endl;
                //insertData to client
                
                std::cout << "server reading message : " << msgFromClient
                << std::endl;
                write_messages_sync();
                boost::this_thread::sleep(boost::posix_time::seconds(4));
            }
        }
        
        void write_messages_sync() {
            
        }
        string read_messages_sync() {
            
            //		char* data = buffer.getData();
            //		std::size_t dataSize = buffer.getSize();
            //
            //		std::cout << "data to send is : " << data << std::endl;
            //		std::cout << "dataSize to send is : " << dataSize << std::endl;
            //
            //		boost::array<char, 128> buf;
            //		boost::system::error_code error;
            //
            //		size_t len = socket_->read_some(boost::asio::buffer(buf), error);
            //		return buffer.getString();
            string empty;
            return empty;
        }
        
        void processCommand() {
            //		std::cout << "processCommand :  " << readBuffer.getString() << "\n ";
            //
            //		if (readBuffer.getString().compare("insert") == 0) {
            //			cout << "server recieved insert message from client" << endl;
            //			//insertData to client
            //			write_messages();
            //		}
            //		write_messages();
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
            << optionCollection.getNumberOfRecords() << " \n";
            cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
            cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
            optionCollection.update(x1, x2);
            //optionCollection.deleteRecord(x1);
            
            std::cout << "end of function readingAndWritingRecordData() "
            << std::endl;
            
        }
        void processDataFromClient(std::string messageFromClient) {
            
            try {
                
                pico_message reply = requestProcessor_.processRequest(messageFromClient);
                
                messageToClientQueue_.push(reply);
                messageClientQueueIsEmpty.notify_all();
                
                write_messages();
                
            } catch (std::exception &e) {
                cout << " this is the error : " << e.what() << endl;
            }
            
        }
        void read_messages() {
            
            auto self(shared_from_this());
            cout << "session is trying to read messages" << endl;
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
                                        processTheMessageJustRead(currentBuffer,t);
                                        
                                        clientIsAllowedToWrite.notify_all();
                                        
                                    });
        }
        void processTheMessageJustRead(bufferTypePtr currentBuffer,std::size_t t){
            
            string str =currentBuffer->toString();
            std::cout<<"\nthis is the message that server read just now \n "<<str<<endl;
            if(str.empty())
            {
                processIncompleteData();
            }else          if(find_last_of_string(currentBuffer))
            {
                std::cout<<("session: this buffer is an add on to the last message..dont process anything..read the next buffer\n");
                
                append_to_last_message(str);
                processIncompleteData();
            }
            else {
                //                str =last_read_message.toString();
                str =last_read_message;
                std::cout<<"session: message was read completely..process the last message :  "<<str<<endl;
                // print(error,t,str);
                
                processDataFromClient(str);
                //                last_read_message.clear();
            }
        }
        static bool find_last_of_string(bufferTypePtr currentBuffer)
        {
            int pos = pico_buffer::max_size-6;
            if(currentBuffer->data_[pos] != 'd' ||
               currentBuffer->data_[--pos] != 'n' ||
               currentBuffer->data_[--pos] != 'e' ||
               currentBuffer->data_[--pos] != 'p' ||
               currentBuffer->data_[--pos] != 'p' ||
               currentBuffer->data_[--pos] != 'a' )
                return false;
            else return true;
            
        }
        void  processIncompleteData()
        {
            string msg("dear client your incomplete message was received successfuly");
            
            pico_message reply = pico_message::build_message_from_string(msg);
            
          	messageToClientQueue_.push(reply);
            messageClientQueueIsEmpty.notify_all();
            write_messages();
            
        }
        void print(const boost::system::error_code& error,std::size_t t,string& str)
        {
            if(error) std::cout<<" error msg : "<<error.message()<<std::endl;
            std::cout << "Server received "<<std::endl;
            std::cout<<t<<" bytes read from Client "<<std::endl;
            std::cout<<(" data read from client is ");
            std::cout<<(str);
            std::cout<<("-------------------------");
            
        }
        void append_to_last_message(string str) {
            last_read_message.append(str);
            
        }
        void write_messages() {
            
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
            //					std::cout << "Server sent "<<std::endl;
            //					std::cout<<(t<<" bytes sent to client "<<std::endl;
            //					if(error) std::cout<<(" error msg : "<<error.message()<<std::endl;
            //					std::cout<<( " data sent to client is "<<str<<std::endl;
            //					std::cout << "-------------------------"<<std::endl;
            //					read_messages();
            //				});
            
            
            if (messageToClientQueue_.empty())
            {
                std::cout<<("session queue of messages is empty...going to wait on the lock");
                messageClientQueueIsEmpty.wait(writeMessageLock);
            }
            else {
                cout << "session is writing async now\n";
                
                queueType message = messageToClientQueue_.pop();
                
                
                cout << " session is going to send this message to server : " << message.toString()
                << std::endl;
                
                
                
                
                if(!message.buffered_message.msg_in_buffers->empty())
                {
                    while(!message.buffered_message.msg_in_buffers->empty())
                    {
                        auto curBuf = message.buffered_message.msg_in_buffers->pop();
                        std::shared_ptr<pico_buffer> curBufPtr(new pico_buffer(curBuf));
                        writeOneBuffer(curBufPtr);
                        clientIsAllowedToWrite.wait(allowedToWriteLock);
                    }
                }
                else{
                    //msg is empty //write a listening msg to client
                    
                    std::string str("IAMLISTENING");
                    std::shared_ptr<pico_buffer> curBufPtr(new pico_buffer(str));
                    writeOneBuffer(curBufPtr);
                }
            }
        }
        void writeOneBuffer(bufferTypePtr currentBuffer)
        {
            
            cout << " session is writing one buffer to client : " << std::endl;
            char* data = currentBuffer->getData();
            std::size_t dataSize = currentBuffer->getSize();
            auto self(shared_from_this());
            
            boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                     [this,self,currentBuffer](const boost::system::error_code& error,
                                                               std::size_t t) {
                                         string str = currentBuffer->toString();
                                         std::cout << "Session Sent :  "<<std::endl;
                                         std::cout<<t<<" bytes from Client "<<std::endl;
                                         if(error) std::cout<<" error msg : "<<error.message()<<std::endl;
                                         std::cout<<" data sent to client is "<<str<<endl;
                                         std::cout<<("-------------------------");
                                         read_messages();
                                     });
            
        }
        
        writer_buffer_container writer_buffer_container_;
        asyncReader asyncReader_;
        request_processor requestProcessor_;
        pico_concurrent_list<queueType> messageToClientQueue_;
        boost::mutex sessionMutex;   // mutex for the condition variable
        boost::mutex allowedToWriteLockMutext;
        boost::condition_variable messageClientQueueIsEmpty;
        boost::condition_variable clientIsAllowedToWrite;
        boost::unique_lock<boost::mutex> allowedToWriteLock;
        boost::unique_lock<boost::mutex> writeMessageLock;
        string last_read_message;
    };
}

#endif /* CLIENTHANDLER_H_ */
