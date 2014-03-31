#include <currency.h>
#include <iostream>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>

Currency::Currency() {
	std::cout << "default constructor called" << std::endl;
	name = "defaultName";
	price = "defaultPrice";
	symbol = "defaultSymbol";
	ts = "defaultTS";
	type = "defaultType";
	utctime = "defaultUTCtime";
	volume = "defaultVolume";
}

std::string Currency::operator()() {
	std::cout << " Currency::operator() called" << std::endl;
	return this->toString();
}

bool Currency::operator==(const Currency &other) const //for using in the unordered map
		{
	return (name == other.name && price == other.price && symbol == other.symbol
			&& ts == other.ts && type == other.type && utctime == other.utctime
			&& volume == other.volume);
}
Currency& Currency::setName(std::string n) {
	name = n;
	return *this;
}
Currency& Currency::setPrice(std::string pr) {
	price = pr;
	return *this;
}

Currency::Currency(const Currency& other) {
	std::cout << "copy constructor called" << std::endl;
	name = other.name;
	price = other.price;
	symbol = other.symbol;
	ts = other.ts;
	type = other.type;
	utctime = other.utctime;
	volume = other.volume;
}

Currency::Currency(const Currency&& other) {
	std::cout << "move constructor called" << std::endl;
	name = other.name;
	price = other.price;
	symbol = other.symbol;
	ts = other.ts;
	type = other.type;
	utctime = other.utctime;
	volume = other.volume;
}
Currency& Currency::operator=(const Currency& other) {
	std::cout << "copy assignment called" << std::endl;
	name = other.name;
	price = other.price;
	symbol = other.symbol;
	ts = other.ts;
	type = other.type;
	utctime = other.utctime;
	volume = other.volume;
	return *this;
}
Currency& Currency::setVolume(std::string str) {

	volume = str;
	return *this;
}
Currency::~Currency() {
	std::cout << "destructor for currency called" << std::endl;
}
std::string Currency::toString() const {

	return "name : " + name + " price  " + price + " symbol " + symbol + " ts "
			+ ts + " type " + type + " utctime " + utctime + "  volume  "
			+ volume;
}
std::string Currency::toJson() const {
	return "{ \"name\" : \"" + name + "\",  \"price\":  \"" + price
			+ "\", \"symbol\": \"" + symbol + "\", \"ts\": " + ts
			+ "\", type\":\"" + type + "\" , \"utctime\": \"" + utctime
			+ "\",  \"volume\":  \"" + volume + "\"}";
}
std::ostream& Currency::operator<<(std::ostream& os) {

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
double Currency::operator*(const Currency& other) {
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

bool Currency::operator<(const Currency & other) const {
	std::hash<std::string> hash_fn;
	std::size_t str_hash1 = hash_fn(name);
	std::size_t str_hash2 = hash_fn(other.name);
	return (str_hash1 < str_hash2);
}
size_t Currency::hash_code() const
{
	std::size_t h1 = std::hash<std::string>()(name);
    std::size_t h2 = std::hash<std::string>()(price);
    std::size_t h3 = std::hash<std::string>()(ts);
    std::size_t h4 = std::hash<std::string>()(volume);
    std::size_t h5 = std::hash<std::string>()(type);
    std::size_t h6 = std::hash<std::string>()(symbol);
    
    return (h1 ^ (h2 << 1) ^ h3 ^ h4 ^ h5 ^ h6);
}
//void Currency::serialize(boost::archive::text_oarchive &ar,
//		const unsigned int version) {
//	ar & name;
//	ar & price;
//	ar & symbol;
//}
//
//void Currency::serialize(boost::archive::text_iarchive &ar,
//		const unsigned int version) {
//	ar & name;
//	ar & price;
//	ar & symbol;
//}
double Currency::calPrice(){return 0;}
double Currency::getValue() {
	double priceThis =0;
	try {
		priceThis = boost::lexical_cast<double>(price);
	} catch (boost::bad_lexical_cast const&) {
		priceThis = 0;
	}
	return priceThis;
}
