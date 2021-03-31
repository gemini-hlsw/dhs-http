/**
 * IDhsAdapter.h
 *
 * This file defines the interface class IDhsAdapter
 *
 *  Created on: Feb 18, 2015
 *      Author: jluhrs
 */

#ifndef DHSTRANSLATOR_SRC_IDHSADAPTER_H_
#define DHSTRANSLATOR_SRC_IDHSADAPTER_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <dhs.h>
#include "DhsUtil.h"

/** Interface IDhsAdapter. This interface offers access to the DHS server for
 * creating and configuring images. All methods block the caller.
 */
class IDhsAdapter {
public:
    typedef std::string ImageId; /**< Image Id type */
    static ImageId& imageIdFromString(std::string &str) { return str; }
    static ImageId const & imageIdFromString(std::string const &str) { return str; }
    /**
     * Class to define keywords
     */
    class Keyword {
    public:
        Keyword() {
            type = DHS_DT_INT32;
            value.intVal = 0;
        }
        Keyword(const std::string &name, int8_t val) {
            this->name = name;
            type = DHS_DT_INT8;
            value.intVal = val;
        }
        Keyword(const std::string &name, int16_t val) {
            this->name = name;
            type = DHS_DT_INT16;
            value.intVal = val;
        }
        Keyword(const std::string &name, int32_t val) {
            this->name = name;
            type = DHS_DT_INT32;
            value.intVal = val;
        }
        Keyword(const std::string &name, uint8_t val) {
            this->name = name;
            type = DHS_DT_UINT8;
            value.intVal = val;
        }
        Keyword(const std::string &name, uint16_t val) {
            this->name = name;
            type = DHS_DT_UINT16;
            value.intVal = val;
        }
        Keyword(const std::string &name, uint32_t val) {
            this->name = name;
            type = DHS_DT_UINT32;
            value.intVal = val;
        }
        Keyword(const std::string &name, float val) {
            this->name = name;
            type = DHS_DT_FLOAT;
            value.floatVal = val;
        }
        Keyword(const std::string &name, double val) {
            this->name = name;
            type = DHS_DT_DOUBLE;
            value.doubleVal = val;
        }
        Keyword(const std::string &name, bool val) {
            this->name = name;
            type = DHS_DT_BOOLEAN;
            value.boolVal = val;
        }
        Keyword(const std::string &name, const std::string &val) {
            this->name = name;
            type = DHS_DT_STRING;
            value.stringVal = new std::string(val);
        }
        Keyword(const Keyword &other) {
            name = other.name;
            type = other.type;
            if (type == DHS_DT_STRING) {
                value.stringVal = new std::string(*other.value.stringVal);
            } else {
                value = other.value;
            }
        }
        Keyword & operator=(const Keyword &other) {
            name = other.name;
            type = other.type;
            if (type == DHS_DT_STRING) {
                value.stringVal = new std::string(*other.value.stringVal);
            } else {
                value = other.value;
            }
            return *this;
        }

        ~Keyword() {
            if (type == DHS_DT_STRING && value.stringVal != NULL) {
                delete value.stringVal;
            }
        }
        const std::string& getName() const {
            return name;
        }
        DHS_DATA_TYPE getType() const {
            return type;
        }
        template<typename T> T getValue() const {
            throw(std::logic_error(
                    "Attempt to retrieve unsupported type of value from Keyword object."));
        }
    private:
        std::string name;
        DHS_DATA_TYPE type;
        union {
            bool boolVal;
            unsigned long int uintVal;
            long int intVal;
            float floatVal;
            double doubleVal;
            std::string *stringVal;
        } value;
    };

    /**
     * Request an unique image ID to the DHS Server. The caller is responsible
     * for providing the object that will receive the image ID, and to
     * dispose of it.
     *
     * @param id pointer to object that will receive the image ID.
     * @return DHS_S_SUCCESS if the operation was successful, or the error code
     * otherwise.
     */
    virtual DHS_STATUS createImage(ImageId& id) = 0;

    /**
     * Configure the life time of an image.
     *
     * @param id pointer the image ID.
     * @param lifeTime type of life time.
     * @return DHS_S_SUCCESS if the operation was successful, or the error code
     * otherwise.
     */
    virtual DHS_STATUS setImageLifeTime(const ImageId& id,
            DHS_BD_LIFETIME lifeTime) = 0;

    /**
     * Set the list of contributor for an image.
     *
     * @param id pointer the image ID.
     * @param contribs vector with the names of the contributors.
     * @return DHS_S_SUCCESS if the operation was successful, or the error code
     * otherwise.
     */
    virtual DHS_STATUS setImageContrib(const ImageId& id,
            const std::vector<std::string>& contribs) = 0;

    /**
     * Set a group of keywords in an image.
     *
     * @param id pointer the image ID.
     * @param keywords a vector of keywords to be set.
     * @param final indicates if this is the last piece of the image.
     * @return DHS_S_SUCCESS if the operation was successful, or the error code
     * otherwise.
     */
    virtual DHS_STATUS setImageKeywords(const ImageId& id,
            const std::vector<Keyword>& keywords, bool final = false) = 0;

    virtual ~IDhsAdapter() {
    }

};

template<> inline int8_t IDhsAdapter::Keyword::getValue<int8_t>() const {
    if (type == DHS_DT_INT8) {
        return value.intVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve int8_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline int16_t IDhsAdapter::Keyword::getValue<int16_t>() const {
    if (type == DHS_DT_INT16) {
        return value.intVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve int16_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline int32_t IDhsAdapter::Keyword::getValue<int32_t>() const {
    if (type == DHS_DT_INT32) {
        return value.intVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve int32_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline uint8_t IDhsAdapter::Keyword::getValue<uint8_t>() const {
    if (type == DHS_DT_UINT8) {
        return value.uintVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve uint8_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline uint16_t IDhsAdapter::Keyword::getValue<uint16_t>() const {
    if (type == DHS_DT_UINT16) {
        return value.uintVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve uint16_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline uint32_t IDhsAdapter::Keyword::getValue<uint32_t>() const {
    if (type == DHS_DT_UINT32) {
        return value.uintVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve uint32_t value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline float IDhsAdapter::Keyword::getValue<float>() const {
    if (type == DHS_DT_FLOAT) {
        return value.floatVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve float value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline double IDhsAdapter::Keyword::getValue<double>() const {
    if (type == DHS_DT_DOUBLE) {
        return value.doubleVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve double value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline const std::string& IDhsAdapter::Keyword::getValue<
        const std::string&>() const {
    if (type == DHS_DT_STRING) {
        return *value.stringVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve string value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline std::string IDhsAdapter::Keyword::getValue<std::string>() const {
    if (type == DHS_DT_STRING) {
        return *value.stringVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve string value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

template<> inline bool IDhsAdapter::Keyword::getValue<bool>() const {
    if (type == DHS_DT_BOOLEAN) {
        return value.boolVal;
    }
    throw(std::logic_error(
            "Attempt to retrieve boolean value from Keyword of type "
                    + DhsUtil::translateType(type)));
}

#endif /* DHSTRANSLATOR_SRC_IDHSADAPTER_H_ */
