/*
 * currency.h
 *
 *  Created on: Jan 13, 2014
 *      Author: mahmoudtaabodi
 */

#pragma once
#include <string>
#include <ostream>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

using std::string;
class Currency {
public:
	std::string name;
	std::string price;
	std::string symbol;
	std::string ts;
	std::string type;
	std::string utctime;
	std::string volume;

	Currency& setVolume(std::string str);
	Currency();//default constructor
	Currency(const Currency& cur);//copy constructor
	Currency(const Currency&& cur);//move constructor
	Currency& operator=(const Currency& other); //copy assignment
	std::string operator()(); //overloading () operator
	bool operator<(const Currency&  other) const;
	double operator*(const Currency& other);
	~Currency();
	std::string toString() const;
	std::string toJson() const;
//	std::ostream& operator<<(const currency& o);
	std::ostream& operator<<(std::ostream& o);
	Currency& setName(std::string name);
	Currency& setPrice(std::string price);
	double getValue();
	double calPrice();
	bool operator==(const Currency &other) const; //for using in the unordered map
//	void serialize(boost::archive::text_oarchive &ar,const unsigned int version);
//	void serialize(boost::archive::text_iarchive &ar,const unsigned int version);
    size_t hash_code() const;
};

