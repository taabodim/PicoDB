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
#include <pico_logger_wrapper.h>
#include <PicoConfig.h>
#include <assert.h>
using namespace std;

namespace pico {

string COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY(
		"COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY");
string LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY(
		"LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY");

typedef long offsetType;
typedef std::string messageType;
bool log = true;
enum class comMsg {
	STOP, SEND_ME_MORE, END_OF_MESSAGE
};
enum log_level {
	debug, trace, all, error, warning
};

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
    
string getFullCollectionName(string name) {
	string fullname = PicoConfig::dataDir;
	std::string ext(".dat");
	fullname.append(name);
	fullname.append(ext);
	return fullname;
}


}

#endif /* PICO_UTILS_H_ */
