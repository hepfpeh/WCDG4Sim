/*
 * elecTypes.hh
 *
 *  Created on: 23/10/2017
 *      Author: hepf
 */

#ifndef INCLUDE_ELECTRONICS_ELECTYPES_HH_
#define INCLUDE_ELECTRONICS_ELECTYPES_HH_

// C

#include <cstdint>
#include <vector>

// Root

#include "Rtypes.h"

// Geant4

#include "globals.hh"


typedef struct WCDtankEventInfo_s {
	Double_t 				Primary_Energy;
	Double_t 				Zenith_angle;
	Int_t 					Direction;
	Double_t 				Deposited_Energy;
	Double_t 				Track_Length;
	Int_t	 				Cherenkov_Photon_Count;
	Int_t	 				PMT_Photon_Count;
} WCDtankEventInfo;

typedef struct elecSignalPoint_s { 
    Double_t Time;
    Double_t Voltage;
} elecSignalPoint;

typedef std::vector < elecSignalPoint > elecEventSignal;

typedef struct elecEvent_s {
    WCDtankEventInfo EventInfo;
    elecEventSignal  EvenSignal; 
} elecEvent;

typedef std::vector < elecEvent > elecEventCollection;



#endif /* INCLUDE_ELECTRONICS_ELECTYPES_HH_ */
