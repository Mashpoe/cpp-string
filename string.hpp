//
//  string.hpp
//  cpp-string
//
//  Created by Mashpoe on 7/28/19.
//

#ifndef string_hpp
#define string_hpp

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace mp {

class string {
private:
	char* str;
	size_t len;
	
	// private constructor to avoid copying
	string(size_t length, void* value) : str((char*)value), len(length) {}
	
public:
	
	// initialize str with 1 null byte ""
	string() : str((char*)calloc(1, sizeof(char))), len(0) {}
	string(decltype(nullptr)) : str((char*)calloc(1, sizeof(char))), len(0) {}
	
	string(const char* value) {
		if (value == nullptr) {
			len = 0;
			str = (char*)calloc(1, sizeof(char));
			return;
		}
		
		len = strlen(value);
		str = (char*)malloc(len + 1);
		strcpy(str, value);
	}
	string(const char* value, size_t length) {
		len = length;
		str = (char*)malloc(len + 1);
		strcpy(str, value);
	}
	
	// implement your own constructors for other types
	template <typename T>
	string(T value) {
		std::string str_value = std::to_string(value);
		len = str_value.length();
		str = (char*)malloc(len + 1);
		str = strcpy(str, str_value.c_str());
	}
	// an example of a custom constructor for a specific type
	string(bool value) {
		if (value) {
			len = 4;
			str = (char*)malloc(len + 1);
			str = strcpy(str, "true");
		} else  {
			len = 5;
			str = (char*)malloc(len + 1);
			str = strcpy(str, "false");
		}
	}
	
	~string() {
		if (str != nullptr) {
			free(str);
		}
	}
	
	inline size_t size() {
		return len;
	}
	
	inline size_t length() {
		return len;
	}
	
	// copy constructor
	string(string const& other) {
		str = (char*)malloc(other.len + 1);
		strcpy(str, other.str);
		len = other.len;
	}
	
	// copy assignment
	string& operator = (string const& other) {
		str = (char*)malloc(other.len + 1);
		strcpy(str, other.str);
		len = other.len;
		return *this;
	}

	// move constructor
	string(string&& other) noexcept {
		// take ownership
		str = other.str;
		len = other.len;

		// reset other
		other.str = nullptr;
	}

	// move assignment
	string& operator = (string&& other) noexcept {
		if (this != &other) {
			// take ownership
			str = other.str;
			len = other.len;

			// reset other
			other.str = nullptr;
		}
		return *this;
	}
	
	operator const char* () {
		return str;
	}
	
	const char* c_str() const {
		return str;
	}

	char& operator [] (int pos) {
		return str[pos];
	}
	
	bool operator ! () {
		/* empty strings have 1 null byte allocated because
				a) you can use realloc on it
				b) it prevents internal null checks
				c) it can be cast to an empty string "" */
		return str[0] == '\0';
	}
	
	explicit operator bool() {
		return str[0] != '\0';
	}
	
	bool operator == (const char* value) {
		return strcmp(str, value) == 0;
	}
	
	bool operator != (const char* value) {
		return strcmp(str, value) != 0;
	}
	
	bool operator == (string value) {
		return len == value.len && strcmp(str, value.str) == 0;
	}
	
	void insert(size_t pos, const string& value) {
		// allocate sum of both string lengths + 1 for null terminator
		size_t new_len = len + value.len;
		// don't use realloc becase we only want to copy the first section of the string
		char* new_str = (char*)malloc(new_len + 1);
		
		// copy the contents of both strings
		memcpy(new_str, str, pos);
		memcpy(&new_str[pos], value.str, value.len);
		memcpy(&new_str[pos + value.len], &str[pos], len - pos);
		
		// add null terminator
		new_str[new_len] = '\0';
		
		// free old string
		free(str);
		str = new_str;
		len = new_len;
	}
	
	template <typename T>
	void insert(size_t pos, T& value) {
		insert(pos, string(value));
	}
	
	void append(string const& value) {
		
		// allocate sum of both string lengths + 1 for null terminator
		size_t new_len = len + value.len;
		str = (char*)realloc(str, new_len + 1);
		
		// copy the other string
		memcpy(&str[len], value.str, value.len);
		
		// add null terminator
		str[new_len] = '\0';
		
		// update length
		len = new_len;
	}
	
	void append(const char* value) {
		
		size_t value_len = strlen(value);
		
		// allocate sum of both string lengths + 1 for null terminator
		size_t new_len = len + value_len;
		str = (char*)realloc(str, new_len + 1);
		
		// copy the other string
		memcpy(&str[len], value, value_len);
		
		// add null terminator
		str[new_len] = '\0';
		
		// update length
		len = new_len;
	}
	
	inline string& operator += (string const& value) {
		append(value);
		return *this;
	}
	
	inline string& operator += (const char* value) {
		append(value);
		return *this;
	}
	
	template <typename T>
	string& operator += (T& value) {
		append(string(value));
		return *this;
	}
	
	void erase(size_t pos, size_t count) {
		size_t new_len = len - count;
		
		// move the right side of the erased portion
		memmove(&str[pos], &str[pos + count], len - pos - count);
		
		// add null terminator
		str[new_len] = '\0';
		len = new_len;
	}
	
	void remove(size_t pos) {
		erase(pos, 1);
	}
	
	// concatenation overloads
	string friend operator + (string const& left, string const& right);
	// rvalue optimization (we can only optimize the left side)
	string friend operator + (string&& left, string const& right);
	
	// generic types
	template <typename T>
	string friend operator + (string const& left, T const& right);
	template <typename T>
	string friend operator + (T const& left, const string& right);
	// rvalue optimization (we can only optimize the left side)
	template <typename T>
	string friend operator + (string&& left, T const& right);
	
	// regular c string
	string friend operator + (string const& left, const char* right);
	string friend operator + (const char* left, string const& right);
	// rvalue optimization (we can only optimize the left side)
	string friend operator + (string&& left, const char* right);
};

string operator + (string const& left, string const& right) {
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right.len;
	// call private constructor with void* to avoid copying
	string s(new_len, malloc(new_len + 1));
	
	// copy the contents of both strings
	memcpy(s.str, left.str, left.len);
	memcpy(&s.str[left.len], right.str, right.len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

string operator + (string&& left, string const& right) {
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right.len ;
	// call private constructor with void* to avoid copying
	string s(new_len, realloc(left.str, new_len + 1));
	// take ownership of left.str
	left.str = nullptr;
	
	// copy the contents of both strings
	memcpy(&s.str[left.len], right.str, right.len);
	
	// add null terminator
	s.str[new_len] = '\0';	
	
	return s;
}

template <typename T>
inline string operator + (string const& left, T const& right) {
	return left + string(right);
}

template <typename T>
inline string operator + (T const& left, string const& right) {
	return string(left) + right;
}

template <typename T>
inline string operator + (string&& left, T const& right) {
	return std::forward<string>(left) + string(right);
}

string operator + (string const& left, const char* right) {
	size_t right_len = strlen(right);
	
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right_len;
	// call private constructor with void* to avoid copying
	string s(new_len, malloc(new_len + 1));
	
	// copy the contents of both strings
	memcpy(s.str, left.str, left.len);
	memcpy(&s.str[left.len], right, right_len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

string operator + (const char* left, string const& right) {
	size_t left_len = strlen(left);
	
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left_len + right.len;
	// call private constructor with void* to avoid copying
	string s(new_len, malloc(new_len + 1));
	
	// copy the contents of both strings
	memcpy(s.str, left, left_len);
	memcpy(&s.str[left_len], right.str, right.len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

string operator + (string&& left, const char* right) {
	size_t right_len = strlen(right);
	
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right_len;
	// call private constructor with void* to avoid copying
	string s(new_len, realloc(left.str, new_len + 1));
	// take ownership of left.str
	left.str = nullptr;
	
	// copy the contents of the other string
	memcpy(&s.str[left.len], right, right_len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

// only works if using namespace mp, "string"_mp will be equal to string("string", 6)
inline string operator""_mp(const char* value, unsigned long size) {
	return string(value, size);
}

};

#endif /* string_hpp */
