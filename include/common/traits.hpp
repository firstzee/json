#pragma once

#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <optional>

namespace json::traits {

template <typename T>
struct is_vector: std::false_type {};

template <typename T>
struct is_vector<std::vector<T>>: std::true_type {};

template <typename T>
constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
struct is_map: std::false_type {};

template <typename T>
struct is_map<std::map<std::string, T>>: std::true_type {};

template <typename T>
constexpr bool is_map_v = is_map<T>::value;

template <typename T>
struct is_string: std::false_type {};

template <class T, class Traits, class Alloc>
struct is_string<std::basic_string<T, Traits, Alloc>> : std::true_type {};

template <typename T>
constexpr bool is_string_v = is_string<T>::value;

template <typename T>
struct is_optional: std::false_type {};

template <class T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
constexpr bool is_optional_v = is_optional<T>::value;

template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

}
