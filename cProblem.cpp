#include "stdafx.h"
#include "cProblem.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "General.h"



cProblem::cProblem(){
}

cProblem::cProblem (char *DataSet)
{
	int i, j;
	char ch;
	cResource* pTempRes;
	string my_code;
	FILE *pIF;
	string InputFile("Input/");
	InputFile.append(DataSet);

	pIF = fopen (InputFile.c_str(), "r");
	if (!pIF){
		cerr << "\nInput File " << InputFile << "didn't open" << endl;
	}
	
	MaxCapacity = 0;

	do{ch = getc(pIF);}	while (ch != '=');
	fscanf_s (pIF, "%d;\n", &ProdNum); //number of Products
	
	do{ch = getc(pIF);}	while (ch != '=');
	fscanf_s (pIF, "%d;\n", &ResNum); //number of Resources
	
	for (i=0; i<ProdNum; i++){
		cProduct* pTempProd = new cProduct(ResNum);
		vProduct.push_back(*pTempProd);

		vProduct[i].TotalRes = ResNum;
					
	}
	
	
	TotalThroughput = 0; //initializing
	//read product data
	fscanf_s (pIF, "\n");

	for (i=0; i<ProdNum; i++){
		do{ch = getc(pIF);}	while (ch != ':');
		my_code.clear();
		do{
			ch = getc(pIF);
			if(ch!=',')
				my_code.push_back(ch);
			} while(ch!=','); 
		PutStringBitsInCharacterArray (vProduct[i].Code, my_code); 
				
		for (j=0; j<ResNum; j++)
			fscanf_s (pIF, "%f,", &vProduct[i].ProcessTime[j]); 
		
		fscanf_s (pIF, "%d,%f\n", &vProduct[i].MarketDemand, &vProduct[i].ContributionMargin); 
		
	}
			
	//read Resource data
	fscanf_s (pIF, "\n");
	for (j=0; j<ResNum; j++){
		pTempRes = new cResource;
		vResource.push_back(*pTempRes);

		do{ch = getc(pIF);}	while (ch != ':');
		my_code.clear();
		do{
			ch = getc(pIF);
			if(ch!=',')
				my_code.push_back(ch);
			} while(ch!=','); 
		PutStringBitsInCharacterArray (vResource[j].Code, my_code);
		
		fscanf_s (pIF, "%f,%f;\n", &vResource[j].Capacity, &vResource[j].DegreeOfCriticality); 

		//update problem's MaxCapacity
		if (vResource[j].Capacity > MaxCapacity)
			MaxCapacity = vResource[j].Capacity;

		vResource[j].UsedCapacity = 0;
		vResource[j].LeftCapacity = vResource[j].Capacity;
		vResource[j].IndexInProblemResourceArray = j;
		
	}
	
	//assign Products' Resources
	for (i=0; i<ProdNum; i++)
		std::copy(vResource.begin(), vResource.end(), vProduct[i].vResource.begin());
	
	
}

//}

void cProblem::DisplayProblem() // to be used for debugging
{
	int i, j;
	cout << "Data set:\n";
	cout << "\nProdNum=" << ProdNum << ";";
	cout << "\nResNum=" << ResNum << ";";
			
	for(i=0; i<ProdNum; i++){
		cout << "\nProd:" << TrimedString(vProduct[i].Code) << ",";
		for (j=0; j<ResNum; j++)
			cout << vProduct[i].ProcessTime[j] << ",";
		
		cout << vProduct[i].MarketDemand << "," << vProduct[i].ContributionMargin << ";";
	}

	for (j=0; j<ResNum; j++)
		cout << "\nRes:" << TrimedString(vResource[j].Code) << "," << vResource[j].Capacity << "," << vResource[j].DegreeOfCriticality << ";";
	
}

bool cProblem::EveryProductUsesEveryResource(){
	int i, j;
	bool Condition = true;
	for (i=0; i<ProdNum; i++){
		for (j=0; j<ResNum; j++){
			if (vProduct[i].ProcessTime[j]==0){
				Condition = false;
				j=ResNum;
				i=ProdNum;
			}
		}
	}
	return (Condition);
}

void cProblem::PrintDataSetIntoOutputFile(const char* OutFilePrefix){
	int i, j;
	stringstream MainOutFile;

	MainOutFile.clear();
	MainOutFile << "Output/" << OutFilePrefix << ".out";
	ofstream pOF;
	pOF.open(MainOutFile.str().c_str()); //open file for the first time for writing 
	if (!pOF){
		cerr << "Output file " << MainOutFile << "did not open" << endl;
		exit(0);
	}

	pOF << "Data Set " << OutFilePrefix << ":\n";
	pOF << "\nProdNum=" << ProdNum << ";";
	pOF << "\nResNum=" << ResNum << ";";
			
	for(i=0; i<ProdNum; i++){
		pOF << "\nProd:" << TrimedString(vProduct[i].Code) << ",";
		for (j=0; j<ResNum; j++)
			pOF << vProduct[i].ProcessTime[j] << ",";
		
		pOF << vProduct[i].MarketDemand << "," << vProduct[i].ContributionMargin << ";";
	}

	for (j=0; j<ResNum; j++)
		pOF << "\nRes:" << TrimedString(vResource[j].Code) << "," << vResource[j].Capacity << "," << vResource[j].DegreeOfCriticality << ";";

	pOF << "\n\nEnd of Data Set";
	pOF << "\n---------------";

	pOF.close();
	
}

void cProblem::PrintDataSetIntoOutputFileInTabularFormat(const char* OutFilePrefix){
	int i, j;
	stringstream MainOutFile;

	MainOutFile.clear();
	MainOutFile << "Output/" << OutFilePrefix << ".out";
	ofstream pOF;
	pOF.open(MainOutFile.str().c_str()); //open file for the first time for writing 
	if (!pOF){
		cerr << "Output file " << MainOutFile << "did not open" << endl;
		exit(0);
	}

	pOF << "Data Set " << OutFilePrefix << ":\n\n";
	
	pOF << setw(32) << "Process Times on Resources";
	pOF << left << setw(7) << "\nProd  ";
	for (j=0; j<ResNum; j++)
		pOF << left << setw(7) << TrimedString(vResource[j].Code);

	pOF << setw(10) << left << "Di" << setw(10) << left << "CMi";

	for(i=0; i<ProdNum; i++){
		pOF << "\n" << left << setw(6) << TrimedString(vProduct[i].Code);
		for (j=0; j<ResNum; j++)
			pOF << left << setw(7) << vProduct[i].ProcessTime[j];
		
		pOF << left << setw(10) << vProduct[i].MarketDemand << left << setw(10) << vProduct[i].ContributionMargin;
	}

	pOF << "\n\n";

	pOF << left << setw(7) << "Capa ";

	for (j=0; j<ResNum; j++)
		pOF << left << setw(7) << vResource[j].Capacity;

	pOF << left << setw(7) << "\nCrit ";
	for (j=0; j<ResNum; j++)
		pOF << left << setw(7) << vResource[j].DegreeOfCriticality;
	
	pOF << "\n\nEnd of Data Set";
	pOF << "\n---------------";

	pOF.close();
	
}
		

cProblem :: ~cProblem(){
	
}