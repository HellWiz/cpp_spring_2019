#pragma once
#include<algorithm>

class BigInt {
public:
	BigInt() : value(new uint8_t[1]),
		size(1),
		capacity(1),
		isNegative(false){
		value[0] = 0;
	}

	BigInt(int64_t num) {
		if (num < 0) { isNegative = true; }
		else { isNegative = false; }
		size_t len = 1;
		int64_t tmp = num;
		while ((tmp /= 10) != 0) {
			++len;
		}
		size = len;
		capacity = len;
		value = new uint8_t[capacity];
		if (isNegative) { num = -num; }
		for (size_t i = 0; i < len; ++i) {
			value[i] = num % 10;
			num /= 10;
		}
	}

	BigInt(const BigInt& copied) : value(new uint8_t[copied.capacity]),
		size(copied.size),
		capacity(copied.capacity),
		isNegative(copied.isNegative){
		std::copy(copied.value, copied.value + capacity, value);
	}

	BigInt(BigInt&& moved) : value(moved.value),
		size(moved.size),
		capacity(moved.capacity),
		isNegative(moved.isNegative){
		moved.value = nullptr;
		moved.size = 0;
		moved.capacity = 0;
	}

	~BigInt() {
		delete[] value;
	}

	BigInt& operator=(const BigInt& copied) {
		if (this == &copied) {
			return *this;
		}
		uint8_t* tmp = new uint8_t[copied.capacity];
		delete[] value;
		value = tmp;
		size = copied.size;
		capacity = copied.capacity;
		isNegative = copied.isNegative;
		std::copy(copied.value, copied.value + capacity, value);
		return *this;
	}

	BigInt& operator=(BigInt&& moved) {
		if (this == &moved) {
			return *this;
		}
		delete[] value;
		value = moved.value;
		capacity = moved.capacity;
		size = moved.size;
		isNegative = moved.isNegative;
		moved.value = nullptr;
		moved.capacity = 0;
		moved.isNegative = 0;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const BigInt& num);

	BigInt operator-() const {
		BigInt tmp(*this);
		tmp.isNegative = !isNegative;
		return tmp;
	}

	BigInt operator+(const BigInt& other) const {
		if ((isNegative && other.isNegative) || ((!isNegative) && (!other.isNegative))) {
			BigInt tmp;
			tmp.capacity = std::max(capacity, other.capacity) + 1;
			tmp.isNegative = isNegative;
			tmp.size = tmp.capacity;
			tmp.value = new uint8_t[tmp.capacity];
			size_t lim = std::min(capacity, other.capacity);
			uint8_t tonext = 0;
			for (size_t i = 0; i < lim; ++i) {
				tmp.value[i] = value[i] + other.value[i] + tonext;
				tonext = tmp.value[i] / 10;
				tmp.value[i] %= 10;
			}
			const BigInt* longest;
			if (capacity >= other.capacity) {
				longest = this;
			}
			else {
				longest = &other;;
			}
			for (size_t i = lim; i < longest->capacity; ++i) {
				tmp.value[i] = longest->value[i] + tonext;
				tonext = tmp.value[i] / 10;
				tmp.value[i] %= 10;
			}
			tmp.value[longest->capacity] = tonext;
			return tmp;
		}
		else { return *this - -other; }
	}

	BigInt operator-(const BigInt& other) const {
		if ((isNegative && other.isNegative) || ((!isNegative) && (!other.isNegative))) {
			if (!isNegative) {
				if (*this >= other) {
					BigInt tmp(*this);
					uint8_t fromnext = 0;
					for (size_t i = 0; i < other.capacity; ++i) {
						if (tmp.value[i] >= (other.value[i] + fromnext)) {
							tmp.value[i] -= (other.value[i] + fromnext);
							fromnext = 0;
						}
						else {
							tmp.value[i] += (10 - other.value[i] - fromnext);
							fromnext = 1;
						}
					}
					for (size_t i = other.capacity; i < capacity; ++i) {
						if (tmp.value[i] >= fromnext) {
							tmp.value[i] -= (fromnext);
							fromnext = 0;
						}
						else {
							tmp.value[i] += (10 - fromnext);
							fromnext = 1;
						}
					}
					return tmp;
				}
				else { return -(other - *this); }
			}
			else { return -(-*this - (-other)); }
		}
		else { return *this + -other; }
	}

