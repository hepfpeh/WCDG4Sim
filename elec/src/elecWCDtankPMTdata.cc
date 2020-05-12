/*
 * elecWCDtankPMTdata.cc
 *
 *  Created on: 12/10/2017
 *      Author: hepf
 */

#include "elecWCDtankPMTdata.hh"

// C

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

// Root

#include "TFile.h"
#include "TTree.h"
#include "Rtypes.h"
#include "TBrowser.h"
#include "TROOT.h"

// Geant4

#include "globals.hh"

elecWCDtankPMTdata::elecWCDtankPMTdata(void)
{
	PMTdata				= 0;
	Entries				= -1;

	PDG_Code	= -1;
	Energy		= -1;
	Zenith_angle			= -1;
	Direction			= -1;
	Deposited_Energy	= -1;
	Track_Length		= -1;
	Cherenkov_Photon_Count		= -1;
	PMT_Photon_Count	= -1;
	PulseTimeData		= 0;
}

elecWCDtankPMTdata::~elecWCDtankPMTdata(void)
{}

void elecWCDtankPMTdata::LoadRootFile( const char* RootFileName)
{
	TFile *RootFile = new TFile( RootFileName );
	PMTdata = (TTree*)RootFile->Get("WCDtank;1");

	std::cout << "Ramas en " << RootFileName << ": " << PMTdata->GetListOfBranches()->GetEntries() << std::endl;
	PMTdata->SetBranchAddress("PDG_Code",		&PDG_Code);
	PMTdata->SetBranchAddress("Energy",		&Energy);
	PMTdata->SetBranchAddress("Zenith_angle",		&Zenith_angle);
	PMTdata->SetBranchAddress("Direction",			&Direction);
	PMTdata->SetBranchAddress("Deposited_Energy",	&Deposited_Energy);
	PMTdata->SetBranchAddress("Track_Length",		&Track_Length);
	PMTdata->SetBranchAddress("Cherenkov_Photon_Count",		&Cherenkov_Photon_Count);
	PMTdata->SetBranchAddress("PMT_Photon_Count",		&PMT_Photon_Count);
	PMTdata->SetBranchAddress("PMT_PhotoElectric_Photons_Time",&PulseTimeData);

	Entries = PMTdata->GetEntriesFast();

}

void elecWCDtankPMTdata::SetPulse( Long64_t PulseNumber )
{
	if( !PulseTimeData )
		PulseTimeData = new std::vector < Double_t >;

	if (PulseNumber < Entries ) PMTdata->GetEntry(PulseNumber);
	else {
		std::cout << "Invalid pulse Number" << std::endl;
		return;
	}

	std::sort( PulseTimeData->begin(), PulseTimeData->end() );
}

std::vector< Double_t >* elecWCDtankPMTdata::GetPulseTimeData( void )
{
	return PulseTimeData;
}

WCDtankEventInfo elecWCDtankPMTdata::GetEventInfo( void )
{
	WCDtankEventInfo tmp;

	tmp.PDG_Code				= PDG_Code;
	tmp.Energy					= Energy;
	tmp.Zenith_angle			= Zenith_angle;
	tmp.Direction				= Direction;
	tmp.Deposited_Energy		= Deposited_Energy;
	tmp.Track_Length			= Track_Length;
	tmp.Cherenkov_Photon_Count	= Cherenkov_Photon_Count;
	tmp.PMT_Photon_Count		= PMT_Photon_Count;
	
	return tmp;
}
