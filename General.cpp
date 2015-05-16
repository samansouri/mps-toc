#include "stdafx.h"
#include "General.h"

string HrMinString (int Minutes){
	stringstream my_ss;
	int hr, min;
	my_ss.clear();

	hr = int (Minutes/60);
	min = Minutes - hr*60;

	if(hr<10){
		my_ss << "0" << hr;
	}
	else{
		my_ss << hr;
	}

	my_ss << ":";

	if(min<10){
		my_ss << "0" << min;
	}
	else{
		my_ss << min;
	}
	return (my_ss.str());

}

void GoTo_xy(short x,short y){ //got this function from http://www.iceteks.com/forums/index.php?act=ST&f=12&t=2232&
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = {x, y}; 
	SetConsoleCursorPosition(hStdout, position);
}


void DisplayProgress(double ThisSearchTime, double MaxSearchTime){
	GoTo_xy(0, 1);
	stringstream Percent_ss;
	Percent_ss << int((ThisSearchTime/MaxSearchTime)*100) << "%";
	cout << Percent_ss.str() << " complete";
}

//string IndexedOutputFile(string Name, const char* Index){
string IndexedOutputFile(string Name, string Code){
	stringstream my_ss;
	my_ss << Name << Code << ".out";
	return (my_ss.str());
}

string TrimedString (char* c_array){ //trims empty bits off the code character arrays 
	stringstream my_ss;
	int k;
	k=0;
	while(c_array[k]!='/' && c_array[k]!=NULL){
		my_ss << c_array[k];
		k++;
	}

	return (my_ss.str());
}

void PutStringBitsInCharacterArray (char* c_array, string given_string){
	int k;
	string::iterator itor;
	k=0;
	for(itor=given_string.begin(); itor!=given_string.end(); ++itor){
		c_array[k++] = *itor;
	}

	c_array[k] = '/'; //end of array
}


int MaxFeasibleProdQty(cProduct Prod, list<cResource> CR){
	std::list<cResource>::iterator itor_r;
	int MaxProdQty = Prod.MarketDemand;
	int FeasibleQty;
	for(itor_r=CR.begin(); itor_r!=CR.end(); itor_r++){

		FeasibleQty = int(itor_r->LeftCapacity/Prod.ProcessTime[itor_r->IndexInProblemResourceArray]);
		if( FeasibleQty < MaxProdQty)
			MaxProdQty = FeasibleQty;
	}
	return (MaxProdQty);
}
