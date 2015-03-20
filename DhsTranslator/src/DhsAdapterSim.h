/*
 * DhsAdapterSim.h
 *
 *  Created on: Feb 18, 2015
 *      Author: jluhrs
 */

#ifndef DHSTRANSLATOR_SRC_DHSADAPTERSIM_H_
#define DHSTRANSLATOR_SRC_DHSADAPTERSIM_H_

#include <string>
#include <dhs.h>
#include <axutil_log.h>
#include <pthread.h>
#include "IDhsAdapter.h"

class DhsAdapterSim: public IDhsAdapter {
private:
    static const int MAX_IMAGE_ID = 10000;

    std::string myName;
    std::string serverHost;
    std::string serverName;
    int nextId;
    axutil_log_t * log;
    pthread_mutex_t lock;

    std::string translateLifetime(DHS_BD_LIFETIME val);
    std::string translateType(DHS_DATA_TYPE type);

public:
    DhsAdapterSim(std::string &myName, std::string &serverHost,
            std::string &serverName, axutil_log_t * log);
    virtual ~DhsAdapterSim();

    virtual DHS_STATUS createImage(ImageId& id);
    virtual DHS_STATUS setImageLifeTime(const ImageId& id,
            DHS_BD_LIFETIME lifeTime);
    virtual DHS_STATUS setImageContrib(const ImageId& id,
            const std::vector<std::string> &contribs);
    virtual DHS_STATUS setImageKeywords(const ImageId& id,
            const std::vector<Keyword> &keywords, bool final = false);
    virtual void setTimeout(unsigned int timeout);

};

#endif /* DHSTRANSLATOR_SRC_DHSADAPTERSIM_H_ */
