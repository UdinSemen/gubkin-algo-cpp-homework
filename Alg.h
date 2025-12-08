#pragma once
#include<set>
using namespace std;

template<typename T>
set linear_search(const vector<flight> &flights,T flight::*member,const T &searched_elem) {
	set<int> result;
	for (int i = 0; i < flights.size(); i++)
	{
		if (flights[i].*member == searched_elem)
		{
			result.insert(i);
		}
	}
	return result;
}

temlpate<typename T>
set binary_search(const vector<flight>& flights, T flight::* member, const T& searched_elem) { // only works for sorted flights
	set<int> result;
	bot = 0;
	top = flights.size() - 1;
	while (bot <= top) {
	 
	}
}