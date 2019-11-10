/*--------------------------------------------------------------------------*/
/*  Copyright 2019 Sergei Vostokin, Irina Bobyleva                          */
/*                                                                          */
/*  Licensed under the Apache License, Version 2.0 (the "License");         */
/*  you may not use this file except in compliance with the License.        */
/*  You may obtain a copy of the License at                                 */
/*                                                                          */
/*  http://www.apache.org/licenses/LICENSE-2.0                              */
/*                                                                          */
/*  Unless required by applicable law or agreed to in writing, software     */
/*  distributed under the License is distributed on an "AS IS" BASIS,       */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*  See the License for the specific language governing permissions and     */
/*  limitations under the License.                                          */
/*--------------------------------------------------------------------------*/

#pragma once

#include <string>

bool initEverestSession(const std::string& username, const std::string& password);
bool getApplications();
bool getApplication(const std::string& id);
bool submitPrepare(int i, int vi, std::string& id);
bool submitPlay(int i, int j, int vi, int vj, std::string& id);
bool probeJob(const std::string& id, std::string& state);
bool deleteJob(const std::string& id);
bool cleanupEverestSession();


