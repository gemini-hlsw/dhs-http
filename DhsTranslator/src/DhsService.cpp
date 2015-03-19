/*
 * DhsService.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: jluhrs
 */

#include "defines.h"
#include "DhsService.h"
#include <axiom_element.h>
#include <axis2_conf.h>
#include <sstream>
#include "DhsAdapter.h"
#include "DhsAdapterSim.h"

using namespace std;
using namespace boost;

const char * const DhsService::SERVICE_NAME = "dhs";
const char * const DhsService::SIM_PARAM = "dhsSimulate";
const char * const DhsService::MY_NAME_PARAM = "myName";
const char * const DhsService::DHS_HOST_PARAM = "dhsHost";
const char * const DhsService::DHS_NAME_PARAM = "dhsName";
const char * const DhsService::RESPONSE_NODE_NAME = "response";
const char * const DhsService::STATUS_NODE_NAME = "status";
const char * const DhsService::RESULT_NODE_NAME = "result";
const char * const DhsService::ERRORS_NODE_NAME = "errors";
const char * const DhsService::ERROR_STATUS = "error";
const char * const DhsService::SUCCESS_STATUS = "success";
const char * const DhsService::XSI_NAMESPACE_PREFIX = "xsi";
const char * const DhsService::XSI_NAMESPACE_URI =
        "http://www.w3.org/2001/XMLSchema-instance";
const char * const DhsService::ENC_NAMESPACE_PREFIX = "enc";
const char * const DhsService::ENC_NAMESPACE_URI =
        "http://schemas.xmlsoap.org/soap/encoding/";
const char * const DhsService::DHS_ERROR = "DHS_ERROR";
const char * const DhsService::BAD_REQUEST = "BAD_REQUEST";
const char * const DhsService::INTERNAL_ERROR = "INTERNAL_SERVER_ERROR";

DhsService *DhsService::theService = NULL;
const axis2_svc_skeleton_ops_t DhsService::skeletonOps = { DhsService::stubInit,
        DhsService::stubInvoke, DhsService::stubOnFault, DhsService::stubFree,
        DhsService::stubInitWithConfig };

DhsService::DhsService() {
    dhsAdapter = NULL;
}

DhsService::~DhsService() {
    if (dhsAdapter != NULL) {
        delete dhsAdapter;
    }
}

axis2_status_t DhsService::initWithConfig(axis2_svc_skeleton_t*,
        const axutil_env_t*, struct axis2_conf *) {

    return AXIS2_SUCCESS;
}

axiom_node_t* DhsService::invoke(axis2_svc_skeleton_t*, const axutil_env_t* env,
        axiom_node_t* node, axis2_msg_ctx_t* msg_ctx) {

    if (node) {
        if (axiom_node_get_node_type(node, env) == AXIOM_ELEMENT) {
            axiom_element_t *element = NULL;
            element = (axiom_element_t *) axiom_node_get_data_element(node,
                    env);
            if (element) {
                axis2_char_t *op_name = axiom_element_get_localname(element,
                        env);

                if (op_name) {
                    if (axutil_strcmp(op_name, "createImage") == 0) {
                        return createImage(env, node, msg_ctx);
                    }
                    if (axutil_strcmp(op_name, "setParameters") == 0) {
                        return setParameters(env, node, msg_ctx);
                    }
                    if (axutil_strcmp(op_name, "setKeywords") == 0) {
                        return setKeywords(env, node, msg_ctx);
                    }
                }
            }
        }
    }
    return NULL;
}

axiom_node_t* DhsService::onFault(axis2_svc_skeleton_t*, const axutil_env_t*,
        axiom_node_t*) {
    return NULL;
}

axis2_status_t DhsService::free(axis2_svc_skeleton_t* svc_skeleton,
        const axutil_env_t* env) {
    if (svc_skeleton != NULL) {
        if (svc_skeleton->func_array) {
            axutil_array_list_free(svc_skeleton->func_array, env);
            svc_skeleton->func_array = NULL;
        }

        delete svc_skeleton;
        svc_skeleton = NULL;
    }

    return AXIS2_SUCCESS;
}

axis2_svc_skeleton_t* DhsService::create(const axutil_env_t* env) {
    AXIS2_LOG_INFO_MSG(env->log, "Create service.");

    if (theService == NULL) {
        theService = new DhsService();
    }

    axis2_svc_skeleton_t *skeleton = new axis2_svc_skeleton_t;
    skeleton->ops = &DhsService::skeletonOps;
    skeleton->func_array = NULL;

    return skeleton;
}

