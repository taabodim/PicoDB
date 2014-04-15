/*
 * currency.h
 *
 *  Created on: Jan 13, 2014
 *      Author: mahmoudtaabodi
 */

#pragma once
#include <string>
#include <ostream>
#include <logger.h>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <currency.h>
#include <iostream>
#include <pico/pico_utils.h>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>
using namespace pico;





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
//    //logger mylogger;
    
	Currency() {//default constructor
        std::cout<<( "default constructor called" );
        name = "defaultName";
        price = "defaultPrice";
        symbol = "defaultSymbol";
        ts = "defaultTS";
        type = "defaultType";
        utctime = "defaultUTCtime";
        volume = "defaultVolume";
    }
    
	Currency(const Currency&& other)//move constructor
    {
        std::cout<<("move constructor called" );
        name = other.name;
        price = other.price;
        symbol = other.symbol;
        ts = other.ts;
        type = other.type;
        utctime = other.utctime;
        volume = other.volume;
    }

	std::string operator()() //overloading () operator
	{
    	std::cout<<(" operator() called" );
        return this->toString();
    }
//    bool operator<(const Currency&  other) const;
//	double operator*(const Currency& other);
//	~Currency();
//	std::string toString() const;
//	std::string toJson() const;
//	std::ostream& operator<<(const currency& o);
//	std::ostream& operator<<(std::ostream& o);
//	Currency& setName(std::string name);
//	Currency& setPrice(std::string price);
//	double getValue();
//	double calPrice();
//	bool operator==(const Currency &other) const; //for using in the unordered map
//	void serialize(boost::archive::text_oarchive &ar,const unsigned int version);
//	void serialize(boost::archive::text_iarchive &ar,const unsigned int version);
    
    bool operator==(const Currency &other) const //for using in the unordered map
    {
        return (name == other.name && price == other.price && symbol == other.symbol
                && ts == other.ts && type == other.type && utctime == other.utctime
                && volume == other.volume);
    }
    Currency& setName(std::string n) {
        name = n;
        return *this;
    }
    Currency& setPrice(std::string pr) {
        price = pr;
        return *this;
    }
    
    Currency(const Currency& other) {
        std::cout<<("copy constructor called" );
        name = other.name;
        price = other.price;
        symbol = other.symbol;
        ts = other.ts;
        type = other.type;
        utctime = other.utctime;
        volume = other.volume;
    }
    
    
    Currency& operator=(const Currency& other) {
        std::cout<<("copy assignment called" );
        name = other.name;
        price = other.price;
        symbol = other.symbol;
        ts = other.ts;
        type = other.type;
        utctime = other.utctime;
        volume = other.volume;
        return *this;
    }
    Currency& setVolume(std::string str) {
        
        volume = str;
        return *this;
    }
    ~Currency() {
        std::cout<<( "destructor for currency called");
    }
    const std::string toString() const {
        
        return "name : " + name + " price  " + price + " symbol " + symbol + " ts "
        + ts + " type " + type + " utctime " + utctime + "  volume  "
        + volume;
    }
    std::string toJson() const {
        return "{ \"name\" : \"" + name + "\",  \"price\":  \"" + price
        + "\", \"symbol\": \"" + symbol + "\", \"ts\": " + ts
        + "\", type\":\"" + type + "\" , \"utctime\": \"" + utctime
        + "\",  \"volume\":  \"" + volume + "\"}";
    }
    std::ostream& operator<<(std::ostream& os) {
        
        os << "----------------------------------\n";
        os << " name : " << name << "\n";
        os << " price : " << price << "\n";
        os << " symbol : " << symbol << "\n";
        os << " ts : " << ts << "\n";
        os << " type : " << type << "\n";
        os << " utctime : " << utctime << "\n";
        os << " volume : " << volume << "\n";
        os << "----------------------------------" << "\n";
        return os;
    }
    //because we are using them in multiset
    double operator*(const Currency& other) {
        double priceOther;
        double priceThis;
        try {
            priceOther = boost::lexical_cast<double>(other.price);
            priceThis = boost::lexical_cast<double>(price);
        } catch (boost::bad_lexical_cast const&) {
            priceOther = 0;
            priceThis = 0;
        }
        
        return priceOther * priceThis;
    }
    
    bool operator<(const Currency & other) const {
        std::hash<std::string> hash_fn;
        std::size_t str_hash1 = hash_fn(name);
        std::size_t str_hash2 = hash_fn(other.name);
        return (str_hash1 < str_hash2);
    }
    size_t hash_code() const
    {
        std::size_t h1 = std::hash<std::string>()(name);
        std::size_t h2 = std::hash<std::string>()(price);
        std::size_t h3 = std::hash<std::string>()(ts);
        std::size_t h4 = std::hash<std::string>()(volume);
        std::size_t h5 = std::hash<std::string>()(type);
        std::size_t h6 = std::hash<std::string>()(symbol);
        
        return (h1 ^ (h2 << 1) ^ h3 ^ h4 ^ h5 ^ h6);
    }
    //void serialize(boost::archive::text_oarchive &ar,
    //		const unsigned int version) {
    //	ar & name;
    //	ar & price;
    //	ar & symbol;
    //}
    //
    //void serialize(boost::archive::text_iarchive &ar,
    //		const unsigned int version) {
    //	ar & name;
    //	ar & price;
    //	ar & symbol;
    //}
    double calPrice(){return 0;}
    double getValue() {
        double priceThis =0;
        try {
            priceThis = boost::lexical_cast<double>(price);
        } catch (boost::bad_lexical_cast const&) {
            priceThis = 0;
        }
        return priceThis;
    }

};

