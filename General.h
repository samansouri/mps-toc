#ifndef GNRL
#define GNRL 1

#include "cProblem.h"
#include <time.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <algorithm>
#include  <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include "cProduct.h"
#include "cResource.h"

using namespace std;

#define random(num)  (rand()%(num))
#define randomize()  srand((unsigned)time(NULL))

string HrMinString (int Minutes);
void GoTo_xy(short x,short y);
void DisplayProgress(double, double);
string IndexedOutputFile(string, string);
string TrimedString (char*);
void PutStringBitsInCharacterArray (char* c_array, string given_string);
int MaxFeasibleProdQty(cProduct Prod, list<cResource> CR);

#endif 
