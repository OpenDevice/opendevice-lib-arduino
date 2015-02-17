/*
 * DataUtils.cpp
 *
 *  Created on: 30/01/2015
 *      Author: ricardo
 */

#include "DataUtils.h"

namespace DataUtils
{
	int getMin(int *values, int size){
		int min = values[0];
		for (int i = 0; i < size; i++)
		if (values[i] < min){
			min = values[i];
		}
		return min;
	}

	unsigned int getMin(unsigned int *values, int size){
		unsigned int min = values[0];
		for (int i = 0; i < size; i++)
		if (values[i] < min){
			min = values[i];
		}
		return min;
	}

}

