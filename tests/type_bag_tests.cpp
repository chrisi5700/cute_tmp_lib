//
// Created by chris on 02.02.24.
//


#include <variadic_utils/type_bag.hpp>


template<class>
struct to_int {
	using type = int;
};
struct S {
};

using empty_bag = tmp::type_bag<>;

template<class T>
struct size {
	static constexpr std::size_t value = sizeof(T);
};

void bag_union()
{
	using left = tmp::type_bag<int, double, int>;
	using right = tmp::type_bag<int, float>;
	static_assert(std::same_as<tmp::type_bag<int, double, int, int, float>,
							   tmp::bag_union<left, right>::type>);

	static_assert(std::same_as<empty_bag, tmp::bag_union<empty_bag, empty_bag>::type>);
}


void prepend()
{
	{
		using bag = tmp::type_bag<int, double, int>;
		static_assert(std::same_as<tmp::type_bag<float, int, double, int>,
								   tmp::prepend<float, bag>::type>);
	}

	{
		using bag = tmp::type_bag<>;
		static_assert(std::same_as<tmp::type_bag<float>,
								   tmp::prepend<float, bag>::type>);
	}
}


void append()
{
	{
		using bag = tmp::type_bag<int, double, int>;
		static_assert(std::same_as<tmp::type_bag<int, double, int, float>,
								   tmp::append<float, bag>::type>);
	}

	{
		using bag = tmp::type_bag<>;
		static_assert(std::same_as<tmp::type_bag<float>,
								   tmp::append<float, bag>::type>);
	}
}


void contains()
{
	using bag = tmp::type_bag<int, double, int>;
	using bag2 = tmp::type_bag<int>;

	static_assert(tmp::contains<int, bag>::value);
	static_assert(not tmp::contains<float, bag>::value);
	static_assert(not tmp::contains<float, tmp::type_bag<>>::value);
	static_assert(tmp::contains<int, bag2>::value);
	static_assert(not tmp::contains<float, bag2>::value);

}

void count()
{
	using bag = tmp::type_bag<int, double, int>;
	using bag2 = tmp::type_bag<int>;

	static_assert(tmp::count<int, bag>::value == 2);
	static_assert(tmp::count<double, bag>::value == 1);
	static_assert(tmp::count<float, bag>::value == 0);
	static_assert(tmp::count<float, tmp::type_bag<>>::value == 0);
	static_assert(tmp::count<float, bag2>::value == 0);
	static_assert(tmp::count<int, bag2>::value == 1);


}



void transform()
{


	using bag = tmp::type_bag<int, double, float>;

	static_assert(std::same_as<tmp::type_bag<int, int, int>,
							   typename tmp::transform<to_int, bag>::type>);


	static_assert(std::same_as<tmp::type_bag<>,
							   typename tmp::transform<to_int, tmp::type_bag<>>::type>);
}




void filter()
{
	using bag = tmp::type_bag<float, int, short, double, S>;
	using bag2 = tmp::type_bag<float>;
	using bag3 = tmp::type_bag<int>;


	static_assert(std::same_as<tmp::type_bag<float, double>,
							   tmp::filter<std::is_floating_point, bag>::type>);

	static_assert(std::same_as<empty_bag,
							   tmp::filter<std::is_floating_point, empty_bag>::type>);

	static_assert(std::same_as<tmp::type_bag<float>,
							   tmp::filter<std::is_floating_point, bag2>::type>);

	static_assert(std::same_as<tmp::type_bag<>,
							   tmp::filter<std::is_floating_point, bag3>::type>);
}


void type_at()
{
	using bag = tmp::type_bag<float, int, short, double, S>;
	using bag2 = tmp::type_bag<float>;

	static_assert(std::same_as<int, tmp::type_at<1, bag>::type>);
	static_assert(std::same_as<float, tmp::type_at<0, bag>::type>);
	static_assert(std::same_as<S, tmp::type_at<4, bag>::type>);

	static_assert(std::same_as<float, tmp::type_at<0, bag2>::type>);
}


