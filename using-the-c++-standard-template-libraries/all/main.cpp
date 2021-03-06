/**=======================================================================

	main.cpp

		@author: binazersy@gmail.com

=======================================================================**/


#include <iostream>
#include <array>
#include <numeric>
#include <vector>
#include <algorithm>
#include <deque>
#include <list>
#include <queue>
#include <memory>
#include <map>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <iterator>
#include <set>
#include <random>
#include <fstream>
#include <ratio>
#include <chrono>
#include <complex>


template <typename RandomIter>
void bubble_sort(RandomIter start, RandomIter last)
{
	bool out_of_order = false;
	while (true)
	{
		for (auto first = start + 1; first != last; ++first)
		{
			if (*(first - 1) > *first)
			{
				std::swap(*first, *(first - 1));
				out_of_order = true;
			}
		}
		if (!out_of_order)
			break;
		out_of_order = false;
	}
}


void test_array()
{
	std::array<std::string, 5> words{ "one", "two", "three", "four", "five" };
	std::cout << std::get<3>(words).c_str() << std::endl;
	//std::cout << std::get<6>(words).c_str() << std::endl;		// out of bound compile time error

	std::array<double, 10> values;
	std::iota(std::begin(values), std::end(values), 10.0);		// <numeric>
	std::cout << "double values" << std::endl;
	std::copy(values.begin(), values.end(), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;

	bubble_sort(std::begin(words), std::end(words));

	for (auto x : words)
	{
		std::cout << x.c_str();
	}
}


void test_vector()
{
	std::vector<std::string> words{ "first", "second" };
	auto iter = words.emplace(++std::begin(words), 5, 'A');
	words.emplace(++iter, "$$$$$");

	auto iter2 = std::find(std::begin(words), std::end(words), "second");

	// remove
	std::vector<int> data(100, 99);
	auto iter3 = data.erase(std::begin(data) + 1);
	data.pop_back();
	data.clear();

	std::vector<std::string> words2{ "one", "two", "none", "four", "five", "none", "most", "all" };
	auto iter4 = std::remove(std::begin(words2), std::end(words2), "none");
}


void test_deque()
{
	std::deque<std::string> words{ "one", "two", "none", "four", "five", "none", "most", "all" };
	std::cout << words.at(2).c_str() << std::endl;

	std::deque<int> numbers{ 2,3,4 };
	numbers.push_front(11);
	numbers.push_back(12);
	numbers.pop_front();		// 2 3 4 12

	words.assign({ "seven", "eight", "nine" });
	words.assign(8, "eight");
	words = { "seven", "eight", "nine" };
}


class my_greater
{
public:
	bool operator()(const std::string& s1, const std::string& s2)
	{
		if (s1[0] == s2[0])
			return s1.length() > s2.length();
		else
			return s1 > s2;
	}
};


void test_list()
{
	std::list<std::string> names{ "jane", "jim", "jules", "janet" };
	names.push_front("ian");
	names.push_back("kitty");

	names.emplace_front("ian");
	names.emplace_back("kitty");

	std::list<int> numbers{ 2,5,2,3,6,7,8,2,9 };
	numbers.remove(2);
	numbers.remove_if([](int n) {return n % 2 == 0; });

	names.sort(std::greater<std::string>());
	names.sort(my_greater());
}


template<typename T> class Numeric_Iterator;

template<typename T>
class Numeric_Range
{
	static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value,
		"Numeric_Range type argument must be numeric");

	friend class Numeric_Iterator <T>;

private:
	T _start;
	T _step;
	size_t _count;

public:
	explicit Numeric_Range(T first = 0, T incr = 1, size_t n = 2)
		: _start(first), _step(incr), _count(n) {}

	Numeric_Iterator<T> begin() { return Numeric_Iterator<T>(*this); }
	Numeric_Iterator<T> end()
	{
		Numeric_Iterator<T> end_iter(*this);
		end_iter._value = _start + _count * _step;
		return end_iter;
	}
};


template<typename T>
class Numeric_Iterator : public std::iterator<std::forward_iterator_tag, T>
{
	friend class Numeric_Range<T>;

private:
	Numeric_Range<T>& _range;
	T _value;

	void check_range()
	{
		if (_value == static_cast<T>(_range._start + _range._count * _range._step))
		{
			throw std::logic_error("Cannot dereference an end iterator.");
		}
	}

public:
	explicit Numeric_Iterator(Numeric_Range<T>& range)
		: _range(range), _value(range._start)
	{}

	Numeric_Iterator& operator=(const Numeric_Iterator& src)
	{
		_range = src._range;
		_value = src._value;
	}
	
	T& operator*()
	{
		check_range();
		return _value;
	}

	Numeric_Iterator& operator++()
	{
		check_range();
		_value += _range._step;
		return *this;
	}

	Numeric_Iterator& operator++(int)
	{
		check_range();
		auto temp = *this;
		_value += _range._step;
		return temp;
	}

	bool operator<(const Numeric_Iterator& iter) const { return _value < iter._value; }
	bool operator==(const Numeric_Iterator& iter) const { return _value == iter._value; }
	bool operator!=(const Numeric_Iterator& iter) const { return _value != iter._value; }
	bool operator>(const Numeric_Iterator& iter) const { return _value > iter._value; }
	bool operator<=(const Numeric_Iterator& iter) const { *this < iter || *this == iter; }
	bool operator>=(const Numeric_Iterator& iter) const { *this > iter || *this == iter; }
};


