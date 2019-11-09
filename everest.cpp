#include <iostream>
#include <list>
#include <chrono>
#include <thread>
#include <vector>

#include "inteverest.h"

const int MAX_SUBMITTED_TASKS = 10;
const int SLEEP_IN_SECONDS = 1;

using namespace std;

// количество блоков
int   NUM_BLOCKS;

// таблицы управления слиянием блоков
const int NA = -1;

// от какого слияния Dn(i,j) зависит
// данное слияние (i,j)
int** D1;
int** D2;

// на какое слияние In(i,j)
// влияет данное слияние (i,j)
int** I1;
int** I2;

// число операций слияния/сортировки
// D(i,j), предшествующих слиянию (i,j) 
int** D;

// на какое слияние S(k)->(i,j)
// влияет сортировка блока k
int** S;

// операция слияния B(k)->(i,j), которая последняя
// обрабатывала блок B(k)
int** B;

void init_tables()
{
	D1 = new int *[NUM_BLOCKS];
	D2 = new int *[NUM_BLOCKS];

	I1 = new int *[NUM_BLOCKS];
	I2 = new int *[NUM_BLOCKS];

	D = new int *[NUM_BLOCKS];

	S = new int *[NUM_BLOCKS];
	B = new int *[NUM_BLOCKS];

	for (int i = 0; i < NUM_BLOCKS; i++) {
		D1[i] = new int[NUM_BLOCKS];
		D2[i] = new int[NUM_BLOCKS];

		I1[i] = new int[NUM_BLOCKS];
		I2[i] = new int[NUM_BLOCKS];

		D[i] = new int[NUM_BLOCKS];

		S[i] = new int[2];
		B[i] = new int[2];
	}
}

// генерация таблиц, управляющих слиянием
void sym_init()
{
	for (int i = 0; i<NUM_BLOCKS; i++) {
		S[i][0] = S[i][1] = NA;
		B[i][0] = B[i][1] = NA;

		for (int j = 0; j<NUM_BLOCKS; j++) {
			D1[i][j] = D2[i][j] = NA;
			I1[i][j] = I2[i][j] = NA;
			D[i][j] = 0;
		}
	}
}

void sym_merge(int less_block, int more_block)
{
	if (B[less_block][0] != NA) {
		D1[more_block][less_block] = B[less_block][0]; D1[less_block][more_block] = B[less_block][1];
	}

	if (B[more_block][0] != NA) {
		D2[more_block][less_block] = B[more_block][0]; D2[less_block][more_block] = B[more_block][1];
	}

	B[less_block][0] = less_block; B[less_block][1] = more_block;
	B[more_block][0] = less_block; B[more_block][1] = more_block;
}

void sym_build_I1_I2_S()
{
	for (int i = 0; i < NUM_BLOCKS - 1; i++) {
		for (int j = i + 1; j < NUM_BLOCKS; j++) {

			bool d1 = false, d2 = false;

			if (D1[i][j] != NA) {

				if (I1[D1[i][j]][D1[j][i]] == NA) {
					I1[D1[i][j]][D1[j][i]] = i;
					I1[D1[j][i]][D1[i][j]] = j;
				}
				else if (I2[D1[i][j]][D1[j][i]] == NA) {
					I2[D1[i][j]][D1[j][i]] = i;
					I2[D1[j][i]][D1[i][j]] = j;
				}

				d1 = true;
			}
			if (D2[i][j] != NA) {

				if (I1[D2[i][j]][D2[j][i]] == NA) {
					I1[D2[i][j]][D2[j][i]] = i;
					I1[D2[j][i]][D2[i][j]] = j;
				}
				else if (I2[D2[i][j]][D2[j][i]] == NA) {
					I2[D2[i][j]][D2[j][i]] = i;
					I2[D2[j][i]][D2[i][j]] = j;
				}

				d2 = true;
			}

			if (!d1 && !d2) {
				S[i][0] = i; S[i][1] = j;
				S[j][0] = i; S[j][1] = j;
			}
			else if (d1 && !d2) {
				if (i == D1[j][i] || i == D1[i][j]) { S[j][0] = i; S[j][1] = j; }
				if (j == D1[j][i] || j == D1[i][j]) { S[i][0] = i; S[i][1] = j; }
			}
			else if (!d1 && d2) {
				if (i == D2[j][i] || i == D2[i][j]) { S[j][0] = i; S[j][1] = j; }
				if (j == D2[j][i] || j == D2[i][j]) { S[i][0] = i; S[i][1] = j; }
			}
		}
	}
}

