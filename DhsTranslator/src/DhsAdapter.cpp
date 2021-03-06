/*
 * DhsAdapter.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: jluhrs
 */

#include "defines.h"
#include "DhsAdapter.h"
#include <sstream>
#include <iomanip>
#include <iterator>

using namespace std;
using namespace boost;

DhsAdapter::DhsAdapter(std::string &myName, string &serverHost,
        string &serverName, axutil_log_t * log) {
    this->myName = myName;
    this->serverHost = serverHost;
    this->serverName = serverName;
    this->log = log;

    isConnected = false;
    connection = DHS_CONNECT_NULL;

    DHS_STATUS status = DHS_S_SUCCESS;
    AXIS2_LOG_INFO_MSG(log, "Initializing DHS library.");
    dhsInit(this->myName.c_str(), 1024 * 1024, &status);
    dhsEventLoop(DHS_ELT_THREADED, &thread, &status);
    if (status == DHS_S_SUCCESS) {
        dhsInitialized = true;
    }

    checkConnection();
}

DhsAdapter::~DhsAdapter() {
    if (connection != DHS_CONNECT_NULL) {
        DHS_STATUS status = DHS_S_SUCCESS;
        lock.lock();
        dhsDisconnect(connection, &status);
        connection = DHS_CONNECT_NULL;
        lock.unlock();
    }
    if (dhsInitialized) {
        AXIS2_LOG_INFO_MSG(log, "Stopping DHS event loop.");
        DHS_STATUS status = DHS_S_SUCCESS;
        dhsEventLoopEnd(&status);
        dhsExit(&status);
    }
}

bool DhsAdapter::checkConnection() {

    if (!lock.timed_lock(posix_time::seconds(LOCK_TIMEOUT))) {
        return false;
    }

    stringstream msg1, msg2;

	msg1 << "Checking connection, state is " << boolalpha << isConnected << ".";
    AXIS2_LOG_INFO_MSG(log, msg1.str().c_str());

	if(!isConnected) {
        DHS_STATUS status = DHS_S_SUCCESS;
        AXIS2_LOG_INFO_MSG(log, "Establishing connection to DHS.");
        connection = dhsConnect(this->serverHost.c_str(), this->serverName.c_str(),
                    NULL, &status);
        isConnected = ( status == DHS_S_SUCCESS);
    	msg2 << "Connection attempt result is " << boolalpha << isConnected << ".";
        AXIS2_LOG_INFO_MSG(log, msg2.str().c_str());
    }
    lock.unlock();
    return isConnected;
}

DHS_STATUS DhsAdapter::createImage(ImageId& id) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if(!checkConnection()) {
        return DHS_E_CTL_CMD;
    }

    if (!lock.timed_lock(posix_time::seconds(LOCK_TIMEOUT))) {
        return DHS_E_CTL_CMD;
    }
    char *newId = dhsBdName(connection, &status);
    lock.unlock();

    stringstream msg1, msg2;
    msg1 << "Create image request. Result is " << boolalpha << (status == DHS_S_SUCCESS) << ".";
    AXIS2_LOG_INFO_MSG(log, msg1.str().c_str());

    if (status == DHS_S_SUCCESS) {
        id.assign(newId);
        msg2 << "New image id is " << id << ".";
        AXIS2_LOG_INFO_MSG(log, msg2.str().c_str());
    }

    return status;
}

DHS_STATUS DhsAdapter::setImageLifeTime(const ImageId& id,
        DHS_BD_LIFETIME lifeTime) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if(!checkConnection()) {
        return DHS_E_CTL_CMD;
    }

    if (!lock.timed_lock(posix_time::seconds(LOCK_TIMEOUT))) {
        return DHS_E_CTL_CMD;
    }
    dhsBdCtl(connection, DHS_BD_CTL_LIFETIME, id.c_str(), lifeTime, &status);
    lock.unlock();

    stringstream msg;
    msg << "Set life time for image " << id << " to " << DhsUtil::translateLifetime(lifeTime) 
        << ", result is " << boolalpha << (status == DHS_S_SUCCESS) << ".";
    AXIS2_LOG_INFO_MSG(log, msg.str().c_str());

    return status;
}