void index_of()
{
	using bag = tmp::type_bag<float, int, short, double, S, float>;
	using bag2 = tmp::type_bag<int>;
	static_assert(tmp::index_of<int, bag>::value == 1);
	static_assert(tmp::index_of<float, bag>::value == 0 or tmp::index_of<float, bag>::value == 5);
	static_assert(tmp::index_of<short, bag>::value == 2);

	static_assert(0 == tmp::index_of<int, bag2>::value);
}

void bag_eq()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<float, short, double, S>;
	using bag3 = tmp::type_bag<float, short, double, S, float>;
	using bag4 = tmp::type_bag<float, short, float, double, S>;

	static_assert(tmp::bag_eq<bag1, bag2>::value);
	static_assert(not tmp::bag_eq<bag1, bag3>::value);
	static_assert(not tmp::bag_eq<bag2, bag3>::value);
	static_assert(tmp::bag_eq<bag4, bag3>::value);
	static_assert(tmp::bag_eq<bag3, bag3>::value);
	static_assert(tmp::bag_eq<bag3, bag4>::value);
	static_assert(tmp::bag_eq<empty_bag, empty_bag>::value);
	static_assert(not tmp::bag_eq<empty_bag, bag3>::value);
	static_assert(not tmp::bag_eq<bag3, empty_bag>::value);
}

void remove_n()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<float, short, double, S, float>;
	using bag3 = tmp::type_bag<int, int, int>;

	static_assert(tmp::bag_eq<tmp::type_bag<double, S, float>,
							  typename tmp::remove_n<short, 1, bag1>::type>::value);

	static_assert(tmp::bag_eq<tmp::type_bag<short, S, double>,
							  typename tmp::remove_n<float, 2, bag2>::type>::value);

	static_assert(tmp::bag_eq<bag1,
							  typename tmp::remove_n<float, 0, bag1>::type>::value);

	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::remove_n<float, 0, empty_bag>::type>::value);

	static_assert(std::same_as<bag3, typename tmp::remove_n<int, 0, bag3>::type>);
}


void intersection()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<float, short, double, S>;
	using bag3 = tmp::type_bag<float, short, double, S, float>;
	using bag4 = tmp::type_bag<int, float, short, float, double, S>;

	using inter_b1_b2 = tmp::type_bag<short, float, S, double>;
	using inter_b2_b1 = inter_b1_b2;
	using inter_b2_b3 = tmp::type_bag<float, short, double, S>;
	using inter_b3_b4 = tmp::type_bag<float, short, double, S, float>;

	static_assert(tmp::bag_eq<inter_b1_b2,
							  typename tmp::intersection<bag1, bag2>::type>::value);
	static_assert(tmp::bag_eq<inter_b2_b1,
							  typename tmp::intersection<bag2, bag1>::type>::value);
	static_assert(tmp::bag_eq<inter_b2_b3,
							  typename tmp::intersection<bag2, bag3>::type>::value);
	static_assert(tmp::bag_eq<inter_b3_b4,
							  typename tmp::intersection<bag3, bag4>::type>::value);

	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::intersection<empty_bag, bag4>::type>::value);
	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::intersection<bag4, empty_bag>::type>::value);
	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::intersection<empty_bag, empty_bag>::type>::value);
}

void remove_all()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<float, short, double, S>;
	using bag3 = tmp::type_bag<float, short, double, S, float>;
	using bag4 = tmp::type_bag<int, float, short, float, double, S>;
	using bag5 = tmp::type_bag<int, int, int, int>;


	static_assert(tmp::bag_eq<tmp::type_bag<double, S, float>,
							  typename tmp::remove_all<short, bag1>::type>::value);

	static_assert(tmp::bag_eq<tmp::type_bag<double, S, float>,
							  typename tmp::remove_all<short, bag2>::type>::value);

	static_assert(tmp::bag_eq<tmp::type_bag<float, double, S, float>,
							  typename tmp::remove_all<short, bag3>::type>::value);

	static_assert(tmp::bag_eq<tmp::type_bag<int, short, double, S>,
							  typename tmp::remove_all<float, bag4>::type>::value);

	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::remove_all<int, bag5>::type>::value);

	static_assert(tmp::bag_eq<empty_bag,
							  typename tmp::remove_all<int, empty_bag>::type>::value);

	static_assert(tmp::bag_eq<bag1, typename tmp::remove_all<int, bag1>::type>::value);
}


