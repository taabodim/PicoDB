//
//  pico_logger_wrapper.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/25/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_logger_wrapper_h
#define PicoDB_pico_logger_wrapper_h
#include <logger.h>

namespace pico
{
    
    template<typename T>
    logger& operator << (logger& wrapper,T nonstr)
    {
        string str = boost::lexical_cast<std::string>(nonstr);
        // std::cout<<str;
        wrapper.log(str);
        return wrapper;
    }
    
    template<>
    logger& operator << (logger& wrapper,const std::string& str)
    {
        //std::cout<<str;
        wrapper.log(str);
        return wrapper;
    }

//    template<typename T>
//     logger* operator << (logger*  wrapper,T nonstr)
//    {
//        string str = boost::lexical_cast<std::string>(nonstr);
//        // std::cout<<str;
//        wrapper->log(str);
//        return wrapper;
//    }
//    
//    template<>
//    logger*  operator << (logger*  wrapper,const std::string& str)
//    {
//        //std::cout<<str;
//        wrapper->log(str);
//        return wrapper;
//    }
    class pico_logger_wrapper {
    
    public:
    logger mylogger;
        static logger* myloggerPtr;;
        //this is to have only one instance of logger everywhere
        
        pico_logger_wrapper():mylogger("gicapods"){
       
        if(myloggerPtr==NULL)
         {
             myloggerPtr = new logger("gicapods");//to delete the file once
         }
    }
        
        void log(const std::string& str){
            mylogger<<str;
        }
        
    
       
    };
   
    //this is how you make a singleton properly
    //class S
//    {
//    public:
//        static S& getInstance()
//        {
//            static S    instance;
//            return instance;
//        }
//    private:
//        S() {}
//        S(S const&);              // Don't Implement.
//        void operator=(S const&); // Don't implement
//    };
 
}


#endif
