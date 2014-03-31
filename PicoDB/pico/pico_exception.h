/*
 * pico_excecption.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_EXCECPTION_H_
#define PICO_EXCECPTION_H_
namespace pico{
class pico_exception: public std::exception {
public:
	pico_exception(std::string ss) :
			s(ss) {
	}
	~pico_exception() throw () {
	} // Updated
	const char* what() const throw () {
		return s.c_str();
	}
private:
	std::string s;

};
}



#endif /* PICO_EXCECPTION_H_ */
