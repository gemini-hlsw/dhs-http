/*
 * DhsAdapterSim.cpp
 *
 *  Created on: Feb 18, 2015
 *      Author: jluhrs
 */

#include "defines.h"
#include "DhsAdapterSim.h"
#include "DhsUtil.h"
#include <ctime>
#include <locale>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

using namespace std;

map<string, DHS_DATA_TYPE> DhsAdapterSim::keyTypes;

DhsAdapterSim::DhsAdapterSim(string &myName, string &serverHost,
        string &serverName, axutil_log_t * log) {
    this->myName = myName;
    this->serverHost = serverHost;
    this->serverName = serverName;
    this->nextId = 1;
    this->log = log;

    if (keyTypes.empty()) {
        keyTypes["RA"] = DHS_DT_DOUBLE;
        keyTypes["PREIMAGE"] = DHS_DT_BOOLEAN;
        keyTypes["NAMPS"] = DHS_DT_INT32;
    }

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
        if (++nextId >= MAX_IMAGE_ID) {
            nextId = 1;
        }
    }
    pthread_mutex_unlock(&lock);

    stringstream formatedId;
    formatedId << 'S' << setfill('0') << setw(4) << (local->tm_year + 1900)
            << setw(2) << local->tm_mon << local->tm_mday << 'S' << setw(4)
            << nextId;

    id.assign(formatedId.str());

    stringstream msgFormatter;
    msgFormatter << "Simulated DHS: Created image " << formatedId.str();
    AXIS2_LOG_INFO_MSG(log, msgFormatter.str().c_str());
    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageLifeTime(const ImageId& id,
        DHS_BD_LIFETIME lifeTime) {
    stringstream msgFormatter;
    msgFormatter << "Simulated DHS: Set lifetime for image " << id << " to "
            << DhsUtil::translateLifetime(lifeTime);
    AXIS2_LOG_INFO_MSG(log, msgFormatter.str().c_str());
    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageContrib(const ImageId& id,
        const vector<string>& contribs) {
    stringstream msgFormatter;
    msgFormatter << "Simulated DHS: Set contributors for image " << id
            << ": { ";

    copy(contribs.begin(), contribs.end(),
            ostream_iterator<string>(msgFormatter, " "));

    msgFormatter << "}";
    AXIS2_LOG_INFO_MSG(log, msgFormatter.str().c_str());

    return DHS_S_SUCCESS;
}

DHS_STATUS DhsAdapterSim::setImageKeywords(const ImageId& id,
        const vector<Keyword>& keywords, bool final) {

    stringstream msgFormatter;
    msgFormatter << "Simulated DHS: Set keywords for image " << id
            << " (final = " << boolalpha << final << ")";
    AXIS2_LOG_INFO_MSG(log, msgFormatter.str().c_str());
    DHS_STATUS ret = DHS_S_SUCCESS;

    for (vector<Keyword>::const_iterator iter = keywords.begin();
            iter != keywords.end(); iter++) {
        msgFormatter.str("");
        msgFormatter << '\t' << iter->getName() << " = ";
        switch (iter->getType()) {
        case DHS_DT_INT8: {
            msgFormatter << iter->getValue<int8_t>();
            break;
        }
        case DHS_DT_INT16: {
            msgFormatter << iter->getValue<int16_t>();
            break;
        }
        case DHS_DT_INT32: {
            msgFormatter << iter->getValue<int32_t>();
            break;
        }
        case DHS_DT_UINT8: {
            msgFormatter << iter->getValue<uint8_t>();
            break;
        }
        case DHS_DT_UINT16: {
            msgFormatter << iter->getValue<uint16_t>();
            break;
        }
        case DHS_DT_UINT32: {
            msgFormatter << iter->getValue<uint32_t>();
            break;
        }
        case DHS_DT_FLOAT: {
            msgFormatter << iter->getValue<float>();
            break;
        }
        case DHS_DT_DOUBLE: {
            msgFormatter << iter->getValue<double>();
            break;
        }
        case DHS_DT_STRING: {
            msgFormatter << iter->getValue<const string&>();
            ;
            break;
        }
        case DHS_DT_BOOLEAN: {
            msgFormatter << boolalpha << iter->getValue<bool>();
            break;
        }
        default: {
            throw(std::logic_error("Keyword object with invalid type."));
        }
        }
        msgFormatter << " (" << DhsUtil::translateType(iter->getType()) << ")";
        if (!isKeywordTypeOk(iter->getName(), iter->getType())) {
            ret = DHS_E_CTL_CMD;
        }
        AXIS2_LOG_INFO_MSG(log, msgFormatter.str().c_str());
    }
    return ret;
}

bool DhsAdapterSim::isKeywordTypeOk(const std::string& name,
        DHS_DATA_TYPE type) {
    if (keyTypes.find(name) == keyTypes.end()) {
        return type == DHS_DT_STRING;
    } else {
        return keyTypes[name] == type;
    }
}
