#pragma once

class _MatrixRow {
public:

	void ini(const size_t len) {
		cols = len;
		elements = new int[cols];
	}

	void ini(const _MatrixRow& source) {
		cols = source.cols;
		elements = new int[cols];
		for (size_t i=0; i < cols; ++i) {
			elements[i] = source.elements[i];
		}
	}

	_MatrixRow(const _MatrixRow& source) : cols(source.cols) {
		elements = new int[cols];
		for (size_t i=0; i < cols; ++i) {
			elements[i] = source.elements[i];
		}
	}

	_MatrixRow() : cols(0), elements(nullptr){}

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

	_MatrixRow operator *=(const int a) {
		for (size_t i = 0; i < cols; ++i) {
			elements[i] *= a;
		}
		_MatrixRow tmp(*this);
		return tmp;
	}

private:

	size_t cols;
	int* elements;
	
};

class Matrix {
public:

	Matrix(const size_t row, const size_t col) : rows(row), cols(col){
		lines = new _MatrixRow[rows];
		for (int i = 0; i < rows; ++i) {
			lines[i].ini(col);
		}
	}

	Matrix(const Matrix& source) : rows(source.rows), cols(source.cols){
		lines = new _MatrixRow[rows];
		for (int i = 0; i < rows; ++i) {
			lines[i].ini(source.lines[i]);
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

	Matrix operator *=(const int a) {
		for (size_t i = 0; i < rows; ++i) {
			lines[i] *= a;
		}
		Matrix tmp(*this);
		return tmp;
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