void sym_build_D()
{
	for (int i = 0; i<NUM_BLOCKS; i++)
		if (S[i][0] != NA) D[S[i][0]][S[i][1]]++;

	for (int i = 0; i<NUM_BLOCKS - 1; i++) {
		for (int j = i + 1; j<NUM_BLOCKS; j++) {
			if (I1[i][j] != NA) D[I1[j][i]][I1[i][j]]++;
			if (I2[i][j] != NA) D[I2[j][i]][I2[i][j]]++;
		}
	}
}

void gen_optim(int less_block, int more_block)
{
	if (more_block - less_block + 1 == 1) return;

	if (more_block - less_block + 1 == 2) {
		sym_merge(less_block, more_block);
		return;
	}

	int less_block_0 = less_block;
	int more_block_0 = less_block + (more_block - less_block + 1) / 2 - 1;

	int less_block_1 = less_block + (more_block - less_block + 1) / 2;
	int more_block_1 = more_block;

	gen_optim(less_block_0, more_block_0);
	gen_optim(less_block_1, more_block_1);

	for (int i = more_block_1; i >= less_block_1; i--)
		for (int j = less_block_0; j <= more_block_0; j++) {
			sym_merge(j, i);
		}
}

// состояние портфеля задач
std::list< std::pair<int, int> > task_queue;

// список запущенных задач
struct submitted_task {
	int i, j;
	std::string id;
	bool completed;
};

std::list<submitted_task> submitted_task_list;

// версии блоков
std::vector<int> block_ver;

void init_sort()
{
	for (int i = 0; i<NUM_BLOCKS; i++)
		for (int j = 0; j<NUM_BLOCKS; j++)  D[i][j] = 0;

	for (int i = 0; i<NUM_BLOCKS; i++)
		if (S[i][0] != NA) D[S[i][0]][S[i][1]]++;

	for (int i = 0; i<NUM_BLOCKS - 1; i++) {
		for (int j = i + 1; j<NUM_BLOCKS; j++) {
			if (I1[i][j] != NA) D[I1[j][i]][I1[i][j]]++;
			if (I2[i][j] != NA) D[I2[j][i]][I2[i][j]]++;
		}
	}

	for (int i = 0; i < NUM_BLOCKS; i++) {
		std::pair<int, int> p(i, NA);
		task_queue.push_back(p);
	}
}

// обновление очереди задач по завершению задачи слияния 
void on_merge_end(int i, int j)
{
	if (I1[i][j] != NA) {
		if (--D[I1[j][i]][I1[i][j]] == 0) {
			std::pair<int, int> p(I1[j][i], I1[i][j]);
			task_queue.push_back(p);
		}
	}
	if (I2[i][j] != NA) {
		if (--D[I2[j][i]][I2[i][j]] == 0) {
			std::pair<int, int> p(I2[j][i], I2[i][j]);
			task_queue.push_back(p);
		}
	}
}

// обновление очереди задач по завершению задачи сортировки 
void on_sort_end(int i)
{
	if (--D[S[i][0]][S[i][1]] == 0) {
		std::pair<int, int> p(S[i][0], S[i][1]);
		task_queue.push_back(p);
	}
}

