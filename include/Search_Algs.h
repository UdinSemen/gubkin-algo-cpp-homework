#pragma once
#include<vector>
#include "flight.h"
#include<algorithm>
#include<string>
#include<ctime>
using namespace std;

template<typename T, typename Getter>
vector<flight> linear_search(const vector<flight>& flights, Getter getter, const T& searched_elem) {
	vector<flight> result;
	for (const auto& item : flights)
	{
		if (getter(item) == searched_elem)
		{
			result.push_back(item);
		}
	}

	return result;
}

template<typename T, typename Getter>
vector<flight> binary_search(const vector<flight>& flights, Getter getter, const T& searched_elem) { // only works for sorted flights
	vector<flight> result;

	int bot = 0;
	int top = static_cast<int>(flights.size() - 1);
	while (bot <= top) {

		int mid = bot + (top - bot) / 2;

		auto midVal = getter(flights[mid]);
		if (mid < 0 || mid >= flights.size()) {
			break;
		}
		if (midVal == searched_elem) {
			int i = mid;
			while (i >= 0 && getter(flights[i]) == searched_elem)
			{
				result.push_back(flights[i]);
				i--;
			}
			i = mid + 1;
			while (i < flights.size() && getter(flights[i]) == searched_elem)
			{
				result.push_back(flights[i]);
				i++;
			}
			return result;
		}
		if (midVal < searched_elem) {
			bot = mid + 1;
		}
		if (midVal > searched_elem) {
			top = mid - 1;
		}
	}
	return result;
}

template<typename T, typename Getter>
vector<flight> Fibonacci_search(const vector<flight>& flights, Getter getter, const T& searched_elem) { //only works for sorted vectors
	vector<flight> result;
	int n = flights.size();

	int a = 0, b = 0, c = 1;
	while (c < n) {
		a = b;
		b = c;
		c = a + b;
	}

	int elim = -1;

	while (c > 1) {
		int i = min(elim + a, n - 1);
		auto iVal = getter(flights[i]);
		if (iVal < searched_elem) {
			c = b;
			b = a;
			a = c - b;
			elim = i;
		}

		else if (iVal > searched_elem) {
			c = a;
			b = b - a;
			a = c - b;
		}
		else {
			int i1 = i;
			auto finVal = getter(flights[i1]);
			while (i1 >= 0 && getter(flights[i1]) == searched_elem)
			{

				result.push_back(flights[i1]);
				i1--;
			}
			i1 = i + 1;
			while (i1 < flights.size() && getter(flights[i1]) == searched_elem)
			{

				result.push_back(flights[i1]);
				i1++;
			}
			return result;

		}
	}
	return result;
}
