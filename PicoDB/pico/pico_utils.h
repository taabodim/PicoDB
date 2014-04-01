/*
 * pico_utils.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_UTILS_H_
#define PICO_UTILS_H_
#include <pico_buffer.h>
using namespace std;
namespace pico{



typedef long offsetType;
typedef std::shared_ptr<tcp::socket> socketType;
typedef tcp::acceptor acceptorType;

typedef std::string messageType;
    
    
    enum log_level {debug,trace,all,error,warning};
    boost::mutex log_mutex;
    
    void log(const std::string& str)
    {
        boost::interprocess::scoped_lock<boost::mutex> lock( log_mutex);
        std::cout<<str<<std::endl;
    }
    
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
    
     std::string random_number(std::size_t N=5)
    {
       
        char ar[N];
        std::generate_n(ar, N, std::rand); // Using the C function rand()
        
        std::cout << "ar: ";
        std::copy(ar, ar+N, std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
         std::string retStr(ar);
        std::cout << "retStr : "<<retStr<<std::endl;
        return retStr;
    }
}



#endif /* PICO_UTILS_H_ */
