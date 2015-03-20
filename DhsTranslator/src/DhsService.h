/*
 * DhsService.h
 *
 *  Created on: Feb 12, 2015
 *      Author: jluhrs
 */

#ifndef DHSTRANSLATOR_SRC_DHSSERVICE_H_
#define DHSTRANSLATOR_SRC_DHSSERVICE_H_

#include <string>
#include <sstream>
#include <vector>
#include <boost/optional.hpp>
#include <axis2_svc_skeleton.h>
#include <axiom_text.h>
#include <axiom_element.h>
#include <axiom_util.h>
#include <dhs.h>

#include "../src/IDhsAdapter.h"

class DhsService {
private:
    static const char * const SERVICE_NAME;
    static const char * const SIM_PARAM;
    static const char * const MY_NAME_PARAM;
    static const char * const DHS_HOST_PARAM;
    static const char * const DHS_NAME_PARAM;
    static const char * const RESPONSE_NODE_NAME;
    static const char * const STATUS_NODE_NAME;
    static const char * const RESULT_NODE_NAME;
    static const char * const ERRORS_NODE_NAME;
    static const char * const ERROR_STATUS;
    static const char * const SUCCESS_STATUS;
    static const char * const XSI_NAMESPACE_PREFIX;
    static const char * const XSI_NAMESPACE_URI;
    static const char * const ENC_NAMESPACE_PREFIX;
    static const char * const ENC_NAMESPACE_URI;

    static const char * const DHS_ERROR;
    static const char * const BAD_REQUEST;
    static const char * const INTERNAL_ERROR;

    IDhsAdapter *dhsAdapter;
    static DhsService *theService;
    static const axis2_svc_skeleton_ops_t skeletonOps;

    DhsService();

    // Object methods implementing the axis2c skeleton methods
    axis2_status_t initWithConfig(axis2_svc_skeleton_t *svc_skeleton,
            const axutil_env_t *env, struct axis2_conf *conf);
    axiom_node_t* invoke(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);
    axiom_node_t* onFault(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env, axiom_node_t *node);
    axis2_status_t free(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env);

    // These are the actual operations
    axiom_node_t* createImage(const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);
    axiom_node_t* setParameters(const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);
    boost::optional<axiom_node_t*> setParameters(const std::string *imageId,
            const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);
    axiom_node_t* setKeywords(const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);

    // Helper method to translate from string to lifetime type.
    boost::optional<DHS_BD_LIFETIME> translateLifetime(
            boost::optional<std::string> val);
    // Helper method to translate from string to data type.
    boost::optional<DHS_DATA_TYPE> translateType(
            boost::optional<std::string> type);

    static axiom_node_t* createErrorNode(const std::string &type,
            const std::string &message, const axutil_env_t* env);
    static axiom_node_t* buildErrorResponse(const std::string &type,
            const std::string &message, const axutil_env_t* env);
    static axiom_node_t* buildErrorResponse(
            const std::vector<axiom_node_t*> &array, const axutil_env_t* env);
    static axiom_node_t* buildSuccessResponse(axiom_node_t* result,
            const axutil_env_t* env);

    //This parser will work for all types that work with std::istringstream
    template<typename T> boost::optional<T> parseVal(const axutil_env_t *env,
            axiom_node_t *node) {
        if (env != NULL && node != NULL
                && axiom_node_get_node_type(node, env) == AXIOM_TEXT) {
            axiom_text_t *valueElement =
                    (axiom_text_t*) axiom_node_get_data_element(node, env);
            const axis2_char_t *value = axiom_text_get_value(valueElement, env);
            if (value != NULL) {
                std::istringstream converter(value);
                T val;
                converter >> val;
                if (!converter.fail()) {
                    return boost::optional<T>(val);
                }
            }
        }
        return boost::optional<T>();
    }

