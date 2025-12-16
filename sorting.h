#ifndef DATASETREADING_SORTING_H
#define DATASETREADING_SORTING_H

#include <vector>
#include "flight.h"

void mergeSortByArrivalDelay(std::vector<flight>& flights);
void specialFlightSort(std::vector<flight>& flights);

#endif