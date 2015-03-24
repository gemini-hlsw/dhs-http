/*
 * DhsUtil.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: jluhrs
 */

#include "DhsUtil.h"

using namespace std;

string DhsUtil::translateLifetime(DHS_BD_LIFETIME val) {
    switch (val) {
    case DHS_BD_LT_PERMANENT: {
        return "DHS_BD_LT_PERMANENT";
    }
    case DHS_BD_LT_TEMPORARY: {
        return "DHS_BD_LT_TEMPORARY";
    }
    default: {
        return "DHS_BD_LT_TRANSIENT";
    }
    }
}

string DhsUtil::translateType(DHS_DATA_TYPE type) {
    switch (type) {
    case DHS_DT_UINT8: {
        return "DHS_DT_UINT8";
    }
    case DHS_DT_UINT16: {
        return "DHS_DT_UINT16";
    }
    case DHS_DT_UINT32: {
        return "DHS_DT_UINT32";
    }
    case DHS_DT_INT8: {
        return "DHS_DT_INT8";
    }
    case DHS_DT_INT16: {
        return "DHS_DT_INT16";
    }
    case DHS_DT_INT32: {
        return "DHS_DT_INT32";
    }
    case DHS_DT_FLOAT: {
        return "DHS_DT_FLOAT";
    }
    case DHS_DT_DOUBLE: {
        return "DHS_DT_DOUBLE";
    }
    case DHS_DT_STRING: {
        return "DHS_DT_STRING";
    }
    case DHS_DT_BOOLEAN: {
        return "DHS_DT_BOOLEAN";
    }
    default: {
        return "DHS_DT_STRING";
    }
    }
}
