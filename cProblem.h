#ifndef PROB
#define PROB 1

#include "cProduct.h"
#include "cResource.h"

class cProblem 
{
public:
	cProblem(); // default constructor
	cProblem (char *DataSet); // constructor
	int ProdNum; // number of Products
	int ResNum; //number of Resources
	std::vector<cProduct> vProduct; //array of Products
	std::vector<cResource> vResource; //array of Resources
	void DisplayProblem(); //for debugging 
	float TotalThroughput; //total throughput of the problem
	bool EveryProductUsesEveryResource();
	float MaxCapacity; //the maximum capacity among all resources
	void PrintDataSetIntoOutputFile(const char*);
	void PrintDataSetIntoOutputFileInTabularFormat(const char*);
   ~cProblem(); // destructor
};


#endif