#pragma once 

// --- Concepts ---

template<typename From, typename To> 
concept ConvertibleTo = __is_convertible(From, To);

template <typename T>
concept LessThanComparable = requires (T a, T b) {
    { a < b } -> ConvertibleTo<bool>;
};

// --- Math ---

template<LessThanComparable T> 
constexpr T min(T a, T b) { return a < b ? a : b; }
template<LessThanComparable T>
constexpr T max(T a, T b) { return a < b ? b : a; }
template<LessThanComparable T>
constexpr T clamp(T a, T mi, T ma) { return max(mi, min(a, ma)); }

template <typename T> constexpr T&& move(T& a) { return static_cast<T&&>(a); }
template <typename T> constexpr void swap(T& a, T& b) {
    T t = move(a); 
    a = move(b); 
    b = move(t);
}