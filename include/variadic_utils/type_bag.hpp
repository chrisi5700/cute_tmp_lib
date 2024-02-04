//
// Created by chris on 01.02.24.
//

#ifndef TMP_LIB_VARIADIC_UTILS_HPP
#define TMP_LIB_VARIADIC_UTILS_HPP


#include "variadic_utils_core.hpp"

namespace tmp
{



	// Atomic small building blocks to build larger algorithms on type_bags


	/// Combines the types of two type_bags
	/// \tparam ...
	template<class bag1, class bag2>
	struct bag_union;

	template<class... Args1, class... Args2>
	struct bag_union<type_bag<Args1...>, type_bag<Args2...>>
	{
		using type = type_bag<Args1..., Args2...>;
	};



	/// Adds T to the front of a type_bag
	/// \tparam ...
	template <class T, class type_bag>
	struct prepend;

	template<class T, class... Args>
	struct prepend<T, type_bag<Args...>>
	{
		using type = type_bag<T, Args...>;
	};



	/// Adds T to the back of a type_bag
	/// \tparam ...
	template <class T, class type_bag>
	struct append;

	template<class T, class... Args>
	struct append<T, type_bag<Args...>>
	{
		using type = type_bag<Args..., T>;
	};


	template <class T, class type_bag>
	struct contains;


	template <class T, class... Args>
	struct contains<T, type_bag<Args...>> : std::bool_constant<(std::same_as<T, Args> || ...)>
	{};



	template <class T, class type_bag>
	struct count;


	template <class T, class... Args>
	struct count<T, type_bag<Args...>> : std::integral_constant<std::size_t, (static_cast<std::size_t>(std::same_as<T, Args>) + ...)>
	{};

	template <class T>
	struct count<T, type_bag<>> : std::integral_constant<std::size_t, 0>
	{};






	namespace detail_type_bag {


		template<template<class> class F, class Bag>
		struct transform_impl;

		template<template<class> class F, class... Args>
		struct transform_impl<F, type_bag<Args...>>
		{
			using type = type_bag<typename F<Args>::type...>;
		};

		template<template<class> class, class type_bag>
		struct filter_impl;

		template<template<class> class Filter, class Arg, class... Args>
		struct filter_impl<Filter, type_bag<Arg, Args...>> : std::conditional<Filter<Arg>::value,
																			 typename prepend<Arg, typename filter_impl<Filter, type_bag<Args...>>::type>::type,
																			 typename filter_impl<Filter, type_bag<Args...>>::type
																			 >

		{};


		template<template<class> class Filter, class Arg>
		struct filter_impl<Filter, type_bag<Arg>> : std::conditional<Filter<Arg>::value,
																	type_bag<Arg>,
																	type_bag<>>
		{};

		template<template<class> class Filter>
		struct filter_impl<Filter, type_bag<>>
		{
			using type = type_bag<>;
		};



		template<std::size_t, class type_bag>
		struct type_at_impl;

		// Base case
		template<class Head, class... Args>
		struct type_at_impl<0, type_bag<Head, Args...>>
		{
			using type = Head;
		};

		template<std::size_t N, class Head, class... Tail>
		struct type_at_impl<N, type_bag<Head, Tail...>>
		{
			static_assert(sizeof...(Tail) != 0); 	// Index out of range
			using type = typename type_at_impl<N-1, type_bag<Tail...>>::type;
		};




		template <typename T, typename type_bag>
		struct index_of_impl;

		// Specialization for an empty type_bag
		template <typename T>
		struct index_of_impl<T, type_bag<>> : std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()>
		{};


		// Partial specialization for a non-empty type_bag
		template <typename T, typename... Types>
		struct index_of_impl<T, type_bag<T, Types...>> : std::integral_constant<std::size_t, 0>
		{};

		// Partial specialization for when the head of the list is not the type we're looking for.
		template <typename T, typename Head, typename... Tail>
		struct index_of_impl<T, type_bag<Head, Tail...>> : std::integral_constant<std::size_t, 1 + index_of_impl<T, type_bag<Tail...>>::value>
		{};



