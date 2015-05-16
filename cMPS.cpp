#include "stdafx.h"
#include "cMPS.h"
#include <stdio.h>
#include <stdlib.h>
#include "General.h"

cMPS::cMPS(cProblem* pCurrentProblem){
	pCP = pCurrentProblem;
	vProductList = pCurrentProblem->vProduct;
}


int cMPS::MaxFeasibleProdQty(cProduct Prod, list<cResource> CR){
	std::list<cResource>::iterator itor_r;
	int MaxProdQty = Prod.MarketDemand;
	int FeasibleQty;
	//int j;
	for(itor_r=CR.begin(); itor_r!=CR.end(); itor_r++){

		FeasibleQty = int(itor_r->LeftCapacity/Prod.ProcessTime[itor_r->IndexInProblemResourceArray]);
		if( FeasibleQty < MaxProdQty)
			MaxProdQty = FeasibleQty;
	}
	return (MaxProdQty);
}

void cMPS::InitializeCapacitiesOf_CR_Set(){
	int i;
	list<cResource>::iterator itor_cr;
	for (i=0; i!=vProductList.size(); i++){
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
			vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity = 0;
			vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity = 
				vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].Capacity;
				
		}
	}
}

void cMPS::DevelopMPS_BasedOnFirstMemberOf_CR(){
	int ind_1, ind_2, FeasibleProdQty;
	list<cResource>::iterator itor_cr;
	//initialization
	Calculate_R_Ratio_BasedOnGivenMemberOf_CR(CR.begin());
	std::sort(vProductList.begin(), vProductList.end());
	InitializeCapacitiesOf_CR_Set();
		for (ind_1=0; ind_1!=vProductList.size(); ind_1++){
			FeasibleProdQty = vProductList[ind_1].MaxFeasibleProdQtyBasedOnPrimaryBottleneck(CR);
			vProductList[ind_1].ProdQty = FeasibleProdQty;
			for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
				float Used_Cap = vProductList[ind_1].ProdQty*vProductList[ind_1].ProcessTime[itor_cr->IndexInProblemResourceArray];
				vProductList[ind_1].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity = Used_Cap;
				
				//update used and left capacities
				for (ind_2=ind_1; ind_2<vProductList.size(); ind_2++){
					vProductList[ind_2].vResource[itor_cr->IndexInProblemResourceArray].UpdateLeftCapacity(Used_Cap);
				}
			}
		}
		//calculate Throughputs
		CalculateThroughputs();
}


void cMPS::DevelopMPS_BasedOn_B_Greedy_AlgorithmAndGivenResource(list<cResource>::iterator itor_GivenCR){
	int ind_1, ind_2, FeasibleProdQty;
	Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_GivenCR);
	std::sort(vProductList.begin(), vProductList.end());
	InitializeCapacitiesOf_CR_Set();
		for (ind_1=0; ind_1!=vProductList.size(); ind_1++){
			FeasibleProdQty = vProductList[ind_1].MaxFeasibleProdQtyBasedOnGivenResource(itor_GivenCR);
			vProductList[ind_1].ProdQty = FeasibleProdQty;
				float Used_Cap = vProductList[ind_1].ProdQty*vProductList[ind_1].ProcessTime[itor_GivenCR->IndexInProblemResourceArray];
				vProductList[ind_1].vResource[itor_GivenCR->IndexInProblemResourceArray].UsedCapacity = Used_Cap;
				
				//update used and left capacities
				for (ind_2=ind_1; ind_2<vProductList.size(); ind_2++){
					vProductList[ind_2].vResource[itor_GivenCR->IndexInProblemResourceArray].UpdateLeftCapacity(Used_Cap);
				}
			
		}
		//calculate Throughputs
		CalculateThroughputs(); //which might be infeasible as not all CRs are taken into account 

		//calculate actual left capacities on CRs based on current MPS
		CalculateLeftCapacitiesOnCRsBasedOnCurrentMPS();
}

