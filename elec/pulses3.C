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

void AvRTGraph(const char* WCDTankDataRootFileName, TH2* AvRTHist){
	Long64_t Entries;
	std::vector<double> *PulseData = new std::vector<double>();

	TFile *WCDTankRootFile = new TFile( WCDTankDataRootFileName );
	TTree *WCDTankData = (TTree*)WCDTankRootFile->Get("WCDtank;1");
	Entries = WCDTankData->GetEntriesFast();	
	WCDTankData->SetBranchAddress("PMT_PhotoElectric_Photons_Time",&PulseData);

/*
	if (PulseNumber < Entries ) WCDTankData->GetEntry(PulseNumber);
	else { 
		std::cout << "Invalid pulse Number" << std::endl;
		return;
	}
*/

	//TH2 *AvRTHist = new TH2D("H1","Amplitude vs Raize Time",50,0,50,100,0,100);
		

	for( Long_t Event = 0 ; Event < Entries ; Event++ )
	{
		WCDTankData->GetEntry(Event);
	
		Int_t DataEntries = PulseData->size();
		//std::cout << "Number of photons: " << DataEntries << std::endl;
		
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

		//std::cout << "=============================" << std::endl;
		//std::cout << "Event: " << Event << std::endl;
		//std::cout << "Max time: " << DataMax << std::endl;
		//std::cout << "Min time: " << DataMin << std::endl;

		TH1 *tmpHist = new TH1D("H2","Photon arrival time",50,0,50);
		for( int i = 0 ; i < DataEntries ; i++ )
			tmpHist->Fill( PulseData->at( i ) );

		Int_t MaxBin = tmpHist->GetMaximumBin();
		Double_t MaxBinValue = tmpHist->GetMaximum();

		//std::cout << "Max bin: " << MaxBin << std::endl;
		//std::cout << "Raize time: " << MaxBin-DataMin << std::endl;
		//std::cout << "Max bin value (amplitude): " << MaxBinValue << std::endl;

		AvRTHist->Fill( MaxBin-DataMin,  MaxBinValue );

		delete tmpHist;
	}	
	
	//TCanvas *ShowHists = new TCanvas("Algo", "otro", 600, 400);
	//AvRTHist->Draw();
	//ShowHists->Update();

} 

void ComposeHist(void){

	//THStack *hs = new THStack("hs","");
	TH2 *Hist1 = new TH2D("H1","Muon - electron",50,0,10,100,0,500);
	TH2 *Hist2 = new TH2D("H2","H2",50,0,10,100,0,500);
	TH2 *Hist3 = new TH2D("H3","H3",50,0,10,100,0,500);
	TH2 *Hist4 = new TH2D("H4","H4",50,0,10,100,0,500);

	AvRTGraph("/home/hector/Simulaciones/muon-electron/Muon_150MeV.root", Hist1);
	AvRTGraph("/home/hector/Simulaciones/muon-electron/Electron_150MeV.root", Hist2);
	AvRTGraph("/home/hector/Simulaciones/muon-electron/Muon_1GeV.root", Hist3);
	AvRTGraph("/home/hector/Simulaciones/muon-electron/Electron_1GeV.root", Hist4);

	//hs->Add(Hist1);
	//hs->Add(Hist2);

	TCanvas *ShowHists = new TCanvas("T1", "Ventanita", 600, 400);
	//gStyle->SetOptStat(0);
	//hs->Draw("");
	Hist1->SetStats(0);
	Hist1->SetMarkerColor(kRed);
	Hist1->Draw();
	ShowHists->Update();

	Hist2->SetStats(0);
	Hist2->SetMarkerColor(kGreen);
	Hist2->Draw("same");
	ShowHists->Update();

	Hist3->SetStats(0);
	Hist3->SetMarkerColor(kOrange);
	Hist3->Draw("same");
	ShowHists->Update();

	Hist4->SetStats(0);
	Hist4->SetMarkerColor(kCyan);
	Hist4->Draw("same");
	ShowHists->Update();

}