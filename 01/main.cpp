#include "numbers.dat"
#include <iostream>

extern const int Data[];
extern const int Size;

int searchIndexInData(int key, bool isLeft) {
	int left = 0;
	int right = Size - 1;
	int mid;
	while (true) {
		mid = (left + right) / 2;
		if (key < Data[mid]) {
			right = mid - 1;
		}
		else if (key > Data[mid]) {
			left = mid + 1;
		}
		else {
			break;
		}
		if (left > right) {
			return -1;
		}
	}
	if (isLeft) {
		int res = mid - 1;
		while (res >= 0 && Data[res] == Data[mid]) {
			--res;
		}
		return(res + 1);
	}
	else {
		int res = mid + 1;
		while (res < Size && Data[res] == Data[mid]) {
			++res;
		}
		return(res - 1);
	}
}

bool isSimple(int n) {
	if (n != 1) {
		if (n != 2) {
			if (n % 2 != 0) {
				bool flag = true;
				for (int i = 3; i*i <= n && flag; i += 2) {
					if (n%i == 0) {
						flag = false;
					}
				}
				return flag;

			}
			else {
				return false;
			}
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

int countSimple(int left, int right) {
	int res = 0;
	for (int i = left; i <= right; ++i) {
		if (isSimple(Data[i])) ++res;
	}
	return res;
}

int main(int argc, char* argv[]) {
	if (argc < 3) return -1;
	if (argc % 2 == 0) return -1;
	int n = (argc - 1) / 2;
	for (int i = 0; i < n; ++i) {
		int left = std::atoi(argv[2 * i + 1]);
		int right = std::atoi(argv[2 * (i + 1)]);
		left = searchIndexInData(left, true);
		right = searchIndexInData(right, false);
		if (left != -1 && right != -1) {
			std::cout << countSimple(left, right) << std::endl;
		}
		else {
			std::cout << "0" << std::endl;
		}
	}
	return 0;
}
