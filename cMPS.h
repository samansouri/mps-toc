#ifndef MPS_HD
#define MPS_HD 1

#include "cProblem.h"
#include "cProduct.h"
#include "General.h"
#include "cTradeoff.h"
#include <vector>
#include <list>
using namespace std;



class cMPS 
{
public:
	cMPS(cProblem *pCurrentProblem);
	cProblem* pCP;
	vector<cProduct> vProductList;
	list<cResource> CR; //Critical Resources
	vector<float> AvailableCapacityOn_CR;
	cResource PrimaryBN; //to be used in our algorithm
	list<cResource>::iterator itor_PrimaryBN;
	int MaxFeasibleProdQty(cProduct Prod, list<cResource> CR);
	float TotalThroughput;
	void InitializeCapacitiesOf_CR_Set();
	void DevelopMPS_BasedOnFirstMemberOf_CR();
	void DevelopMPS_BasedOnAllBottlenecks();
	void DevelopMPS_BasedOn_B_Greedy_AlgorithmAndGivenResource(list<cResource>::iterator itor_cr); //used in SN12 alogirithm
	void DevelopMPS_BasedOn_B_Greedy_M_Algorithm();
	int IndexOfTheLastProductWithMinimumUnsatisfiedDemand();
	int IndexOfTheProductWithMaximuR_RatioAndMinimumUnsatisfiedDemand();
	int IndexOfTheProductWithMaximumR_Ratio();
	void LocalSearchUsing_B_Greedy_M_Algorithm (float DemandReductionFactor, int selectedProduct);
	void CalculateThroughputs();
	void CalculateCapacityShortagesAndDetermine_CR_Set();
	void Calculate_R_Ratio_BasedOnGivenMemberOf_CR(list<cResource>::iterator itor_cr);
	void CalculateLeftCapacitiesOnCRsBasedOnCurrentMPS();
	bool IsCurrentMPSFeasible();
	void Display_MPS_Table();
	void Display_MPS_Table_WitoutUsedCapacities();
	void Display_MPS_Table_WitoutUsedCapacities_With_R_Ratio_PBN();
	void IdentifySet_X();
	vector<int> Set_X; //set of indexes of candidate products for tradeoff
	void DoTradeoffAmongMembersOfSet_X();
	void DoTradeoffAmongImprovingPairs_gr(); //to be used in our algorithm
	bool CR_HasSufficientCapacityForAnExtraUnitProduction(int Prod_i);
	bool LeftCapacityOnCriticalResourcesIsSufficientForAnExtraUnitProduction(int Prod_Index);
	void UpdateCapacitiesByDeltaCapacity(int ProductIndex_Onward, int Delta_ProdQty);
	void InitializeAvailableCapacityOn_CR();
	void SetAvailableCapacityOn_CR_ToGivenProduct(int ProdIndex);
	void DecreaseAvailableCapacityOn_CR(int ProductIndex, int Delta_ProdQty);
	void DisplayLeftCapacitiesForProductionOfGivenProduct(int Prod_Index); //this function is only required for debugging
	void DisplayAvailableCapacityOn_CR(); //this function is only required for debugging
	void MoveResource_CL_ToTheBeginningOf_CR();
	void Calculate_R_Ratio_PBN_AndSetPrimaryBN();
	list<cTradeoff> ImprovingTradeoffs;
	void UpdateImprovingTradeoffs();
	void IdentifyPrimaryBottleneck();
	void Update_R_Ratio_PBN();
	list<cResource> DominatedBottlenecks;
	void IdentifyAndRemoveDominatedBottlenecksFrom_CR();
	void PrintMPS_TableIntoFile(const char*, const char*);
	
	~cMPS();
};


#endif