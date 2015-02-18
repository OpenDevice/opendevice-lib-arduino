/*
 * ******************************************************************************
 *  Copyright (c) 2013-2014 CriativaSoft (www.criativasoft.com.br)
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *  Ricardo JL Rufino - Initial API and Implementation
 * *****************************************************************************
 */

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