void subset_eq()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<float, short, double, S>;
	using bag3 = tmp::type_bag<float, short, double, S, float>;
	using bag4 = tmp::type_bag<int, float, short, float, double, S>;
	using bag5 = tmp::type_bag<int, int, int, int>;
	using bag6 = tmp::type_bag<int, int>;

	static_assert(tmp::subset_eq<bag1, bag2>::value);
	static_assert(tmp::subset_eq<bag2, bag2>::value);
	static_assert(tmp::subset_eq<bag2, bag1>::value);
	static_assert(tmp::subset_eq<bag3, bag4>::value);
	static_assert(not tmp::subset_eq<bag4, bag3>::value);
	static_assert(not tmp::subset_eq<bag5, bag6>::value);
	static_assert(tmp::subset_eq<bag6, bag5>::value);
}


void merge()
{
	using bag1 = tmp::type_bag<char, char, short, int>;
	using bag2 = tmp::type_bag<char, short, long long>;
	using merged_b1_b2 = tmp::type_bag<char, char, char, short, short, int, long long>;

	using bag3 = tmp::type_bag<char[1], char[1], char[2]>;
	using bag4 = tmp::type_bag<char[3], char[5], char[8]>;
	using merged_b3_b4 = tmp::type_bag<char[1], char[1], char[2], char[3], char[5], char[8]>;


	using bag5 = tmp::type_bag<char[1], char[3], char[5]>;
	using bag6 = tmp::type_bag<char[2], char[4], char[6]>;
	using merged_b5_b6 = tmp::type_bag<char[1], char[2], char[3], char[4], char[5], char[6]>;

	static_assert(std::same_as<merged_b1_b2, tmp::merge<size, bag1, bag2>::type>);
	static_assert(std::same_as<merged_b1_b2, tmp::merge<size, bag2, bag1>::type>);

	static_assert(std::same_as<merged_b3_b4, tmp::merge<size, bag3, bag4>::type>);
	static_assert(std::same_as<merged_b3_b4, tmp::merge<size, bag4, bag3>::type>);

	static_assert(std::same_as<merged_b5_b6, tmp::merge<size, bag5, bag6>::type>);
	static_assert(std::same_as<merged_b5_b6, tmp::merge<size, bag6, bag5>::type>);

	static_assert(std::same_as<bag1, tmp::merge<size, bag1, empty_bag>::type>);
}

void first_n()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using one = tmp::type_bag<short>;
	using two = tmp::type_bag<short, double>;
	using three = tmp::type_bag<short, double, S>;
	using four = tmp::type_bag<short, double, S, float>;


	static_assert(tmp::bag_eq<one, tmp::first_n<1, bag1>::type>::value);
	static_assert(tmp::bag_eq<two, tmp::first_n<2, bag1>::type>::value);
	static_assert(tmp::bag_eq<three, tmp::first_n<3, bag1>::type>::value);
	static_assert(tmp::bag_eq<four, tmp::first_n<4, bag1>::type>::value);
	static_assert(tmp::bag_eq<empty_bag, tmp::first_n<0, empty_bag>::type>::value);
	static_assert(tmp::bag_eq<empty_bag, tmp::first_n<0, bag1>::type>::value);
}