axis2_status_t DhsService::remove(axis2_svc_skeleton_t *svc_skeleton,
        const axutil_env_t *env) {
    AXIS2_LOG_INFO_MSG(env->log, "Remove service.");

    if (theService == NULL) {
        return AXIS2_FAILURE;
    }
    axis2_status_t status = theService->free(svc_skeleton, env);

    delete theService;
    theService = NULL;

    return status;
}

int DhsService::stubInitWithConfig(axis2_svc_skeleton_t* svc_skeleton,
        const axutil_env_t* env, struct axis2_conf *conf) {

    if (theService == NULL) {
        return AXIS2_FAILURE;
    }

    bool simulate = false;
    struct axis2_svc* service = axis2_conf_get_svc(conf, env, SERVICE_NAME);
    axutil_param_t* param = axis2_svc_get_param(service, env, SIM_PARAM);
    if (param != NULL) {
        const char* simFlag = (const char*) axutil_param_get_value(param, env);
        if (simFlag != NULL) {
            istringstream converter(simFlag);
            converter >> boolalpha >> simulate;
        }
    }
    param = axis2_svc_get_param(service, env, MY_NAME_PARAM);
    string myName((const char*) axutil_param_get_value(param, env));

    param = axis2_svc_get_param(service, env, DHS_HOST_PARAM);
    string dhsHost((const char*) axutil_param_get_value(param, env));

    param = axis2_svc_get_param(service, env, DHS_NAME_PARAM);
    string dhsName((const char*) axutil_param_get_value(param, env));

    IDhsAdapter *pAdapter = NULL;
    if (simulate) {
        pAdapter = new DhsAdapterSim(myName, dhsHost, dhsName, env->log);

        AXIS2_LOG_INFO_MSG(env->log, "Using simulated DHS adapter.");

    } else {
        pAdapter = new DhsAdapter(myName, dhsHost, dhsName, env->log);

        AXIS2_LOG_INFO_MSG(env->log, "Using real DHS adapter.");
    }

    if (pAdapter == NULL) {
        return AXIS2_FAILURE;
    }

    theService->setDhsAdapter(pAdapter);

    return theService->initWithConfig(svc_skeleton, env, conf);
}

axiom_node_t* DhsService::stubInvoke(axis2_svc_skeleton_t* svc_skeleton,
        const axutil_env_t* env, axiom_node_t* node, axis2_msg_ctx_t* msg_ctx) {
    if (theService != NULL) {
        try {
            return theService->invoke(svc_skeleton, env, node, msg_ctx);
        } catch (std::exception &e) {
            AXIS2_LOG_ERROR_MSG(env->log, e.what());
        }
    }
    return buildErrorResponse(INTERNAL_ERROR,
            "Internal error processing request.", env);
}

axiom_node_t* DhsService::stubOnFault(axis2_svc_skeleton_t* svc_skeleton,
        const axutil_env_t* env, axiom_node_t* node) {
    if (theService == NULL) {
        return NULL;
    }
    return theService->onFault(svc_skeleton, env, node);
}

int DhsService::stubInit(axis2_svc_skeleton_t*, const axutil_env_t*) {
    return AXIS2_SUCCESS;
}

axis2_status_t DhsService::stubFree(axis2_svc_skeleton_t* svc_skeleton,
        const axutil_env_t* env) {
    if (theService == NULL) {
        return AXIS2_FAILURE;
    }
    return theService->free(svc_skeleton, env);
}

axiom_node_t* DhsService::createImage(const axutil_env_t* env,
        axiom_node_t* node, axis2_msg_ctx_t* msg_ctx) {

    AXIS2_LOG_INFO_MSG(env->log, "Processing createImage request.");

    IDhsAdapter::ImageId imageId;
    DHS_STATUS status = dhsAdapter->createImage(imageId);

    if (status != DHS_S_SUCCESS) {
        return buildErrorResponse(DHS_ERROR,
                "DHS error while trying to create new data set.", env);
    }

    if (node != NULL && axiom_node_get_first_child(node, env) != NULL) {
        optional<axiom_node_t*> errors = setParameters(&imageId, env, node,
                msg_ctx);
        if (errors) {
            return errors.get();
        }
    }

    axiom_node_t *retNode = NULL;
    axiom_text_create(env, NULL, imageId.c_str(), &retNode);
    return buildSuccessResponse(retNode, env);
}

axiom_node_t* DhsService::setParameters(const axutil_env_t* env,
        axiom_node_t* node, axis2_msg_ctx_t* msg_ctx) {

    AXIS2_LOG_INFO_MSG(env->log, "Processing setParameters request.");

    optional<axiom_node_t*> errors = setParameters(NULL, env, node, msg_ctx);
    if (errors) {
        return errors.get();
    }

    return buildSuccessResponse(NULL, env);
}

