// root script file

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>

void AvRTGraph(const char* WCDTankDataRootFileName, Int_t PulseNumber){
	Long64_t Entries;
	std::vector<double> *PulseData = new std::vector<double>();

	TFile *WCDTankRootFile = new TFile( WCDTankDataRootFileName );
	TTree *WCDTankData = (TTree*)WCDTankRootFile->Get("WCDtank;1");
	Entries = WCDTankData->GetEntriesFast();	
	WCDTankData->SetBranchAddress("Photon_Arrival_Time",&PulseData);

/*
	if (PulseNumber < Entries ) WCDTankData->GetEntry(PulseNumber);
	else { 
		std::cout << "Invalid pulse Number" << std::endl;
		return;
	}
*/

	TH2 *AvRTHist = new TH2D("H1","Amplitude vs Raize Time",50,0,50,100,0,100);
		

	for( Long_t Event = 0 ; Event < Entries ; Event++ )
	{
		WCDTankData->GetEntry(Event);
	
		Int_t DataEntries = PulseData->size();
		std::cout << "Number of photons: " << DataEntries << std::endl;
		
		if( DataEntries == 0 )
		{
			std::cout << "=============================" << std::endl;
			std::cout << "Event: " << Event << std::endl;
			std::cout << "------ No photon data -------" << std::endl;
			continue;
		}

		// Ordenando por tiempo

		std::sort( PulseData->begin(), PulseData->end() );

		Double_t DataMax = PulseData->back();
		Double_t DataMin = PulseData->front();

		std::cout << "=============================" << std::endl;
		std::cout << "Event: " << Event << std::endl;
		std::cout << "Max time: " << DataMax << std::endl;
		std::cout << "Min time: " << DataMin << std::endl;

		TH1 *tmpHist = new TH1D("H2","Photon arrival time",50,0,50);
		for( int i = 0 ; i < DataEntries ; i++ )
			tmpHist->Fill( PulseData->at( i ) );

		Int_t MaxBin = tmpHist->GetMaximumBin();
		Double_t MaxBinValue = tmpHist->GetMaximum();

		std::cout << "Max bin: " << MaxBin << std::endl;
		std::cout << "Raize time: " << MaxBin-DataMin << std::endl;
		std::cout << "Max bin value (amplitude): " << MaxBinValue << std::endl;

		AvRTHist->Fill( MaxBin-DataMin,  MaxBinValue );

		delete tmpHist;
	}	
	
	TCanvas *ShowHists = new TCanvas("Algo", "otro", 600, 400);
	AvRTHist->Draw();
	ShowHists->Update();

} 

