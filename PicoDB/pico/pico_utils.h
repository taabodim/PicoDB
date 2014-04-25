/*
 * pico_utils.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_UTILS_H_
#define PICO_UTILS_H_
#include <boost/lexical_cast.hpp>
#include <chrono>
using namespace std;

namespace pico{
    
    typedef long offsetType;
    typedef std::string messageType;
    
    enum class comMsg {STOP, SEND_ME_MORE, END_OF_MESSAGE};
    enum log_level {debug,trace,all,error,warning};
    
    std::string random_string( size_t length )
    {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
        std::string str(length,0);
        std::generate_n( str.begin(), length, randchar );
        return str;
    }
    
    std::string random_string( string firstPart,
                              size_t length )
    {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
        std::string str(length,0);
        std::generate_n( str.begin(), length, randchar );
        str.append(firstPart);
        return str;
    }
    template<typename T>
    T random_number(std::size_t N=5)
    {
        auto randchar = []() -> char
        {
            using namespace std::chrono;
            
            steady_clock::time_point t1 = steady_clock::now();
            
            
            const char charset[] =
            "0123456789";
            const size_t max_index = (sizeof(charset) - 1);
            
            steady_clock::time_point t2 = steady_clock::now();
            
            nanoseconds now = duration_cast<nanoseconds>(t2-t1);
            
            return charset[ (rand() ^ now.count() ^ now.count()  ) % max_index ];
        };
        std::string retStr(N,0);
        std::generate_n( retStr.begin(), N, randchar );
        
        T num  = boost::lexical_cast<T>(retStr);
        //std::cout << "retStr : "<<retStr<<std::endl;
        return num;
    }
    template<typename T>
    std::string convertToString(T i)
    {
        std::string str = boost::lexical_cast<std::string>(i);
        return str;
    }
    
    bool openFileIfItDoesntExist(const std::string& name) {
        std::fstream f;
        f.open(name, std::fstream::in); //this will open file if it doesnt exist
        if (f.good()) {
            f.close();
            //std::cout<<"file does exist...\n";
            return true;
        } else {
            //std::cout<<"file does NOT exist,creating one now...\n";
            FILE * pFile;
            pFile = fopen (name.c_str(),"w");
            fclose (pFile);
            
            f.close();
            return false;
        }
    }
    
    offsetType getEndOfFileOffset(std::fstream& file) {//was debugged
        
        file.seekg(0, std::ifstream::end);
       // //std::cout<<(" getEndOfFileOffset : filename is ");
       // //std::cout<<filename<<std::endl;
        //std::cout<<" getEndOfFileOffset : file.tellg() :  "<<file.tellg()<<std::endl;
        
        return file.tellg();
        
    }
}



#endif /* PICO_UTILS_H_ */
