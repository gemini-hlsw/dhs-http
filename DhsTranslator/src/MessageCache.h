/*
 * MessageCache.h
 *
 *  Created on: Aug 11, 2020
 *      Author: jluhrs
 */

#ifndef SRC_MESSAGECACHE_H_
#define SRC_MESSAGECACHE_H_

#include <string>
#include <map>
#include <list>
#include <ctime>
#include <stddef.h>
#include <functional>
#include <boost/functional/hash.hpp>
#include <boost/thread.hpp>
#include "IDhsAdapter.h"

struct Message {
    size_t hash;
    std::time_t timestamp;
    std::string data;
};

class MessageCache {
private:
	boost::hash<std::string> hasher;
	std::map<IDhsAdapter::ImageId, std::list<struct Message*> > cache;
	std::time_t validAge;
	boost::timed_mutex lock;

	static bool isEmptyList(std::pair<IDhsAdapter::ImageId, std::list<struct Message*> > pp) {
	    return pp.second.empty();
	}

public:

	MessageCache(std::time_t deltat);

	virtual ~MessageCache();

	void purgeOldMessages(time_t currentTime);

	bool exists(IDhsAdapter::ImageId const &id, std::string const &data);

	void add(IDhsAdapter::ImageId const &id, time_t timestamp, std::string const &data);

};

#endif /* SRC_MESSAGECACHE_H_ */
