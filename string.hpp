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
	
	// compile time constants
	enum {
		in_situ_capacity = 16,
		in_situ_size = in_situ_capacity - 1
	};
	
	char* str;
	size_t len;
	union {
		size_t capacity;
		// last byte has flag bit, 1 = large string
		// 0 = small string and null terminator
		char data[in_situ_capacity];
	} in_situ;
	
	inline void init(size_t length) {
		len = length;
		if (length > in_situ_size) {
			size_t capacity = length + 1;
			
			in_situ.capacity = capacity;
			in_situ.data[in_situ_size] = '\1';
			
			str = (char*)malloc(capacity);
		} else {
			in_situ.data[in_situ_size] = '\0';
			str = in_situ.data;
		}
	}
	
	// change the string's capacity if the new capacity is greater
	// does not change len
	void resize(size_t new_capacity) {
		
		// check capacity using flag bit
		if (in_situ.data[in_situ_size] == '\1') {
			
			if (new_capacity > in_situ.capacity) {
				in_situ.capacity = new_capacity;
				str = (char*)realloc(str, new_capacity);
			}
			
		} else {
			// capacity <= in_situ_size
			if (new_capacity > in_situ_size) {
				
				// move data to the heap
				char* new_str = (char*)malloc(new_capacity);
				
				// copy str including null teminator
				memcpy(new_str, str, len + 1);
				
				str = new_str;
				
				// change flag bit to large string
				in_situ.data[in_situ_size] = '\1';
				
				in_situ.capacity = new_capacity;
				
			}
			
		}
	}
	
	// private constructor to avoid copying
	string(size_t length, void* value) : str((char*)value), len(length) {}
	
	// creates a string with enough capacity for length
	string(size_t length, size_t capacity) {
		len = length;
		if (capacity > in_situ_capacity) {
			str = (char*)malloc(capacity);
			in_situ.capacity = capacity;
			// set flag bit
			in_situ.data[in_situ_size] = '\1';
		} else {
			str = in_situ.data;
			in_situ.data[in_situ_size] = '\0';
		}
	}
	