// Returns an empty optional if successful
optional<axiom_node_t*> DhsService::setParameters(const std::string *imageId,
        const axutil_env_t* env, axiom_node_t* node, axis2_msg_ctx_t*) {

    vector<string> contributors;
    DHS_BD_LIFETIME lifetime = DHS_BD_LT_TRANSIENT;
    bool lifetimeSet = false;
    string id;
    if (imageId != NULL) {
        id = *imageId;
    }

    for (axiom_node_t *paramNode = axiom_node_get_first_child(node, env);
            paramNode != NULL;
            paramNode = axiom_node_get_next_sibling(paramNode, env)) {
        if (axiom_node_get_node_type(paramNode, env) == AXIOM_ELEMENT) {
            axiom_element_t *paramElement =
                    (axiom_element_t *) axiom_node_get_data_element(paramNode,
                            env);
            axis2_char_t *paramName = axiom_element_get_localname(paramElement,
                    env);
            if (std::string("lifetime").compare(paramName) == 0) {
                optional<DHS_BD_LIFETIME> optLifetime = translateLifetime(
                        parseVal<string>(env,
                                axiom_node_get_first_child(paramNode, env)));
                if (optLifetime) {
                    lifetime = optLifetime.get();
                    lifetimeSet = true;
                }
            } else if (std::string("contributors").compare(paramName) == 0) {
                parseArrayNode(env, paramNode, contributors);
            } else if (string("id").compare(paramName) == 0 && imageId == NULL) {
                optional<string> optIdVal = parseVal<string>(env,
                        axiom_node_get_first_child(paramNode, env));
                if (optIdVal) {
                    id.assign(optIdVal.get());
                }
            }
        }
    }
    if (!id.empty()) {
        if (lifetimeSet) {
            DHS_STATUS status = dhsAdapter->setImageLifeTime(id, lifetime);
            if (status != DHS_S_SUCCESS) {
                return buildErrorResponse(DHS_ERROR,
                        "DHS error while trying to set data set lifetime.",
                        env);
            }
        }
        if (!contributors.empty()) {
            DHS_STATUS status = dhsAdapter->setImageContrib(id, contributors);
            if (status != DHS_S_SUCCESS) {
                return buildErrorResponse(DHS_ERROR,
                        "DHS error while trying to set data set contributors.",
                        env);
            }
        }
    } else {
        return buildErrorResponse(BAD_REQUEST, "Could not parse image id.", env);
    }
    return optional<axiom_node_t*>();
}

axiom_node_t* DhsService::setKeywords(const axutil_env_t* env,
        axiom_node_t* node, axis2_msg_ctx_t*) {

    AXIS2_LOG_INFO_MSG(env->log, "Processing setKeywords request.");

    vector<IDhsAdapter::Keyword> keywords;
    string id;
    bool final = false;
    bool keywordsParsed = false;

    for (axiom_node_t *paramNode = axiom_node_get_first_child(node, env);
            paramNode != NULL;
            paramNode = axiom_node_get_next_sibling(paramNode, env)) {
        if (axiom_node_get_node_type(paramNode, env) == AXIOM_ELEMENT) {
            axiom_element_t *paramElement =
                    (axiom_element_t *) axiom_node_get_data_element(paramNode,
                            env);
            axis2_char_t *paramName = axiom_element_get_localname(paramElement,
                    env);
            if (string("keywords").compare(paramName) == 0) {
                keywordsParsed = parseArrayNode(env, paramNode, keywords);
            } else if (string("id").compare(paramName) == 0) {
                optional<string> optId = parseVal<string>(env,
                        axiom_node_get_first_child(paramNode, env));
                if (optId) {
                    id = optId.get();
                }
            } else if (string("final").compare(paramName) == 0) {
                optional<bool> optFinal = parseVal<bool>(env,
                        axiom_node_get_first_child(paramNode, env));
                if (optFinal) {
                    final = optFinal.get();
                }
            }
        }
    }

    if (!id.empty() && keywordsParsed) {
        DHS_STATUS status = dhsAdapter->setImageKeywords(id, keywords, final);
        if (status != DHS_S_SUCCESS) {
            return buildErrorResponse(DHS_ERROR,
                    "DHS error while trying to set data set keywords.", env);
        }
    } else {
        if (id.empty()) {
            return buildErrorResponse(BAD_REQUEST, "Could not parse image id.",
                    env);
        } else {
            return buildErrorResponse(BAD_REQUEST, "Could not parse keywords.",
                    env);
        }
    }

    return buildSuccessResponse(NULL, env);
}