	bool operator==(const BigInt& other)const {
		if (this == &other)
			return true;
		if (isNegative == other.isNegative) {
			if (capacity == other.capacity) {
				for (size_t j = 0; j < capacity; ++j) {
					if (value[j] != other.value[j]) return false;
				}
				return true;
			}
			else {
				const BigInt* longest,*shortest;
				if (capacity >= other.capacity) {
					longest = this;
					shortest = &other;
				}
				else {
					longest = &other;;
					shortest = this;
				}
				for (size_t j = 0; j < shortest->capacity; ++j) {
					if (value[j] != other.value[j]) return false;
				}
				for (size_t j = shortest->capacity; j < longest->capacity; ++j) {
					if (longest->value[j] != 0)return false;
				}
				return true;
			}
		}
		else {
			bool flag = false;
			for (size_t i = 0; i < capacity; ++i) {
				if (value[i] != 0)flag = true;
			}
			if (flag)return false;
			for (size_t i = 0; i < other.capacity; ++i) {
				if (other.value[i] != 0)flag = true;
			}
			if (flag)return false;
			return true;
		} 
	}

	bool operator!=(const BigInt& other)const {
		return !(*this == other);
	}

	bool operator<(const BigInt& other)const {
		if (this == &other) return false;
		if (isNegative && !other.isNegative) return true;
		if (!isNegative && other.isNegative) return false;
		if (isNegative && other.isNegative) return (-(*this)) > (-other);
		size_t firstNotNull1 = capacity-1;
		size_t firstNotNull2 = other.capacity-1;
		for (; firstNotNull1 > 0; --firstNotNull1) {
			if (value[firstNotNull1] != 0) break;
		}
		for (; firstNotNull2 > 0; --firstNotNull2) {
			if (other.value[firstNotNull2] != 0) break;
		}
		if (firstNotNull1 < firstNotNull2) return true;
		if (firstNotNull1 > firstNotNull2) return false;
		for (size_t i = firstNotNull1; i > 0; --i) {
			if (value[i] > other.value[i]) return false;
			if (value[i] < other.value[i]) return true;
		}
		if (value[0] > other.value[0]) return false;
		if (value[0] < other.value[0]) return true;
		return false;
	}

	bool operator>(const BigInt& other)const {
		if (this == &other) return false;
		if (isNegative && !other.isNegative) return false;
		if (!isNegative && other.isNegative) return true;
		if (isNegative && other.isNegative) return (-(*this)) < (-other);
		size_t firstNotNull1 = capacity - 1;
		size_t firstNotNull2 = other.capacity - 1;
		for (; firstNotNull1 > 0; --firstNotNull1) {
			if (value[firstNotNull1] != 0) break;
		}
		for (; firstNotNull2 > 0; --firstNotNull2) {
			if (other.value[firstNotNull2] != 0) break;
		}
		if (firstNotNull1 > firstNotNull2) return true;
		if (firstNotNull1 < firstNotNull2) return false;
		for (size_t i = firstNotNull1; i > 0; --i) {
			if (value[i] < other.value[i]) return false;
			if (value[i] > other.value[i]) return true;
		}
		if (value[0] < other.value[0]) return false;
		if (value[0] > other.value[0]) return true;
		return false;
	}

	bool operator<=(const BigInt& other)const {
		return!(*this > other);
	}

	bool operator>=(const BigInt& other)const {
		return!(*this < other);
	}

private:
	uint8_t* value;
	size_t size;
	size_t capacity;
	bool isNegative;
};

std::ostream& operator<<(std::ostream& out, const BigInt& num) {
	if (num == (-BigInt(0))) { 
		out << 0; 
		return out; 
	}
	if (num.capacity > 0) {
		bool started = false;
		if (num.isNegative) { out << "-"; }
		for (size_t i = num.capacity - 1; i != 0; --i) {
			if (started) {
				out << num.value[i]+0;
			}
			else { 
				if (num.value[i] != 0) {
				started = true;
				out << num.value[i]+0;
				} 
			}
		}
		out << num.value[0]+0;
	}
	return out;
}
