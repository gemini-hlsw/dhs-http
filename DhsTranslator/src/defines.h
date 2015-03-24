/*
 * defines.h
 *
 *  Created on: Mar 6, 2015
 *      Author: jluhrs
 */

#ifndef DHSTRANSLATOR_SRC_DEFINES_H_
#define DHSTRANSLATOR_SRC_DEFINES_H_

#ifdef AXIS2_LOG_PROJECT_PREFIX
#error "Header file defines.h must be included before axis2c headers."
#else
#define AXIS2_LOG_PROJECT_PREFIX "[dhs]"
#endif

#endif /* DHSTRANSLATOR_SRC_DEFINES_H_ */