void cMPS::DevelopMPS_BasedOn_B_Greedy_M_Algorithm(){ //NOTE: This function assumes the order of jobs has already been defined
	// The following is an adaptation of DevelopMPS_BasedOnAllBottlenecks()function in which, R_Ratios are already defined. 
	// MODIFY if it turned out NOT TO BE the case!!!
	int ind_1, ind_2, FeasibleProdQty;
	list<cResource>::iterator itor_cr;
	std::sort(vProductList.begin(), vProductList.end());
	InitializeCapacitiesOf_CR_Set();
		for (ind_1=0; ind_1!=vProductList.size(); ind_1++){
			FeasibleProdQty = vProductList[ind_1].MaxFeasibleProdQtyBasedOnAllBottlenecks(CR);
			vProductList[ind_1].ProdQty = FeasibleProdQty;
			for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
				float Used_Cap = vProductList[ind_1].ProdQty*vProductList[ind_1].ProcessTime[itor_cr->IndexInProblemResourceArray];
				vProductList[ind_1].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity = Used_Cap;
				
				//update used and left capacities
				for (ind_2=ind_1; ind_2<vProductList.size(); ind_2++){
					vProductList[ind_2].vResource[itor_cr->IndexInProblemResourceArray].UpdateLeftCapacity(Used_Cap);
				}
			}
		}
		CalculateThroughputs();
}

int cMPS::IndexOfTheLastProductWithMinimumUnsatisfiedDemand(){
	int selectedProduct = -1, minUnsatisfiedDemand = 999999999, i;
	for (i=0; i<vProductList.size(); i++){
		if (vProductList[i].ProdQty > 0 && (vProductList[i].MarketDemand - vProductList[i].ProdQty) <= minUnsatisfiedDemand){
			minUnsatisfiedDemand = vProductList[i].MarketDemand - vProductList[i].ProdQty;
			selectedProduct = i;
		}
	}
	if (selectedProduct == -1)
		cout << "\nError in step 2.d. of Sobreiro and Nagano (2012)" << endl;
	
	return selectedProduct;
}

int cMPS::IndexOfTheProductWithMaximuR_RatioAndMinimumUnsatisfiedDemand(){ //assuming that the productss are sorted in decreasing order of R_Ratio
	int selectedProduct = -1, minUnsatisfiedDemand = 999999999, i;
	for (i=0; i<vProductList.size(); i++){
		if (vProductList[i].ProdQty > 0 && (vProductList[i].MarketDemand - vProductList[i].ProdQty) < minUnsatisfiedDemand){
			//The tiny difference between this and IndexOfTheProductWithMinimumUnsatisfiedDemand is in < relation.
			//In the other function, it is <=
			minUnsatisfiedDemand = vProductList[i].MarketDemand - vProductList[i].ProdQty;
			selectedProduct = i;
		}
	}
	if (selectedProduct == -1)
		cout << "\nError in step 2.d. of Sobreiro and Nagano (2012)" << endl;

	return selectedProduct;
}

int cMPS::IndexOfTheProductWithMaximumR_Ratio(){
	int selectedProduct=-1, maxR_Ratio=0, i;
	for (i=0; i<vProductList.size(); i++){
		if (vProductList[i].R_Ratio >= maxR_Ratio){
			maxR_Ratio = vProductList[i].R_Ratio;
			selectedProduct = i;
		}
	}
	if (selectedProduct == -1)
		cout << "\nError in step 2.d. of Sobreiro and Nagano (2012)" << endl;
	
	return selectedProduct;
}

//void cMPS::LocalSearchUsing_B_Greedy_M_Algorithm (list<cResource>::iterator itor_ActiveBN, float DemandReductionFactor, int selectedProduct){
void cMPS::LocalSearchUsing_B_Greedy_M_Algorithm (float DemandReductionFactor, int selectedProduct){
	int minUnsatisfiedDemand = 999999999;
	int i, selectedProduct_LowerBoundDemand, OrigialMarketDemandOfSelectedProduct;
	bool ThroughputIsImproving;
	float Throughput_Before, Throughput_After; 

	//NOTE: removing 'int' from the following formula might cause rounding error in some cases leading to '1' unit difference)
	selectedProduct_LowerBoundDemand = vProductList[selectedProduct].MarketDemand - int(DemandReductionFactor*vProductList[selectedProduct].MarketDemand);
	ThroughputIsImproving = false;
	OrigialMarketDemandOfSelectedProduct = vProductList[selectedProduct].MarketDemand;
	//Calculate_R_Ratio_BasedOnGivenMemberOf_CR(itor_ActiveBN);
	do{
		Throughput_Before = TotalThroughput;
		vProductList[selectedProduct].MarketDemand--;
		DevelopMPS_BasedOn_B_Greedy_M_Algorithm(); // NOTE: calculation of R_Ratio is essential for this function 
		Throughput_After = TotalThroughput;
		if (Throughput_After >= Throughput_Before)
			ThroughputIsImproving = true;
		else{ //i.e. throughput has decreased
			ThroughputIsImproving = false;
			vProductList[selectedProduct].MarketDemand++; //restore to previous status
			DevelopMPS_BasedOn_B_Greedy_M_Algorithm();
		}
	}while (vProductList[selectedProduct].MarketDemand > selectedProduct_LowerBoundDemand && ThroughputIsImproving);
}	

