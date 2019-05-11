#include <iostream>
#include <fstream>
#include <algorithm>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>

constexpr auto data = "data.bin";
constexpr size_t memLim = 6 * 1024 * 1024;
constexpr int threadNum = 2;
constexpr size_t threadMemLim = memLim / threadNum;
constexpr size_t threadUintLim = threadMemLim / sizeof(uint64_t);

std::atomic<bool> finished = false;
std::atomic<int> finishedStep = 0;
std::atomic<int> finishedSort = 0;
std::condition_variable cond;
std::mutex sortFinishM;
std::mutex outQueM;
std::mutex streamReadM;
std::mutex iterFinishM;

void prep(uint64_t * const locBuf, std::ifstream & dataStream,const int id, std::queue<std::string>&outputFiles) {
	const int iter = 0;
	int file = 0;
	while (!dataStream.eof()) {
		std::unique_lock<std::mutex> lock(streamReadM);
		dataStream.read(reinterpret_cast<char *>(locBuf), threadUintLim * sizeof(uint64_t));
		std::streamsize curSize = dataStream.gcount() / sizeof(uint64_t);
		lock.unlock();
		if (curSize != 0) {
			std::sort(locBuf, locBuf + curSize);
			std::string name = std::to_string(iter) + '_' + std::to_string(id) + "_" + std::to_string(file) + ".bin";
			std::ofstream out(name, std::ios::binary);
			out.write(reinterpret_cast<char*>(locBuf), curSize * sizeof(uint64_t));
			outputFiles.push(name);
			++file;
		}
	}
}

void merge(const std::string & str1, const std::string & str2, uint64_t * const buf, const int id,const int iter,const int file,
				std::queue<std::string>&outputFiles) {
	std::ifstream f1(str1, std::ios::binary);
	std::ifstream f2(str2, std::ios::binary);
	const std::string name = std::to_string(iter) + '_' + std::to_string(id) + '_' + std::to_string(file) + ".bin";
	std::ofstream out(name, std::ios::binary);

	const size_t lim = threadUintLim / 4;
	uint64_t* const bufL = buf;
	uint64_t* const bufR = buf + lim;
	uint64_t* const bufM = bufR + lim;

	f1.read(reinterpret_cast<char*>(bufL), lim * sizeof(uint64_t));
	size_t redL = f1.gcount() / sizeof(uint64_t);
	f2.read(reinterpret_cast<char*>(bufR), lim * sizeof(uint64_t));
	size_t redR = f2.gcount() / sizeof(uint64_t);

	const size_t limM = 2 * lim;
	size_t l = 0;
	size_t m = 0;
	size_t r = 0;

	while (!f1.eof() || !f2.eof() || l < redL || r < redR) {
		if (l == redL && !f1.eof()) {
			f1.read(reinterpret_cast<char*>(bufL), lim * sizeof(uint64_t));
			redL = f1.gcount() / sizeof(uint64_t);
			l = 0;
		}
		if (r == redR && !f2.eof()) {
			f2.read(reinterpret_cast<char*>(bufR), lim * sizeof(uint64_t));
			redR = f2.gcount() / sizeof(uint64_t);
			r = 0;
		}
		if (m == limM) {
			out.write(reinterpret_cast<char*>(bufM), m * sizeof(uint64_t));
			m = 0;
		}

		if (l < redL && r < redR) {
			if (bufR[r] < bufL[l]) {
				bufM[m] = bufR[r];
				++m;
				++r;
			}
			else {
				bufM[m] = bufL[l];
				++l;
				++m;
			}
		}
		else {
			if (l == redL && r < redR) {
				bufM[m] = bufR[r];
				++m;
				++r;
			}
			else {
				if (r == redR && l < redL) {
					bufM[m] = bufL[l];
					++l;
					++m;
				}
			}
		}
	}

	out.write(reinterpret_cast<char*>(bufM), m * sizeof(uint64_t));
	std::unique_lock<std::mutex> qlock(outQueM);
	outputFiles.push(name);
}

void MTSort(std::shared_ptr<uint64_t> buf, std::ifstream & dataStream, const int id, std::queue<std::string>&outputFiles) {
	uint64_t * const locBuf = buf.get() + id*threadUintLim;
	int iter = 0;
	int file = 0;
	
	prep(locBuf, dataStream, id, outputFiles);

	++iter;
	std::unique_lock<std::mutex> lock(iterFinishM);
	++finishedStep;
	cond.notify_all();
	while (finishedStep < threadNum) {
		cond.wait(lock);
	}
	lock.unlock();
		
	while (outputFiles.size() >= 2) {
		std::unique_lock<std::mutex> qLock(outQueM);
		if (outputFiles.size() >= 2) {
			std::string tmp1 = outputFiles.front();
			outputFiles.pop();
			std::string tmp2 = outputFiles.front();
			outputFiles.pop();
			qLock.unlock();
			merge(tmp1, tmp2, locBuf, id, iter, file, outputFiles);
			++file;
		}
	}
	std::unique_lock<std::mutex> fLock(sortFinishM);
	++finishedSort;
	if (finishedSort == threadNum) {
		if (outputFiles.empty()) { std::cerr << "error, no output files" << std::endl; }
		else std::cout << "finished, result in file: " << outputFiles.front() << std::endl;
	}
}

int main() {
	std::shared_ptr<uint64_t>  buf  (new uint64_t[memLim / sizeof(uint64_t)]);
	std::ifstream dataStream(data, std::ios::binary);
	std::queue<std::string> outputFiles;
	std::vector<std::thread> threads;
	for (int i = 0; i < threadNum; ++i) {
		threads.emplace_back(MTSort, std::ref(buf), std::ref(dataStream), i, std::ref(outputFiles));
	}
	for (int i = 0; i < threadNum; ++i) {
		threads[i].join();
	}
	system("pause");
	return 0;
}