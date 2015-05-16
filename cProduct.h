#ifndef TSK
#define TSK 1

#include "Parameters.h"
#include "cResource.h"
#include <vector>
#include <list>
using namespace std;


class cProduct{
public:
	cProduct (); //default constructor
	cProduct(int ResNum);
	cProduct (const cProduct& copy_me); //copy constructor
	char Code[CODE_SIZE];
	int IndexInProblemProductArray; // reflects position of the product in the Problem Resource array
	int TotalRes;
	std::vector<float> ProcessTime;
	int MarketDemand;
	float ContributionMargin;
	int ProdQty;
	float Throughput;
	float R_Ratio; // CM/(ti,BN1)
	float R_Ratio_PBN; // R_Ratio Based on Primary Bottlenck. To be used in our algorithm
	std::vector<cResource> vResource;
	int MaxFeasibleProdQty(list<cResource> CR);
	int MaxFeasibleProdQtyBasedOnPrimaryBottleneck(list<cResource> CR);
	int MaxFeasibleProdQtyBasedOnAllBottlenecks(list<cResource> CR);
	int MaxFeasibleProdQtyBasedOnGivenResource(list<cResource>::iterator itor_cr);
	cProduct& operator= (const cProduct& rhs);
	bool QualifiedForSetOfCandidateProductsForTradeoff_X; //set of qualified products for trade-off
	bool cProduct::operator< (const cProduct& rhs);
	bool cProduct::operator == (const cProduct& rhs);
	bool cProduct::operator != (const cProduct& rhs);
	~cProduct();
};

#endif 
