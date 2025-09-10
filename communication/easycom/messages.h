#pragma once

#include <iostream>
#include <tuple>
#include <cstdint>
#include <cstring>
#include <type_traits>

// --- Define the list of supported types ---
struct Foo { int x; };
struct Bar { double y; };
struct Baz { char c; };

using TypeList = std::tuple<Foo, Bar, Baz>;


// contains<T, std::tuple<...>>
template <typename T, typename Tuple>
struct contains;

template <typename T, typename... Ts>
struct contains<T, std::tuple<Ts...>>
    : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

// index_of<T, std::tuple<...>>  (0-based). If T not present -> hard error.
template <typename T, typename Tuple>
struct index_of;

template <typename T, typename... Ts>
struct index_of<T, std::tuple<T, Ts...>> : std::integral_constant<int, 0> {};

template <typename T, typename U, typename... Ts>
struct index_of<T, std::tuple<U, Ts...>>
    : std::integral_constant<int, 1 + index_of<T, std::tuple<Ts...>>::value> {};

// type_at<I, Tuple>
template <int I, typename Tuple>
using type_at = typename std::tuple_element<I, Tuple>::type;

// Convenience alias to get the ID of T within TypeList at compile time
template <typename T>
constexpr int type_id_v = []{
    static_assert(contains<T, TypeList>::value,
                  "Type is not present in TypeList");
    return index_of<T, TypeList>::value;
}();