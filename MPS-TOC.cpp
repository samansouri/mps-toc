// MPS-TOC.cpp : main project file.
/*******************************************************************************
      Algorithms for Master Production Scheduling under Theory of Constraints
                        
				     designed and developed by:
                           Afshin Mansouri© 
			Brunel Business School, Brunel University, UK
			  	              Email: 
				    Afshin.Mansouri@brunel.ac.uk
			           samansouri@hotmail.com
*******************************************************************************/
#include "stdafx.h" // default header created by system
#include <stdio.h>
#include "cProblem.h"
#include "General.h"
#include "cMPS.h"

using namespace System;

//int main(array<System::String ^> ^args)
int main(int argc, char **argv)
{
   	randomize();
	char *DataSet;
	DataSet = argv[1]; 
	
	//create name of the output file
	std::string my_string(DataSet), OutString;
	string::iterator s_itor;
	OutString.clear(); //initializing
	for(s_itor = my_string.begin(); (s_itor != my_string.end() && *s_itor != '.') ; s_itor++){
		OutString.push_back(*s_itor);
	}
	cProblem CP = cProblem (DataSet);
	cProblem* pCP = &CP;

	pCP->PrintDataSetIntoOutputFileInTabularFormat(OutString.c_str());
	
	//************************************************************/
	// The algorithm of Fredendall and Lea (1997) -- STARTS HERE
	//           Source: IJPR, 35(6), 1535-1544
	//************************************************************/
	cout << "\nThe algorithm of Fredendall and Lea (1997) is being implemented..." << endl;
	cMPS MPS_FL97(pCP); // MPS based on Fredendall and Lea (1997)
	cMPS* pMPS_FL97 = &MPS_FL97;
	
	//Step 1: Identify the system's constraints
	    //1.a Calculate capacity shortages and
	    //1.b. Determine CR set (i.e. Critical Resources)

	pMPS_FL97->CalculateCapacityShortagesAndDetermine_CR_Set();
	//Step 2. Decide how to exploit the system's constraints

	if (pMPS_FL97->CR.empty()){
		string Input;
		cout << "This problem has no constraint therefore MPS algorithms under TOC are not applicable.";
		cout << "\nDemand to products can be fully satisfied.";
		cout << "\nPress Enter to exit...";
		getline(cin, Input);
		return 0;
	}

	//2.a. Assume BN1 (the first memeber of CR) is the system constraint
	//unsigned int ind_1, ind_2;
	
	//calculate R_Ratio
	pMPS_FL97->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(pMPS_FL97->CR.begin());
	std::sort(pMPS_FL97->vProductList.begin(), pMPS_FL97->vProductList.end());

	//2.b. Check if BN1 is the dominant bottleneck
	if (pMPS_FL97->CR.size()==1 || (pMPS_FL97->CR.size()>1 && pMPS_FL97->pCP->EveryProductUsesEveryResource())){
		//either (2.b.i) or (2.b.ii) holds 
		bool DominantConstraintIsBN1=true; // current rank is OK so no action is needed!
	}
	else{
		//(2.b.iii) holds; determine dominant bottleneck

		//2.b.iii.1. Rank products in decreasing order of Ri,BN1 (which has already been done in ProductList) 
		//           and determine feasible production quantities considering market demand and available capacities
		std::list<cResource>::iterator itor_cr;
		bool PrimaryBottleneckCorrectlyIdentified = false;
		pMPS_FL97->DevelopMPS_BasedOnFirstMemberOf_CR();

		//examine which critical resource has run out of capacity first
		list<cResource>::iterator itor_true_BN;
		itor_true_BN = pMPS_FL97->CR.begin(); //initialize true_BN
		float CapacityShortage = 0;
		for (itor_cr=pMPS_FL97->CR.begin(); itor_cr!=pMPS_FL97->CR.end(); itor_cr++){
			for (int i=0; i<pCP->ProdNum; i++){
				if(pMPS_FL97->vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity < CapacityShortage){
					CapacityShortage = pMPS_FL97->vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity;
					itor_true_BN = itor_cr;
					i=pCP->ProdNum;
				}
			}
		}
		if (itor_true_BN!=pMPS_FL97->CR.begin()){ // move this resource to the begining of CR list and increase rank of other resources
			cResource* pTempRes = new cResource;
			*pTempRes = *itor_true_BN;
			pMPS_FL97->CR.remove(*itor_true_BN);
			pMPS_FL97->CR.push_front(*pTempRes);
		}
		else{
			PrimaryBottleneckCorrectlyIdentified = true; //just for info!
		}
		
	}
		//2.c. to 2.f Schedule all the products requiring BN1 in descending order of R-ratio ...
		//            Calculate left time on bottleneck machines (members of CR)

	pMPS_FL97->DevelopMPS_BasedOnAllBottlenecks();

		//2.g. Identify candidate set of candidate products for trade-off (SetOfCandidateProducts_X) 
		pMPS_FL97->IdentifySet_X();

		//2.h. examine if trading off production quantities among members of set X can improve throughput
		pMPS_FL97->DoTradeoffAmongMembersOfSet_X();

		pMPS_FL97->PrintMPS_TableIntoFile (OutString.c_str(), "Fredendall and Lea (1997)");

		//*********************************************************/
		// The algorithm of Fredendall and Lea (1997)-- ENDS HERE
		//*********************************************************/


		//************************************************************/
		// The algorithm of Sobreiro and Nagano (2012)-- STARTS HERE 
		//           Source: IJPR, 50(20), 5936–5948
		//************************************************************/
		cout << "\nThe algorithm of Sobreiro and Nagano (2012) is being implemented..." << endl;
		cMPS MPS_SN12(pCP); // MPS based on the algorithm of Sobreiro and Nagano (2012)
		cMPS* pMPS_SN12 = &MPS_SN12;
		int i;
		bool MPS_HasChanged_ComparedToS2;

		// Step 1: Identify the system’s constraint(s)
		// 1.a. Calculate the difference between resourcess capacity and their demands (dj)
		// 1.b. Determine CR={BN1,BN2 . . . ,BNq} considering that dq<=0 and d1<=d2<=...<=dq

		pMPS_SN12->CalculateCapacityShortagesAndDetermine_CR_Set();

		if (pMPS_SN12->CR.empty()){ // not really needed as this condition has already been checked for the algorithm of Fredendall and Lea (1997)
			string Input;
			cout << "This problem has no constraint therefore MPS algorithms under TOC are not applicable.";
			cout << "\nDemand to products can be fully satisfied.";
			cout << "\nPress Enter to exit...";
			getline(cin, Input);
			return 0;
		}

		// Step 2: Decide how to exploit the constraint(s)
		// 2.a. Assume that BN1 is the dominant bottleneck. Calculate Ri (or R_Ratio) for each product. 
		// Sort the products in non-ascending order of Ri.
		pMPS_SN12->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(pMPS_SN12->CR.begin());
		std::sort(pMPS_SN12->vProductList.begin(), pMPS_SN12->vProductList.end());

		// 2.b. Check if BN1 is the dominant bottleneck:
		// 2.b.i. Apply algorithm B-Greedy considering Ri obtained in Step 2a and the current production schedule.
		list<cResource>::iterator itor_cr, itor_ActiveBN, itor_ActiveBN_S1, itor_ActiveBN_S5;
		itor_cr = pMPS_SN12->CR.begin();
		pMPS_SN12->DevelopMPS_BasedOn_B_Greedy_AlgorithmAndGivenResource(itor_cr);  

		// 2.b.ii. Considering the product mix obtained in Step 2.b.i, check if there is a dj<0. If so, assume
        // next BN in CR as BN1, sort the products in view of BN1 and go to Step 2a. If BN1 is the same as
        // BNq, and dj is still less than zero, go to Step 2.b.iii. Otherwise, if all dj are higher than zero go to Step 2.c.
		while (pMPS_SN12->IsCurrentMPSFeasible()==false && itor_cr!=pMPS_SN12->CR.end()){//i.e. there is at least one dj<0
			itor_cr++;
			if (itor_cr != pMPS_SN12->CR.end()){
				pMPS_SN12->DevelopMPS_BasedOn_B_Greedy_AlgorithmAndGivenResource(itor_cr);
				itor_ActiveBN = itor_cr;

			}
		}

		if (pMPS_SN12->IsCurrentMPSFeasible()==false){
			// 2.b.iii. Use the B-Greedy-M algorithm considering the production schedule in view of BNq to obtain 
			// the product mix. This product mix should have all dj values greater than or equal to zero.
			itor_cr=pMPS_SN12->CR.end();
			itor_cr--;
			pMPS_SN12->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_cr);
			pMPS_SN12->DevelopMPS_BasedOn_B_Greedy_M_Algorithm(); //NOTE: calculation of R_Ratio is essential for this function
			itor_ActiveBN = itor_cr;
		}
		else{ //pMPS_SN12->IsCurrentMPSFeasible()==true 
			itor_ActiveBN = itor_cr;
		}

		// 2.c. Consider the system’s throughput or solution obtained to be S1.
		cMPS S1 (pCP); // S1 is the current MPS found by the algorithm of Sobreiro and Nagano (2012)
		cMPS* pS1 = &S1;
		*pS1 = *pMPS_SN12;
		itor_ActiveBN_S1 = itor_ActiveBN;

		// 2.d. Check the possibility to improve the obtained solution
		// 2.d.i. 
		/*
		QUOTE FROM THE PAPER, p.5942: "In the production schedule that provided S1, identify the last product with the best Ri and that the
		production is the same as that which the market demands, in other words, xi=Di. Reduce the market
		demand of this product, one unit at a time, and use B-Greedy-M considering the sort of products in
		view of Ri, with the objective of identifying a new product mix. Perform this procedure until the
		market demand of the product i is the same as Di=Di*0.2 or if it is not possible to increase the
		quantities to be produced for the other products. Classify the products with the same Ri in descending
		order of CMi. Consider as S2 the system’s best throughput obtained in this procedure."

		BASED ON ABOVE WE ASSUME: In S1 find the product with the highest Ri (with respect to the active bottleneck) from among those for which 
		market demand is fully satisfied (i.e. xi=Di). Reduce its market demand (Di) one unit at a time, 
		and use B-Greedy-M considering to develop MPS (with reference to the active bottleneck) until Di=0.80*Di (i.e its demand is reduced by 
		20%) or if it is not possible to increase product quantities of other products. Sort the products with the same Ri 
		in descending order of CMi. Consider as S2 the system’s best throughput obtained in this procedure.
		*/
		cMPS S2 (pCP); // S1 is the current MPS found by the algorithm of Sobreiro and Nagano (2012)
		cMPS* pS2 = &S2;
		*pS2 = *pS1;
		// IT IS ASSUMED THAT THE PRODUCTS IN S2 ARE ALREADY SORTED ON R_BN 
		// identify the last product in S2 that satisfy xi=Di
		// In case none of the products satisfy the above condition, the algorithm has nowhere to go!
		// To avoid the deadlock, I CHANGE this condition to 
		// Identify the last product in S2 with MIN (Di-xi). In this way, there will be always a way forward
		pS2->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_ActiveBN_S1);
		std::sort(pS2->vProductList.begin(), pS2->vProductList.end());
		//pS2->LocalSearchUsing_B_Greedy_M_Algorithm (itor_ActiveBN, 0.20, pS2->IndexOfTheProductWithMaximuR_RatioAndMinimumUnsatisfiedDemand());
		pS2->LocalSearchUsing_B_Greedy_M_Algorithm (0.20, pS2->IndexOfTheProductWithMaximuR_RatioAndMinimumUnsatisfiedDemand());
		
		// 2.d.ii. 
		/*
		FROM THE PAPER, p.5943 (with slight corrections): Calculate RAi for all products as follows: 
		RAi=\underset{g=1,...,q}Sigma(CMi=ti,BNg).
		Schedule the products in non-ascending order based on RAi. If two products have the same Ri, schedule 
		the product with the higher CMi first. With this schedule, use the B-Greedy-M algorithm to identify a 
		new initial product mix. If this new schedule is the same as the schedule developed in Step 2.d.i (i.e. in S2)
		go to Step 2.d.iii. Otherwise, identify the last product for which xi=Di. Reduce the market demand of this product
		one unit at a time, and schedule the products considering this new market demand until this market demand is 
		Di-Di*0.1 or until it is not possible to increase the quantities to be produced of the other products. Comparing 
		the throughput obtained between the new product mix and the product mix that resulted from the reduction of the market
		demand of product xi, assume the best throughput obtained to be S3.
		*/
		cMPS S3 (pCP); // S1 is the current MPS found by the algorithm of Sobreiro and Nagano (2012)
		cMPS* pS3 = &S3;
		*pS3 = *pS2;

		//cacculate RAi's and let Ri=RAi then call B_Greedy_M with CR.end() as reference BN 
		// so that products will be sorted based on RAi
		std::vector<float> vRA;
		vRA.resize(pCP->ProdNum);
		for (i=0; i<vRA.size(); i++)
			vRA[i]=0; //initialize RA[i]

		//calculate RAi's
		for (itor_cr=pS3->CR.begin(); itor_cr!=pS3->CR.end(); itor_cr++){
			pS3->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_cr);
			for (i=0; i<pCP->ProdNum; i++)
				vRA[i] += pS3->vProductList[i].R_Ratio; 
		}

		for (i=0; i<vRA.size(); i++)
			pS3->vProductList[i].R_Ratio = vRA[i];

		//sort products
		std::sort(pS3->vProductList.begin(), pS3->vProductList.end());
		pS3->DevelopMPS_BasedOn_B_Greedy_M_Algorithm(); 

		//check if the sequence of products has changed
		MPS_HasChanged_ComparedToS2 = false;
		for (i=0; i<pCP->ProdNum; i++){
			if (pS3->vProductList[i]!=pS2->vProductList[i] || 
				(pS3->vProductList[i]==pS2->vProductList[i] && pS3->vProductList[i].ProdQty!=pS2->vProductList[i].ProdQty)){
				MPS_HasChanged_ComparedToS2 = true;
				break;
			}
		}

		if (MPS_HasChanged_ComparedToS2){
			//NOTE that the products are already sorted according to RA's
			pS3->LocalSearchUsing_B_Greedy_M_Algorithm (0.10, pS3->IndexOfTheLastProductWithMinimumUnsatisfiedDemand()); //NOT SURE IF ACTIVE BN SHOULD BE CHANGED??
		}

		// 2.d.iii.
		/* 
		Considering the product mix obtained in solution S1, identify the product that exhibits the best Ri. Reduce the 
		market demand of this product, one unit at a time, and schedule the products based on this new market demand, 
		using B-Greedy-M. Perform this procedure until the market demand of this product is the same as Di-Di*0.1 or 
		until it is not possible to increase the quantities to be produced of the other products. Assume the higher 
		throughput obtained to be S4.
		*/
		cMPS S4 (pCP); // S1 is the current MPS found by the algorithm of Sobreiro and Nagano (2012)
		cMPS* pS4 = &S4;
		*pS4 = *pS1;

		pS4->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_ActiveBN_S1);
		std::sort(pS4->vProductList.begin(), pS4->vProductList.end());
		pS4->LocalSearchUsing_B_Greedy_M_Algorithm (0.10, pS4->IndexOfTheProductWithMaximumR_Ratio());

		// 2.d.iv.
		/*
		If the dominant bottleneck is BN1, assume S5 is the same as zero and go to Step 2e. Otherwise, schedule 
		the products based on the first bottleneck of set CR. If two products to be scheduled have the same Ri, 
		schedule the product with the highest CMi first. Use the B-Greedy-M algorithm in order to identify a 
		product mix. Considering this product mix identifies the last product that presents xi=Di, reduce one 
		unit at a time the market demand of this product and schedule the other products. Perform this procedure 
		until the market demand of this product is the same as Di-Di*0.1 or until it is not possible to increase 
		the quantities to be produced of the other products. Comparing the results obtained between the product mix 
		obtained based on BN1 and the product mix resulting from the reduction of the market demand of product xi, 
		assume the higher throughput obtained to be S5.
		*/
		cMPS S5 (pCP); // S1 is the current MPS found by the algorithm of Sobreiro and Nagano (2012)
		cMPS* pS5 = &S5;
		*pS5 = *pS4;
		pS5->IdentifyPrimaryBottleneck();
		if (pS5->itor_PrimaryBN->Code == pS5->CR.begin()->Code){
			for (i=0; i<pCP->ProdNum; i++)
				pS5->vProductList[i].ProdQty = 0;

			pS5->CalculateThroughputs();
		}
		else{	
			pS5->Calculate_R_Ratio_BasedOnGivenMemberOf_CR(pS5->CR.begin());
			std::sort(pS5->vProductList.begin(), pS5->vProductList.end());
			pS5->LocalSearchUsing_B_Greedy_M_Algorithm (0.10, pS5->IndexOfTheLastProductWithMinimumUnsatisfiedDemand()); 
			
		}

		// 2.e. Assume as the heuristics’ final solution the product mix that exhibits 
		// the highest throughput of the set {S1, S2, S3, S4, S5}.
		*pMPS_SN12 = *pS1;
		float maxThroughput = pS1->TotalThroughput;
		if (pS2->TotalThroughput > maxThroughput){
			*pMPS_SN12 = *pS2;
			maxThroughput = pS2->TotalThroughput; 
		}

		if (pS3->TotalThroughput > maxThroughput){
			*pMPS_SN12 = *pS3;
			maxThroughput = pS3->TotalThroughput; 
		}

		if (pS4->TotalThroughput > maxThroughput){
			*pMPS_SN12 = *pS4;
			maxThroughput = pS4->TotalThroughput; 
		}

		if (pS5->TotalThroughput > maxThroughput){
			*pMPS_SN12 = *pS5;
			maxThroughput = pS5->TotalThroughput; 
		}
			
		pMPS_SN12->PrintMPS_TableIntoFile (OutString.c_str(), "Sobreiro and Nagano (2012)");

		//************************************************************/
		// The algorithm of Sobreiro and Nagano (2012)-- ENDS HERE 
		//************************************************************/

	

		//*************************************/
		// COLOMAPS Algorithm -- STARTS HERE  
		//*************************************/
		cout << "\nOur algorithm is being implemented..." << endl;
		cMPS MPS_COLOMAPS(pCP); // MPS based on COLOMAPS Algorithm
		cMPS* pMPS_COLOMAPS = &MPS_COLOMAPS;

		//Phase 1: Stages a-d to calculate Gama ratios have already been taken in Excel the results are imported from the input file 
		//Step 1.e. 
		pMPS_COLOMAPS->CalculateCapacityShortagesAndDetermine_CR_Set();
		pMPS_COLOMAPS->IdentifyAndRemoveDominatedBottlenecksFrom_CR();

		//add/move critical resource (CL) to the beginning of CR
		pMPS_COLOMAPS->MoveResource_CL_ToTheBeginningOf_CR();

		//Phase 2. MPS design

		// Step 2.f. 
		//first identify primary bottleneck which is needed for trade-off analysis
		pMPS_COLOMAPS->DevelopMPS_BasedOnFirstMemberOf_CR();

		//identify primary bottleneck for calcualtion of R_Ratio
		pMPS_COLOMAPS->Calculate_R_Ratio_PBN_AndSetPrimaryBN();
		
		pMPS_COLOMAPS->DevelopMPS_BasedOnAllBottlenecks();
		
		// Step 2.h Do tradeoff among improving pairs
		pMPS_COLOMAPS->DoTradeoffAmongImprovingPairs_gr();

		pMPS_COLOMAPS->PrintMPS_TableIntoFile (OutString.c_str(), "COLOMAPS Algorithm");

		//*******************************/
		// COLOMAPS Algorithm -- ENDS HERE  
		//*******************************/
	
	

cout << "\nPress ant key to exit...";

return 0;
}






