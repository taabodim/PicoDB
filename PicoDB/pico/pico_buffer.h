#ifndef PICO_BUFFER_H
#define PICO_BUFFER_H

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <pico/pico_exception.h>
#include <pico/pico_utils.h>
#include <logger.h>
using boost::asio::ip::tcp;

namespace pico {
    class pico_buffer;
    typedef pico_buffer bufferType;
    typedef std::shared_ptr<pico_buffer> bufferTypePtr;
    typedef std::shared_ptr<tcp::socket> socketType;
    typedef tcp::acceptor acceptorType;
    
    class pico_buffer {
    public:
        logger mylogger;
        std::string parentMessageId; //this is the id of the parent that
        //this buffer belongs to
        long parentSequenceNumber; //this is the number ,that defines
        //which part of message or record this buffer belongs to
        ~pico_buffer()
        
        {
            
            //        std::cout<<("pico buffer being destructed..");
            
        }
        bool isBufferBeingUsed() {
            return bufferBeingUsed;
        }
        void setBufferBeingUsed(bool val) {
            bufferBeingUsed = val;
        }
        void setData(char data[], int size) {
            if (bufferBeingUsed)
                throw new pico_exception("buffer is already in use");
            bufferBeingUsed = true;
            for (int i = 0; i < size; i++) {
                data_[i] = data[i];
            }
            for (int i = size; i < max_size; i++) {
                data_[i] = 0;
            }
            
        }
        // Construct from a std::string.
        void setData(std::string data) {
            if (bufferBeingUsed)
            {
                std::cout<<"exception : buffer is already in use"<<endl;
                throw new pico_exception("buffer is already in use");
            }
            bufferBeingUsed = true;
            strncpy(data_, data.c_str(), data.size());
            data_[data.size()] = 0;
            
            for (int i = data.size(); i < max_size; i++) {
                data_[i] = 0;
            }
            
        }
        
        explicit pico_buffer(std::string data) {
            
            setData(data);
            
        }
        void setDataToNull() {
            for (int i = 0; i < max_size; i++) {
                data_[i] = 0;
            }
            
        }
        pico_buffer() {
            setDataToNull();
            bufferBeingUsed = false;
            arraySize = max_size;
        }
        
        pico_buffer(const pico_buffer& buffer) {
            
            bufferBeingUsed = true;
            arraySize = buffer.arraySize;
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            
        }
        
        pico_buffer& operator=(pico_buffer& buffer) {
            if (bufferBeingUsed)
                throw new pico_exception("buffer is already in use");
            bufferBeingUsed = true;
            arraySize = buffer.arraySize;
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            return *this;
        }
        void clear() {
            bufferBeingUsed = false;
            for (int i = 0; i < arraySize; i++) {
                data_[i] = '\0';
            }
        }
        char* getData() {
            return data_;
        }
        std::string toString() const {
            std::string temp(data_);	//this  should not be in seperated
            return temp;
        }
        size_t getSize() {
            return arraySize;
        }
        const static int max_size = 128;
        const static int command_size = 24;
        char data_[max_size];
    private:
        int arraySize;
        bool bufferBeingUsed;
    };
}

#endif
