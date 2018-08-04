/*
 * DuplicatedValueFilter.h
 *
 *  Created on: 30 de jul de 2018
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_UTILITY_DUPLICATEDVALUEFILTER_H_
#define LIBRARIES_OPENDEVICE_SRC_UTILITY_DUPLICATEDVALUEFILTER_H_

#include "Device.h"

class DuplicatedValueFilter: public ValueFilter {
public:
	DuplicatedValueFilter(unsigned long interval);
	virtual ~DuplicatedValueFilter();

	bool accept(value_t value);

private:
	unsigned long filterInterval;
};


#endif /* LIBRARIES_OPENDEVICE_SRC_UTILITY_DUPLICATEDVALUEFILTER_H_ */
