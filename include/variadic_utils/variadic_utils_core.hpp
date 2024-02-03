//
// Created by chris on 02.02.24.
//

#ifndef TMP_LIB_VARIADIC_UTILS_CORE_HPP
#define TMP_LIB_VARIADIC_UTILS_CORE_HPP

#include <concepts>
#include <type_traits>
#include <limits>


namespace tmp
{
	/// Unordered types with duplicates
	template<class...>
	struct type_bag {};


	// Ordered types with duplicates
	template<class...>
	struct type_list {};


	// Unordered types without duplicates
	template<class...>
	struct type_set {};
}

#endif//TMP_LIB_VARIADIC_UTILS_CORE_HPP