		template<class T, std::size_t N, class Bag>
		struct remove_n_impl;


		template<class T, std::size_t N, class... Tail> requires (N != 0)
		struct remove_n_impl<T, N, type_bag<T, Tail...>>
		{
			using type = typename remove_n_impl<T, N-1, type_bag<Tail...>>::type;
		};


		// needs the requires clause otherwise ambiguous with default case
		template<class T, std::size_t N, class Head, class... Tail> requires (N != 0)
		struct remove_n_impl<T, N, type_bag<Head, Tail...>>
		{
			using type = typename remove_n_impl<T, N, type_bag<Tail..., Head>>::type;
		};


		template<class T, class... Tail>
		struct remove_n_impl<T, 0, type_bag<Tail...>>
		{
			using type =  type_bag<Tail...>;
		};




		template<class type_bag1, class type_bag2>
		struct intersection_impl;

		// if Head is contained in type_bag2 then add it to the intersection and remove it from both type_bags
		template<class Head, class... Tail, class... Args>
		struct intersection_impl<type_bag<Head, Tail...>, type_bag<Args...>>
		{
		private:
			static constexpr bool is_contained = contains<Head, type_bag<Args...>>::value;
			using type_bag2_New = typename remove_n_impl<Head, 1, type_bag<Args...>>::type;
			using TailResult = typename intersection_impl<type_bag<Tail...>, type_bag2_New>::type;
		public:
			using type = std::conditional_t<is_contained,
											typename append<Head, TailResult>::type,
											TailResult>;


		};



		// intersection of empty bag and any other bag is always empty
		template<class Head, class... Args>
		struct intersection_impl<type_bag<Head, Args...>, type_bag<>>
		{
			using type = type_bag<>;
		};

		template<class Head, class... Args>
		struct intersection_impl<type_bag<>, type_bag<Head, Args...>>
		{
			using type = type_bag<>;
		};

		template<>
		struct intersection_impl<type_bag<>, type_bag<>>
		{
			using type = type_bag<>;
		};




		template<class T, class bag>
		struct remove_all_impl;


		template<class T>
		struct remove_all_impl<T, type_bag<>>
		{
			using type = type_bag<>;
		};

		template<class T, class Head, class... Tail>
		struct remove_all_impl<T, type_bag<Head, Tail...>> {
		private:
			// Continue with the tail of the list
			using tail_result = typename remove_all_impl<T, type_bag<Tail...>>::type;

			// If Head matches T, skip it, else include Head in the resulting type_bag
		public:
			using type = typename std::conditional_t<
					std::same_as<T, Head>,
					tail_result, // If Head is T, skip Head
					typename prepend<Head, tail_result>::type // If Head is not T, include Head
					>;
		};



		template<class bag1, class bag2>
		struct subset_eq_impl;

		template<class Head, class... Tail, class Bag2>
		struct subset_eq_impl<type_bag<Head, Tail...>, Bag2>
		{
		private:
			static constexpr std::size_t count2 = count<Head, Bag2>::value;
			static constexpr std::size_t count1 = count<Head, type_bag<Tail...>>::value + 1;
			using new_bag1 = typename remove_all_impl<Head, type_bag<Tail...>>::type;
			using new_bag2 = typename remove_all_impl<Head, Bag2>::type;
		public:
			// there are more Head in Bag2 than in Bag1 and Bag1 without Head is a subset of Bag2 without Head
			static constexpr bool value = count2 >= count1 and subset_eq_impl<new_bag1, new_bag2>::value;
		};

		template<class Bag2>
		struct subset_eq_impl<type_bag<>, Bag2> : std::true_type
		{};


		template<template<class> class, class bag1, class bag2>
		struct merge_impl;

		template<template<class> class F, class Head1, class... Tail1, class Head2, class... Tail2>
		struct merge_impl<F, type_bag<Head1, Tail1...>, type_bag<Head2, Tail2...>>
		{
		private:
			static constexpr bool h1_less_h2 = F<Head1>::value < F<Head2>::value;
			using new_bag1 = std::conditional_t<h1_less_h2,
												type_bag<Tail1...>,
												type_bag<Head1, Tail1...>>;
			using new_bag2 = std::conditional_t<h1_less_h2,
												type_bag<Head2, Tail2...>,
												type_bag<Tail2...>>;
			using to_prepend = std::conditional_t<h1_less_h2,
												  Head1,
												  Head2>;
		public:
			using type = typename prepend<to_prepend, typename merge_impl<F, new_bag1, new_bag2>::type>::type;

		};