public:
	
	// initialize str with 1 null byte ""
	string() {
		str = in_situ.data;
		len = 0;
		in_situ.data[0] = '\0';
		in_situ.data[in_situ_size] = '\0';
	}
	string(decltype(nullptr)) {
		str = in_situ.data;
		len = 0;
		in_situ.data[0] = '\0';
		in_situ.data[in_situ_size] = '\0';
	}
	
	string(const char* value) {
		if (value == nullptr) {
			len = 0;
			in_situ.data[0] = '\0';
			in_situ.data[15] = '\0';
			return;
		}
		
		init(strlen(value));
		memcpy(str, value, len);
		return;
	}
	string(const char* value, size_t length) {
		init(length);
		memcpy(str, value, len);
	}
	
	// implement your own constructors for other types
	// must be explicit to avoid accidental conversions
	template <typename T>
	explicit string(T value) {
		std::string str_value = std::to_string(value);
		init(str_value.length());
		memcpy(str, str_value.c_str(), len);
	}
	// an example of a custom constructor for a specific type
	// we don't need to allocate because it will fit on the stack
	explicit string(bool value) {
		if (value) {
			init(4);
			memcpy(str, "true", len);
		} else  {
			init(5);
			memcpy(str, "false", len);
		}
	}
	
	
	~string() {
		// this can be manually set to 1 for move semantics
		if (in_situ.data[in_situ_size] == '\1') {
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
		
		// check flag bit
		if (other.in_situ.data[in_situ_size] == '\1') {
			len = other.len;
			in_situ.capacity = len + 1;
			in_situ.data[in_situ_size] = '\1';
			
			str = (char*)malloc(in_situ.capacity);
			memcpy(str, other.str, in_situ.capacity);
		} else {
			// just copy the stack data
			str = in_situ.data;
			len = other.len;
			memcpy(&in_situ.data, &other.in_situ.data, len + 1);
			in_situ.data[in_situ_size] = '\0';
		}
		
	}
	
	// copy assignment
	string& operator = (string const& other) {
		
		// check flag bit
		if (other.in_situ.data[in_situ_size] == '\1') {
			len = other.len;
			in_situ.capacity = len + 1;
			in_situ.data[in_situ_size] = '\1';
			
			str = (char*)malloc(in_situ.capacity);
			memcpy(str, other.str, in_situ.capacity);
		} else {
			// just copy the stack data
			str = in_situ.data;
			len = other.len;
			memcpy(&in_situ.data, &other.in_situ.data, len + 1);
			in_situ.data[in_situ_size] = '\0';
		}
		
		return *this;
		
	}
	
	// move constructor
	string(string&& other) noexcept {
		
		// check flag bit
		if (other.in_situ.data[in_situ_size] == '\1') {
			len = other.len;
			in_situ.capacity = other.in_situ.capacity;
			in_situ.data[in_situ_size] = '\1';
			
			// take ownership
			str = other.str;
			// set other flag bit to 0 so destructor won't free
			other.in_situ.data[in_situ_size] = '\0';
		} else {
			// just copy the stack data
			str = in_situ.data;
			len = other.len;
			memcpy(&in_situ.data, &other.in_situ.data, len + 1);
			in_situ.data[in_situ_size] = '\0';
		}
		
	}
	
	// move assignment
	string& operator = (string&& other) noexcept {
		// unfortunate but necessary repetition of code
		if (this != &other) {
			// check flag bit
			if (other.in_situ.data[in_situ_size] == '\1') {
				len = other.len;
				in_situ.capacity = other.in_situ.capacity;
				in_situ.data[in_situ_size] = '\1';
				
				// take ownership
				str = other.str;
				// set other flag bit to 0 so destructor won't free
				other.in_situ.data[in_situ_size] = '\0';
			} else {
				// just copy the stack data
				str = in_situ.data;
				len = other.len;
				memcpy(&in_situ.data, &other.in_situ.data, len + 1);
				in_situ.data[in_situ_size] = '\0';
			}
		}
		return *this;
	}
	
	const char* c_str() const {
		return str;
	}
	
	bool empty() const {
		return len == 0;
	}
	
	operator const char* () {
		return str;
	}
	
	char& operator [] (int pos) {
		return str[pos];
	}
	
	bool operator ! () {
		return len == 0;
	}
	
	explicit operator bool() {
		return len != 0;
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
		resize(new_len + 1);
		
		// move the right side of pos
		memmove(&str[pos + value.len], &str[pos], len - pos);
		memcpy(&str[pos], value.str, value.len);
		
		// add null terminator
		str[new_len] = '\0';
		
		len = new_len;
	}
	
	void insert(size_t pos, const char* value) {
		
		// allocate sum of both string lengths + 1 for null terminator
		size_t value_len = strlen(value);
		size_t new_len = len + value_len;
		resize(new_len + 1);
		
		// move the right side of pos
		memmove(&str[pos + value_len], &str[pos], len - pos);
		memcpy(&str[pos], value, value_len);
		
		// add null terminator
		str[new_len] = '\0';
		
		len = new_len;
	}
	
	template <typename T>
	void insert(size_t pos, T const& value) {
		insert(pos, string(value));
	}
	
	void append(string const& value) {
		
		// allocate sum of both string lengths + 1 for null terminator
		size_t new_len = len + value.len;
		resize(new_len + 1);
		
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
		resize(new_len + 1);
		
		// copy the other string
		memcpy(&str[len], value, value_len);
		
		// add null terminator
		str[new_len] = '\0';
		
		// update length
		len = new_len;
	}
	
	template <typename T>
	void append(T const& value) {
		append(string(value));
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
	string& operator += (T const& value) {
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
	// call private constructor to allocate an empty string with the given length
	string s(new_len, new_len + 1);
	
	// copy the contents of both strings
	memcpy(s.str, left.str, left.len);
	memcpy(&s.str[left.len], right.str, right.len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

string operator + (string&& left, string const& right) {
	/* left is an rvalue, so we can just modify then move it
	 to increase performance */
	
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right.len;
	
	// now resize left (won't change left.len)
	left.resize(new_len + 1);
	
	// copy the contents of the other string
	memcpy(&left.str[left.len], right.str, right.len);
	
	// update left.len
	left.len = new_len;
	
	// add null terminator
	left.str[new_len] = '\0';
	
	// transfer from left before it gets destroyed
	return std::move(left);
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
	// call private constructor to avoid copying
	string s(new_len, new_len + 1);
	
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
	// call private constructor to avoid copying
	string s(new_len, new_len + 1);
	
	// copy the contents of both strings
	memcpy(s.str, left, left_len);
	memcpy(&s.str[left_len], right.str, right.len);
	
	// add null terminator
	s.str[new_len] = '\0';
	
	return s;
}

string operator + (string&& left, const char* right) {
	/* left is an rvalue, so we can just modify then move it
	 to increase performance */
	
	size_t right_len = strlen(right);
	
	// allocate sum of both string lengths + 1 for null terminator
	size_t new_len = left.len + right_len;
	
	// now resize left (won't change left.len)
	left.resize(new_len + 1);
	
	// copy the contents of the other string
	memcpy(&left.str[left.len], right, right_len);
	
	// update left.len
	left.len = new_len;
	
	// add null terminator
	left.str[new_len] = '\0';
	
	// transfer from left before it gets destroyed
	return std::move(left);
}

// only works if using namespace mp, "string"_mp will be equal to string("string", 6)
inline string operator""_mp(const char* value, unsigned long size) {
	return string(value, size);
}

};

#endif /* string_hpp */