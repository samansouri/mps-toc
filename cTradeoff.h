#ifndef TRDF
#define TRDF 1

class cTradeoff{
public:
	cTradeoff();
	cTradeoff (const cTradeoff& copy_me); //copy constructor
	int Giver_g; 
	int Receiver_r; 
	float Delta_gr;

	cTradeoff& operator= (const cTradeoff& rhs);
	bool cTradeoff::operator< (const cTradeoff& rhs);

	~cTradeoff();
};




#endif 