    //Helper to build Keyword with typed value
    template<typename T> boost::optional<IDhsAdapter::Keyword> buildKeyword(
            const std::string &name, const axutil_env_t *env,
            axiom_node_t *node) {
        axiom_node_t *valNode = axiom_util_get_node_by_local_name(env, node,
                (axis2_char_t*) "value");
        if (valNode != NULL) {
            boost::optional<T> optVal = parseVal<T>(env,
                    axiom_node_get_first_child(valNode, env));
            if (optVal) {
                return boost::optional<IDhsAdapter::Keyword>(
                        IDhsAdapter::Keyword(name, optVal.get()));
            }
        }
        return boost::optional<IDhsAdapter::Keyword>();
    }

    /* Parse an array node to fill an array of type T. It relays on the
     * existence of a proper parseVal<T>. If there was a problem in the parsing
     * it puts returns false.
     */
    template<typename T> bool parseArrayNode(const axutil_env_t *env,
            axiom_node_t *node, std::vector<T> &array,
            const std::string &type) {
        bool ret = true;

        if (axiom_node_get_node_type(node, env) == AXIOM_ELEMENT) {
            axiom_element_t *nodeElement =
                    (axiom_element_t *) axiom_node_get_data_element(node, env);
            axutil_qname_t *typeAttrName = axutil_qname_create(env, "arrayType",
                    "http://schemas.xmlsoap.org/soap/encoding/", "enc");
            axiom_attribute_t *typeAttribute = axiom_element_get_attribute(
                    nodeElement, env, typeAttrName);
            if (typeAttribute != 0) {
                axutil_string_t * arrayType = axiom_attribute_get_value_str(
                        typeAttribute, env);
                if (std::string(axutil_string_get_buffer(arrayType, env)).compare(
                        0, type.size(), type) == 0) {
                    for (axiom_node_t *itemNode = axiom_node_get_first_child(
                            node, env); itemNode != NULL; itemNode =
                            axiom_node_get_next_sibling(itemNode, env)) {
                        if (axiom_node_get_node_type(itemNode, env)
                                == AXIOM_ELEMENT) {
                            axiom_element_t *itemElement =
                                    (axiom_element_t *) axiom_node_get_data_element(
                                            itemNode, env);
                            axis2_char_t *itemElementName =
                                    axiom_element_get_localname(itemElement,
                                            env);
                            if (std::string("item").compare(itemElementName)
                                    == 0) {
                                axiom_node_t *itemValNode =
                                        axiom_node_get_first_child(itemNode,
                                                env);
                                boost::optional<T> val = parseVal<T>(env,
                                        itemValNode);
                                if (val) {
                                    array.push_back(val.get());
                                } else {
                                    ret = false;
                                }
                            } else {
                                ret = false;
                            }
                        } else {
                            ret = false;
                        }
                    }
                } else {
                    ret = false;
                }
            } else {
                ret = false;
            }
            axutil_qname_free(typeAttrName, env);
        } else {
            ret = false;
        }
        return ret;
    }

    bool parseArrayNode(const axutil_env_t *env, axiom_node_t *node,
            std::vector<int> &array) {
        return parseArrayNode(env, node, array, "int");
    }
    bool parseArrayNode(const axutil_env_t *env, axiom_node_t *node,
            std::vector<std::string> &array) {
        return parseArrayNode(env, node, array, "string");
    }
    bool parseArrayNode(const axutil_env_t *env, axiom_node_t *node,
            std::vector<axiom_node_t *> &array) {
        return parseArrayNode(env, node, array, "object");
    }
    bool parseArrayNode(const axutil_env_t *env, axiom_node_t *node,
            std::vector<IDhsAdapter::Keyword> &array) {
        return parseArrayNode(env, node, array, "object");
    }

public:
    virtual ~DhsService();
    static axis2_svc_skeleton_t* create(const axutil_env_t *env);
    static axis2_status_t remove(axis2_svc_skeleton_t *svc_skeleton,
            const axutil_env_t *env);

    static int stubInit(axis2_svc_skeleton_t *svc_skeleton,
            const axutil_env_t *env);
    static int stubInitWithConfig(axis2_svc_skeleton_t *svc_skeleton,
            const axutil_env_t *env, struct axis2_conf * conf);
    static axiom_node_t* stubInvoke(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env, axiom_node_t *node,
            axis2_msg_ctx_t *msg_ctx);
    static axiom_node_t* stubOnFault(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env, axiom_node_t *node);
    static axis2_status_t stubFree(axis2_svc_skeleton_t *svc_skeli,
            const axutil_env_t *env);

