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

#include <vector>
#include <iostream>
#include <chrono>
#include <map>
#include <fstream>
#include <string>

const char VERSION[] = "Release x86 v.1";
const char files_path[] = "E:\\";

using namespace std;

void block_merge(int less_block_num, int more_block_num, int less_block_ver, int more_block_ver) {
    char n_buf[10];

    std::string fname_less(files_path);
    std::string fname_more(files_path);

    sprintf(n_buf, "file%.3d", less_block_num);
    fname_less.append(n_buf);
    sprintf(n_buf, ".v%.2d", less_block_ver);
    fname_less.append(n_buf);

    sprintf(n_buf, "file%.3d", more_block_num);
    fname_more.append(n_buf);
    sprintf(n_buf, ".v%.2d", more_block_ver);
    fname_more.append(n_buf);

    map<string, int> allData;

    std::ifstream infile(fname_less);
    if (infile.is_open()) {
        std::string str;
        int number;
        while (infile >> str  >> number) allData[str] = number;   
    }
    infile.close();

    infile.open(fname_more);
    if (infile.is_open()) {
        std::string str;
        int number;
        while (infile >> str  >> number) allData[str] += number;
     }
    infile.close();

	std::string fname_less_new;
	std::string fname_more_new;

    fname_less_new.append(files_path);
    sprintf(n_buf, "file%.3d", less_block_num);
    fname_less_new.append(n_buf);
    sprintf(n_buf, ".v%.2d", (less_block_ver+1)%100);
    fname_less_new.append(n_buf);

    fname_more_new.append(files_path);
    sprintf(n_buf, "file%.3d", more_block_num);
    fname_more_new.append(n_buf);
    sprintf(n_buf, ".v%.2d", (more_block_ver+1)%100);
    fname_more_new.append(n_buf);

    unsigned halfsize = allData.size()/2 + allData.size()%2;
    unsigned currentPoint=1;
    std::ofstream outfile;
    std::ofstream outfile2;
    outfile.open(fname_less_new);
    outfile2.open(fname_more_new);
    for(auto elem : allData)
    {
        if (currentPoint<=halfsize){

            outfile << elem.first<<" "<<elem.second;
            if (currentPoint<halfsize)
                outfile<<endl;

        } else{
            outfile2 << elem.first<<" "<<elem.second;
            if (currentPoint<allData.size())
                outfile2<<endl;
        }

        currentPoint++;
     }
    outfile.close();
    outfile2.close();

	remove(fname_less.c_str());
	remove(fname_more.c_str());

}
int main(int argc, char *argv[])
{
    int less_block_num, more_block_num;
    int less_block_ver, more_block_ver;

    if (argc == 5 &&

        sscanf(argv[1], "%d", &less_block_num) == 1 &&
        sscanf(argv[2], "%d", &more_block_num) == 1 &&
        sscanf(argv[3], "%d", &less_block_ver) == 1 &&
        sscanf(argv[4], "%d", &more_block_ver) == 1) {

        cout << "merger.exe version: " << VERSION << endl;
   
		auto start = std::chrono::system_clock::now();
        block_merge(less_block_num, more_block_num, less_block_ver, more_block_ver);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = (end - start);
     
		cout << "Execution time is: " << diff.count() << "s" << endl;
        
		return EXIT_SUCCESS;
    }
    cout << "Error: bad params" << endl;

    return EXIT_FAILURE;
}