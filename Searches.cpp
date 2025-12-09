#include<iostream>
#include<vector>
#include"Search_Algs.h"
#include "flight.h"
#include "Reading_for_search.h"
#include<ctime>
using namespace std;

linear_search_test(const vector<flight> flights) {
	time_t before_sort = time(nullptr);
	vector<flight> res_float = linear_search(flights, flight::distance, 833.0);
	time_t after_sort = time(nullptr);
	cout << "To find a distance(float) of 833.0 linear search takes" << difftime(after_sort, before_sort) << "and found" << res_float.size() << "entries";
}


int main(){
	vector<flight> flights = reading_by_instances();
	linear_search_test(flights);
	return 0;
}