		template<template<class> class F, class... Args>
		struct merge_impl<F, type_bag<>, type_bag<Args...>>
		{
			using type = type_bag<Args...>;
		};

		template<template<class> class F, class... Args>
		struct merge_impl<F, type_bag<Args...>, type_bag<>>
		{
			using type = type_bag<Args...>;
		};


		template<template<class> class F>
		struct merge_impl<F, type_bag<>, type_bag<>>
		{
			using type = type_bag<>;
		};


		template<std::size_t N, class Bag>
		struct first_n_impl;

		template<std::size_t N, class Head, class... Args>
		struct first_n_impl<N, type_bag<Head, Args...>>
		{
			using type = typename prepend<Head, typename first_n_impl<N-1, type_bag<Args...>>::type>::type;
		};

		template<class Head, class... Args>
		struct first_n_impl<0, type_bag<Head, Args...>>
		{
			using type = type_bag<>;
		};

		template<>
		struct first_n_impl<0, type_bag<>>
		{
			using type = type_bag<>;
		};


		template<std::size_t N, class Bag>
		struct drop_n_impl;

		template<std::size_t N, class Head, class... Tail>
		struct drop_n_impl<N, type_bag<Head, Tail...>>
		{
			using type = typename drop_n_impl<N-1, type_bag<Tail...>>::type;
		};


		//base case with at least 1
		template<class Head, class... Args>
		struct drop_n_impl<0, type_bag<Head, Args...>>
		{
			using type = type_bag<Head, Args...>;
		};

		// empty base case
		template<>
		struct drop_n_impl<0, type_bag<>>
		{
			using type = type_bag<>;
		};


		template<std::size_t N, class Bag>
		struct last_n_impl;

		template<std::size_t N, class... Args>
		struct last_n_impl<N, type_bag<Args...>>
		{
		private:
			static constexpr std::size_t length = sizeof...(Args);
		public:
			using type = typename drop_n_impl<length - N, type_bag<Args...>>::type;
		};


		template<class Bag1>
		struct split_impl;

		template<class... Args>
		struct split_impl<type_bag<Args...>>
		{
		private:
			static constexpr std::size_t length = sizeof...(Args);
			static constexpr std::size_t left_length = length / 2;
			static constexpr std::size_t right_length = length - left_length;
		public:

			using left = typename first_n_impl<left_length, type_bag<Args...>>::type;
			using right = typename last_n_impl<right_length, type_bag<Args...>>::type;
		};



		template<template<class> class, class bag>
		struct merge_sort_impl;

		template<template<class> class F, class H, class T>
		struct merge_sort_impl<F, type_bag<H, T>> : std::conditional<(F<H>::value) < (F<T>::value),
														type_bag<H, T>,
														type_bag<T, H>>
		{};

		template<template<class> class F, class H>
		struct merge_sort_impl<F, type_bag<H>>
		{
			using type = type_bag<H>;
		};

		template<template<class> class F>
		struct merge_sort_impl<F, type_bag<>>
		{
			using type = type_bag<>;
		};

		template<template<class> class F, class... Args>
		struct merge_sort_impl<F, type_bag<Args...>>
		{
		private:
			using split = split_impl<type_bag<Args...>>;
			using left = typename split::left;
			using right = typename split::right;
			using sorted_left = typename merge_sort_impl<F, left>::type;
			using sorted_right = typename merge_sort_impl<F, right>::type;
		public:
			using type = typename merge_impl<F, sorted_left, sorted_right>::type;
		};


		// order does not matter but count of each class must be eq
		template<class Bag1, class Bag2>
		struct bag_eq_impl;


