#include "stdafx.h"
#include "cTradeoff.h"

cTradeoff::cTradeoff(){
}

cTradeoff::cTradeoff (const cTradeoff& copy_me){
	Giver_g = copy_me.Giver_g;
	Receiver_r = copy_me.Receiver_r;
	Delta_gr = copy_me.Delta_gr;
}

cTradeoff& cTradeoff::operator= (const cTradeoff& rhs){
		Giver_g = rhs.Giver_g;
		Receiver_r = rhs.Receiver_r;
		Delta_gr = rhs.Delta_gr;
		return *this;
}

	bool cTradeoff::operator< (const cTradeoff& rhs){
		return (Delta_gr>rhs.Delta_gr);
	}

cTradeoff::~cTradeoff(){
}