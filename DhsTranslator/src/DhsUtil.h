/*
 * DhsUtil.h
 *
 *  Created on: Mar 24, 2015
 *      Author: jluhrs
 */

#ifndef DHSUTIL_H_
#define DHSUTIL_H_

#include <string>
#include <dhs.h>

class DhsUtil {
private:
    DhsUtil() {};
public:
    static std::string translateLifetime(DHS_BD_LIFETIME val);
    static std::string translateType(DHS_DATA_TYPE type);
};

#endif /* DHSUTIL_H_ */
