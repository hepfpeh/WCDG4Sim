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


void AnalizeData(const char* FileName)
{
    TFile *File = new TFile(FileName);
    if( !File )
    {
        std::cout<<"Can't open " << FileName << std::endl;
        return;
    }

    TTree *Config;
    TTree *Data;

    Float_t ADC_Bits;
    Float_t ADC_Vref;
    Float_t ADC_Sample_Rate;
    Float_t ADC_VSignal_Offset;
    Float_t ADC_Samples_per_Pulse;
    Float_t ADC_Trigger_Voltage;
    Float_t ADC_Pre_Trigger_Samples;

    Config = (TTree*)File->Get("PulsePar_Config");
    Config->SetBranchAddress("ADC_Bits",            &ADC_Bits);
    Config->SetBranchAddress("ADC_Vref",            &ADC_Vref);
    Config->SetBranchAddress("ADC_Vin_Offset",      &ADC_VSignal_Offset);
    Config->SetBranchAddress("Sample_Rate",         &ADC_Sample_Rate);
    Config->SetBranchAddress("Trigger_Voltage",     &ADC_Trigger_Voltage);
    Config->SetBranchAddress("Samples_per_Pulse",   &ADC_Samples_per_Pulse);
    Config->SetBranchAddress("Pre_Trigger_Samples", &ADC_Pre_Trigger_Samples);

    Config->GetEntry(0);
	
	Int_t 		PDG_Code;
    Double_t 	Energy;
	Double_t 	Zenith_angle;
	Int_t 		Direction;
	Double_t 	Deposited_Energy;
	Double_t 	Track_Length;
	Int_t	 	Cherenkov_Photon_Count;
	Int_t	 	PMT_Photon_Count;
    std::vector< Double_t > *Event_Data = 0;
    Double_t    Baseline;
    Double_t    Area;
    Double_t    Amplitud;
    Double_t    Risetime;

    Data = (TTree*)File->Get("PulsePar_Data");
    Data->SetBranchAddress("PDG_Code",                  &PDG_Code);
    Data->SetBranchAddress("Energy",                    &Energy);
	Data->SetBranchAddress("Zenith_angle",              &Zenith_angle);
	Data->SetBranchAddress("Direction",                 &Direction);
	Data->SetBranchAddress("Deposited_Energy",          &Deposited_Energy);
	Data->SetBranchAddress("Track_Length",              &Track_Length);
	Data->SetBranchAddress("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	Data->SetBranchAddress("PMT_Photon_Count",          &PMT_Photon_Count);
    Data->SetBranchAddress("Digitalized_Data",          &Event_Data);
    Data->SetBranchAddress("Pulse_Baseline",            &Baseline);
    Data->SetBranchAddress("Pulse_Area",                &Area);
    Data->SetBranchAddress("Pulse_Amplitud",            &Amplitud);
    Data->SetBranchAddress("Pulse_Rise_time",           &Risetime);

    Long_t NEntries = Data->GetEntries();


    // TH1D *eRisetime = new TH1D("Hert","Electron rise time",200,0,15);
    // TH1D *mRisetime = new TH1D("Hmrt","Muon rise time",200,0,15);
    // TH2D *eRtvsA = new TH2D("HeRA","Rise time vs Amplitud for electrons",200,0,15,200,0,8200);
    // TH2D *mRtvsA = new TH2D("HmRA","Rise time vs Amplitud for muons",200,0,15,200,0,8200);
    
    TH2D *aHist = new TH2D("H2D","A hist",200,0,0,200,0,0);

    for( Long_t i = 0; i< NEntries; i++)
    {

        Data->GetEntry(i);

        // if(PDG_Code == 11 )
        // {
        //     eRisetime->Fill(Risetime);
        //     eRtvsA->Fill(Risetime,Amplitud);
        // }
        // else
        // {
        //     mRisetime->Fill(Risetime);
        //     mRtvsA->Fill(Risetime,Amplitud);
        // }

        if(Track_Length < 780 )
            if(Amplitud < 8100 )
                if(PDG_Code == 13)
                    aHist->Fill(PMT_Photon_Count,Area);
    }

    
    TCanvas *aCanvas1 = new TCanvas("C1","Histograms", 1200,700);
    // aCanvas1->Divide(2,2);
    // aCanvas1->cd(1);
    // eRisetime->Draw();
    // aCanvas1->cd(2);
    // eRtvsA->Draw();
    // aCanvas1->cd(3);
    // mRisetime->Draw();
    // aCanvas1->cd(4);
    // mRtvsA->Draw();
    // aCanvas1->Modified();
    // aCanvas1->Update();

    aHist->Draw();


    // Double_t t_array[(Int_t)ADC_Samples_per_Pulse];
    // Double_t *d_array = Event_Data->data();
    // for(Int_t i=0; i<ADC_Samples_per_Pulse; i++ )
    //     t_array[i] = i;
    
    // TGraph *aGraph = new TGraph((Int_t)ADC_Samples_per_Pulse, t_array, d_array);
    // aGraph->SetTitle("Digitalized signal");
    // TCanvas *aCanvas2 = new TCanvas("C2","Simulated WCD output", 800,600);
    // aCanvas2->cd();
    // aGraph->Draw();



}