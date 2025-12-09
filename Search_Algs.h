#pragma once
#include<vector>
#include "flight.h"
#include<algorithm>
using namespace std;

template<typename T>
vector<flight> linear_search(const vector<flight> &flights,T flight::*member,const T &searched_elem) {
	vector<flight> result;
	for (int i = 0; i < flights.size(); i++)
	{
		if (flights[i].*member == searched_elem)
		{
			result.push_back(flights[i]);
		}
	}
	return result;
}

template<typename T>
vector<flight> binary_search(const vector<flight>& flights,T flight::* member, const T& searched_elem) { // only works for sorted flights
	vector<flight> result;
	/*sort(flights.begin(), flights.end(),
		[](const flight& a, const flight& b) {
			return a.*member < b.*member;
		});*/ //usage of sort depends on whether or not search get already sorted vector

	int bot = 0;
	int top = flights.size() - 1;
	while (bot <= top) {
		int mid = bot + (top-bot) / 2;
		if (flights[mid].*member == searched_elem) {
			int i = mid;
			while (i >= 0 && flights[i].*member == searched_elem)
			{
				result.push_back(flights[i]);
				i--;
			}
			i = mid + 1;
			while (i < flights.size() && flights[i].*member == searched_elem)
			{
				result.push_back(flights[i]);
				i++;
			}
			return result;
		}
		if (flights[mid].*member < searched_elem) {
			bot = mid+1;
		}
		if (flights[mid].*member > searched_elem) {
			top = mid-1;
		}
	}
	return result;
}

template<typename T>
vector<flight> Fibonachi_search(const vector<flight>& flights, T flight::* member, const T& searched_elem) { //only works for sorted vectors
	vector<flight> result;
	/*sort(flights.begin(), flights.end(),
		[](const flight& a, const flight& b) {
			return a.*member < b.*member;
		});*/ //usage of sort depends on whether or not search get already sorted vector
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
		if (flights[i].*member < searched_elem) {
			c = b;
			b = a;
			a = c - b;
			elim = i;
		}

		else if (flights[i].*member > searched_elem) {
			c = a;
			b = b - a;
			a = c - b;
		}
		else {
			int i1 = i;
			while (i1 >= 0 && flights[i1].*member == searched_elem)
			{
				result.push_back(flights[i1]);
				i1--;
			}
			i1 = i + 1;
			while (i1 < flights.size() && flights[i1].*member == searched_elem)
			{
				result.push_back(flights[i1]);
				i1++;
			}
			return result;
		
		}
	}
	return result;
}