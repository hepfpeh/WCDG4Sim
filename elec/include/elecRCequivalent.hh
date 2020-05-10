/*
 * elecRLCequivalent.hh
 *
 *  Created on: 13/10/2017
 *      Author: hepf
 */

#ifndef INCLUDE_ELECTRONICS_ELECRCEQUIVALENT_HH_
#define INCLUDE_ELECTRONICS_ELECRCEQUIVALENT_HH_

#include "elecWCDtankPMTdata.hh"
#include "elecVoltageSignal.hh"

class elecWCDtankPMTdata;
class elecVoltageSignal;


class elecRCequivalent {
public:
	elecRCequivalent(void);
	~elecRCequivalent(void);

	void PMTPhotonsToVoltageSignal(elecWCDtankPMTdata* PMTdata, elecVoltageSignal* PMTPulseVoltageData);
	Double_t GetConst_k(void){ return Const_k; };

private:
// Circuit Parameters
	Double_t Circuit_R;
	Double_t Circuit_C;
	Double_t Circuit_V;
	Double_t Const_k;
};


#endif /* INCLUDE_ELECTRONICS_ELECRCEQUIVALENT_HH_ */
