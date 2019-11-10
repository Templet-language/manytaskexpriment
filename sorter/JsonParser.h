/*--------------------------------------------------------------------------*/
/*  Copyright 2019 Templet project developers                               */
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

#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/encodedstream.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

FILE* file = NULL;
bool isText = false;
bool isFullText = false;
bool ignoreText = false;
bool isHashtagArray = false;
bool isHashtag = false;
bool waitForCheckLang = false;
bool checkLang = false;

struct MyHandler : public  rapidjson::BaseReaderHandler< rapidjson::UTF8<>, MyHandler>
{
	ofstream fout;
	string textBuf;
	string fullTextBuf;
	bool Null() { return true; }
	bool Bool(bool b) { return true; }
	bool Int(int i) { return true; }
	bool Uint(unsigned u) { return true; }
	bool Int64(int64_t i) { return true; }
	bool Uint64(uint64_t u) { return true; }
	bool Double(double d) { return true; }
	bool String(const char* str, rapidjson::SizeType length, bool copy)
	{
		if (isText)
		{
			if (isHashtag == false)  // ���� �� ������ �� ������� ��������, �������� text � textBuf
			{
				textBuf = deleteURL(str);
				waitForCheckLang = true;
			}
			else
			{
				isHashtag = false;
			}
			isText = false;
		}
		else if (isFullText)
		{
			fullTextBuf = deleteURL(str);
			isFullText = false;
		}
		else if (checkLang)
		{
			if (strcmp(str, "en") == 0)
			{
				
				if (!fullTextBuf.empty())  //  ���� ���� full_text ���������� text � full_text
				{
                    //fout << fullTextBuf << " ";
					// ���� ������ 85 �������� ���������, ��������� ������ full_text
					//if (fullTextBuf.compare(0, 85, textBuf.substr(0, 85)) == 0) textBuf.clear();
				}
				if (!textBuf.empty())
				{
					// ���� �� ������
					if (textBuf.find("RT") == string::npos) fout << textBuf << " ";
				}
				
				fout << textBuf << " ";
				textBuf.clear();
				fullTextBuf.clear();
			}
			waitForCheckLang = false;
			checkLang = false;
		}
		return true;
	}
	bool StartObject() { return true; }
	bool Key(const char* str, rapidjson::SizeType length, bool copy)
	{
		if (strcmp(str, "text") == 0)	// ����������� ���� text
		{
			isText = true;
		}
		else if (strcmp(str, "full_text") == 0)	 // ����������� ���� full_text
		{
			isFullText = true;
		}
		else if (strcmp(str, "lang") == 0)		// ����������� ���� lang
		{
			if (waitForCheckLang) checkLang = true;		// ���� lang ��������� � ���� text, ������� ��������� �������� lang
		}
		else if (strcmp(str, "hashtags") == 0)
		{
			isHashtagArray = true;
		}
		else if ((strcmp(str, "indices") == 0))
		{
			if (isHashtagArray) isHashtag = true;
		}
		return true;
	}
	bool EndObject(rapidjson::SizeType memberCount) { return true; }
	bool StartArray() { return true; }
	bool EndArray(rapidjson::SizeType elementCount)
	{
		if ((isHashtagArray) && (isHashtag == false)) isHashtagArray = false;
		return true;
	}

	string deleteURL(const char* str)
	{
		string string = str;
		size_t beginOfNextLine = string.find_first_of('\n');
		while (beginOfNextLine != string::npos)
		{
			string.erase(beginOfNextLine, 1);
			beginOfNextLine = string.find_first_of('\n');
		}

		size_t beginURL = string.find("http");
		while (beginURL != string::npos)
		{
			if (beginURL != 0) string.erase(beginURL - 1, 24);
			else string.erase(beginURL, 23);
			beginURL = string.find("http");
		}
		return string;
	}
};

class JsonParser
{
private:

	char readBuffer[256];

public:

	MyHandler handler;

	void parse(string filename)
	{
		fopen_s(&file, filename.c_str(), "r");
		if (file == NULL) {
		    throw exception();
		} else {
			rapidjson::FileReadStream in(file, readBuffer, sizeof(readBuffer));
			handler.fout.open(filename.append(".txt"));
			rapidjson::Reader reader;
			int counter = 0;
			while (!reader.Parse(in, handler)) {
//				std::cout << ++counter << endl;
			}
		}
	}

	~JsonParser()
	{
		if (file!=NULL) fclose(file);
		handler.fout.close();
	}
};