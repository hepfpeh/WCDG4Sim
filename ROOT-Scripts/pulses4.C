// root script file

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

#include <cstdlib>
#include <iostream>

void ReadElecSimRootFile(const char* FileName, unsigned long EventNumber)
{
    TFile *RFile = new TFile(FileName);
    if( !RFile )
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


    Config = (TTree*)RFile->Get("ADC_CP;1");
    Config->SetBranchAddress("Bits", &ADC_Bits);
    Config->SetBranchAddress("Vref", &ADC_Vref);
    Config->SetBranchAddress("Sample_Rate", &ADC_Sample_Rate);
    Config->SetBranchAddress("Signal_Offset", &ADC_VSignal_Offset);
    Config->SetBranchAddress("Samples_per_Pulse", &ADC_Samples_per_Pulse);
    Config->SetBranchAddress("Trigger_Voltage", &ADC_Trigger_Voltage);
    Config->SetBranchAddress("Pre_Trigger_Samples", &ADC_Pre_Trigger_Samples);

    Config->GetEntry(0);

    std::vector< Double_t > *Event_Data = 0;

    Data = (TTree*)RFile->Get("ACD_Output;1");
    Data->SetBranchAddress("Data",&Event_Data);

    Long_t NEntries = Data->GetEntries();

    if(EventNumber >= NEntries)
    {
        std::cout<< "Invalid event number" << std::endl;
        return;
    }

    Data->GetEntry(0);

    std::cout<< Event_Data->size() << std::endl;

    Double_t t_array[(Int_t)ADC_Samples_per_Pulse];
    Double_t *d_array = Event_Data->data();
    for(Int_t i=0; i<ADC_Samples_per_Pulse; i++ )
        t_array[i] = i;
    
    TGraph *aGraph = new TGraph((Int_t)ADC_Samples_per_Pulse, t_array, d_array);
    aGraph->Draw();

    RFile->Close();

}