optional<DHS_BD_LIFETIME> DhsService::translateLifetime(
        boost::optional<string> val) {
    if (val) {
        if (val.get().compare("PERMANENT") == 0) {
            return optional<DHS_BD_LIFETIME>(DHS_BD_LT_PERMANENT);
        } else if (val.get().compare("TEMPORARY") == 0) {
            return optional<DHS_BD_LIFETIME>(DHS_BD_LT_TEMPORARY);
        } else if (val.get().compare("TRANSIENT") == 0) {
            return optional<DHS_BD_LIFETIME>(DHS_BD_LT_TRANSIENT);
        }
        return optional<DHS_BD_LIFETIME>();
    }
    return optional<DHS_BD_LIFETIME>();
}

optional<DHS_DATA_TYPE> DhsService::translateType(optional<string> type) {
    if (type) {
        if (type.get().compare("INT8") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_INT8);
        } else if (type.get().compare("INT16") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_INT16);
        } else if (type.get().compare("INT32") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_INT32);
        } else if (type.get().compare("UINT8") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_UINT8);
        } else if (type.get().compare("UINT16") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_UINT16);
        } else if (type.get().compare("UINT32") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_UINT32);
        } else if (type.get().compare("DOUBLE") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_DOUBLE);
        } else if (type.get().compare("FLOAT") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_FLOAT);
        } else if (type.get().compare("BOOLEAN") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_BOOLEAN);
        } else if (type.get().compare("STRING") == 0) {
            return optional<DHS_DATA_TYPE>(DHS_DT_STRING);
        }
    }
    return optional<DHS_DATA_TYPE>();
}

axiom_node_t* DhsService::createErrorNode(const std::string &type,
        const std::string &message, const axutil_env_t* env) {
    axiom_node_t *node, *typeNode, *msgNode, *dummy;
    axiom_element_create(env, NULL, "item", NULL, &node);
    axiom_element_create(env, node, "type", NULL, &typeNode);
    axiom_text_create(env, typeNode, type.c_str(), &dummy);
    axiom_element_create(env, node, "message", NULL, &msgNode);
    axiom_text_create(env, msgNode, message.c_str(), &dummy);
    return node;
}

axiom_node_t* DhsService::buildErrorResponse(const vector<axiom_node_t*> &array,
        const axutil_env_t* env) {
    axiom_node_t *node, *statusNode, *errorsNode, *dummy;
    axiom_namespace_t *xsiNamespace = axiom_namespace_create(env,
            XSI_NAMESPACE_URI, XSI_NAMESPACE_PREFIX);
    axiom_element_create(env, NULL, RESPONSE_NODE_NAME, xsiNamespace, &node);
    axiom_element_create(env, node, STATUS_NODE_NAME, NULL, &statusNode);
    axiom_text_create(env, statusNode, ERROR_STATUS, &dummy);
    axiom_element_t *errorsElement = axiom_element_create(env, node,
            ERRORS_NODE_NAME,
            NULL, &errorsNode);
    if (array.empty()) {
        axiom_element_add_attribute(errorsElement, env,
                axiom_attribute_create(env, "nil", "true", xsiNamespace),
                errorsNode);
    } else {
        ostringstream formatter;
        formatter << "object[" << array.size() << "]";
        axiom_element_add_attribute(errorsElement, env,
                axiom_attribute_create(env, "arrayType",
                        formatter.str().c_str(),
                        axiom_namespace_create(env, ENC_NAMESPACE_URI,
                                ENC_NAMESPACE_PREFIX)), errorsNode);
        for (vector<axiom_node_t*>::const_iterator iter = array.begin();
                iter != array.end(); iter++) {
            axiom_node_add_child(errorsNode, env, *iter);
        }
    }

    return node;
}

axiom_node_t* DhsService::buildErrorResponse(const std::string &type,
        const std::string &message, const axutil_env_t* env) {
    vector<axiom_node_t*> errors;
    errors.push_back(createErrorNode(type, message, env));
    return buildErrorResponse(errors, env);
}

axiom_node_t* DhsService::buildSuccessResponse(axiom_node_t* result,
        const axutil_env_t* env) {
    axiom_node_t *node, *statusNode, *resultNode, *dummy;
    axiom_namespace_t *xsiNamespace = axiom_namespace_create(env,
            XSI_NAMESPACE_URI, XSI_NAMESPACE_PREFIX);
    axiom_element_create(env, NULL, RESPONSE_NODE_NAME, NULL, &node);
    axiom_element_create(env, node, STATUS_NODE_NAME, NULL, &statusNode);
    axiom_text_create(env, statusNode, SUCCESS_STATUS, &dummy);
    axiom_element_t *resultElement = axiom_element_create(env, node,
            RESULT_NODE_NAME,
            NULL, &resultNode);
    if (result == NULL) {
        axiom_element_add_attribute(resultElement, env,
                axiom_attribute_create(env, "nil", "true", xsiNamespace),
                resultNode);
    } else {
        axiom_node_add_child(resultNode, env, result);
    }

    return node;
}
