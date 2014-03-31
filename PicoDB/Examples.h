//
//  Examples.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/27/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_Examples_h
#define PicoDB_Examples_h
#include <cstdlib>
#include <list>
#include <sstream>
#include <memory>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/foreach.hpp>
#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <currency.h>
#include <vector>
#include <list>
#include <chrono>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <boost/thread.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <functional>
#include <exception>      // std::set_unexpected
#include <stock.h>
#include "ObjectPool.h"
#include "Runnable.h"
#include "ThreadWorker.h"
#include "pico/pico_server.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <pico/pico_buffer.h>
#include <pico/pico_record.h>
#include <pico/pico_collection.h>
#include <jsonCppExamples.h>
#include <string>
#include <vector>
using namespace boost::filesystem;
using namespace pico;
using namespace std;

class AddressBook {
public:
	// using a template allows us to ignore the differences between functors, function pointers
	// and lambda
	template<typename Func>
	std::vector<std::string> findMatchingAddresses(Func func) {
		std::vector<std::string> results;
		for (auto itr = _addresses.begin(), end = _addresses.end(); itr != end;
             ++itr) {
			// call the function passed into findMatchingAddresses and see if it matches
			if (func(*itr)) {
				results.push_back(*itr);
			}
		}
		return results;
	}
    
private:
	std::vector<std::string> _addresses;
};
void sleepViaBoost(int seconds) {
	cout << " this thread " << boost::this_thread::get_id()
    << " is going to sleep for " << seconds << " secs" << endl;
	boost::this_thread::sleep(boost::posix_time::seconds(seconds));
    
}


void boostFileLSExample() {
	namespace fs = boost::filesystem;
    
	string myPath = ".";
	fs::path p(fs::current_path());
    
	p = fs::system_complete(myPath);
    
	unsigned long file_count = 0;
	unsigned long dir_count = 0;
	unsigned long other_count = 0;
	unsigned long err_count = 0;
    
	if (!fs::exists(p)) {
		std::cout << "\nNot found: " << p << std::endl;
		return;
	}
    
	if (fs::is_directory(p)) {
		std::cout << "\nIn directory: " << p << "\n\n";
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_itr(p); dir_itr != end_iter;
             ++dir_itr) {
			try {
				if (fs::is_directory(dir_itr->status())) {
					++dir_count;
					std::cout << dir_itr->path().filename() << " [directory]\n";
				} else if (fs::is_regular_file(dir_itr->status())) {
					++file_count;
					std::cout << dir_itr->path().filename() << "\n";
				} else {
					++other_count;
					std::cout << dir_itr->path().filename() << " [other]\n";
				}
                
			} catch (const std::exception & ex) {
				++err_count;
				std::cout << dir_itr->path().filename() << " " << ex.what()
                << std::endl;
			}
		}
		std::cout << "\n" << file_count << " files\n" << dir_count
        << " directories\n" << other_count << " others\n" << err_count
        << " errors\n";
	} else // must be a file
	{
		std::cout << "\nFound: " << p << "\n";
	}
}



template<typename T>
class bag {
    
public:
	void insert(T& item) {
		//		_bag.insert(item);
	}
    std::size_t size() {
		return _bag.size();
	}
private:
	vector<T> _bag;
};


void threadExample() {
    
	boost::thread producerThread(runProducerThread);
	boost::thread consumerThread(runConsumerThread);
    
	producerThread.join();
	consumerThread.join();
    
}
template<typename T>
T triangleArea(T base, T height) {
	return base * height;
}
template<typename T>
double multiplyObject(T first, T second) {
	return first * second;
}



void templateFunctionExample() {
	std::cout << " area of triangle is " << triangleArea<int>(2, 50)
    << std::endl;
	std::cout << " area of triangle is " << triangleArea<double>(2.12, 50)
    << std::endl;
	std::cout << " area of triangle is " << triangleArea<float>(2.12, 50)
    << std::endl;
    
	Currency c1, c2;
	c1.price = "11";
	c2.price = "13";
	std::cout << " multiplication of currencies prices are  " << c1 * c2
    << std::endl;
	std::cout << " multiplyObject(c1 * c2) :   "
    << multiplyObject<Currency>(c1, c2) << std::endl;
    
}
void templateClassExample() {
	bag<Currency> curBag;
	Currency c1;
	curBag.insert(c1);
	std::cout << "size of bag is " << curBag.size();
}
void BoostBindExample() {
	struct XClass {
		bool printVariable(int a) {
			std::cout << " function is bound , parameter is " << a << std::endl;
			return true;
		}
	};
    
	XClass xObj;
    
	boost::shared_ptr<XClass> p(new XClass);
    
	int i = 5;
    
	bind(&XClass::printVariable, boost::ref(xObj), _1)(i);		// x.f(i)
	bind(&XClass::printVariable, &xObj, _1)(i);			//(&x)->f(i)
	bind(&XClass::printVariable, xObj, _1)(i);		// (internal copy of x).f(i)
	bind(&XClass::printVariable, p, _1)(i);		// (internal copy of p)->f(i)
    
}
void timeNow() {
    //	boost::asio::buffer
}




void appendToFile(string filename, pico_buffer x) {
    
	std::ofstream outfile;
    
	outfile.open(filename, std::ios_base::app);
    
	outfile.write(x.getData(), x.max_size);
	outfile.close();
}
void writeToFile(string filename, pico_buffer x) {
    
	std::ofstream outfile;
	outfile.seekp(0);
	outfile.open(filename, std::ios_base::app);
	outfile.write(x.getData(), x.max_size);
	outfile.close();
}

void readingAndWritingComplexData() {
    
	pico_buffer x;
	pico_buffer x1;
	string data1 = " hello I am data 1;";
	string data2 = "hello I am data 2 from nyc ny and I am happy today";
    
	x.setData(data1);
	x1.setData(data2);
    
	writeToFile("text.txt", x);
	appendToFile("text.txt", x1);
    
	std::cout << "file was written to" << std::endl;
    
	char x_key[x.max_size];
	char x_value[x.max_size];
    
	std::fstream myFile("text.txt", ios::in | ios::out | ios::binary);
	myFile.seekg(0);
    
	myFile.read((char*) x_key, x.max_size);
	myFile.read((char*) x_value, x.max_size);
    
	pico_buffer data1X;
	pico_buffer data2X;
	data1X.setData(x_key, x.max_size);
	data2X.setData(x_value, x.max_size);
    
	std::cout << " data1 :  " << data1X.getString() << " " << std::endl;
	std::cout << " data2 :  " << data2X.getString() << " " << std::endl;
    
	std::cout << "file was read from " << std::endl;
	myFile.close();
}


#endif
