/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _RSA_REQUEST_HANDLER_SERVICE_H_
#define _RSA_REQUEST_HANDLER_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <celix_properties.h>
#include <celix_errno.h>
#include <sys/uio.h>

#define RSA_REQUEST_HANDLER_SERVICE_NAME "rsa_request_handler_service"
#define RSA_REQUEST_HANDLER_SERVICE_VERSION "1.0.0"
#define RSA_REQUEST_HANDLER_SERVICE_USE_RANGE "[1.0.0,2)"

typedef struct rsa_request_handler_service {
    void *handle;
    /**
     * @brief
     *
     * @param handle
     * @param[in, out] metadata If metadata is not required，it can be set NULL
     * @param request
     * @param response
     * @return
     */
    celix_status_t (*handleRequest)(void *handle, celix_properties_t *metadata, const struct iovec *request, struct iovec *response);
}rsa_request_handler_service_t;


#ifdef __cplusplus
}
#endif

#endif /* _RSA_REQUEST_HANDLER_SERVICE_H_ */
