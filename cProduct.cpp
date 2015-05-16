#include "stdafx.h"
#include "cProduct.h"
#include "General.h"

cProduct::cProduct()
{
}

cProduct::cProduct(int ResNum){
	TotalRes = ResNum;
	ProcessTime.resize(ResNum);
	vResource.resize(ResNum);
	QualifiedForSetOfCandidateProductsForTradeoff_X = false;	
}

cProduct::cProduct (const cProduct& copy_me):

	IndexInProblemProductArray(copy_me.IndexInProblemProductArray),
	TotalRes(copy_me.TotalRes),
	//ProcessTime(new float[copy_me.TotalRes]), 
	MarketDemand(copy_me.MarketDemand),
	ContributionMargin(copy_me.ContributionMargin),
	ProdQty(copy_me.ProdQty),
	Throughput(copy_me.Throughput),
	R_Ratio(copy_me.R_Ratio),
	R_Ratio_PBN(copy_me.R_Ratio_PBN){
		std::copy(copy_me.Code, copy_me.Code+CODE_SIZE, Code);
		//std::copy(copy_me.vResource.begin(), copy_me.vResource.end(), vResource.begin());
		vResource = copy_me.vResource; //this line works well in place of the above and has to be inside {}
									   // THIS HOWEVER COULD CAUSE PROBLEM IF IT ONLY COPIES ADDRESSES AND NOT CONTENTS 
		ProcessTime = copy_me.ProcessTime;
	}


	int cProduct::MaxFeasibleProdQty(list<cResource> CR){
	std::list<cResource>::iterator itor_r;
	int MaxProdQty = MarketDemand;
	int FeasibleQty;
	//int j;
	for(itor_r=CR.begin(); itor_r!=CR.end(); itor_r++){

		//test starts @@@@@@@@@@
		//float lef_cap = vResource[itor_r->IndexInProblemResourceArray].LeftCapacity;
		//float proc_time = ProcessTime[itor_r->IndexInProblemResourceArray];
		//test ends @@@@@@@@@@@@

		if (ProcessTime[itor_r->IndexInProblemResourceArray]>0){
			FeasibleQty = int(vResource[itor_r->IndexInProblemResourceArray].LeftCapacity/
				ProcessTime[itor_r->IndexInProblemResourceArray]);
		
			if( FeasibleQty < MaxProdQty)
				MaxProdQty = FeasibleQty;
		}
	}
	return (MaxProdQty);
}

	int cProduct::MaxFeasibleProdQtyBasedOnPrimaryBottleneck(list<cResource> CR){
		int MaxProdQty = MarketDemand;
		int FeasibleProdQty;
		if (ProcessTime[CR.begin()->IndexInProblemResourceArray]>0){
			FeasibleProdQty = int(vResource[CR.begin()->IndexInProblemResourceArray].LeftCapacity/
				ProcessTime[CR.begin()->IndexInProblemResourceArray]);
				if( FeasibleProdQty < MaxProdQty)
					MaxProdQty = FeasibleProdQty;
		}
		return (MaxProdQty);
	}


	int cProduct::MaxFeasibleProdQtyBasedOnGivenResource(list<cResource>::iterator itor_cr){
		int MaxProdQty = MarketDemand;
		int FeasibleProdQty;
		if (ProcessTime[itor_cr->IndexInProblemResourceArray]>0){
			FeasibleProdQty = int(vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity/
				ProcessTime[itor_cr->IndexInProblemResourceArray]);
				if( FeasibleProdQty < MaxProdQty)
					MaxProdQty = FeasibleProdQty;
		}
		return (MaxProdQty);
	}

	int cProduct::MaxFeasibleProdQtyBasedOnAllBottlenecks(list<cResource> CR){
	int MaxProdQty = MarketDemand;
	int FeasibleProdQty;
	list<cResource>::iterator itor_cr;
	for(itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		if (ProcessTime[itor_cr->IndexInProblemResourceArray]>0){
			FeasibleProdQty = int(vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity/
			ProcessTime[itor_cr->IndexInProblemResourceArray]);
			if( FeasibleProdQty < MaxProdQty)
				MaxProdQty = FeasibleProdQty;
		}
	}
	return (MaxProdQty);
}

cProduct& cProduct::operator= (const cProduct& rhs){
	//int j, k;
	std::copy(rhs.Code, rhs.Code+CODE_SIZE, Code); 
	IndexInProblemProductArray = rhs.IndexInProblemProductArray;
	TotalRes = rhs.TotalRes;
	std::copy(rhs.ProcessTime.begin(), rhs.ProcessTime.end(), ProcessTime.begin());
	std::copy(rhs.vResource.begin(), rhs.vResource.end(), vResource.begin());
	
	MarketDemand = rhs.MarketDemand;
	ContributionMargin = rhs.ContributionMargin;
	ProdQty = rhs.ProdQty;
	Throughput = rhs.Throughput;
	R_Ratio = rhs.R_Ratio;
	R_Ratio_PBN = rhs.R_Ratio_PBN;
	return *this;
}

bool cProduct::operator < (const cProduct& rhs){
	if (R_Ratio < ContributionMargin / 0)// i.e. is defined
		return (R_Ratio>rhs.R_Ratio || (R_Ratio==rhs.R_Ratio && ContributionMargin>rhs.ContributionMargin));
	else return (ContributionMargin>rhs.ContributionMargin);
}

bool cProduct::operator == (const cProduct& rhs){
	return (Code == rhs.Code);
}

bool cProduct::operator != (const cProduct& rhs){
	return (Code != rhs.Code);
}

cProduct::~cProduct()
{
	//delete[] ProcessTime;
}