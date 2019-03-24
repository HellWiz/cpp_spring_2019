#pragma once

class _MatrixRow {
public:
	_MatrixRow() : cols(0), elements(nullptr){}

	_MatrixRow(const size_t len) :cols(len) {
		elements = new int[cols];
	}

	~_MatrixRow(){
		delete[] elements;

	}

	const int& operator[](size_t i) const {
		if (i >= cols) { throw std::out_of_range(""); }
		return elements[i];
	}

	int& operator[](size_t i) {
		if (i >= cols) { throw std::out_of_range(""); }
		return elements[i];
	}

	bool operator == (const _MatrixRow& other) const {
		if (this == &other) {
			return true;
		}
		if (cols != other.cols) {
			return false;
		}
		for (size_t i = 0; i < cols; ++i) {
			if (elements[i] != other.elements[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator != (const _MatrixRow& other) const {
		return !(*this == other);
	}

	_MatrixRow& operator *=(const int a) {
		for (size_t i = 0; i < cols; ++i) {
			elements[i] *= a;
		}
		return *this;
	}

private:

	size_t cols;
	int* elements;
	
};

class Matrix {
public:

	Matrix(const size_t row, const size_t col) : rows(row), cols(col){
		lines = new _MatrixRow[rows];
		for (size_t i = 0; i < rows; ++i) {
			new (lines+i) _MatrixRow (col);
		}
	}

	~Matrix() {
		delete[] lines;
	}

	const _MatrixRow& operator[](size_t i) const{
		if (i >= rows) { throw std::out_of_range(""); }
		return lines[i];
	}

	_MatrixRow& operator[](size_t i) {
		if (i >= rows) { throw std::out_of_range(""); }
		return lines[i];
	}

	bool operator == (const Matrix& other) const {
		if (this == &other) {
			return true;
		}
		if (cols != other.cols || rows != other.rows) {
			return false;
		}
		for (size_t i = 0; i < rows; ++i) {
			if (lines[i] != other.lines[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator != (const Matrix& other) const {
		return !(*this == other);
	}

	Matrix& operator *=(const int a) {
		for (size_t i = 0; i < rows; ++i) {
			lines[i] *= a;
		}
		return *this;
	}

	size_t getRows() const{
		return rows;
	}

	size_t getColumns() const{
		return cols;
	}

private:

	size_t rows;
	size_t cols;
	_MatrixRow* lines;
};