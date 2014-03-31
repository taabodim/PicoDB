/*
 * Stock.h
 *
 *  Created on: Mar 6, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef STOCK_H_
#define STOCK_H_
template<typename stockValueType>
class Stock {
private:
	stockValueType todayValue;
	float sotckRisk;
	float marketRisk;
public:
	Stock(float sRisk,float mRisk): sotckRisk(sRisk),marketRisk(mRisk){

	}
	virtual ~Stock(){

	}
	stockValueType getValue(){
		return todayValue;
	}

	stockValueType calPrice(){
			return todayValue * sotckRisk * (1-marketRisk);
		}

};

#endif /* STOCK_H_ */