void cMPS::CalculateLeftCapacitiesOnCRsBasedOnCurrentMPS(){
	int ind_1, ind_2;
	float Used_Cap;
	list<cResource>::iterator itor_cr;
	//initialization
	InitializeCapacitiesOf_CR_Set();
	for (ind_1=0; ind_1!=vProductList.size(); ind_1++){
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
			Used_Cap = vProductList[ind_1].ProdQty*vProductList[ind_1].ProcessTime[itor_cr->IndexInProblemResourceArray];
			vProductList[ind_1].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity = Used_Cap;
			//update used and left capacities
			for (ind_2=ind_1; ind_2<vProductList.size(); ind_2++){
				vProductList[ind_2].vResource[itor_cr->IndexInProblemResourceArray].UpdateLeftCapacity(Used_Cap);
			}
		}
	}
}

void cMPS::DevelopMPS_BasedOnAllBottlenecks(){
	int ind_1, ind_2, FeasibleProdQty;
	list<cResource>::iterator itor_cr;
	Calculate_R_Ratio_BasedOnGivenMemberOf_CR(CR.begin());
	std::sort(vProductList.begin(), vProductList.end());
	InitializeCapacitiesOf_CR_Set();
		for (ind_1=0; ind_1!=vProductList.size(); ind_1++){
			FeasibleProdQty = vProductList[ind_1].MaxFeasibleProdQtyBasedOnAllBottlenecks(CR);
			vProductList[ind_1].ProdQty = FeasibleProdQty;
			for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
				float Used_Cap = vProductList[ind_1].ProdQty*vProductList[ind_1].ProcessTime[itor_cr->IndexInProblemResourceArray];
				vProductList[ind_1].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity = Used_Cap;
				
				//update used and left capacities
				for (ind_2=ind_1; ind_2<vProductList.size(); ind_2++){
					vProductList[ind_2].vResource[itor_cr->IndexInProblemResourceArray].UpdateLeftCapacity(Used_Cap);
				}
			}
		}

		//calculate Throughputs
		CalculateThroughputs();
		
}

void cMPS::CalculateThroughputs(){
	int i;
	TotalThroughput = 0;
	if (IsCurrentMPSFeasible()){
		for (i=0; i<vProductList.size(); i++)
			vProductList[i].Throughput = vProductList[i].ProdQty * vProductList[i].ContributionMargin;
	}
	else{
		for (i=0; i<vProductList.size(); i++)
			vProductList[i].Throughput = 0;
	}

	for (i=0; i<vProductList.size(); i++)
		TotalThroughput += vProductList[i].Throughput;
}


void cMPS::CalculateCapacityShortagesAndDetermine_CR_Set(){
	//Calculate Capacity Shortages
	int i, j;
	std::list<cResource>::iterator itor_r, itor_cr;
	for (j=0; j<pCP->ResNum; j++){
		pCP->vResource[j].CapacityDifference = pCP->vResource[j].Capacity; // initializing
		
		for (i=0; i<pCP->ProdNum; i++)
			pCP->vResource[j].CapacityDifference -= pCP->vProduct[i].ProcessTime[j] * pCP->vProduct[i].MarketDemand;
	}

	//Determine CR Set
	CR.clear(); // initializing
	for (j=0; j<pCP->ResNum; j++){
		if(pCP->vResource[j].CapacityDifference <= 0){
			CR.push_back(pCP->vResource[j]);
		}
	}
	CR.sort();
}

void cMPS::Calculate_R_Ratio_BasedOnGivenMemberOf_CR(list<cResource>::iterator itor_cr){
	int i;
	for (i=0; i<vProductList.size(); i++)
		if(vProductList[i].ProcessTime[itor_cr->IndexInProblemResourceArray]>0)
			vProductList[i].R_Ratio = vProductList[i].ContributionMargin/vProductList[i].ProcessTime[itor_cr->IndexInProblemResourceArray];
		else
			vProductList[i].R_Ratio = 0;
}

