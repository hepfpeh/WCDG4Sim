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

Double_t GetBaseline(const std::vector<Double_t> *pData, Int_t initSamples)
{
    Double_t Baseline = TMath::Mean(initSamples, pData->data());
    return Baseline;
}

Double_t GetArea(const std::vector<Double_t> *pData, Double_t Baseline )
{
    /* Implementación de método del trapecio */
    Int_t nPoints = pData->size();
    Double_t pArea = 0;
    for( Int_t i = 0; i<(nPoints-1); i++ )
    {
        pArea += ( pData->at(i) + pData->at(i+1) ) / 2.0;
    }

    pArea -= Baseline*(nPoints-1);

    return pArea;
}

Double_t GetRisetime(const std::vector<Double_t> *pData, Int_t initSamples, Double_t Baseline )
{
    Int_t nPoints = pData->size();

    Int_t nAmp = TMath::LocMin( nPoints, pData->data() );

    Int_t mPoints = nAmp - initSamples;

    std::vector<Double_t> percentage;
    std::vector<Double_t> index;

    for( Int_t i = ( initSamples + 1 ) ; i <= nAmp ; i++)
    {
        percentage.push_back( ( pData->at(i) - Baseline ) / ( pData->at(nAmp) - Baseline ) );
        index.push_back(i);
    }

    TF1 *parab = new TF1("parab","[0]*x*x+[1]*x+[2]",0,mPoints);
    TGraph *mpg = new TGraph(mPoints,index.data(),percentage.data());
    mpg->Fit("parab","Q");

    Double_t aPar = parab->GetParameter(0);
    Double_t bPar = parab->GetParameter(1);
    Double_t cPar = parab->GetParameter(2);

    Double_t nNinty = ( -bPar + TMath::Sqrt( bPar*bPar - 4*aPar*(cPar-0.9) ) ) / (2*aPar);
    Double_t nTen = ( -bPar + TMath::Sqrt( bPar*bPar - 4*aPar*(cPar-0.1) ) ) / (2*aPar);

    delete parab;
    delete mpg;

    return (nNinty - nTen);
}

void ComputePulseParameters(const char* inFileName, const char* outFileName)
{
    TFile *inFile = new TFile(inFileName);
    if( !inFile )
    {
        std::cout<<"Can't open " << inFileName << std::endl;
        return;
    }

    TTree *inConfig;
    TTree *inData;

    Float_t ADC_Bits;
    Float_t ADC_Vref;
    Float_t ADC_Sample_Rate;
    Float_t ADC_VSignal_Offset;
    Float_t ADC_Samples_per_Pulse;
    Float_t ADC_Trigger_Voltage;
    Float_t ADC_Pre_Trigger_Samples;

    inConfig = (TTree*)inFile->Get("ElecSim_info");
    inConfig->SetBranchAddress("ADC_Bits",            &ADC_Bits);
    inConfig->SetBranchAddress("ADC_Vref",            &ADC_Vref);
    inConfig->SetBranchAddress("ADC_Vin_Offset",      &ADC_VSignal_Offset);
    inConfig->SetBranchAddress("Sample_Rate",         &ADC_Sample_Rate);
    inConfig->SetBranchAddress("Trigger_Voltage",     &ADC_Trigger_Voltage);
    inConfig->SetBranchAddress("Samples_per_Pulse",   &ADC_Samples_per_Pulse);
    inConfig->SetBranchAddress("Pre_Trigger_Samples", &ADC_Pre_Trigger_Samples);

    inConfig->GetEntry(0);
	
	Int_t 		PDG_Code;
    Double_t 	Energy;
	Double_t 	Zenith_angle;
	Int_t 		Direction;
	Double_t 	Deposited_Energy;
	Double_t 	Track_Length;
	Int_t	 	Cherenkov_Photon_Count;
	Int_t	 	PMT_Photon_Count;

    std::vector< Double_t > *Event_Data = 0;

    inData = (TTree*)inFile->Get("ElecSim_Output");
    inData->SetBranchAddress("PDG_Code",                  &PDG_Code);
    inData->SetBranchAddress("Energy",                    &Energy);
	inData->SetBranchAddress("Zenith_angle",              &Zenith_angle);
	inData->SetBranchAddress("Direction",                 &Direction);
	inData->SetBranchAddress("Deposited_Energy",          &Deposited_Energy);
	inData->SetBranchAddress("Track_Length",              &Track_Length);
	inData->SetBranchAddress("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	inData->SetBranchAddress("PMT_Photon_Count",          &PMT_Photon_Count);
    inData->SetBranchAddress("Digitalized_Data",          &Event_Data);

    Long_t NEntries = inData->GetEntries();


    TFile *outFile = new TFile(outFileName,"recreate");

    if(!outFile)
    {
        std::cout<<"Can't open " << outFileName << std::endl;
        return;
    }

    TTree *outConfig = new TTree("PulsePar_Config","ComputePulseParameters config from ElecSim file");
    TTree *outData = new TTree("PulsePar_Data","WCDtankSim ElecSim data plus ComputePulseParameters data");

    outConfig->Branch("ADC_Bits",            &ADC_Bits);
    outConfig->Branch("ADC_Vref",            &ADC_Vref);
    outConfig->Branch("ADC_Vin_Offset",      &ADC_VSignal_Offset);
    outConfig->Branch("Sample_Rate",         &ADC_Sample_Rate);
    outConfig->Branch("Trigger_Voltage",     &ADC_Trigger_Voltage);
    outConfig->Branch("Samples_per_Pulse",   &ADC_Samples_per_Pulse);
    outConfig->Branch("Pre_Trigger_Samples", &ADC_Pre_Trigger_Samples);

    outConfig->Fill();

    Double_t Baseline;
    Double_t Area;
    Double_t Amplitud;
    Double_t Risetime;

    outData->Branch("PDG_Code",                  &PDG_Code);
    outData->Branch("Energy",                    &Energy);
	outData->Branch("Zenith_angle",              &Zenith_angle);
	outData->Branch("Direction",                 &Direction);
	outData->Branch("Deposited_Energy",          &Deposited_Energy);
	outData->Branch("Track_Length",              &Track_Length);
	outData->Branch("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	outData->Branch("PMT_Photon_Count",          &PMT_Photon_Count);
    outData->Branch("Digitalized_Data",          &Event_Data);
    outData->Branch("Pulse_Baseline",            &Baseline);
    outData->Branch("Pulse_Area",                &Area);
    outData->Branch("Pulse_Amplitud",            &Amplitud);
    outData->Branch("Pulse_Rise_time",           &Risetime);

    for( Long_t i = 0; i< NEntries; i++)
    {

    
        inData->GetEntry(i);

        Baseline = GetBaseline( Event_Data, (Int_t)ADC_Pre_Trigger_Samples - 1 );

        Area = GetArea( Event_Data, Baseline );

        auto it = TMath::LocMin(Event_Data->begin(), Event_Data->end());
        Amplitud = Baseline - *it;

        Risetime = GetRisetime(Event_Data,(Int_t)ADC_Pre_Trigger_Samples,Baseline);

        outData->Fill();

    }


    inFile->Close();

    outConfig->Write();
    outData->Write();

    outFile->Close();

    
}