void drop_n()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using one = tmp::type_bag<double, S, float>;
	using two = tmp::type_bag<S, float>;
	using three = tmp::type_bag<float>;
	using four = tmp::type_bag<>;


	static_assert(tmp::bag_eq<one, tmp::drop_n<1, bag1>::type>::value);
	static_assert(tmp::bag_eq<two, tmp::drop_n<2, bag1>::type>::value);
	static_assert(tmp::bag_eq<three, tmp::drop_n<3, bag1>::type>::value);
	static_assert(tmp::bag_eq<four, tmp::drop_n<4, bag1>::type>::value);
	static_assert(tmp::bag_eq<empty_bag, tmp::drop_n<0, empty_bag>::type>::value);
	static_assert(tmp::bag_eq<bag1, tmp::drop_n<0, bag1>::type>::value);
}


void last_n()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using one = tmp::type_bag<float>;
	using two = tmp::type_bag<S, float>;
	using three = tmp::type_bag<double, S, float>;
	using four = tmp::type_bag<short, double, S, float>;

	static_assert(tmp::bag_eq<one, tmp::last_n<1, bag1>::type>::value);
	static_assert(tmp::bag_eq<two, tmp::last_n<2, bag1>::type>::value);
	static_assert(tmp::bag_eq<three, tmp::last_n<3, bag1>::type>::value);
	static_assert(tmp::bag_eq<four, tmp::last_n<4, bag1>::type>::value);
	static_assert(tmp::bag_eq<empty_bag, tmp::last_n<0, empty_bag>::type>::value);
	static_assert(tmp::bag_eq<empty_bag , tmp::last_n<0, bag1>::type>::value);


}

void split()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using b1_l = tmp::type_bag<short, double>;
	using b1_r = tmp::type_bag<S, float>;
	using b1_split = tmp::split<bag1>;

	using bag2 = tmp::type_bag<short, double, S, float, int>;
	using b2_l = tmp::type_bag<short, double>;
	using b2_r = tmp::type_bag<S, float, int>;
	using b2_split = tmp::split<bag2>;


	static_assert(std::same_as<b1_l, b1_split::left> and std::same_as<b1_r, b1_split::right>);

	static_assert(std::same_as<b2_l, b2_split::left> and std::same_as<b2_r, b2_split::right>);

	{
		using bag = tmp::type_bag<short>;
		using split_result = tmp::split<bag>;
		static_assert(std::same_as<tmp::type_bag<short>, split_result::right>);
		static_assert(std::same_as<tmp::type_bag<>, split_result::left>);
	}
}


void merge_sort()
{
	using bag1 = tmp::type_bag<char[3], char[2], char[1]>;
	using bag1_s = tmp::type_bag<char[1], char[2], char[3]>;

	using bag2 = tmp::type_bag<char[2], char[4], char[3], char[1]>;
	using bag2_s = tmp::type_bag<char[1], char[2], char[3], char[4]>;

	static_assert(std::same_as<tmp::merge_sort<size, bag1>::type, bag1_s>);
	static_assert(std::same_as<tmp::merge_sort<size, bag2>::type, bag2_s>);

	static_assert(std::same_as<tmp::merge_sort<size, empty_bag>::type, empty_bag>);

}


void bag_difference()
{
	using bag1 = tmp::type_bag<short, double, S, float>;
	using bag2 = tmp::type_bag<short, float>;
	using b1_m_b2 = tmp::type_bag<double, S>;

	using bag3 = tmp::type_bag<int, float, float, int, char, float, double>;
	using bag4 = tmp::type_bag<float, float, double, float>;
	using b3_m_b4 = tmp::type_bag<int, int, char>;

	using bag5 = tmp::type_bag<int, int, int, int>;
	using bag6 = tmp::type_bag<int, int>;

	using bag7 = tmp::type_bag<int, int, int, int>;
	using bag8 = tmp::type_bag<float>;

	static_assert(tmp::bag_eq<typename tmp::bag_difference<bag5, bag6>::type, bag6>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<bag1, bag2>::type, b1_m_b2>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<bag3, bag4>::type, b3_m_b4>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<bag7, bag8>::type, bag7>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<bag7, empty_bag>::type, bag7>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<empty_bag , empty_bag>::type, empty_bag>::value);
	static_assert(tmp::bag_eq<typename tmp::bag_difference<empty_bag , bag7>::type, empty_bag>::value);





}

int main()
{
}