bool cMPS::IsCurrentMPSFeasible(){
	int i;
	list<cResource>::iterator itor_cr;
	bool Condition = true;
	for (i=0; i<vProductList.size(); i++){
		if(vProductList[i].ProdQty < 0){
			Condition = false;
			break;
		}
		
	}

	if (Condition==true){
		for(i=0; i<vProductList.size(); i++){
			for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
				if (vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity<0){
					Condition = false;
					i=vProductList.size();
					break;
				}
			}
		}
	}

	return (Condition);

}


void cMPS::Display_MPS_Table(){
	list<cResource>::iterator itor_cr;
	int i, j;
	cout << "\n\n" << "Prod\t" << "R_Ratio\t" << "P_Qty\t";
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
		cout << "R-" << TrimedString(itor_cr->Code) << "_U\t" << "R-" << TrimedString(itor_cr->Code) << "_L\t";  

	cout << "Troughput";
	for (i=0; i<vProductList.size(); i++){
		cout << "\n" << TrimedString(vProductList[i].Code) << "\t";
		printf("%5.2f", vProductList[i].R_Ratio);

		printf("\t%5d\t", vProductList[i].ProdQty);
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
			printf("%6.1f\t%6.1f\t", vProductList[i].ProdQty*vProductList[i].ProcessTime[itor_cr->IndexInProblemResourceArray], 
			   vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity);

		printf("%9.1f", vProductList[i].Throughput);
	}
	cout << "\n\n";
	
	cout << "Total Throughput:";
	//for (j=0; j<CR.size(); j++)
	//	cout << "\t\t\t\t";
	printf ("%8.1f", TotalThroughput);


	if (IsCurrentMPSFeasible())
		cout <<"\nCurrent MPS is feasible";
	else
		cout <<"\nCurrent MPS is NOT feasible";

}


void cMPS::Display_MPS_Table_WitoutUsedCapacities(){
	list<cResource>::iterator itor_cr;
	int i, j;
	cout << "\n\n" << "Prod\t" << "R_Ratio\t" << "P_Qty\t";
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
		cout << "R-" << TrimedString(itor_cr->Code) << "_L\t";  

	cout << "Troughput";
	for (i=0; i<vProductList.size(); i++){
		cout << "\n" << TrimedString(vProductList[i].Code) << "\t";
		printf("%5.2f", vProductList[i].R_Ratio);

		printf("\t%5d\t", vProductList[i].ProdQty);
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
			printf("%6.1f\t", vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity);

		printf("%9.1f", vProductList[i].Throughput);
	}
	cout << "\n\n";
	

	cout << "Total Throughput:";
	printf ("%8.1f", TotalThroughput);


	if (IsCurrentMPSFeasible())
		cout <<"\nCurrent MPS is feasible";
	else
		cout <<"\nCurrent MPS is NOT feasible";

}


void cMPS::Display_MPS_Table_WitoutUsedCapacities_With_R_Ratio_PBN(){
	list<cResource>::iterator itor_cr;
	int i, j;
	cout << "\n\nProd\tR_Ratio\tR_R_PBN\tP_Qty\t";
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
		cout << "R-" << TrimedString(itor_cr->Code) << "_L\t";  

	cout << "Troughput";
	for (i=0; i<vProductList.size(); i++){
		cout << "\n" << TrimedString(vProductList[i].Code) << "\t";
		printf("%5.2f\t%5.2f", vProductList[i].R_Ratio, vProductList[i].R_Ratio_PBN);

		printf("\t%5d\t", vProductList[i].ProdQty);
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
			printf("%6.1f\t", vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity);

		printf("%9.1f", vProductList[i].Throughput);
	}
	cout << "\n\n";
	

	cout << "Total Throughput:";
	printf ("%8.1f", TotalThroughput);


	if (IsCurrentMPSFeasible())
		cout <<"\nCurrent MPS is feasible";
	else
		cout <<"\nCurrent MPS is NOT feasible";


}

