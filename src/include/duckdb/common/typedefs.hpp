//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/common/typedefs.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>

namespace duckdb {

//! a saner size_t for loop indices etc
struct idx_t {
	uint64_t value = 0;
	idx_t() = default;

	constexpr idx_t(size_t value_p) : value(value_p) {
	}
	constexpr idx_t(uint64_t value_p) : value(value_p) {
	}
	constexpr idx_t(int64_t value_p) : value(value_p) {
	}
	constexpr idx_t(uint32_t value_p) : value(value_p) {
	}
	constexpr idx_t(int32_t value_p) : value(value_p) {
	}
	constexpr idx_t(long value_p) : value(value_p) {
	}

	// comparision
	template <class T>
	constexpr bool operator==(const T &rhs) const {
		return value == uint64_t(rhs);
	}

	template <class T>
	constexpr bool operator!=(const T &rhs) const {
		return value != uint64_t(rhs);
	}

	template <class T>
	constexpr bool operator<=(const T &rhs) const {
		return value <= uint64_t(rhs);
	}

	template <class T>
	constexpr bool operator>=(const T &rhs) const {
		return value >= uint64_t(rhs);
	}

	template <class T>
	constexpr bool operator>(const T &rhs) const {
		return value > uint64_t(rhs);
	}

	template <class T>
	constexpr bool operator<(const T &rhs) const {
		return value < uint64_t(rhs);
	}

	// arithmetic
	template <class T>
	constexpr idx_t operator+(const T &rhs) const {
		// TODO overflow check
		return value + uint64_t(rhs);
	}

	template <class T>
	constexpr idx_t operator-(const T &rhs) const {
		// TODO underflow check
		return value - uint64_t(rhs);
	}

	template <class T>
	constexpr idx_t operator*(const T &rhs) const {
		// TODO overflow check
		return value * uint64_t(rhs);
	}

	template <class T>
	constexpr idx_t operator/(const T &rhs) const {
		return value / uint64_t(rhs);
	}

	template <class T>
	constexpr idx_t operator%(const T &rhs) const {
		return value % uint64_t(rhs);
	}

	// bitwise

	template <class T>
	constexpr idx_t operator>>(const T &rhs) const {
		return value >> uint64_t(rhs);
	}

	template <class T>
	constexpr idx_t operator>>(T &rhs) const {
		return value >> uint64_t(rhs);
	}
	template <class T>
	constexpr idx_t operator<<(const T &rhs) const {
		return value << uint64_t(rhs);
	}
	template <class T>
	constexpr idx_t operator&(const T &rhs) const {
		return value & uint64_t(rhs);
	}
	template <class T>
	constexpr idx_t operator|(const T &rhs) const {
		return value | uint64_t(rhs);
	}
	template <class T>
	constexpr idx_t operator^(const T &rhs) const {
		return value ^ uint64_t(rhs);
	}

	idx_t constexpr operator~() const {
		return ~value;
	}

	// in-place
	template <class T>
	idx_t &operator+=(const T &rhs) {
		value += uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator-=(const T &rhs) {
		value -= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator*=(const T &rhs) {
		value *= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator/=(const T &rhs) {
		value /= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator%=(const T &rhs) {
		value %= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator>>=(const T &rhs) {
		value >>= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator<<=(const T &rhs) {
		value <<= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator&=(const T &rhs) {
		value &= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator|=(const T &rhs) {
		value |= uint64_t(rhs);
		return *this;
	}
	template <class T>
	idx_t &operator^=(const T &rhs) {
		value ^= uint64_t(rhs);
		return *this;
	}

	// operators
	idx_t operator++() {
		value++;
	}
	idx_t operator++(int) {
		++value;
	}

	constexpr operator uint64_t() const {
		return value;
	}
	constexpr operator bool() const {
		return value != 0;
	}
	constexpr operator size_t() const {
		return value;
	}
	constexpr operator long() const {
		return value;
	}

	// explicit constexpr operator uint8_t() const {return value;}
	// explicit constexpr operator int8_t() const {return value;}
	// explicit constexpr operator uint16_t() const {return value;}
	// explicit constexpr operator int16_t() const {return value;}
	// explicit constexpr operator int64_t() const {return value;}
	// explicit constexpr operator int32_t() const {return value;}
	// explicit constexpr operator uint32_t() const {return value;}
	// explicit constexpr operator long() const {return value;}
	//
};

std::string to_string(const idx_t &x);

//! The type used for row identifiers
typedef int64_t row_t;

//! The type used for hashes
typedef uint64_t hash_t;

//! data pointers
typedef uint8_t data_t;
typedef data_t *data_ptr_t;
typedef const data_t *const_data_ptr_t;

//! Type used for the selection vector
typedef uint32_t sel_t;
//! Type used for transaction timestamps
typedef idx_t transaction_t;

//! Type used to identify connections
typedef idx_t connection_t;

//! Type used for column identifiers
typedef idx_t column_t;
//! Type used for storage (column) identifiers
typedef idx_t storage_t;

template <class SRC>
data_ptr_t data_ptr_cast(SRC *src) { // NOLINT: naming
	return reinterpret_cast<data_ptr_t>(src);
}

template <class SRC>
const_data_ptr_t const_data_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const_data_ptr_t>(src);
}

template <class SRC>
char *char_ptr_cast(SRC *src) { // NOLINT: naming
	return reinterpret_cast<char *>(src);
}

template <class SRC>
const char *const_char_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const char *>(src);
}

template <class SRC>
const unsigned char *const_uchar_ptr_cast(const SRC *src) { // NOLINT: naming
	return reinterpret_cast<const unsigned char *>(src);
}

template <class SRC>
uintptr_t CastPointerToValue(SRC *src) {
	return reinterpret_cast<uintptr_t>(src);
}

template <class SRC>
uint64_t cast_pointer_to_uint64(SRC *src) {
	return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src));
}

template <class SRC = data_t>
SRC *cast_uint64_to_pointer(uint64_t value) {
	return reinterpret_cast<SRC *>(static_cast<uintptr_t>(value));
}

} // namespace duckdb

namespace std {
template <>
struct hash<duckdb::idx_t> {
	size_t operator()(const duckdb::idx_t &val) const {
		using std::hash;
		return hash<uint64_t> {}(val.value);
	}
};
} // namespace std
