/**
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

#include "Phase2Cmp.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

void Phase2Cmp::setPhase1(IPhase1* phase1) {
    std::cout << "setting phase1 for phase2\n";
    this->phase1 = phase1;
}

void Phase2Cmp::setLogService(const log_service_t* logSrv) {
    this->logSrv = logSrv;
}

double Phase2Cmp::getData() {
    logSrv->log(logSrv->logger, OSGI_LOGSERVICE_DEBUG, (char*)"getting data from phase2cmp B\n");
    return phase1->getData() * 24.0;
};