void cMPS::IdentifySet_X(){
	int i, k;
	list<cResource>::iterator itor_cr;

	//initialize set of candidate products X
	if (!Set_X.empty())
		Set_X.erase(Set_X.begin(), Set_X.end());

	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		for (i=0; i<vProductList.size()-1; i++){
			if(vProductList[i+1].ProcessTime[itor_cr->IndexInProblemResourceArray]>0){
				if ((vProductList[i+1].R_Ratio*
					(itor_cr->LeftCapacity + vProductList[i].ProcessTime[itor_cr->IndexInProblemResourceArray])/
					vProductList[i].ContributionMargin) >= 1){
						if (vProductList[i].ProdQty < vProductList[i].MarketDemand ||
							vProductList[i+1].ProdQty < vProductList[i+1].MarketDemand){
								for (k=i; k<vProductList.size(); k++)
									Set_X.push_back(k);

								i=vProductList.size()-1;
								while (itor_cr!=CR.end()){
									itor_cr++;
								}
								itor_cr--;
							
						}
				}
			}
		}
	}
}

void cMPS::DoTradeoffAmongMembersOfSet_X(){
	int i, cr, k;
	float Gain;
	list<cResource>::iterator itor_cr;
	InitializeAvailableCapacityOn_CR();
	bool ThroughputHasImproved;

	do{
		ThroughputHasImproved = false;

		if (!Set_X.empty()){
			for (i=0; i<(Set_X.size()-1); i++){
				itor_cr = CR.begin();
				SetAvailableCapacityOn_CR_ToGivenProduct(Set_X[i]);
				Gain = 0;
				for (k=i+1; k<Set_X.size(); k++){
					while(vProductList[Set_X[k]].ProdQty<vProductList[Set_X[k]].MarketDemand && 
						CR_HasSufficientCapacityForAnExtraUnitProduction(Set_X[k])){
							Gain += vProductList[Set_X[k]].ContributionMargin;
							DecreaseAvailableCapacityOn_CR(Set_X[k], +1);
					}
				}
				if (Gain>vProductList[Set_X[i]].ContributionMargin && vProductList[Set_X[i]].ProdQty>0){
					ThroughputHasImproved = true;
					//do the above tradeoff and update capacities
					SetAvailableCapacityOn_CR_ToGivenProduct(Set_X[i]);
					vProductList[Set_X[i]].ProdQty--;
					UpdateCapacitiesByDeltaCapacity (Set_X[i], -1);

					for (k=i+1; k<Set_X.size(); k++){
						while(vProductList[Set_X[k]].ProdQty<vProductList[Set_X[k]].MarketDemand && 
							CR_HasSufficientCapacityForAnExtraUnitProduction(Set_X[k])){
								vProductList[Set_X[k]].ProdQty++;
								UpdateCapacitiesByDeltaCapacity (Set_X[k], +1);
								DecreaseAvailableCapacityOn_CR(Set_X[k], +1);
		
						}
					}
				}
			}//end of (for i) loop
		}//end of if (!Set_X.empty()) condition
		CalculateThroughputs();
	}
	while (ThroughputHasImproved);
}


void cMPS::DoTradeoffAmongImprovingPairs_gr(){ //improving pairs of g (Giver) and r (Receivers)

	list<cResource>::iterator itor_cr;
	int g, r, Giver, Receiver;
	float TempDelta_gr;
	bool Prod_QtyOfReceiverHasIncreased;
	bool MPS_CannotBeImprovedFurther;
	bool ProdQtyOfGiverWasDecreased;
	InitializeAvailableCapacityOn_CR();

	MPS_CannotBeImprovedFurther = false;

	while (!MPS_CannotBeImprovedFurther){
		//identify the list of improving tradeoffs
		UpdateImprovingTradeoffs();

		if (ImprovingTradeoffs.size()>0){ //do the above tradeoff and update capacities
			list<cTradeoff>::iterator itor_tr;
			itor_tr = ImprovingTradeoffs.begin();
			Prod_QtyOfReceiverHasIncreased = false;

			while (!Prod_QtyOfReceiverHasIncreased && itor_tr!=ImprovingTradeoffs.end()){
				Giver = itor_tr->Giver_g;
				Receiver = itor_tr->Receiver_r;

				Prod_QtyOfReceiverHasIncreased = false;
				MPS_CannotBeImprovedFurther = true;
		
				SetAvailableCapacityOn_CR_ToGivenProduct(Giver);
				if (vProductList[Giver].ProdQty > 0){
					vProductList[Giver].ProdQty--;
					UpdateCapacitiesByDeltaCapacity (Giver, -1);
					ProdQtyOfGiverWasDecreased = true;
				}
				else{
					ProdQtyOfGiverWasDecreased = false;
				}
		
				while(ProdQtyOfGiverWasDecreased==true &&
					  vProductList[Receiver].ProdQty<vProductList[Receiver].MarketDemand && 
					  CR_HasSufficientCapacityForAnExtraUnitProduction(Receiver)){
						vProductList[Receiver].ProdQty++;
						UpdateCapacitiesByDeltaCapacity (Receiver, +1);
						DecreaseAvailableCapacityOn_CR(Receiver, +1);
						Prod_QtyOfReceiverHasIncreased = true;
						MPS_CannotBeImprovedFurther = false;

				}
				if (Prod_QtyOfReceiverHasIncreased==false && ProdQtyOfGiverWasDecreased==true){ //restore deducted production quantity of the Giver
					vProductList[Giver].ProdQty++;
					UpdateCapacitiesByDeltaCapacity (Giver, +1);
				}
				itor_tr++;

			}
			UpdateImprovingTradeoffs();
		}
		else{
			MPS_CannotBeImprovedFurther = true;
		}
	}
	CalculateThroughputs();
}






