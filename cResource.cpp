#include "stdafx.h"
#include "cResource.h"

cResource::cResource(){
}

cResource::cResource (const cResource& copy_me):

	IndexInProblemResourceArray(copy_me.IndexInProblemResourceArray),
	Capacity(copy_me.Capacity),
	UsedCapacity(copy_me.UsedCapacity),
	LeftCapacity(copy_me.LeftCapacity),
	CapacityDifference(copy_me.CapacityDifference),
	DegreeOfCriticality(copy_me.DegreeOfCriticality),
	IsDominated(copy_me.IsDominated),
	Priority(copy_me.Priority){
		std::copy(copy_me.Code, copy_me.Code+CODE_SIZE, Code);
	}


cResource& cResource::operator= (const cResource& rhs){
	//int k;
	//for (k=0; k<CODE_SIZE; k++)
	//	Code[k] = rhs.Code[k];
	std::copy(rhs.Code, rhs.Code+CODE_SIZE, Code);
	IndexInProblemResourceArray = rhs.IndexInProblemResourceArray;
	Capacity = rhs.Capacity;
	UsedCapacity = rhs.UsedCapacity;
	LeftCapacity = rhs.LeftCapacity;
	CapacityDifference = rhs.CapacityDifference;
	DegreeOfCriticality = rhs.DegreeOfCriticality;
	Priority = rhs.Priority;
	IsDominated=rhs.IsDominated;

	return *this;
}


bool cResource::operator < (const cResource& rhs){
	return (CapacityDifference<rhs.CapacityDifference);
}

bool cResource::operator== (const cResource& rhs){
		bool Condition = true;
		for (int i=0; i<CODE_SIZE; i++){
			if (Code[i]!=rhs.Code[i]){
				Condition = false;
				i=CODE_SIZE;
			}
		}
		return (Condition);
		
	};

cResource::~cResource(){
}