int main(int argc, char *argv[])
{
	if (argc == 4 && sscanf_s(argv[1], "%d", &NUM_BLOCKS) == 1) {

		string USERNAME(argv[2]);
		string PASSWORD(argv[3]);
		
		
		if (!initEverestSession(USERNAME,PASSWORD)) {
			cout << "Error: cannon init Everest session" << endl;
			exit(EXIT_FAILURE);
		}
		
		init_tables();
		sym_init();
		gen_optim(0, NUM_BLOCKS - 1);
		sym_build_I1_I2_S();
		sym_build_D();
		init_sort();

		bool done = false;
		int submitted_tasks_num = 0;
		submitted_task_list.clear();

		block_ver.assign(NUM_BLOCKS, 0);

		std::chrono::seconds delay(SLEEP_IN_SECONDS);
		auto start = std::chrono::system_clock::now();
		
		while (!done) {

			std::list<submitted_task>::iterator
				it = submitted_task_list.begin();

			// проверка статуса запущенных задач
			
			while(it != submitted_task_list.end()){
				submitted_task stask = *it;
				bool finished, failed;
				std::string state;
				
				if (!probeJob(stask.id, state)) {
					cout << "Error: job probe failed" << endl;
					exit(EXIT_FAILURE);
				}

				if (stask.j == NA) cout << "PROBE: (" << stask.i << ")--> " << state << endl;
				else cout << "PROBE: (" << stask.i << "," << stask.j << ")--> " << state << endl;

				if (state == "DONE") { 
					finished = true; failed = false;
					if (DELETE_JOBS) {
						if (!deleteJob(stask.id)) {
							cout << "Error: delete job failed" << endl;
							exit(EXIT_FAILURE);
						}
						if (stask.j == NA) cout << "DELETE: (" << stask.i << ")" << endl;
						else cout << "DELETE: (" << stask.i << "," << stask.j << ")" << endl;
					}
				}
				else if (state == "FAILED") {
					finished = true; failed = true;
					if (DELETE_JOBS) {
						if (!deleteJob(stask.id)) {
							cout << "Error: delete job failed" << endl;
							exit(EXIT_FAILURE);
						}
						if (stask.j == NA) cout << "DELETE FAILED JOB: (" << stask.i << ")" << endl;
						else cout << "DELETE FAILED JOB: (" << stask.i << "," << stask.j << ")" << endl;
					}
				}
				else finished = false;
				
				if (finished) {
					if (!failed) {
						stask.j == NA ? on_sort_end(stask.i) : on_merge_end(stask.i, stask.j);
						block_ver[stask.i]++; if (stask.j != NA) block_ver[stask.j]++;
					}
					else {
						std::pair<int, int> p(stask.i, stask.j);
						task_queue.push_back(p);
					}
					submitted_tasks_num--;
					it = submitted_task_list.erase(it);
				}
				else it++;
			}

			// запускаем задачи
			
			while (!task_queue.empty() && submitted_tasks_num < MAX_SUBMITTED_TASKS) {
				pair<int, int> t = task_queue.back();
				task_queue.pop_back();
				submitted_task stask;
				stask.i = t.first;
				stask.j = t.second;

				// запуск одной задачи
				if (stask.j == NA) {
					if (!submitPrepare(stask.i, block_ver[stask.i], stask.id)) {
						cout << "Error: submit prepare task failed" << endl;
						exit(EXIT_FAILURE);
					}
					cout << "SUBMIT: (" << stask.i << ")" << endl;
				}
				else {
					if (!submitPlay(stask.i, stask.j, 
							block_ver[stask.i], block_ver[stask.j], stask.id)) {
						cout << "Error: submit play task failed" << endl;
						exit(EXIT_FAILURE);
					}
					cout << "SUBMIT: (" << stask.i << "," << stask.j << ")" << endl;
				}

				submitted_task_list.push_back(stask);
				submitted_tasks_num++;
			}

			done = submitted_task_list.empty();
			if(!done)std::this_thread::sleep_for(delay);
		}
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = (end - start);

		cout << "----------------------" << endl;
		cout << "Block size: " << BLOCK_SIZE << " integers" << endl;
		cout << "Number of blocks: " << NUM_BLOCKS << endl;
		cout << "Max. number of submited tasks: " << MAX_SUBMITTED_TASKS << endl;
		cout << "Execution time is: " << diff.count() << "s" << endl;

		return cleanupEverestSession()?EXIT_SUCCESS:EXIT_FAILURE;
	}
	cout << "Error: bad parameters, <number of blocks> <login> <password> are required" << endl;
	
	return EXIT_FAILURE;
}