bool cMPS::CR_HasSufficientCapacityForAnExtraUnitProduction(int Prod_i){
	list<cResource>::iterator itor_cr;
	int cr;
	bool Condition = true;
	cr=0;
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		if (AvailableCapacityOn_CR[cr] < vProductList[Prod_i].ProcessTime[itor_cr->IndexInProblemResourceArray]){
			Condition = false;
			break;
		}
		cr++;
	}
	return (Condition);
}


bool cMPS::LeftCapacityOnCriticalResourcesIsSufficientForAnExtraUnitProduction(int Prod_Index){
	list<cResource>::iterator itor_cr;
	bool Condition = true;
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		if (vProductList[Prod_Index].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity <
			vProductList[Prod_Index].ProcessTime[itor_cr->IndexInProblemResourceArray]){
			Condition = false;
			break;
		}
	}
	return (Condition);
}

void cMPS::UpdateCapacitiesByDeltaCapacity(int ProductIndex_Onward, int Delta_ProdQty){
	// negative Delta_ProdQty means capacity is released whereas positive Delta_ProdQty implies capacity is used 
	int i;
	list<cResource>::iterator itor_cr;
	for (i=ProductIndex_Onward; i<pCP->ProdNum; i++){
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
			vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity -= 
				Delta_ProdQty*vProductList[ProductIndex_Onward].ProcessTime[itor_cr->IndexInProblemResourceArray];
			vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].UsedCapacity += 
				Delta_ProdQty*vProductList[ProductIndex_Onward].ProcessTime[itor_cr->IndexInProblemResourceArray];
		}
	}
}

void cMPS::InitializeAvailableCapacityOn_CR(){
	int cr;	 
	//check if sizes of 'AvailableCapacityOn_CR' and 'CR' are the same
	if (AvailableCapacityOn_CR.size()!=CR.size()){
		AvailableCapacityOn_CR.erase(AvailableCapacityOn_CR.begin(), AvailableCapacityOn_CR.end());
		for (cr=0; cr<CR.size(); cr++)
			AvailableCapacityOn_CR.push_back(0); 
	}
}


void cMPS::SetAvailableCapacityOn_CR_ToGivenProduct(int ProdIndex){
	list<cResource>::iterator itor_cr;
	int cr;
	cr=0;
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		AvailableCapacityOn_CR[cr] = vProductList[pCP->ProdNum-1].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity + 
			vProductList[ProdIndex].ProcessTime[itor_cr->IndexInProblemResourceArray];
		cr++;
	}
}

void cMPS::DecreaseAvailableCapacityOn_CR(int ProdIndex, int Delta_ProdQty){
	// positive Delta_ProdQty means capacity is released whereas negative Delta_ProdQty implies capacity is used 
	int cr;
	list<cResource>::iterator itor_cr;
	cr=0;
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		AvailableCapacityOn_CR[cr] -= Delta_ProdQty*vProductList[ProdIndex].ProcessTime[itor_cr->IndexInProblemResourceArray];
		cr++;
	}
}

void cMPS::DisplayLeftCapacitiesForProductionOfGivenProduct(int Prod_Index){
	list<cResource>::iterator itor_cr;
	cout << "\nLeft Capacities on Critical Resources for product " << TrimedString(vProductList[Prod_Index].Code) << "\n";
			for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
				cout << vProductList[Prod_Index].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity << ", ";
}

