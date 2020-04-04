/*
 * elecRLCequivalent.cc
 *
 *  Created on: 13/10/2017
 *      Author: hepf
 */


#include "elecRCequivalent.hh"
#include "elecTypes.hh"

#include "Rtypes.h"

#include <vector>

elecRCequivalent::elecRCequivalent(void)
{
	Circuit_R = 50; 	// ohms.
	Circuit_C = 0.4; 	// ns/ohm = nF.
	Circuit_V = 1500.0; // en V.
	Const_k = 1.0 / ( Circuit_R * Circuit_C );// en 1/ns.
}

elecRCequivalent::~elecRCequivalent(void)
{}


void elecRCequivalent::PMTPhotonsToVoltageSignal(elecWCDtankPMTdata* PMTdata, elecVoltageSignal* PMTOutputSignal)
{
	Double_t Dep_q = -1.6e-13;								// en C. ( se fija como e x 10^6)


	std::vector<Double_t>* PhotonData;

	Long64_t DataEntries = PMTdata->GetNumberOfPulses();

	elecSignalPoint *PMTSPoint = new elecSignalPoint;
	elecEventSignal *PMTSignal = new elecEventSignal;

	for( Long64_t EventNumber = 0;  EventNumber < DataEntries; EventNumber++)
	{
		PMTdata->SetPulse( EventNumber );
		PhotonData = PMTdata->GetPulseTimeData();

		Int_t PhotonDataEntries = PhotonData->size();

		if( PhotonDataEntries == 0 ) continue;
		
		Double_t t_cur = 0;
		Double_t t_ant;
		Double_t A_cur = 0;
		Double_t A_ant;
		Double_t V_cur = 0;


//		std::cout << "Pulse: " << EventNumber << " Photon count: "<< PhotonDataEntries  << std::endl;

		PMTSPoint->Time = t_cur;
		PMTSPoint->Voltage = V_cur;

		PMTSignal->push_back( *PMTSPoint );



		t_ant = t_cur;
		A_ant = A_cur;


		for( int i = 0 ; i < PhotonDataEntries ; i++ ){
			t_cur = PhotonData->at( i );
			A_cur = 1.0 + A_ant * exp( -Const_k * (t_cur - t_ant) );
			V_cur = 1 / ( Circuit_C * 1.0e-9 ) * Dep_q * A_cur;
			PMTSPoint->Time = t_cur;
			PMTSPoint->Voltage = V_cur;

			PMTSignal->push_back( *PMTSPoint );

			t_ant = t_cur;
			A_ant = A_cur;
		}

		PMTOutputSignal->AppendEventData(PMTSignal);
		PMTOutputSignal->SetKConstant(Const_k);

		PMTSignal->clear();
	}
}
