#include <condition_variable>
#include <thread>
#include <mutex>
#include <stdio.h>

std::mutex out;
std::condition_variable cond;
bool isPing = true;
constexpr int NUMBER = 500000;

void Ping() {
	std::unique_lock<std::mutex> lock(out);
	for (int i = 0; i < NUMBER; ++i) {
		while (!isPing) {
			cond.wait(lock);
		}
		printf("ping\n");
		isPing = false;
		cond.notify_one();
	}
}

void Pong() {
	std::unique_lock<std::mutex> lock(out);
	for (int i = 0; i < NUMBER; ++i) {
		while (isPing) {
			cond.wait(lock);
		}
		printf("pong\n");
		isPing = true;
		cond.notify_one();
	}
}

int main() {
	std::thread tPing(Ping);
	std::thread tPong(Pong);
	tPing.join();
	tPong.join();

	system("pause");
	return 0;
}
