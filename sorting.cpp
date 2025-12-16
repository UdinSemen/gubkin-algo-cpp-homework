#include "sorting.h"
#include <algorithm>

static void merge(std::vector<flight>& data, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<flight> L(n1), R(n2);

    for (int i = 0; i < n1; ++i) L[i] = data[left + i];
    for (int j = 0; j < n2; ++j) R[j] = data[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].get_arr_delay() <= R[j].get_arr_delay()) {
            data[k++] = L[i++];
        }
        else {
            data[k++] = R[j++];
        }
    }
    while (i < n1) data[k++] = L[i++];
    while (j < n2) data[k++] = R[j++];
}

static void mergeSortImpl(std::vector<flight>& data, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortImpl(data, left, mid);
        mergeSortImpl(data, mid + 1, right);
        merge(data, left, mid, right);
    }
}

void mergeSortByArrivalDelay(std::vector<flight>& flights) {
    if (!flights.empty()) {
        mergeSortImpl(flights, 0, static_cast<int>(flights.size()) - 1);
    }
}


static bool specialComparator(const flight& a, const flight& b) {
    if (a.is_canceled() != b.is_canceled())
        return a.is_canceled() > b.is_canceled(); 
    if (a.get_dest_state() != b.get_dest_state())
        return a.get_dest_state() < b.get_dest_state(); 
    return a.get_arr_delay() > b.get_arr_delay(); 
}

void specialFlightSort(std::vector<flight>& flights) {
    std::sort(flights.begin(), flights.end(), specialComparator);
}