    const IDhsAdapter* getDhsAdapter() const {
        return dhsAdapter;
    }

    void setDhsAdapter(IDhsAdapter* dhsAdapter) {
        this->dhsAdapter = dhsAdapter;
    }
};

//Type bool needs an specialized version because it needs to enable alpha
//values in the conversion.
template<> inline boost::optional<bool> DhsService::parseVal<bool>(
        const axutil_env_t *env, axiom_node_t *node) {
    if (env != NULL && node != NULL
            && axiom_node_get_node_type(node, env) == AXIOM_TEXT) {
        axiom_text_t *valueElement =
                (axiom_text_t*) axiom_node_get_data_element(node, env);
        const axis2_char_t *value = axiom_text_get_value(valueElement, env);
        if (value != NULL) {
            std::istringstream converter(value);
            bool val;
            try {
                converter >> std::boolalpha >> val;
            } catch (...) {
                return boost::optional<bool>();
            }
            return boost::optional<bool>(val);
        }
    }
    return boost::optional<bool>();
}

template<> inline boost::optional<std::string> DhsService::parseVal<std::string>(
        const axutil_env_t *env, axiom_node_t *node) {
    if (env != NULL && node != NULL
            && axiom_node_get_node_type(node, env) == AXIOM_TEXT) {
        axiom_text_t *valueElement =
                (axiom_text_t*) axiom_node_get_data_element(node, env);
        const axis2_char_t *value = axiom_text_get_value(valueElement, env);
        return boost::optional<std::string>(std::string(value));
    }
    return boost::optional<std::string>();
}

template<> inline boost::optional<axiom_node_t*> DhsService::parseVal<
        axiom_node_t*>(const axutil_env_t *env, axiom_node_t *node) {
    if (env != NULL && node != NULL) {
        return boost::optional<axiom_node_t*>(node);
    }
    return boost::optional<axiom_node_t*>();
}

template<> inline boost::optional<IDhsAdapter::Keyword> DhsService::parseVal<
        IDhsAdapter::Keyword>(const axutil_env_t *env, axiom_node_t *node) {
    boost::optional<std::string> optName;
    boost::optional<DHS_DATA_TYPE> optType;
    axiom_node_t *parentNode = axiom_node_get_parent(node, env);
    axiom_node_t *nameNode = axiom_util_get_node_by_local_name(env, parentNode,
            (axis2_char_t*) "name");
    if (nameNode != NULL) {
        optName = parseVal<std::string>(env,
                axiom_node_get_first_child(nameNode, env));
    }
    axiom_node_t *typeNode = axiom_util_get_node_by_local_name(env, parentNode,
            (axis2_char_t*) "type");
    if (typeNode != NULL) {
        optType = translateType(
                parseVal<std::string>(env,
                        axiom_node_get_first_child(typeNode, env)));
    }
    if (optName && optType) {
        switch (optType.get()) {
        case DHS_DT_UINT8: {
            return buildKeyword<uint8_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_UINT16: {
            return buildKeyword<uint16_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_UINT32: {
            return buildKeyword<uint32_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_INT8: {
            return buildKeyword<int8_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_INT16: {
            return buildKeyword<int16_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_INT32: {
            return buildKeyword<int32_t>(optName.get(), env, parentNode);
        }
        case DHS_DT_FLOAT: {
            return buildKeyword<float>(optName.get(), env, parentNode);
        }
        case DHS_DT_DOUBLE: {
            return buildKeyword<double>(optName.get(), env, parentNode);
        }
        case DHS_DT_STRING: {
            return buildKeyword<std::string>(optName.get(), env, parentNode);
        }
        case DHS_DT_BOOLEAN: {
            return buildKeyword<bool>(optName.get(), env, parentNode);
        }
        default: {
            throw(std::logic_error("Keyword object with invalid type."));
        }
        }
    }
    return boost::optional<IDhsAdapter::Keyword>();
}

#endif /* DHSTRANSLATOR_SRC_DHSSERVICE_H_ */
