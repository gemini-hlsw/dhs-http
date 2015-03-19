/*
 * DhsAdapter.h
 *
 *  Created on: Mar 18, 2015
 *      Author: jluhrs
 */

#ifndef DHSADAPTER_H_
#define DHSADAPTER_H_

#include "IDhsAdapter.h"
#include <dhs.h>
#include <axutil_log.h>
#include <pthread.h>

class DhsAdapter: public IDhsAdapter {
private:
    std::string myName;
    std::string serverHost;
    std::string serverName;
    axutil_log_t * log;
    pthread_mutex_t lock;
    DHS_CONNECT connection;
    DHS_THREAD  thread;
    bool dhsInitialized;

public:
    DhsAdapter(std::string &myName, std::string &serverHost,
            std::string &serverName, axutil_log_t * log);
    virtual ~DhsAdapter();

    DHS_STATUS createImage(ImageId& id);
    DHS_STATUS setImageLifeTime(const ImageId& id, DHS_BD_LIFETIME lifeTime);
    DHS_STATUS setImageContrib(const ImageId& id,
            const std::vector<std::string>& contribs);
    DHS_STATUS setImageKeywords(const ImageId& id,
            const std::vector<Keyword>& keywords, bool final = false);
    void setTimeout(unsigned int timeout);
};

#endif /* DHSADAPTER_H_ */