void cMPS::DisplayAvailableCapacityOn_CR(){
	int cr;
	cout << "\nAvailable Capacities on CR: ";
	for (cr=0; cr<AvailableCapacityOn_CR.size(); cr++)
		cout << AvailableCapacityOn_CR[cr] << ", ";
}


void cMPS::MoveResource_CL_ToTheBeginningOf_CR(){
	cResource CriticalResource;
	int j;
	float Max_Gamma;

	//identify Critical Resource (CL)
	Max_Gamma = 0;
	for (j=0; j<pCP->ResNum; j++){
		if(pCP->vResource[j].DegreeOfCriticality >= Max_Gamma){
			CriticalResource = pCP->vResource[j];
			Max_Gamma = pCP->vResource[j].DegreeOfCriticality;
		}
	}
	//add CL to the beginning of CR. If it already is there, there will be duplication that need to be removed
	CR.sort();
	CR.remove(CriticalResource); //in case CL already in CR and to avoid duplication
	CR.push_front(CriticalResource);
}

void cMPS::Calculate_R_Ratio_PBN_AndSetPrimaryBN(){
	//identify primary bottleneck who has run out of capacity first
	list<cResource>::iterator itor_cr, itor_PrimaryBN;
	int i;
	float Min_NegativeLeftCapacity = 0;
	itor_PrimaryBN = CR.begin();
	for (i=0; i<pCP->ProdNum; i++){
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
			if (vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity < Min_NegativeLeftCapacity){
				itor_PrimaryBN = itor_cr;
				Min_NegativeLeftCapacity = vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity;
			}
		}
		if (Min_NegativeLeftCapacity < 0)
			i=pCP->ProdNum;
	}
	
	//set PrimaryBN and update R_Ratio_PBN
	PrimaryBN = *itor_PrimaryBN;
	Update_R_Ratio_PBN();
}

void cMPS::UpdateImprovingTradeoffs(){
	int g, r;
	float TempDelta_gr;
	//initialize the list
	if (!ImprovingTradeoffs.empty())
		ImprovingTradeoffs.erase(ImprovingTradeoffs.begin(),ImprovingTradeoffs.end());
	
	IdentifyPrimaryBottleneck(); //Very important! If not executed, Delta values will be calculated incorrectly
	Update_R_Ratio_PBN();

	for (g=0; g<(pCP->ProdNum-1); g++){
		for(r=(g+1); r<pCP->ProdNum; r++){
			TempDelta_gr = (vProductList[r].R_Ratio_PBN *
		               (vProductList[pCP->ProdNum-1].vResource[PrimaryBN.IndexInProblemResourceArray].LeftCapacity +
		                vProductList[g].ProcessTime[PrimaryBN.IndexInProblemResourceArray]) /
		                vProductList[g].ContributionMargin);
			if(TempDelta_gr > 1){
				cTradeoff* pTempTradeoff = new cTradeoff;
				pTempTradeoff->Giver_g = g;
				pTempTradeoff->Receiver_r = r;
				pTempTradeoff->Delta_gr = TempDelta_gr;
				ImprovingTradeoffs.push_back (*pTempTradeoff);
			}
		}
	}
	ImprovingTradeoffs.sort();	
}

void cMPS::IdentifyPrimaryBottleneck(){
	//We assume PrimaryBN is the resource with MINIMUM Left Capacity
	list<cResource>::iterator itor_cr;
	float MinLeftCapacity = pCP->MaxCapacity; 
	bool PrimaryBN_Identified = false; // a control variable

	for(itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		if (vProductList[pCP->ProdNum-1].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity < MinLeftCapacity){
			MinLeftCapacity = vProductList[pCP->ProdNum-1].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity;
			PrimaryBN = *itor_cr;
			itor_PrimaryBN = itor_cr;
			PrimaryBN_Identified = true;
		}
	}

	if (PrimaryBN_Identified==false)
		cout << "\nERROR: Primary Bottleneck was not identified correctly!";
}

void cMPS::Update_R_Ratio_PBN(){
	int i;
	for (i=0; i<vProductList.size(); i++){
		if(vProductList[i].ProcessTime[PrimaryBN.IndexInProblemResourceArray]>0)
			vProductList[i].R_Ratio_PBN = vProductList[i].ContributionMargin/vProductList[i].ProcessTime[PrimaryBN.IndexInProblemResourceArray];
		else
			vProductList[i].R_Ratio_PBN = 0;
	}
}

