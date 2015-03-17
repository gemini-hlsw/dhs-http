/*
 * DhsAdapterSim.cpp
 *
 *  Created on: Feb 18, 2015
 *      Author: jluhrs
 */

#include "defines.h"
#include "DhsAdapterSim.h"
#include <ctime>
#include <locale>
#include <sstream>
#include <iomanip>

using namespace std;

DhsAdapterSim::DhsAdapterSim(string &myName, string &serverHost,
        string &serverName, axutil_log_t * log) {
    this->myName = myName;
    this->serverHost = serverHost;
    this->serverName = serverName;
    this->nextId = 1;
    this->log = log;

    pthread_mutex_init(&lock, NULL);
}

DhsAdapterSim::~DhsAdapterSim() {
    pthread_mutex_destroy(&lock);
}

DHS_STATUS DhsAdapterSim::createImage(ImageId &id) {
    time_t t;
    time(&t);
    struct tm *local = localtime(&t);
    int latchedId;

    pthread_mutex_lock(&lock);
    {
        latchedId = nextId;
        if (++nextId > 10000) {
            nextId = 1;
        }
    }
    pthread_mutex_unlock(&lock);


    stringstream formatedId;
    formatedId << 'S' << setfill('0') << setw(4) << (local->tm_year + 1900)
            << setw(2) << local->tm_mon << local->tm_mday << 'S' << setw(4)
            << nextId;

    id.assign(formatedId.str());

    stringstream msgFormater;
    msgFormater << "Simulated DHS: Created image " << formatedId.str();
    AXIS2_LOG_INFO_MSG(log, msgFormater.str().c_str());
    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageLifeTime(const ImageId& id,
        DHS_BD_LIFETIME lifeTime) {
    stringstream msgFormater;
    msgFormater << "Simulated DHS: Set lifetime for image " << id << " to "
            << translateLifetime(lifeTime);
    AXIS2_LOG_INFO_MSG(log, msgFormater.str().c_str());
    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageContrib(const ImageId& id,
        const vector<string>& contribs) {
    stringstream msgFormater;
    msgFormater << "Simulated DHS: Set contributors for image " << id << ": { ";

    for (vector<string>::const_iterator iter = contribs.begin();
            iter != contribs.end(); iter++) {
        msgFormater << *iter << " ";
    }
    msgFormater << "}";
    AXIS2_LOG_INFO_MSG(log, msgFormater.str().c_str());

    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageKeywords(const ImageId& id,
        const vector<Keyword>& keywords, bool final) {

    stringstream msgFormater;
    msgFormater << "Simulated DHS: Set keywords for image " << id
            << " (final = " << boolalpha << final << ")";
    AXIS2_LOG_INFO_MSG(log, msgFormater.str().c_str());

    for (vector<Keyword>::const_iterator iter = keywords.begin();
            iter != keywords.end(); iter++) {
        msgFormater.str("");
        msgFormater << '\t' << iter->getName() << " = ";
        switch (iter->getType()) {
        case DHS_DT_INT8: {
            msgFormater << iter->getValue<int8_t>();
            break;
        }
        case DHS_DT_INT16: {
            msgFormater << iter->getValue<int16_t>();
            break;
        }
        case DHS_DT_INT32: {
            msgFormater << iter->getValue<int32_t>();
            break;
        }
        case DHS_DT_UINT8: {
            msgFormater << iter->getValue<uint8_t>();
            break;
        }
        case DHS_DT_UINT16: {
            msgFormater << iter->getValue<uint16_t>();
            break;
        }
        case DHS_DT_UINT32: {
            msgFormater << iter->getValue<uint32_t>();
            break;
        }
        case DHS_DT_FLOAT: {
            msgFormater << iter->getValue<float>();
            break;
        }
        case DHS_DT_DOUBLE: {
            msgFormater << iter->getValue<double>();
            break;
        }
        case DHS_DT_STRING: {
            msgFormater << iter->getValue<const string&>();;
            break;
        }
        case DHS_DT_BOOLEAN: {
            msgFormater << boolalpha << iter->getValue<bool>();
            break;
        }
        default: {
            throw(std::logic_error("Keyword object with invalid type."));
        }
        }
        msgFormater << " (" << translateType(iter->getType()) << ")";
        AXIS2_LOG_INFO_MSG(log, msgFormater.str().c_str());
    }
    return DHS_S_SUCCESS;
}

void DhsAdapterSim::setTimeout(unsigned int) {
}

string DhsAdapterSim::translateLifetime(DHS_BD_LIFETIME val) {
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

string DhsAdapterSim::translateType(DHS_DATA_TYPE type) {
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
