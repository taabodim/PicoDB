/*
 * pico_utils.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_UTILS_H_
#define PICO_UTILS_H_
#include <boost/lexical_cast.hpp>
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
    
    std::string convertToString(int i)
    {
        std::string str = boost::lexical_cast<std::string>(i);
        return str;
    }
}



#endif /* PICO_UTILS_H_ */
