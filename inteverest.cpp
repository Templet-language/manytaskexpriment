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

#include "inteverest.h"
#include "json.hpp"

#include <string>
#include <iostream>
#include <curl/curl.h>

using json = nlohmann::json;
using namespace std;

const string EVEREST_URL = "https://everest.distcomp.org";
const string LABEL = "round-robin-tournament";
const std::string PREPARE_ID = "5ccaba85100000638af4eabe";
const std::string PLAY_ID = "5d22422012000050bcf95406";
const bool CURL_VERBOSE = false;

string ACCESS_TOKEN;

CURL *curl = NULL;
long code;
string response;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((string *)userp)->append((char *)contents, size * nmemb);
	return size * nmemb;
}

bool initEverestSession(const string& username, const string& password)
{
	curl_global_init(CURL_GLOBAL_ALL);
	
	curl = curl_easy_init();
	if (!curl) return false;

	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	if(CURL_VERBOSE)curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	json j;
	j["username"] = username;
	j["password"] = password;
	j["label"] = LABEL;
	string post = j.dump();

	string link = EVEREST_URL;
	link += "/auth/access_token";

	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());

	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
	
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	response.clear();

	curl_easy_perform(curl);
	
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200) {
		json responseJSON = json::parse(response);
		ACCESS_TOKEN = responseJSON["access_token"];
		return true;
	}

	return false;
}

bool getApplications()
{
	if (!curl) return false;

	string link = EVEREST_URL;
	link += "/api/apps/search?name=sort";

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
	response.clear();

	curl_easy_perform(curl);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200) {
		cout << response;
		return true;
	}
	
	return false;
}

bool getApplication(const string& id)
{
	if (!curl) return false;

	string link = EVEREST_URL;
	link += "/api/apps/search?name=";
	link += id;

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
	response.clear();

	curl_easy_perform(curl);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200) {
		cout << response;
		return true;
	}

	return false;
}

bool submitPrepare(int i, int vi, string& id)
{
	if (!curl) return false;

	string link = EVEREST_URL;
	json   jpost;
	string post;

	link += "/api/apps/" + PREPARE_ID;

	jpost["name"] = string("prepare_")+to_string(i);
	jpost["inputs"]["i"] = i;
	jpost["inputs"]["vi"] = vi;
	
	post = jpost.dump();

	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
	response.clear();

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200 || code == 201) {
		json j = json::parse(response);
		id = j["id"];
		return true;
	}

	return false;
}

bool submitPlay(int i, int j, int vi, int vj, std::string& id)
{
	if (!curl) return false;

	string link = EVEREST_URL;
	json   jpost;
	string post;

	link += "/api/apps/" + PLAY_ID;

	jpost["name"] = string("play_") + to_string(i) + "_" + to_string(j);
	jpost["inputs"]["i"] = i;
	jpost["inputs"]["j"] = j;
	jpost["inputs"]["vi"] = vi;
	jpost["inputs"]["vj"] = vj;

	post = jpost.dump();

	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_POST, 1L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
	response.clear();

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200 || code == 201) {
		json j = json::parse(response);
		id = j["id"];
		return true;
	}

	return false;
}

bool probeJob(const string& id, string& state)
{
	if (!curl) return false;

	string link = EVEREST_URL;
	link += "/api/jobs/";
	link += id;

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
	response.clear();

	curl_easy_perform(curl);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	if (code == 200 || code==201) {
		json j = json::parse(response);
		state = j["state"];
		return true;
	}

	return false;
}

bool deleteJob(const std::string& id)
{
	if (!curl) return false;

	string link = EVEREST_URL;
	link += "/api/jobs/";
	link += id;

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());

	curl_easy_perform(curl);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	return code == 200;
}

bool cleanupEverestSession()
{
	if (!curl) return false;
	
	string link = EVEREST_URL;
	link += "/api/auth/access_token";

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_URL, link.c_str());

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	curl = NULL;

	return  code == 200;
}
