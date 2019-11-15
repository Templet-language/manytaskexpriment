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

#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdio>

#include "JsonParser.h"

using namespace std;

const char VERSION[] = "Release x86 v.1";

const string CHARS_TO_TRIM = ".,:\"'`!?\\-+*/=_()[]{}&^¹$@%<>\t\n\v\f\r ";

string& ltrim(string& str, const string& chars)
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

string& rtrim(string& str, const string& chars)
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

string& trim(string& str, const string& chars)
{
	return ltrim(rtrim(str, chars), chars);
}

bool match(string str)
{
	return str.find_first_not_of("-abcdefghijklmnopqrstuvwxyz") == string::npos;
}

string block_number_to_formatted_string(int block_number)
{
	if (block_number >= 0 && block_number < 10) {
		return "00" + to_string(block_number);
	}
	else if (block_number >= 10 && block_number < 100) {
		return "0" + to_string(block_number);
	}
	else if (block_number >= 100 && block_number < 1000) {
		return to_string(block_number);
	}
	else {
		throw exception();
	}
}

string block_version_to_formatted_string(int block_version)
{
	if (block_version >= 0 && block_version < 10) {
		return "0" + to_string(block_version);
	}
	else if (block_version >= 10 && block_version < 100) {
		return "0" + to_string(block_version);
	}
	else {
		throw exception();
	}
}

string resolve_filename(int block_number, int block_version)
{
	return "E:\\file" + block_number_to_formatted_string(block_number) + ".v" + block_version_to_formatted_string(block_version);
}

int main(int argc, char* args[])
{
	chrono::system_clock::time_point start = chrono::system_clock::now();

	if (argc != 3) {
		std::cout << "It is necessary to pass 2 input arguments: first is a data block number"
			<< " and second is a block data version." << std::endl;

		return 1;
	}

	int block_number = stoi(args[1]);
	int block_version = stoi(args[2]);
	string filename = resolve_filename(block_number, block_version).c_str();
	string tempfile = tmpnam(nullptr);

	cout << "sorter.exe version: " << VERSION << endl;

	JsonParser* parser = new JsonParser();
	parser->parse(filename,tempfile);

	ifstream text_file;
	text_file.open(tempfile);

	map<string, int> m = map<string, int>();
	string word;
	while (text_file >> word) {
		string a = word;
		word = trim(word, CHARS_TO_TRIM);
		transform(word.begin(), word.end(), word.begin(), ::tolower);
		if (word.empty() || !match(word)) continue;
		m[word]++;
	}

	text_file.close();
	delete parser;

	filename = resolve_filename(block_number, block_version + 1).c_str();
	ofstream output;
	output.open(filename.c_str());

	for (map<string, int>::iterator it = m.begin(); it != m.end(); ++it) {
		output << it->first << " " << it->second << std::endl;
	}

	output.close();

	remove(tempfile.c_str());

	chrono::system_clock::time_point end = chrono::system_clock::now();
	chrono::duration<double> diff = end - start;
	std::cout << "Execution time is " << diff.count() << "s." << endl;

	return 0;
}
