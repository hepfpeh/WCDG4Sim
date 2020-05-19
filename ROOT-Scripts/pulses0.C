// root script file

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLine.h"
#include "TF1.h"
#include "TH2D.h"

#include <cstdlib>
#include <iostream>


void ViewPulseData(const char* FileName, unsigned long EntryNumber)
{
    TFile *File = new TFile(FileName);
    if( !File )
    {
        std::cout<<"Can't open " << FileName << std::endl;
        return;
    }

    TTree *Data;
	
	Int_t 		PDG_Code;
    Double_t 	Energy;
	Double_t 	Zenith_angle;
	Int_t 		Direction;
	Double_t 	Deposited_Energy;
	Double_t 	Track_Length;
	Int_t	 	Cherenkov_Photon_Count;
	Int_t	 	PMT_Photon_Count;
    std::vector< Double_t > *Event_Data = 0;

    Data = (TTree*)File->Get("WCDtank");
    Data->SetBranchAddress("PDG_Code",                  &PDG_Code);
    Data->SetBranchAddress("Energy",                    &Energy);
	Data->SetBranchAddress("Zenith_angle",              &Zenith_angle);
	Data->SetBranchAddress("Direction",                 &Direction);
	Data->SetBranchAddress("Deposited_Energy",          &Deposited_Energy);
	Data->SetBranchAddress("Track_Length",              &Track_Length);
	Data->SetBranchAddress("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	Data->SetBranchAddress("PMT_Photon_Count",          &PMT_Photon_Count);
    Data->SetBranchAddress("PMT_PhotoElectric_Photons_Time",          &Event_Data);


    Long_t NEntries = Data->GetEntries();

    if( EntryNumber > NEntries )
    {
        std::cout << "Invalid entry" << std::endl;
        return;
    }

    Data->GetEntry(EntryNumber);

    std::cout<< "Entries:\t\t\t\t" << NEntries << std::endl;
    std::cout<< "PDG_Code:\t\t\t\t" << PDG_Code << std::endl;
    std::cout<< "Energy:\t\t\t\t\t" << Energy << std::endl;
    std::cout<< "Zenith_angle:\t\t\t\t" << Zenith_angle << std::endl;
    std::cout<< "Direction:\t\t\t\t" << Direction << std::endl;
    std::cout<< "Deposited_Energy:\t\t\t" << Deposited_Energy << std::endl;
    std::cout<< "Track_Length:\t\t\t\t" << Track_Length << std::endl;
    std::cout<< "Cherenkov_Photon_Count:\t\t\t" << Cherenkov_Photon_Count << std::endl;
    std::cout<< "PMT_Photon_Count:\t\t\t" << PMT_Photon_Count << std::endl;
    std::cout<< "PMT_PhotoElectric_Photons_Time size:\t" << Event_Data->size() << std::endl;



}

void ViewQE(const char* FileName)
{
    TFile *File = new TFile(FileName);
    if( !File )
    {
        std::cout<<"Can't open " << FileName << std::endl;
        return;
    }

    TTree *Data;
	
	Int_t 		PDG_Code;
    Double_t 	Energy;
	Double_t 	Zenith_angle;
	Int_t 		Direction;
	Double_t 	Deposited_Energy;
	Double_t 	Track_Length;
	Int_t	 	Cherenkov_Photon_Count;
	Int_t	 	PMT_Photon_Count;
	Int_t	 	PMT_PhotoElectron_Count;
    std::vector< Double_t > *Event_Data = 0;

    Data = (TTree*)File->Get("WCDtank");
    Data->SetBranchAddress("PDG_Code",                  &PDG_Code);
    Data->SetBranchAddress("Energy",                    &Energy);
	Data->SetBranchAddress("Zenith_angle",              &Zenith_angle);
	Data->SetBranchAddress("Direction",                 &Direction);
	Data->SetBranchAddress("Deposited_Energy",          &Deposited_Energy);
	Data->SetBranchAddress("Track_Length",              &Track_Length);
	Data->SetBranchAddress("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	Data->SetBranchAddress("PMT_Photon_Count",          &PMT_Photon_Count);
	Data->SetBranchAddress("PMT_PhotoElectric_Count",   &PMT_PhotoElectron_Count);
    Data->SetBranchAddress("PMT_PhotoElectric_Photons_Time",          &Event_Data);


    Long_t NEntries = Data->GetEntries();

    for (Long_t i=0; i<NEntries; i++)
    {
        Data->GetEntry(i);
        std::cout << i
                  << " "
                  << (Event_Data->size() + 0.0 ) / PMT_Photon_Count
                  << " "
                  << PMT_PhotoElectron_Count
                  << " "
                  << Event_Data->size()
                  << " "
                  << PMT_Photon_Count
                  << std::endl;
    }



}