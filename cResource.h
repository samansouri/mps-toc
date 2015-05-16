#ifndef RES
#define RES 1

#include "Parameters.h"
#include <vector>
using namespace std;

class cResource{

public:
	cResource(); //constructor
	cResource (const cResource& copy_me); //copy constructor
	char Code[CODE_SIZE];
	int IndexInProblemResourceArray; // reflects position of the resource in the Problem Resource array
	float Capacity;
	float UsedCapacity;
	float LeftCapacity;
	float CapacityDifference; // i.e. dj
	float DegreeOfCriticality; // weighted sum of complexity and capacity shortage
	float Priority; // to be used in sorting constrained resources
	void UpdateLeftCapacity (float UsedCapacity) {
		LeftCapacity -= UsedCapacity;
	}
	bool IsDominated; //to be used in identifying and removal of dominated bottleneck resources
	cResource& operator= (const cResource& rhs);
	bool cResource::operator== (const cResource& rhs);
	bool cResource::operator< (const cResource& rhs);

	~cResource();
};

#endif 