		template<class Head, class... Tail, class Bag2>
		struct bag_eq_impl<type_bag<Head, Tail...>, Bag2>
		{
		private:
			static constexpr std::size_t count2 = count<Head, Bag2>::value;
			static constexpr std::size_t count1 = count<Head, type_bag<Tail...>>::value + 1;
			using new_bag1 = typename remove_all_impl<Head, type_bag<Tail...>>::type;
			using new_bag2 = typename remove_all_impl<Head, Bag2>::type;
		public:
			// make sure all types come up the same amount in each bag
			static constexpr bool value = count1 == count2 and bag_eq_impl<new_bag1, new_bag2>::value;
		};

		template<>
		struct bag_eq_impl<type_bag<>, type_bag<>> : std::true_type
		{};

		template<class Head, class... Tail>
		struct bag_eq_impl<type_bag<Head, Tail...>, type_bag<>> : std::false_type
		{};

		template<class Head, class... Tail>
		struct bag_eq_impl<type_bag<>, type_bag<Head, Tail...>> : std::false_type
		{};


		template<class Bag1, class Bag2>
		struct bag_difference_impl;


		// Head is contained in bag1 so its removed
		template<class... Args, class Head, class... Tail>
		struct bag_difference_impl<type_bag<Args...>, type_bag<Head, Tail...>>
		{
		private:
			using removed_head = remove_n_impl<Head, 1, type_bag<Args...>>::type;
		public:
			using type = bag_difference_impl<removed_head, type_bag<Tail...>>::type;
		};


		// Head isnt contained in bag1 so its just ignored
		template<class... Args, class Head, class... Tail> requires (not tmp::contains<Head, type_bag<Args...>>::value)
		struct bag_difference_impl<type_bag<Args...>, type_bag<Head, Tail...>>
		{
			using type = bag_difference_impl<type_bag<Args...>, type_bag<Tail...>>::type;
		};

		template<class... Args>
		struct bag_difference_impl<type_bag<Args...>, type_bag<>>
		{
			using type = type_bag<Args...>;
		};



	}

	template<template<class> class F, class Bag>
	struct transform : detail_type_bag::transform_impl<F, Bag> {};

	template<template<class> class F, class Bag>
	struct filter : detail_type_bag::filter_impl<F, Bag> {};

	template<std::size_t N, class Bag>
	struct type_at : detail_type_bag::type_at_impl<N, Bag> {};

	template<class T, class Bag>
	struct index_of : detail_type_bag::index_of_impl<T, Bag> {};

	template<class T, std::size_t N, class Bag>
	struct remove_n : detail_type_bag::remove_n_impl<T, N, Bag> {};

	template<class Bag1, class Bag2>
	struct intersection : detail_type_bag::intersection_impl<Bag1, Bag2>  {};

	template<class T, class Bag>
	struct remove_all : detail_type_bag::remove_all_impl<T, Bag> {};

	template<class Bag1, class Bag2>
	struct subset_eq : detail_type_bag::subset_eq_impl<Bag1, Bag2> {};

	template<template<class> class F, class Bag1, class Bag2>
	struct merge : detail_type_bag::merge_impl<F, Bag1, Bag2> {};

	template<std::size_t N, class Bag>
	struct first_n : detail_type_bag::first_n_impl<N, Bag> {};

	template<std::size_t  N, class Bag>
	struct drop_n : detail_type_bag::drop_n_impl<N, Bag> {};

	template<std::size_t N, class Bag>
	struct last_n : detail_type_bag::last_n_impl<N, Bag> {};

	template<class Bag>
	struct split : detail_type_bag::split_impl<Bag> {};

	template<template<class> class F, class Bag>
	struct merge_sort : detail_type_bag::merge_sort_impl<F, Bag> {};

	template<class Bag1, class Bag2>
	struct bag_eq : detail_type_bag::bag_eq_impl<Bag1, Bag2> {};

	template<class Bag1, class Bag2>
	struct bag_difference : detail_type_bag::bag_difference_impl<Bag1, Bag2> {};


	// TODO Maybe sanitize the inputs of the user facing structs for example
		// TODO remove_n -> first check if type_bag even contains N instances of T

}


#endif//TMP_LIB_VARIADIC_UTILS_HPP