void test_numeric_range()
{
	Numeric_Range<double> range{ 1.5, 0.5, 5 };
	auto first = range.begin();
	auto last = range.end();

	std::copy(first, last, std::ostream_iterator<double>(std::cout, " "));
	std::cout << "\nSum = " << std::accumulate(std::begin(range), std::end(range), 0.0) << std::endl;
}


void test_priority_queue()
{
	std::cout << "\npriority_queue\n";

	std::string samples[] { "one", "two", "three", "four" };
	std::priority_queue<std::string> words{ std::begin(samples), std::end(samples) };		// <queue>
	while (!words.empty())
	{
		std::cout << words.top().c_str() << " ";
		words.pop();
	}
	std::cout << std::endl;

	std::priority_queue<std::string, std::deque<std::string>, std::greater<std::string>>
		words2{ std::begin(samples), std::end(samples) };		// <queue>
	while (!words2.empty())
	{
		std::cout << words2.top().c_str() << " ";
		words2.pop();
	}
	std::cout << std::endl;


	std::vector<int> values{ 21, 22, 12, 3, 24, 54, 56 };
	std::priority_queue<int> numbers{ std::less<int>(), values };
	while (!numbers.empty())
	{
		std::cout << numbers.top() << " ";
		numbers.pop();
	}
	std::cout << std::endl;

}


