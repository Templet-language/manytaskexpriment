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


