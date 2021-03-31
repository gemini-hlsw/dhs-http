/*
 * MessageCache.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: jluhrs
 */

#include <ctime>
#include <algorithm>
#include <functional>
#include <list>
#include "MessageCache.h"

using namespace std;

MessageCache::MessageCache(time_t deltat) {
    validAge = deltat;
}

MessageCache::~MessageCache() {
    lock.lock();

    for(map<IDhsAdapter::ImageId, list<Message*> >::iterator i = cache.begin(); i != cache.end(); i++) {
        for(list<Message*>::iterator j = i->second.begin(); j != i->second.end(); j++)
            delete *j;
        i->second.clear();
    }
    cache.clear();

    lock.unlock();
}

class PurgeList {
private:
    time_t validAge;
    time_t currentT;

public:
    PurgeList(time_t delta, time_t current) {
        validAge = delta;
        currentT = current;
    }

    void operator() (list<Message*> &mm) {
        while(!mm.empty() && (currentT - mm.front()->timestamp) > validAge) {
            Message* m = mm.front();
            mm.pop_front();
            delete m;
        }
    }
};

bool isEmpty(pair<IDhsAdapter::ImageId, list<Message*> > const &pp) { return pp.second.empty(); }

void MessageCache::purgeOldMessages(time_t currentTime) {
    PurgeList purgeOld(validAge, currentTime);

    lock.lock();
    for(map<IDhsAdapter::ImageId, list<Message*> >::iterator it = cache.begin(); it != cache.end(); ) {
        purgeOld(it->second);
        if(it->second.empty()) cache.erase(it++);
        else it++;
    }
    lock.unlock();
}

bool MessageCache::exists(IDhsAdapter::ImageId const &id, std::string const &data) {
    lock.lock();

    map<IDhsAdapter::ImageId, list<Message*> >::iterator l = cache.find(id);
    size_t h;

    if(l != cache.end()) {
        h = hasher(data);
        for(list<Message*>::const_iterator i = l->second.begin(); i != l->second.end(); i++) {
            if((*i)->hash == h && (*i)->data == data) {
                lock.unlock();
                return true;
            }
        }
    }
    lock.unlock();
    return false;
}

void MessageCache::add(IDhsAdapter::ImageId const &id, time_t timestamp, std::string const &data) {
    struct Message *msg = new Message;

    msg->data = data;
    msg->hash = hasher(data);
    msg->timestamp = timestamp;

    lock.lock();
    cache[id].push_back(msg);
    lock.unlock();

}
