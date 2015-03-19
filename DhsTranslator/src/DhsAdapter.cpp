/*
 * DhsAdapter.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: jluhrs
 */

#include "defines.h"
#include "DhsAdapter.h"

using namespace std;

DhsAdapter::DhsAdapter(std::string &myName, string &serverHost,
        string &serverName, axutil_log_t * log) {
    this->myName = myName;
    this->serverHost = serverHost;
    this->serverName = serverName;
    this->log = log;

    pthread_mutex_init(&lock, NULL);

    DHS_STATUS status = DHS_S_SUCCESS;
    dhsInit(this->myName.c_str(), 1024 * 1024, &status);
    dhsEventLoop(DHS_ELT_THREADED, &thread, &status);
    if (status == DHS_S_SUCCESS) {
        dhsInitialized = true;
    }
    connection = dhsConnect(this->serverHost.c_str(), this->serverName.c_str(),
    NULL, &status);
}

DhsAdapter::~DhsAdapter() {
    if (connection != DHS_CONNECT_NULL) {
        DHS_STATUS status = DHS_S_SUCCESS;
        pthread_mutex_lock(&lock);
        dhsDisconnect(connection, &status);
        connection = DHS_CONNECT_NULL;
        pthread_mutex_unlock(&lock);
    }
    if (dhsInitialized) {
        DHS_STATUS status = DHS_S_SUCCESS;
        dhsEventLoopEnd(&status);
        dhsExit(&status);
    }

    pthread_mutex_destroy(&lock);
}

DHS_STATUS DhsAdapter::createImage(ImageId& id) {
    DHS_STATUS status = DHS_S_SUCCESS;

    pthread_mutex_lock(&lock);
    char *newId = dhsBdName(connection, &status);
    pthread_mutex_unlock(&lock);
    if (status == DHS_S_SUCCESS) {
        id.assign(newId);
    }

    return status;
}

DHS_STATUS DhsAdapter::setImageLifeTime(const ImageId& id,
        DHS_BD_LIFETIME lifeTime) {
    DHS_STATUS status = DHS_S_SUCCESS;

    pthread_mutex_lock(&lock);
    dhsBdCtl(connection, DHS_BD_CTL_LIFETIME, id.c_str(), lifeTime, &status);
    pthread_mutex_unlock(&lock);

    return status;
}

DHS_STATUS DhsAdapter::setImageContrib(const ImageId& id,
        const vector<string>& contribs) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if (!contribs.empty()) {
        const char **array = new const char*[contribs.size()];
        int i = 0;
        for (vector<string>::const_iterator iter = contribs.begin();
                iter != contribs.end(); iter++) {
            array[i++] = iter->c_str();
        }

        pthread_mutex_lock(&lock);
        dhsBdCtl(connection, DHS_BD_CTL_CONTRIB, id.c_str(),
                (int) contribs.size(), array, &status);
        pthread_mutex_unlock(&lock);

        delete[] array;
    }

    return status;
}

DHS_STATUS DhsAdapter::setImageKeywords(const ImageId& id,
        const vector<Keyword>& keywords, bool final) {
    DHS_STATUS status = DHS_S_SUCCESS;

    if (!keywords.empty()) {
        DHS_AV_LIST avList = dhsAvListNew(&status);
        for (vector<Keyword>::const_iterator iter = keywords.begin();
                iter != keywords.end(); iter++) {
            switch (iter->getType()) {
            case DHS_DT_UINT8: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<uint8_t>(), &status);
                break;
            }
            case DHS_DT_UINT16: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<uint16_t>(), &status);
                break;
            }
            case DHS_DT_UINT32: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<uint32_t>(), &status);
                break;
            }
            case DHS_DT_INT8: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<int8_t>(), &status);
                break;
            }
            case DHS_DT_INT16: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<int16_t>(), &status);
                break;
            }
            case DHS_DT_INT32: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<int32_t>(), &status);
                break;
            }
            case DHS_DT_FLOAT: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<float>(), &status);
                break;
            }
            case DHS_DT_DOUBLE: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<double>(), &status);
                break;
            }
            case DHS_DT_STRING: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<const std::string&>().c_str(), &status);
                break;
            }
            case DHS_DT_BOOLEAN: {
                dhsAvAdd(avList, iter->getName().c_str(), iter->getType(), 0, NULL,
                        iter->getValue<bool>(), &status);
                break;
            }
            default: {
            }
            }
        }
        if(dhsAvListSize(avList, &status) > 0) {
            pthread_mutex_lock(&lock);
            DHS_TAG tag = dhsBdPut(connection, id.c_str(), DHS_BD_PT_DS, final?DHS_TRUE:DHS_FALSE, avList, &status);
            dhsWait(1, &tag, &status);
            if (dhsStatus(tag, NULL, &status) != DHS_CS_DONE) {
                status = DHS_E_CTL_CMD;
            }
            DHS_STATUS s = DHS_S_SUCCESS;
            dhsTagFree(tag, &s);
            pthread_mutex_unlock(&lock);
        }
        DHS_STATUS status2 = DHS_S_SUCCESS;
        dhsAvListFree(avList, &status2);
    }

    return status;
}

void DhsAdapter::setTimeout(unsigned int timeout) {
}
