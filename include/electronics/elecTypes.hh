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


typedef std::vector < std::vector< std::vector <Double_t> > > elecDataTable3;
typedef std::vector < std::vector <Double_t> > elecDataTable;

typedef struct elecSignalPoint_s { 
    Double_t Time;
    Double_t Voltage;
} elecSignalPoint;
typedef std::vector < elecSignalPoint > elecEventSignal;
typedef std::vector < elecEventSignal > elecEventCollection;



#endif /* INCLUDE_ELECTRONICS_ELECTYPES_HH_ */
