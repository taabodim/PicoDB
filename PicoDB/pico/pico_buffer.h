//#ifndef PICO_BUFFER_H
//#define PICO_BUFFER_H
//
//#include <boost/asio.hpp>
//#include <iostream>
//#include <memory>
//#include <utility>
//#include <vector>
//#include <pico/pico_exception.h>
//#include <pico/pico_utils.h>
//#include <logger.h>
//#include <pico/pico_record.h>
//using boost::asio::ip::tcp;
//
//namespace pico {
//    class pico_buffer;
//    typedef pico_buffer pico_record;
//    typedef std::shared_ptr<pico_buffer> std::shared_ptr<pico_record>;
//    typedef std::unique_ptr<pico_buffer> pico_recordUnqiuePtr;
//    typedef std::shared_ptr<tcp::socket> std::shared_ptr<tcp::socket>;
//    typedef tcp::acceptor acceptorType;
//    
//    class pico_buffer {
//    public:
//
//        std::string parentMessageId; //this is the id of the parent that
//        //this buffer belongs to
//        long parentSequenceNumber; //this is the number ,that defines
//        //which part of message or record this buffer belongs to
//        virtual ~pico_buffer()
//        {
//            
//        }
//        bool isBufferBeingUsed() {
//            return bufferBeingUsed;
//        }
//        void setBufferBeingUsed(bool val) {
//            bufferBeingUsed = val;
//        }
//        void setData(char data[], int size) {
//            if (bufferBeingUsed)
//                throw new pico_exception("buffer is already in use");
//            bufferBeingUsed = true;
//            for (int i = 0; i < size; i++) {
//                data_[i] = data[i];
//            }
//            for (int i = size; i < max_size; i++) {
//                data_[i] = 0;
//            }
//            
//        }
//        // Construct from a std::string.
//        void setData(std::string data) {
////            if (bufferBeingUsed)
////            {
////                mylogger<<"exception : buffer is already in use"<<endl;
////                throw new pico_exception("buffer is already in use");
////            }
//            bufferBeingUsed = true;
//            strncpy(data_, data.c_str(), data.size());
//            data_[data.size()] = 0;
//            
//            for (int i = data.size(); i < max_size; i++) {
//                data_[i] = 0;
//            }
//            
//        }
//        
//        explicit pico_buffer(std::string data) {
//            
//            setData(data);
//            
//        }
//        void setDataToNull() {
//            for (int i = 0; i < max_size; i++) {
//                data_[i] = 0;
//            }
//            
//        }
//        pico_buffer() {
//            setDataToNull();
//            bufferBeingUsed = false;
//            arraySize = max_size;
//        }
//        
//        pico_buffer(const pico_buffer& buffer) {
//            
//            bufferBeingUsed = true;
//            arraySize = buffer.arraySize;
//            
//            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
//                      std::begin(this->data_));
//            
//        }
//        
//        pico_buffer& operator=(pico_buffer& buffer) {
//            if (bufferBeingUsed)
//                throw new pico_exception("buffer is already in use");
//            bufferBeingUsed = true;
//            arraySize = buffer.arraySize;
//            
//            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
//                      std::begin(this->data_));
//            return *this;
//        }
//        void clear() {
//            bufferBeingUsed = false;
//            for (int i = 0; i < arraySize; i++) {
//                data_[i] = '\0';
//            }
//        }
//        char* getData() {
//            return data_;
//        }
//        std::string toString() const {
//            std::string temp(data_,max_size);
//            return temp;
//        }
//        std::string toString()  {
// 
//            std::string temp(data_,max_size);	//this  should not be in seperated , this method empties the data_...fix it!!
//            return temp;
//        }
//        size_t getSize() {
//            return arraySize;
//        }
//        const static int max_size = 248; //this should be more than 168 because the smallest message size that i send is 168
//        const static int command_size = 24;
//        char data_[max_size];
//        char key_[pico_record::max_key_size];
//        char value_[pico_record::max_value_size];
//        
//      
//    private:
//        int arraySize;
//        bool bufferBeingUsed;
//    };
//}
//
//#endif