void cMPS::IdentifyAndRemoveDominatedBottlenecksFrom_CR(){
	list<cResource>::iterator itor_cr, itor_DBN, itor_NextToTheLast, itor_X, itor_Y;
	int i, Score_X, Score_Y; // dominance score
	if (CR.size()>=2){
		//initializing IsDominated variable
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
			itor_cr->IsDominated = false;
		itor_NextToTheLast = CR.end()--;

		itor_X = CR.begin();
		while (itor_X!=itor_NextToTheLast){
			itor_Y = itor_X;
			itor_Y++;
			while (itor_Y!=CR.end()){
				//calculate dominance scores
				Score_X = 0;
				Score_Y = 0;
				for (i=0; i<pCP->ProdNum; i++){
					if (vProductList[i].ProcessTime[itor_X->IndexInProblemResourceArray] > vProductList[i].ProcessTime[itor_Y->IndexInProblemResourceArray])
						Score_X++;
					else if (vProductList[i].ProcessTime[itor_Y->IndexInProblemResourceArray] > vProductList[i].ProcessTime[itor_X->IndexInProblemResourceArray])
						Score_Y++;
				}

				//identify dominated rsources
				if (Score_X>Score_Y && Score_Y==0 && itor_X->Capacity<=itor_Y->Capacity){
					itor_Y->IsDominated = true;
				}
				else if (Score_Y>Score_X && Score_X==0 && itor_Y->Capacity<=itor_X->Capacity){
					itor_X->IsDominated = true;
				}

				itor_Y++;
			}
			itor_X++;
		}
		
	}
	
	//initialize DominatedBottlenecks
	if (DominatedBottlenecks.size()>0)
		DominatedBottlenecks.erase(DominatedBottlenecks.begin(), DominatedBottlenecks.end());
	
	//copy dominated bottleneck to DominatedBottlenecks
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++){
		if (itor_cr->IsDominated == true)
			DominatedBottlenecks.push_back(*itor_cr);
	}

	//remove dominated bottleneck from CR
	if (!DominatedBottlenecks.empty()){
		for (itor_DBN=DominatedBottlenecks.begin(); itor_DBN!=DominatedBottlenecks.end(); itor_DBN++)
			CR.remove(*itor_DBN);
	}
}


void cMPS::PrintMPS_TableIntoFile(const char* OutFilePrefix, const char* Algorithm){
	list<int>::iterator itor, itor_2;
	list<cResource>::iterator itor_cr;
	int i, j;
	stringstream MainOutFile;

	MainOutFile.clear();
	MainOutFile << "Output/" << OutFilePrefix << ".out";
	ofstream pOF;
	pOF.open(MainOutFile.str().c_str(), ios::app); 
	if (!pOF){
		cerr << "Output file " << MainOutFile << "did not open" << endl;
		exit(0);
	}

	pOF <<"\n\nFinal MPS of " << Algorithm << ":\n\n";

	pOF << setw(40) << "Left Capacity on Resources";
	pOF << "\n" << left << setw(7) << "Prod " << left << setw(7) << "Qty";
	for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
		pOF << left << setw (10) << TrimedString(itor_cr->Code);  

	pOF << left << setw(15) << "Throughput";
	for (i=0; i<vProductList.size(); i++){
		pOF << "\n" << left << setw(7) << TrimedString(vProductList[i].Code);

		pOF << left << setw(7) << vProductList[i].ProdQty;
		for (itor_cr=CR.begin(); itor_cr!=CR.end(); itor_cr++)
			pOF << left << setw(10) << vProductList[i].vResource[itor_cr->IndexInProblemResourceArray].LeftCapacity;

		pOF << left << setw(15) << vProductList[i].Throughput;
	}
	pOF << "\n\n";
	

	pOF << "Total Throughput: ";
	pOF << TotalThroughput;

	if (!DominatedBottlenecks.empty()){
		pOF << "\nDominated Bottlenecks: ";
			for (itor_cr=DominatedBottlenecks.begin(); itor_cr!=DominatedBottlenecks.end(); itor_cr++)
				pOF << TrimedString(itor_cr->Code) << ", ";
		
	}

	pOF << "\n\nEnd of the MPS Table of " << Algorithm;
	pOF << "\n-------------------------------------------------";

	pOF.close();
	
}


cMPS::~cMPS(){}