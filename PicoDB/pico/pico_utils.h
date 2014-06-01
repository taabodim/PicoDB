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
#include <PicoConfig.h>
#include <assert.h>
#include <chrono>
#include <ctime>
#include <ratio>
#include <stdio.h>      // for sprintf()

#include <iostream>     // for console output
#include <string>       // for std::string

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;

namespace pico {
    
    
    typedef long offsetType;
    typedef std::string messageType;
    
    std::string random_string(size_t length) {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }
    
    std::string random_string(string firstPart, size_t length) {
        auto randchar = []() -> char
        {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        str.append(firstPart);
        return str;
    }
    template<typename T>
    T random_number(std::size_t N = 5) {
        auto randchar = []() -> char
        {
            using namespace std::chrono;
            
            steady_clock::time_point t1 = steady_clock::now();
            
            const char charset[] =
            "09182736455463728190";
            const size_t max_index = (sizeof(charset) - 1);
            
            steady_clock::time_point t2 = steady_clock::now();
            
            nanoseconds now = duration_cast<nanoseconds>(t2-t1);
            
            return charset[ (rand() ^ now.count() ^ now.count() ) % max_index ];
        };
        std::string retStr(N, 0);
        std::generate_n(retStr.begin(), N, randchar);
        
        T num = boost::lexical_cast < T > (retStr);
        // mylogger << "retStr : "<<retStr<<std::endl;
        return num;
    }
    template<typename T>
    std::string convertToString(T i) {
        std::string str = boost::lexical_cast < std::string > (i);
        return str;
    }
    
    template<typename T>
    T convertToSomething(std::string sth) {
        T str = boost::lexical_cast < T > (sth);
        return str;
        
    }
    
    template<typename T>
    std::string toStr(T i) {
        std::string str = boost::lexical_cast < std::string > (i);
        return str;
    }
    
    template<typename T>
    T toStr(std::string sth) {
        T str = boost::lexical_cast < T > (sth);
        return str;
        
    }
    template<typename T>
    T calc_hash_code(T obj) {
        
        std::string objStr = boost::lexical_cast < string > (obj);
        std::size_t hash_code = std::hash<std::string>()(objStr);
        
        return hash_code;
        
    }
    
    std::size_t calc_hash_code(std::string obj) {
        
        std::size_t hash_code = std::hash<std::string>()(obj);
        return hash_code;
        
    }
    bool checkFileExist(const std::string& name) {
        std::fstream f;
        f.open(name, std::fstream::in); //this will open file if it doesnt exist
        if (f.good()) {
            f.close();
            //   mylogger<<"file does exist...\n";
            return true;
        } else {
            //      mylogger<<"file does NOT exist,creating one now...\n";
            return false;
        }
    }
    
    bool openFileIfItDoesntExist(const std::string& name) {
        std::fstream f;
        f.open(name, std::fstream::in); //this will open file if it doesnt exist
        if (f.good()) {
            f.close();
            //   mylogger<<"file does exist...\n";
            return true;
        } else {
            //      mylogger<<"file does NOT exist,creating one now...\n";
            FILE * pFile;
            pFile = fopen(name.c_str(), "w");
            fclose(pFile);
            
            f.close();
            return false;
        }
    }
    void truncateTheFile(const std::string& name) {
        FILE * pFile;
        pFile = fopen(name.c_str(), "w");
        char str[] = "fileContent was deleted!";
    
        fwrite(str , 1 , sizeof(str) , pFile );
        fclose(pFile);
    }
    string calc_request_id() {
        return random_number<string>(32);
    }
    
    void sleepViaBoost(int seconds)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(seconds));
        //this throws some weird exception
        
        //    std::chrono::milliseconds dura( seconds*1000 );
        //    std::this_thread::sleep_for( dura );
        
    }
    string getTimeNowAsString_Old() {
        using std::chrono::system_clock;
        system_clock::time_point today = system_clock::now();
        
        std::time_t tt;
        tt = system_clock::to_time_t ( today );
        string timeStr =toStr( ctime(&tt));
        return timeStr;
        
    }
    string getFullCollectionName(string name) {
        string fullname = PicoConfig::dataDir;
        std::string ext(".dat");
        fullname.append(name);
        fullname.append(ext);
        return fullname;
    }
    void getProperMessageAboutSize(string data,string& properMessageAboutSize)
    {
        const char* dataPtr = data.c_str();
        
        for(size_t i=0;i<4;i++)
        {
            properMessageAboutSize.push_back('#');
        }
        for(int i=0;i<data.size();i++,dataPtr++)
        {
        properMessageAboutSize.push_back(*dataPtr);
        }
        
        
    }
    
    //-----------------------------------------------------------------------------
    // Format current time (calculated as an offset in current day) in this form:
    //
    //     "hh:mm:ss.SSS" (where "SSS" are milliseconds)
    //-----------------------------------------------------------------------------
    std::string getTimeNowAsString()
    {
        // Get current time from the clock, using microseconds resolution
        const boost::posix_time::ptime now =
        boost::posix_time::microsec_clock::local_time();
        
        // Get the time offset in current day
        const boost::posix_time::time_duration td = now.time_of_day();
        
        //
        // Extract hours, minutes, seconds and milliseconds.
        //
        // Since there is no direct accessor ".milliseconds()",
        // milliseconds are computed _by difference_ between total milliseconds
        // (for which there is an accessor), and the hours/minutes/seconds
        // values previously fetched.
        //
        const long hours        = td.hours();
        const long minutes      = td.minutes();
        const long seconds      = td.seconds();
        const long milliseconds = td.total_milliseconds() -
        ((hours * 3600 + minutes * 60 + seconds) * 1000);
        
        //
        // Format like this:
        //
        //      hh:mm:ss.SSS
        //
        // e.g. 02:15:40:321
        //
        //      ^          ^
        //      |          |
        //      123456789*12
        //      ---------10-     --> 12 chars + \0 --> 13 chars should suffice
        //
        // 
        char buf[40];
        sprintf(buf, "%02ld:%02ld:%02ld.%03ld", 
                hours, minutes, seconds, milliseconds);
        
        return buf;
    }

    
}

#endif /* PICO_UTILS_H_ */
