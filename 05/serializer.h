#pragma once
#include <iostream>
#include <sstream>

enum class Error{
	NoError,
	CorruptedArchive
};

template <class T1, class T2>
struct IsSame{
	static constexpr bool value = false;
};

template <class T>
struct IsSame<T, T>{
	static constexpr bool value = true;
};

class Serializer{
	static constexpr char Separator = ' ';
	std::ostream& out_;
	
public:
	explicit Serializer(std::ostream& out)
		: out_(out){}

	template <class T>
	Error save(T& object){
		return object.serialize(*this);
	}

	template <class... ArgsT>
	Error operator()(ArgsT... args){
		return process(args...);
	}

private:
	template<class T>
	Error process(T value) {
		if (IsSame<bool, decltype(value)>::value) {
			out_ << (value ? "true" : "false") << Separator;
			return Error::NoError;
		}
		else if (IsSame<uint64_t, decltype(value)>::value) {
			out_ << value << Separator;
			return Error::NoError;
		}
		else {
			return Error::CorruptedArchive;
		}
	}

	template <class T, class... ArgsT>
	Error process(T value, ArgsT... args) {
		if (IsSame<bool, decltype(value)>::value) {
			out_ << (value ? "true" : "false") << Separator;
		}
		else if (IsSame<uint64_t, decltype(value)>::value) {
			out_ << value << Separator;
		}
		else {
			return Error::CorruptedArchive;
		}
		return process(args...);
	}
};

class Deserializer {
	static constexpr char Separator = ' ';
	std::istream& in_;
public:
	explicit Deserializer(std::istream& in) :
		in_(in) {}

	template <class T>
	Error load(T& object){
		return object.serialize(*this);
	}

	template <class... ArgsT>
	Error operator()(ArgsT&... args){
		return process(args...);
	}

private:
	static bool isNumber(const std::string &str) {
		for (auto ch : str) {
			if ((ch < '0') || (ch > '9')) {
				return false;
			}
			return true;
		}
		return false;
	}

	template<class T>
	Error process(T &value) {
		std::string str;
		std::stringstream str_stream;
		uint64_t tmp;
		if (IsSame<bool&, decltype(value)>::value) {
			in_ >> str;
			if (str == "true") {
				value = true;
				return Error::NoError;
			}
			else if (str == "false") {
				value = false;
				return Error::NoError;
			}
			else {
				return Error::CorruptedArchive;
			}
		}
		else if (IsSame<uint64_t&, decltype(value)>::value) {
			in_ >> str;
			if (isNumber(str)) {
				str_stream << str << Separator;
				str_stream >> tmp;
				value = tmp;
				return Error::NoError;
			}
			else {
				return Error::CorruptedArchive;
			}
		}
		else {
			return Error::CorruptedArchive;
		}
	}

	template <class T, class... ArgsT>
	Error process(T &value, ArgsT&... args) {
		std::string str;
		std::stringstream str_stream;
		uint64_t tmp;
		if (IsSame<bool&, decltype(value)>::value) {
			in_ >> str;
			if (str == "true") {
				value = true;
				return process(args...);
			}
			else if (str == "false") {
				value = false;
				return process(args...);
			}
			else {
				return Error::CorruptedArchive;
			}
		}
		else if (IsSame<uint64_t&, decltype(value)>::value) {
			in_ >> str;
			if (isNumber(str)) {
				str_stream << str << Separator;
				str_stream >> tmp;
				value = tmp;
				return process(args...);
			}
			else {
				return Error::CorruptedArchive;
			}
		}
		else {
			return Error::CorruptedArchive;
		}
		return process(args...);
	}
};