void test_heap()
{
	std::cout << "\nheap\n";

	std::vector<double> numbers{ 2.5, 10.0, 3.5, 6.5, 8.0, 12.0, 1.5, 6.0 };
	std::cout << "original : ";
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;

	std::make_heap(std::begin(numbers), std::end(numbers));
	// <algorithm>
	std::cout << "make_heap: ";
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;

	std::cout << "greater<>: ";
	std::make_heap(std::begin(numbers), std::end(numbers), std::greater<>());		// <algorithm>
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;


	// push
	std::vector<double> numbers2{ 2.5, 10.0, 3.5, 6.5, 8.0, 12.0, 1.5, 6.0 };
	std::make_heap(std::begin(numbers2), std::end(numbers2));
	numbers2.push_back(11);
	std::push_heap(std::begin(numbers2), std::end(numbers2));

	// pop
	std::pop_heap(std::begin(numbers2), std::end(numbers2));
	numbers2.pop_back();

	// sort
	std::cout << "sort\n";
	std::vector<double> numbers3{ 2.5, 10.0, 3.5, 6.5, 8.0, 12.0, 1.5, 6.0 };
	std::cout << "original : ";
	std::copy(std::begin(numbers3), std::end(numbers3), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;

	std::make_heap(std::begin(numbers3), std::end(numbers3));
	std::cout << "make_heap: ";
	std::copy(std::begin(numbers3), std::end(numbers3), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;

	std::cout << "sort_heap: ";
	std::sort_heap(std::begin(numbers3), std::end(numbers3));
	std::copy(std::begin(numbers3), std::end(numbers3), std::ostream_iterator<double>(std::cout, " "));
	std::cout << std::endl;
}


void test_ptr()
{
	std::cout << "ptr\n";

	std::vector<std::shared_ptr<std::string>> words
	{
		std::make_shared<std::string>("one"),
		std::make_shared<std::string>("two"),
		std::make_shared<std::string>("three"),
		std::make_shared<std::string>("four")
	};
	std::make_heap(std::begin(words), std::end(words),
		[](const std::shared_ptr<std::string>& p1, const std::shared_ptr<std::string>& p2) { return *p1 > *p2; });

	for (auto x : words)
	{
		std::cout << x->c_str() << " ";
	}
}


void test_map()
{
	std::cout << "\n\nmap============================\n";

	std::map<std::string, size_t> people
	{
		std::make_pair("Ann", 25),
		std::make_pair("Bill", 46),
		std::make_pair("Jack", 32),
		std::make_pair("Jill", 32)
	};

	auto pr = std::make_pair("Fred", 22);

	auto ret = people.insert(pr);
	std::cout << ret.first->first.c_str() << " " << ret.first->second << " " << std::boolalpha << ret.second << " \n";

	for (const auto& p : people)
	{
		std::cout << std::setw(10) << std::left << p.first.c_str() << " " << p.second << " \n";	// setw <iomanip>
	}

	auto ret2 = people.insert(std::pair<std::string, size_t>{"Baker", 43});

	if (!people.count("Ian"))
		people.insert(std::make_pair("Ian", 38));

	auto cnt = people.erase("Jill");
}


void word_count()
{
	std::cout << "Enter some text and enter * to end:\n";
	std::string text_in{};
	std::getline(std::cin, text_in, '*');

	std::replace_if(std::begin(text_in), std::end(text_in),
		[](const char& ch) { return !isalpha(ch); }, ' ');		// non-alpha to space

	std::istringstream text(text_in);
	std::istream_iterator<std::string> begin(text);
	std::istream_iterator<std::string> end;

	std::map<std::string, size_t> words;
	size_t max_len = 0;

	std::for_each(begin, end, [&max_len, &words](const std::string& word)
	{
		words[word]++;
		max_len = std::max(max_len, word.length());
	});

	size_t per_line = 4, count = 0;
	for (const auto& w : words)
	{
		std::cout << std::left << std::setw(max_len + 1) << w.first <<
			std::setw(3) << std::right << w.second << " ";

		if (++count % per_line == 0) std::cout << std::endl;
	}

	std::cout << std::endl;
}


void test_tuple()
{
	std::cout << "\n\ntuple============================\n";

	auto t1 = std::make_tuple(std::string{ "Peter Piper" }, 42, std::string{ "916 826 8268" });
	std::cout << std::get<0>(t1)
		<< " age = " << std::get<1>(t1)
		<< " tel = " << std::get<2>(t1) << std::endl;

	// can use type
	std::cout << "\n" << std::get<int>(t1) << std::endl;
	// no. of same type must be one
	//std::get<std::string>(t1);	// error

	std::string name{};
	size_t age{};
	std::string phone{};
	std::tie(name, age, phone) = t1;		// from tuple
	std::cout << name
		<< " age = " << age
		<< " tel = " << phone << std::endl;
	
	std::string name2{};
	size_t age2{};
	std::tie(name, age, std::ignore) = t1;		// ignore


	auto the_pair = std::make_pair("these", "those");
	std::tuple<std::string, std::string> t4{ the_pair };
	std::tuple<std::string, std::string> t5{ std::pair<std::string, std::string>{"this", "that"} };

	std::cout << std::boolalpha << (t4 < t5) << std::endl; // true

	t4.swap(t5);
}


void test_multimap()
{
	std::cout << "\n\nmultimap============================\n";

	std::multimap<std::string, size_t> people
	{
		std::make_pair("Ann", 25),
		std::make_pair("Bill", 46),
		std::make_pair("Jack", 32),
		std::make_pair("Jill", 32),
		std::make_pair("Jack", 77)
	};

	std::string name{ "Bill" };
	auto iter = people.find(name);
	if (iter != std::end(people))
		std::cout << name << " is " << iter->second << std::endl;

	auto pr = people.equal_range("Jack");	// find all
	if (pr.first != std::end(people))
	{
		for (auto i = pr.first; i != pr.second; ++i)
		{
			std::cout << i->first << " is " << i->second << std::endl;
		}
	}
}


void test_set()
{
	std::cout << "\n\nmultimap============================\n";

	std::set<int> numbers{ 8, 7, 6, 5, 4, 3, 2, 1 };
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<int> {std::cout, " "});	// 1 2 3 4 5 6 7 8


	std::set<int> num2{ 2, 4, 6, 8, 10, 12, 14 };
	auto iter = num2.erase(++std::begin(num2));		// remove 4, iter -> 6
	auto n = num2.erase(12);						// n is removed = 1
	num2.clear();


	// saving ptr. into set
	// use owner_less<T> for comparison. <memory>
	using Group = std::set<std::shared_ptr<std::string>, std::owner_less<std::shared_ptr<std::string>>>;
	Group group;
	group.insert(std::make_shared<std::string>(std::string("zzzz")));
	group.insert(std::make_shared<std::string>(std::string("aaaa")));
	group.insert(std::make_shared<std::string>(std::string("bbbb")));
	group.insert(std::make_shared<std::string>(std::string("cccc")));

	// may be... declared order, not alphabetic
	for (auto& i : group)
	{
		std::cout << std::endl;
		std::cout << i->c_str() ;
	}
	std::cout << std::endl;


	// algorithm
	std::vector<int> set1{ 1, 2, 3, 4, 5, 6 };	// vector must be ordered
	std::vector<int> set2{ 4, 5, 6, 7, 8, 9 };
	std::vector<int> union_res;
	std::set_union(std::begin(set1), std::end(set1),
		std::begin(set2), std::end(set2),
		std::back_inserter(union_res));
	std::set_union(std::begin(set1), std::end(set1),
		std::begin(set2), std::end(set2),
		std::ostream_iterator<int>{std::cout, " "});
	std::cout << std::endl;

	std::vector<int> inter_res;
	std::set_intersection(std::begin(set1), std::end(set1),
		std::begin(set2), std::end(set2),
		std::inserter(inter_res, std::begin(inter_res)));

	//std::set_difference();				// 차집합
	//std::set_symmetric_difference();	// 한 집합에만 포함되고 두 집합에는 포함 안되는

	std::set<std::string> words1{ "one", "two", "three", "four", "five", "six" };
	std::set<std::string> words2{ "two", "four", "one" };
	std::cout << std::boolalpha << std::includes(std::begin(words1), std::end(words1),
		std::begin(words2), std::end(words2)) << std::endl;
}


void test_sort()
{
	std::cout << "\n\nsort============================\n";

	// swapable (std::swap())
	// must provide random access iterator. so, list and forward_list have its own sort() function.
	std::vector<int> numbers{ 99, 77, 33, 66, 22, 11, 44, 88 };
	std::sort(std::begin(numbers), std::end(numbers));
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;

	std::sort(std::begin(numbers), std::end(numbers), std::greater<>());
	std::copy(std::begin(numbers), std::end(numbers), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;

	// stable_sort
	// 상등시 원본 순차열과 같은 순서로 보존된다. 이 예는 솔직히 의미가 없지만 
	std::vector<int> numbers2{ 99, 77, 33, 33, 66, 22, 11, 33, 44, 88, 44 };
	std::stable_sort(std::begin(numbers2), std::end(numbers2));
	std::copy(std::begin(numbers2), std::end(numbers2), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;

	// partial_sort
	// n개까지만 정렬
	std::vector<int> pnumbers{ 22, 7, 93, 45, 19, 56, 88, 12, 8, 7, 15, 10 };
	std::partial_sort(std::begin(pnumbers), std::begin(pnumbers) + 5, std::end(pnumbers));
	std::copy(std::begin(pnumbers), std::end(pnumbers), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
	
	// partial_sort_copy
	std::vector<int> result(5);
	std::partial_sort_copy(std::begin(pnumbers), std::begin(pnumbers), std::begin(result), std::end(result));

	std::cout << "number is "
		<< (std::is_sorted(std::begin(result), std::end(result)) ? "" : "not ")
		<< "in ascending sequence.\n";
}


void test_merge()
{
	std::cout << "\n\nmerge============================\n";


	std::vector<int> n1{ 2, 15, 4, 11, 6, 7 };
	std::vector<int> n2{ 5, 2, 3, 2, 14, 11, 6 };
	std::stable_sort(std::begin(n1), std::end(n1), std::greater<>());
	std::stable_sort(std::begin(n2), std::end(n2), std::greater<>());
	std::vector<int> result(n1.size() + n2.size() + 10);

	auto end_iter = std::merge(std::begin(n1), std::end(n1), std::begin(n2), std::end(n2),
		std::begin(result), std::greater<>());
	std::copy(std::begin(result), std::end(result), std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;
	std::copy(std::begin(result), end_iter, std::ostream_iterator<int>(std::cout, " "));
	std::cout << std::endl;

	// inplace_merge(first, second, last)
	// 정렬된 두 인접 순차열을 내부에서 병합. 즉 컨테이너는 하나. [first, second) [second, last)
	// 5 17 19 20 24 30 9(second) 13 19 25 29 31 40 41
}


void test_find()
{
	std::cout << "\n\nfind============================\n";

	std::vector<int> n1{ 2, 15, 4, 11, 6, 7 };
	int value{ 11 };
	auto iter = std::find(std::begin(n1), std::end(n1), value);
	if (iter != std::end(n1))
		std::cout << value << " was found.\n";

	int cmp{ 8 };
	auto iter1 = std::find_if(std::begin(n1), std::end(n1), [cmp](int n) {return n > cmp; });
	if (iter1 != std::end(n1))
		std::cout << *iter1 << " was found greater than " << cmp << ".\n";

	// find_if_not

	// find_first_of
	std::string text{ "The world of searching hhh" };
	std::string vowels{ "aeiou" };
	auto it_ff = std::find_first_of(std::begin(text), std::end(text), std::begin(vowels), std::end(vowels));
	if (it_ff != std::end(text))
		std::cout << "We found '" << *it_ff <<"'.\n";		// e

	// adjacent_find
	auto it_aj = std::adjacent_find(std::begin(text), std::end(text));
	if (it_aj != std::end(text))
		std::cout << "In the following text: " << text << "\n"
		<< *it_aj << " is repeated starting at index position "
		<< std::distance(std::begin(text), it_aj) << std::endl;

	// find_end()	last position of subsequence.	
	// search()		first ...
	// search_n()	n-times matching

	// binary_search()
	std::list<int> values{ 17, 11, 40, 36, 22, 54, 48, 70, 61, 82, 78, 89, 99, 92, 43 };
	values.sort();
	int wanted{ 22 };
	if (std::binary_search(std::begin(values), std::end(values), wanted))
		std::cout << wanted << " is in there.\n";
	else
		std::cout << wanted << " cannot be found.\n";

	// lower_bound, upper_bound
	values.sort();
	std::cout << "lower bound for " << wanted << " is " << *std::lower_bound(std::begin(values), std::end(values), wanted) << std::endl;
	std::cout << "upper bound for " << wanted << " is " << *std::upper_bound(std::begin(values), std::end(values), wanted) << std::endl;
	// 22 36 

	// equal_range (lower_bound + upper_bound)
	auto pr = std::equal_range(std::begin(values), std::end(values), wanted);
	std::cout << "lower bound for " << wanted << " is " << *pr.first << std::endl;
	std::cout << "upper bound for " << wanted << " is " << *pr.second << std::endl;

}


void test_cond()
{
	std::cout << "\n\ncond============================\n";

	std::vector<int> values{ 17, 11, 40, 36, 22, 54, 48, 70, 61, 82, 78, 89, 99, 92, 43 };
	int min_age{ 18 };
	std::cout << "there are "
		<< (std::none_of(std::begin(values), std::end(values),
			[min_age](int age) {return age < min_age; }) ? "no" : "some")
		<< " people under " << min_age << std::endl;

	std::cout << "there are "
		<< (std::any_of(std::begin(values), std::end(values),
			[min_age](int age) {return age < min_age; }) ? "some" : "no")
		<< " people under " << min_age << std::endl;

	std::cout << (std::all_of(std::begin(values), std::end(values),
			[](int age) {return age < 100; }) ? "None" : "Some")
		<< " of the people are centenarians." << std::endl;

	std::cout << "There are " << std::count(std::begin(values), std::end(values), 11)
		<< " people aged 11." << std::endl;

	int max_age = 60;
	std::cout << "There are " << std::count_if(std::begin(values), std::end(values),
		[max_age](int age) {return age > max_age; })
		<< " people aged over " << max_age << std::endl;
}


void test_equal()
{
	std::cout << "\n\nequal============================\n";

	std::vector<std::string> w1{ "one", "two", "three", "four", "five", "six" };
	std::vector<std::string> w2{ "two", "three", "four", "five", "six", "seven" };
	auto it1 = std::begin(w1);
	auto eit1 = std::end(w1);
	auto it2 = std::begin(w2);
	auto eit2 = std::end(w2);

	std::cout << "w1: ";
	std::copy(it1, eit1, std::ostream_iterator<std::string>{std::cout, " "});
	std::cout << "\nw2: ";
	std::copy(it2, eit2, std::ostream_iterator<std::string>{std::cout, " "});
	std::cout << std::endl;

	std::cout << "1. compare w1[1] to end with 2:                               ";
	std::cout << std::boolalpha << std::equal(it1 + 1, eit1, it2) << std::endl;			// true

	std::cout << "2. compare w2[0] to second-to-last with w1:                   ";
	std::cout << std::boolalpha << std::equal(it2, eit2 - 1, it1) << std::endl;			// false

	std::cout << "3. compare w1[1] - [3] with w2:                               ";
	std::cout << std::boolalpha << std::equal(it1 + 1, it1 + 4, it2) << std::endl;		// true

	std::cout << "4. compare first 3:                                           ";
	std::cout << std::boolalpha << std::equal(it1, it1 + 3, it2, it2 + 3) << std::endl;	// false

	std::cout << "5. compare all 3 params:                                      ";		// false
	std::cout << std::boolalpha << std::equal(it1, eit1, it2) << std::endl;

	std::cout << "6. compare all 4 params:                                      ";
	std::cout << std::boolalpha << std::equal(it1, eit1, it2, eit2) << std::endl;		// false

	std::cout << "7. compare w1[1] to end with w2 form first to second-to-last: ";
	std::cout << std::boolalpha << std::equal(it1 + 1, eit1, it2, eit2 - 1) << std::endl;	// true

	// and mismatch

	// lexicographical_compare: 사전순서로 범위 비교
	// {"the" "tiger" "of" "wrath"} > {"the" "horses" "of" "instruction"}
}


void test_permutation()
{
	std::cout << "\n\nperm============================\n";

	std::vector<int> range{ 1,2,3,4 };
	do
	{
		std::copy(std::begin(range), std::end(range), std::ostream_iterator<int>(std::cout, " "));
		std::cout << std::endl;
	} while (std::next_permutation(std::begin(range), std::end(range)));

	std::vector<int> range2{ 4, 3, 1, 2 };
	std::cout <<
		(std::is_permutation(std::begin(range), std::end(range), std::begin(range2), std::end(range2))
			? "is permutation" : "not permutation");
	std::cout << std::endl;
}


void test_copy()
{
	// std::copy_n
	// std::copy_if
	// std::copy_backward	뒤집는 게 아니라 뒤에서부터 복사 시작
	// std::reverse_copy
	// std::reverse			1 container
	// std::unique_copy		remove dup.
	// std::unique			1 container, remove dup.
}


void test_move()
{
	// std::rotate		rotate to left(counter-clockwise)	1 2 3 4 => 2 3 4 1
	// std::move
	// std::swap
}

void test_remove()
{
	// std::remove()
	// std::remove_copy()
	// std::remove_if()
	// std::remove_copy_if()
}

void test_fill()
{
	std::cout << "\n\nfill============================\n";

	std::vector<int> data{ 12 };
	std::fill(std::begin(data), std::end(data), 3);
	// std::fill_n()

	std::string chars(30, ' ');
	char ch{ 'a' };
	int inc{};
	std::generate(std::begin(chars), std::end(chars), [ch, &inc]
	{
		inc += 3;
		return ch + (inc % 26);
	});
	std::cout << chars << std::endl;

	std::vector<double> cel{ 21.0, 20.5, 0.0, 3.2, 100.0 };
	std::vector<double> far(cel.size());
	std::transform(std::begin(cel), std::end(cel), std::begin(far),
		[](double temp) { return 32.0 + 9.0 * temp / 5.0; });

	std::vector<double> far_empty;
	std::transform(std::begin(cel), std::end(cel), std::back_inserter(far_empty),
		[](double temp) { return 32.0 + 9.0 * temp / 5.0; });
}


void test_replace()
{
	// std::replace()
	// std::replace_if()
	// std::replace_copy()
}


void test_shuffle()
{
	// <random>
	std::cout << "\n\nshuffle============================\n";

	std::random_device rd;
	std::mt19937 rng{ rd() };
	//std::mt19937 rng2(std::random_device{}());
	std::vector<std::string> words{ "one", "two", "three", "four", "five", "six", "seven" };
	for (size_t i = 0; i < 4; ++i)
	{
		std::shuffle(std::begin(words), std::end(words), rng);
		std::for_each(std::begin(words), std::end(words),
			[](const std::string& w) { std::cout << std::setw(8) << std::left << w; });
		std::cout << std::endl;
	}
}


void test_istream()
{
	std::cout << "\n\nistream============================\n";


	std::cout << "Enter one or more words. Enter ! to end:\n";
	std::istream_iterator<std::string> in{ std::cin };
	std::vector<std::string> words;
	while (true)
	{
		std::string word = *in;
		if (word == "!") break;
		words.push_back(word);
		++in;
	}
	std::wcout << "You entered " << words.size() << " words." << std::endl;

	std::cout << "Enter some integers - enter ctrl+Z to end.\n";
	std::istream_iterator<int> iter{ std::cin };
	std::istream_iterator<int> end_iter;
	int sum{};
	while (iter != end_iter)
	{
		sum += *iter++;
	}
	std::cout << "Total is " << sum << std::endl;
}

void test_ostream()
{
	std::cout << "\n\nostream============================\n";

	std::vector<std::string> words{ "the", "quick", "brown", "fox", "jumped", "over", "the", "lazy", "dog" };
	std::ostream_iterator<std::string> iter{ std::cout };
	for (const auto& word : words)
	{
		*iter++ = word;
		*iter++ = " ";
	}
	*iter++ = "\n";

	for (const auto& word : words)
	{
		iter = word;
		iter = " ";
	}
	iter = "\n";

	std::ostream_iterator<std::string> iter2{ std::cout, " " };
	std::copy(std::begin(words), std::end(words), iter2);
	iter2 = "\n";


	std::string file_in{ "dictionary.txt" };
	std::ifstream in(file_in);
	if (!in)
	{
		std::cerr << file_in << " not open." << std::endl;
		return;
	}

	std::set<std::string> dictionary{ std::istream_iterator<std::string>(in), std::istream_iterator<std::string>() };
	std::cout << dictionary.size() << " words in dictionary." << std::endl;

	//std::string file_out{ "dictionary_copy.txt" };
	//std::ofstream out{ file_out, std::ios_base::out | std::ios_base::trunc };
	//std::copy(std::istream_iterator<std::string> {in}, std::istream_iterator<std::string>{},
	//	std::ostream_iterator<std::string>{out, " "});
	//in.clear();
	//in.close();
	//out.close();
}


void test_bufiterator()
{
	std::cout << "\n\nbufiter============================\n";

	// read
	std::istreambuf_iterator<char> in{ std::cin };
	std::istreambuf_iterator<char> end_in;
	char end{ '*' };
	std::string rubbish;
	while (in != end_in && *in != end) rubbish += *in++;
	std::cout << rubbish << std::endl;

	// write
	std::ofstream file_out{ "dict_copy.txt", std::ios_base::out, std::ios_base::trunc };
	std::ostreambuf_iterator<char> out{ file_out };
	for (const auto& c : rubbish)
		out = c;
	file_out.close();
}


void test_stringstream()
{
	std::cout << "\n\nstringstream============================\n";

	std::string file_in{ "dictionary.txt" };
	std::ifstream in(file_in);
	if (!in)
	{
		std::cerr << file_in << " not open." << std::endl;
		return;
	}

	std::stringstream instr;
	std::copy(std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>(),
		std::ostreambuf_iterator<char>{instr});
	in.close();

	auto end_iter = std::istream_iterator<std::string>();
	std::string word = "3";
	if (end_iter != std::find(std::istream_iterator<std::string>(instr), end_iter, word))
		std::cout << "3 found." << std::endl;
}


void test_numeric()
{
	std::cout << "\n\nnumeric============================\n";

	std::vector<double> data(9);
	std::iota(std::begin(data), std::end(data), -4);
	std::copy(std::begin(data), std::end(data),
		std::ostream_iterator<double>{std::cout << std::fixed << std::setprecision(1), " "});
	std::cout << std::endl;

	// std::accumulate()

	// inner_product
	std::vector<int> v1(10);
	std::vector<int> v2(10);
	std::iota(std::begin(v1), std::end(v1), 2);
	std::iota(std::begin(v2), std::end(v2), 3);
	// 10x1 ip 1x10 = 570
	std::cout << "inner product: " << std::inner_product(std::begin(v1), std::end(v1), std::begin(v2), 0) << std::endl;


	// adjacent_difference
	std::vector<int> ad{2, 3, 5, 7, 11, 13};
	std::wcout << "Difference: ";
	std::adjacent_difference(std::begin(ad), std::end(ad),
		std::ostream_iterator<int>{std::cout, " "});
	std::cout << std::endl;

	// partial sum
	std::wcout << "Partial sum: ";
	std::partial_sum(std::begin(ad), std::end(ad),
		std::ostream_iterator<int>{std::cout, " "});		// 2 5 10 17 28 41
	std::cout << std::endl;

	// std::min_element
	// std::max_element
	// std::minmax_element	: pair

	// <ratio>
	using v2_3 = std::ratio<2, 3>;	// 2/3
	// ratio_subtract<1, 2>
	// ratio_multiply<1, 2>
	// ratio_divide<1, 2>
	// ratio_equal, ...
}


void test_chrono()
{
	std::cout << "\n\nchrono============================\n";


	using namespace std::chrono;

	std::chrono::duration<int, std::milli> IBM650_divide{ 15 }; // 15 ms
	std::chrono::duration <int> minute{ 60 };					// 1 tick=1 sec, 60 sec
	std::chrono::duration <double, std::ratio<60>> hour{ 60 };	// 1 tick=60 sec, 60 min
	std::chrono::duration <long, std::micro> millisec{ 1000L };	// 1 ms
	std::chrono::duration <double, std::ratio<1, 5>> tiny{ 5.5 };	// 1 tick=1/5, 1.1 sec

	// literal
	// h, min, s, ms, us, ns
	std::chrono::seconds el{ 10 };
	el -= 3s;

	// clock
	// system_clock	리얼클록. 증가하지만 감소도 가능. eg)summer time
	// steady_clock 항상 증가
	// high_resolution_clock	가장 짧은 틱 주기를 갖는 클록. system_clock의 별칭에 불과한 구현체도 있다


	// 
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	TimePoint tp1{ std::chrono::duration<int>(20) };
	auto elapsed = tp1.time_since_epoch();
	auto ticks_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();

	// now
	auto instant = Clock::now();
}


void test_complex()
{
	std::cout << "\n\ncomplex============================\n";


	std::complex<double> z1{ 2, 5 };	// 2 + 5i
	std::cout << "Real: " << z1.real()
		<< ", Imaginary: " << z1.imag() << std::endl;
}


void test_random()
{
	std::cout << "\n\nrandom============================\n";


	// random unsigned integer 생성 범용 소스
	std::default_random_engine rng1;	// 기본 시드로 랜덤 넘버 생성기를 생성. 시드가 같으므로 생성 순차열 항상 같음.

	std::default_random_engine rng2{ 10 };	// 시드를 10으로. 위와는 다른 순차열이지만 이 역시 항상 같음.

	std::random_device rd;						// 비결정적 시드
	std::default_random_engine rng3{ rd() };	// rd() 호출시마다 다른 값 반환.

	std::seed_seq sd_seq{ 2, 4, 6, 8 };
	std::default_random_engine rng4{ sd_seq };	// 매번 같은 랜덤 넘버 순차열

	std::random_device rd5;
	std::seed_seq sd_seq5{ rd5(), rd5(), rd5() };
	std::default_random_engine rng5{ sd_seq5 };



	std::uniform_int_distribution<> d;		// int type 0-max distribution
	std::cout << "Range from 0 to "
		<< std::numeric_limits<std::uniform_int_distribution<>::result_type>::max()
		<< std::endl;

	std::cout << "Range from " << d.min() << " to " << d.max() << std::endl;
	std::cout << "Range from " << d.a() << " to " << d.b() << std::endl;

	std::uniform_int_distribution<> d2{ 500 };	// min
	std::cout << "Range from " << d2.a() << " to " << d2.b() << std::endl;

	std::uniform_int_distribution<> dist{ -5L, 5L };	// min max
	std::cout << "Range from " << dist.a() << " to " << dist.b() << std::endl;
	std::random_device rddist;
	std::default_random_engine rngdist{ rddist() };
	for (size_t i = 0; i < 8; ++i)
		std::cout << std::setw(2) << dist(rngdist) << " ";
	std::cout << std::endl;

	std::uniform_int_distribution<> dist2{ 0, 6 };
	std::random_device rddist2;
	std::default_random_engine rngdist2{ rddist2() };
	for (size_t i = 0; i < 8; ++i)
		std::cout << std::setw(3) << dist2(rngdist2) << " ";
	std::cout << std::endl;

	// backup old range and set new range
	auto old_range = dist2.param();
	dist2.param(std::uniform_int_distribution<>::param_type{ -10, 20 });
	for (size_t i = 0; i < 8; ++i)
		std::cout << std::setw(3) << dist2(rngdist2) << " ";
	std::cout << std::endl;

	// restore old
	dist.param(old_range);
	for (size_t i = 0; i < 8; ++i)
		std::cout << std::setw(3) << dist2(rngdist2) << " ";
	std::cout << std::endl;


	std::uniform_real_distribution<> values{ 0.0, 10.0 };
	// std::random_device rd;
	std::default_random_engine rngr{ rd() };
	for (size_t i = 0; i < 8; ++i)
		std::cout << std::fixed << std::setprecision(2) << values(rngr) << " ";
	std::cout << std::endl;
}


std::uniform_int_distribution<size_t>& s_dist()
{
	static std::uniform_int_distribution<size_t> d;
	return d;
}


std::default_random_engine& s_rng()
{
	static std::default_random_engine engine{ std::random_device()() };
	return engine;
}


enum class Suit : size_t { Clubs, Diamonds, Hearts, Spades };
enum class Face : size_t { Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

using Card = std::pair<Suit, Face>;
using Hand = std::list<Card>;
using Deck = std::list<Card>;
using Hands = std::vector<Hand>;
using Range = std::uniform_int_distribution<size_t>::param_type;


std::ostream& operator<<(std::ostream& out, const Card& card)
{
	static std::array<std::string, 4> suits{ "C", "D", "H", "S" };
	static std::array<std::string, 13> values{ "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
	std::string suit{ suits[static_cast<size_t>(card.first)] };
	std::string value{ values[static_cast<size_t>(card.second)] };
	return out << std::right << std::setw(2)  << value << suit;
}


Deck& init_deck(Deck& deck)
{
	static std::array<Suit, 4> suits{ Suit::Clubs, Suit::Diamonds, Suit::Hearts, Suit::Spades };
	static std::array<Face, 13> values{
		Face::Two, Face::Three, Face::Four, Face::Five,
		Face::Six, Face::Seven, Face::Eight, Face::Nine,
		Face::Ten, Face::Jack, Face::Queen, Face::King, Face::Ace };

	deck.clear();
	for (const auto& suit : suits)
	{
		for (const auto& value : values)
		{
			deck.emplace_back(Card{ suit, value });
		}
	}

	return deck;
}


void deal(Hands& hands, Deck& deck)
{
	auto d = s_dist();
	while (!deck.empty())
	{
		for (auto&& hand : hands)
		{
			size_t max_index = deck.size() - 1;
			d.param(Range{ 0, max_index });
			auto iter = std::begin(deck);
			std::advance(iter, d(s_rng()));		// advance to random pos.
			hand.push_back(*iter);
			deck.erase(iter);
		}
	}
}


void sort_hands(Hands& hands)
{
	for (auto&& hand : hands)
		hand.sort([](const auto& cd1, const auto& cd2)
			{ return cd1.first < cd2.first || (cd1.first == cd2.first && cd1.second < cd2.second); });
}


void show_hands(const Hands& hands)
{
	for (auto&& hand : hands)
	{
		std::copy(std::begin(hand), std::end(hand), std::ostream_iterator<Card>{std::cout, " "});
		std::cout << std::endl;
	}
}


void test_card()
{
	std::cout << "\n\ncard============================\n";


	Deck deck;
	init_deck(deck);

	Hands hands(4);
	deal(hands, deck);

	sort_hands(hands);
	show_hands(hands);
}


void test_normal_dist()
{
	std::cout << "\n\nnormal_dist============================\n";

	// 평균: mu, 표준편차: sigma 

	std::normal_distribution<> dist;	// mu: 0, sigma: 1

	double mu{ 50.0 }, sigma{ 10.0 };
	std::normal_distribution<> norm{ mu, sigma };

	std::random_device rd;
	std::default_random_engine rng{ rd() };
	std::cout << "Normally distriuted values: "
		<< norm(rng) << " " << norm(rng) << std::endl;
	std::cout << "mu: " << norm.mean() << " sigma: " << norm.sigma() << " " << norm.stddev() << std::endl;


	// log normal
	double mul{ 5.0 }, sigmal{ 0.5 };
	std::lognormal_distribution<> lognormal{ mul, sigmal };
}


void test_disc_dist()
{
	std::cout << "\n\ndesc_dist============================\n";

	std::discrete_distribution<size_t> d{ 1, 1, 1, 1, 1, 3 };	// 1/8 ~ 3/8, 6th: occur 3 times than others.
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::map<size_t, size_t> results;
	for (size_t i = 0; i < 5000; ++i)
		++results[d(rng)];

	for (const auto&pr : results)
		std::cout << "A " << (pr.first + 1)		// 0-5, so + 1
		<< " was thrown " << pr.second << " times\n";


	std::discrete_distribution<size_t> dist
	{
		6,				// count
		0,				// xmin
		12,				// xmax
		[](double v) { return 2 * v; }		// op(step)
	};
	// 6개가 0~12 까지의 분포이므로
	// 0  2  4  6  8 10 이고 step = 2
	// 여기에 step * 2
	// 2  4  6  8 10 12 를 더해
	// 2  6 10 14 18 22 를 72(총합)로 나누면

	// 1/36 1/12 5/36 7/36 1/4 11/36 이 확률 분포
}


void test_piece_dist()
{
	std::cout << "\n\npiece_dist============================\n";

	// 막대 그래프
	std::vector<double> b{ 10, 30, 50, 80 };	// 경계
	std::vector<double> w{ 4, 10, 6 };			// 가중치
	std::piecewise_constant_distribution<> d{ std::begin(b), std::end(b), std::begin(w) };

	// 경계 내에선 균등하게 분포, 각 경계는 가중치에 의해 다르게 분포
	// 10-30은 4의 가중치 즉 4 / (4 + 10 + 6) = 20%
	// 30-50은 10 ==> 50%
	// 50-80은 6  ==> 30%

	auto intervals = d.intervals();	// std::vector<>
	auto densities = d.densities();

	std::cout << "intervals: ";
	std::copy(std::begin(intervals), std::end(intervals), std::ostream_iterator<double>{std::cout, " "});
	std::cout << "densities: ";
	std::copy(std::begin(densities), std::end(densities), std::ostream_iterator<double>{std::cout, " "});
	std::cout << std::endl;

	// 조각 선형 분포
	// std::piecewise_linear_distribution 경계가 균등하지 않고 선형으로, 막대가 아니라 산 모양

	// 푸아종 분포: 독립사건이 몇 번 발생한 것인지를 표현하는 이산 확률밀도함수를 정의한 것
	// std::poisson_distribution<>
}


int main()
{
	test_array();
	test_vector();
	test_deque();
	test_list();
	test_numeric_range();
	test_priority_queue();
	test_heap();
	test_ptr();
	test_map();
	//word_count();
	test_tuple();
	test_multimap();
	test_set();
	test_sort();
	test_merge();
	test_find();
	test_cond();
	test_equal();
	test_permutation();
	test_fill();
	test_shuffle();
	//test_istream();
	test_ostream();
	//test_bufiterator();
	test_stringstream();
	test_numeric();
	test_complex();
	test_random();
	test_card();
	test_normal_dist();
	test_disc_dist();
	test_piece_dist();
}