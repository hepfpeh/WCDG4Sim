/*
 * elecRLCequivalent.cc
 *
 *  Created on: 13/10/2017
 *      Author: hepf
 */


#include "elecRCequivalent.hh"
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

void elecRCequivalent::PMTPulseVoltage(elecWCDtankPMTdata* PMTdata, elecPulseCollection* PMTPulseVoltageData)
{
	Double_t Dep_q = 1.6e-13;								// en C. ( se fija como e x 10^6)


	std::vector<Double_t>* PulseData;

	Long64_t DataEntries = PMTdata->GetNumberOfPulses();

	elecSignalPoint *PMTPSPoint = new elecSignalPoint;
	elecPulseSignal *PMTPulse = new elecPulseSignal;

	for( Long64_t PulseNumber = 0; PulseNumber < DataEntries; PulseNumber++)
	{
		PMTdata->SetPulse( PulseNumber );
		PulseData = PMTdata->GetPulseTimeData();

		Int_t PulseDataEntries = PulseData->size();
		Double_t t_cur = 0;
		Double_t t_ant;
		Double_t A_cur = 0;
		Double_t A_ant;
		Double_t V_cur = 0;


//		std::cout << "Pulse: " << PulseNumber << " Photon count: "<< PulseDataEntries  << std::endl;

		PMTPSPoint->Time = t_cur;
		PMTPSPoint->Voltage = V_cur;

		PMTPulse->push_back( *PMTPSPoint );



		t_ant = t_cur;
		A_ant = A_cur;


		for( int i = 0 ; i < PulseDataEntries ; i++ ){
			t_cur = PulseData->at( i );
			A_cur = 1.0 + A_ant * exp( -Const_k * (t_cur - t_ant) );
//			V_cur = 1 / ( Circuit_C * 1.0e-9 ) * Dep_q * A_cur * exp( Const_k * t_cur );
			V_cur = 1 / ( Circuit_C * 1.0e-9 ) * Dep_q * A_cur;
			PMTPSPoint->Time = t_cur;
			PMTPSPoint->Voltage = V_cur;
//			std::cout << "Pulse: " << PulseNumber << " Photon: "<< i << " Time: " << t_cur << " V_cur: "<< V_cur << std::endl;

			PMTPulse->push_back( *PMTPSPoint );

			t_ant = t_cur;
			A_ant = A_cur;
		}

		PMTPulseVoltageData->push_back(*PMTPulse);

		PMTPulse->clear();
	}
}
