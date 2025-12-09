#include<iostream>
#include<vector>
#include<chrono>
#include"Search_Algs.h"
#include "flight.h"
#include "flight_gen.h"

using namespace std;

int linear_search_test(const vector<flight> flights) {
	auto start = chrono::high_resolution_clock::now();
	vector<flight> res_float = linear_search(flights, &flight::origin_city, string("Boston"));
	auto end = chrono::high_resolution_clock::now();

	// Calculate duration in milliseconds (or microseconds for even smaller times)
	chrono::duration<double, milli> duration_ms = end - start;
	double diff = duration_ms.count();
	cout << "To find Boston as an origin city linear search takes " << diff << " milliseconds and found " << res_float.size() << " entries";
	return 0;
}

int binary_search_test(const vector<flight> flights) {
	vector<flight> sorted_flights = flights;
	sort(sorted_flights.begin(), sorted_flights.end(),
		[](const flight& a, const flight& b) {
			return a.weather_delay < b.weather_delay;
		});
	auto start = chrono::high_resolution_clock::now();
	vector<flight> res_float = binary_search(sorted_flights, &flight::weather_delay, true);
	auto end = chrono::high_resolution_clock::now();

	// Calculate duration in milliseconds (or microseconds for even smaller times)
	chrono::duration<double, milli> duration_ms = end - start;
	double diff = duration_ms.count();
	cout << "To a weather delay binary search takes " << diff << " milliseconds and found " << res_float.size() << " entries";
	return 0;
}

int fibonachi_search_test(const vector<flight> flights) {
	vector<flight> sorted_flights = flights;
	sort(sorted_flights.begin(), sorted_flights.end(),
		[](const flight& a, const flight& b) {
			return a.distance < b.distance;
		});
	auto start = chrono::high_resolution_clock::now();
	vector<flight> res_float = Fibonachi_search(sorted_flights, &flight::distance, 5000.0f);
	auto end = chrono::high_resolution_clock::now();

	// Calculate duration in milliseconds (or microseconds for even smaller times)
	chrono::duration<double, milli> duration_ms = end - start;
	double diff = duration_ms.count();
	cout << "To find 5000.f distance Fibonachi search takes " << diff << " milliseconds and found " << res_float.size() << " entries";
	return 0;
}


int main() {
	vector<flight> flights = generateRandomFlights(10000000);

	//linear_search_test(flights);
	//binary_search_test(flights);
	fibonachi_search_test(flights);
	
	return 0;
}