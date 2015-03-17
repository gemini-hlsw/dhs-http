/*
 * service.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: jluhrs
 */

#include <axis2_svc_skeleton.h>

#include "DhsService.h"

extern "C" {

AXIS2_EXPORT int axis2_get_instance(axis2_svc_skeleton_t **inst,
        const axutil_env_t *env) {
    *inst = DhsService::create(env);
    if (!(*inst)) {
        return AXIS2_FAILURE;
    }

    return AXIS2_SUCCESS;
}

AXIS2_EXPORT int axis2_remove_instance(axis2_svc_skeleton_t *inst,
        const axutil_env_t *env) {
    axis2_status_t status = AXIS2_FAILURE;
    if (inst) {
        status = DhsService::remove(inst, env);
    }
    return status;
}

}