DHS_STATUS DhsAdapter::setImageContrib(const ImageId& id,
        const vector<string>& contribs) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if(!checkConnection()) {
        return DHS_E_CTL_CMD;
    }

    if (!contribs.empty()) {
        const char **array = new const char*[contribs.size()];
        int i = 0;
        for (vector<string>::const_iterator iter = contribs.begin();
                iter != contribs.end(); iter++) {
            array[i++] = iter->c_str();
        }

        if (!lock.timed_lock(posix_time::seconds(LOCK_TIMEOUT))) {
            return DHS_E_CTL_CMD;
        }
        dhsBdCtl(connection, DHS_BD_CTL_CONTRIB, id.c_str(),
                (int) contribs.size(), array, &status);
        lock.unlock();

        delete[] array;
    }

    stringstream msg;
    msg << "Set contributors for image " << id << ": {";
    copy(contribs.begin(), contribs.end(), ostream_iterator<string>(msg, " "));
    msg << "}, result is " << boolalpha << (status == DHS_S_SUCCESS) << ".";
    AXIS2_LOG_INFO_MSG(log, msg.str().c_str());

    return status;
}

DHS_STATUS DhsAdapter::setImageKeywords(const ImageId& id,
        const vector<Keyword>& keywords, bool final) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if(!checkConnection()) {
        return DHS_E_CTL_CMD;
    }

    if (!keywords.empty()) {
        if (!lock.timed_lock(posix_time::seconds(LOCK_TIMEOUT))) {
            return DHS_E_CTL_CMD;
        }
        DHS_AV_LIST avList = dhsAvListNew(&status);
        for (vector<Keyword>::const_iterator iter = keywords.begin();
                iter != keywords.end(); iter++) {
            switch (iter->getType()) {
            case DHS_DT_UINT8: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<uint8_t>(), &status);
                break;
            }
            case DHS_DT_UINT16: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<uint16_t>(), &status);
                break;
            }
            case DHS_DT_UINT32: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<uint32_t>(), &status);
                break;
            }
            case DHS_DT_INT8: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<int8_t>(), &status);
                break;
            }
            case DHS_DT_INT16: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<int16_t>(), &status);
                break;
            }
            case DHS_DT_INT32: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<int32_t>(), &status);
                break;
            }
            case DHS_DT_FLOAT: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<float>(), &status);
                break;
            }
            case DHS_DT_DOUBLE: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<double>(), &status);
                break;
            }
            case DHS_DT_STRING: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<const std::string&>().c_str(), &status);
                break;
            }
            case DHS_DT_BOOLEAN: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0,
                NULL, iter->getValue<bool>(), &status);
                break;
            }
            default: {
            }
            }
        }
        if (dhsAvListSize(avList, &status) > 0) {
            DHS_TAG tag = dhsBdPut(connection, id.c_str(), DHS_BD_PT_DS,
                    final ? DHS_TRUE : DHS_FALSE, avList, NULL, &status);
            dhsWait(1, &tag, &status);
            if (dhsStatus(tag, NULL, &status) != DHS_CS_DONE) {
                status = DHS_E_CTL_CMD;
            }
            DHS_STATUS s = DHS_S_SUCCESS;
            dhsTagFree(tag, &s);
        }
        DHS_STATUS status2 = DHS_S_SUCCESS;
        dhsAvListFree(avList, &status2);
        lock.unlock();
    }

    stringstream msg;
    msg << "Set " << keywords.size() << " keywords for image " << id << ",  result is "
        << boolalpha << (status == DHS_S_SUCCESS) << ".";
    AXIS2_LOG_INFO_MSG(log, msg.str().c_str());

    return status;
}
