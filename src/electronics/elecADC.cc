/*
 * elecADCsim.cc
 *
 *  Created on: 23/10/2017
 *      Author: hepf
 */

#include <cmath>
#include <algorithm>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "Rtypes.h"
#include "TApplication.h"
#include "TROOT.h"

#include "elecADC.hh"

elecADC::elecADC(void)
{
	ADC_bits = 14;
	ADC_Vref = 2.0; // en Volts. (GND=0)
	ADC_Voffset = 1.0;
	ADC_Sample_Rate = 125.0; // en MHz.
	ADC_Samples_per_Pulse = 20;
	ADC_Trigger_Voltaje = 0.1; // en Volts.
	ADC_Trigger_Sample_Offset = 5;
}

elecADC::~elecADC(void)
{}

void elecADC::DigitalizeVoltagePulses( elecWCDtankPMTdata* PMTPulsesData, elecRCequivalent* RCequivalentCircuit, elecADCoutput Output)
{

	elecDataTable* VoltagePulseData = new elecDataTable();

//	RCequivalentCircuit->PMTPulseVoltage(PMTPulsesData, VoltagePulsesData);

	int argc = 1;
	char* argv[] = { (char*)"elec" };

	TApplication theApp("elec", &argc, argv);
	if (gROOT->IsBatch()) {
	  fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
	  return;
	}

	Long_t ADC_bins = std::pow( 2, ADC_bits );
	Double_t ADC_Dv = ADC_Vref / ADC_bins;

	Double_t Time_increment = 1000.0 / ADC_Sample_Rate;

	std::vector< Double_t > Time_array ( ADC_Samples_per_Pulse );
	std::vector< Double_t > ADC_array ( ADC_Samples_per_Pulse );
	Double_t ADC_Vtmp = 0;

//	std::vector < std::vector<Double_t> > *vect2d_tmp;

	Long64_t NumberOfPulses = PMTPulsesData->GetNumberOfPulses();
//	Long_t NumberOfPulses = VoltagePulseData->size();

	std::vector< Long_t >* ADCmaximumAll = new std::vector< Long_t >;
	std::vector< Long_t >* ADCmaximumVert = new std::vector< Long_t >;
	std::vector< Long_t >* ADCmaximumNonVert = new std::vector< Long_t >;
	//std::vector< Long_t >* ADCcharge = new std::vector< Long_t >;

	for( Long_t Pulse = 0; Pulse < NumberOfPulses; Pulse ++)
	{

		PMTPulsesData->SetPulse( Pulse );

		VoltagePulseData->clear();
		RCequivalentCircuit->PMTPulseVoltage(PMTPulsesData, VoltagePulseData);

		std::fill (Time_array.begin(),Time_array.end(),0.0);
		std::fill (ADC_array.begin(),ADC_array.end(),0.0);

//		vect2d_tmp = &(VoltagePulsesData->at( Pulse ));

//		Double_t t_cur = (vect2d_tmp->at( 0 )).at(0);
		Double_t t_cur = (VoltagePulseData->at( 0 )).at(0);
		Double_t V_n=0;

		Long_t ADC_max = -1;

		bool Trigger_exceeded = false;
		bool ADC_overflow = false;
		bool ADC_underflow = false;

		Int_t Table_pos = 0;
//		Int_t Table_size = vect2d_tmp->size();
		Int_t Table_size = VoltagePulseData->size();

		for( short i = 0; i < ( ADC_Trigger_Sample_Offset - 1 ); i++)
			Time_array.at(i) = i*Time_increment;


		for( Long_t i = ADC_Trigger_Sample_Offset ; i < ADC_Samples_per_Pulse ; i++ )
		{
//			while( ( (vect2d_tmp->at( Table_pos )).at(0) < t_cur ) && ( Table_pos < ( Table_size - 1 ) ) )
//				Table_pos++;
			while( ( (VoltagePulseData->at( Table_pos )).at(0) < t_cur ) && ( Table_pos < ( Table_size - 1 ) ) )
				Table_pos++;

//			V_n = (vect2d_tmp->at( Table_pos )).at(1);
			V_n = (VoltagePulseData->at( Table_pos )).at(1);

			Time_array.at(i) = i*Time_increment;

			ADC_Vtmp =  V_n * exp( -( RCequivalentCircuit->GetConst_k() ) * t_cur ) + ADC_Voffset;

			if ( ADC_Vtmp > ( ADC_Trigger_Voltaje + ADC_Voffset )  ) Trigger_exceeded = true;

			//if( ADC_Vtmp <= 0)
			//	ADC_array.at(i) = 0.0;
			//else if( ADC_Vtmp >= ADC_Vref )
			if( ADC_Vtmp >= ADC_Vref )
			{
				ADC_array.at(i) = ADC_bins - 1.0;
				ADC_overflow = true;
			}
			else if (  ADC_Vtmp <= 0 )
			{
				ADC_array.at(i) = 0.0;
				ADC_underflow = true;
			}
			else
				ADC_array.at(i) = std::floor( ADC_Vtmp / ADC_Dv );

			if( ADC_array.at(i) > ADC_max ) ADC_max = ADC_array.at(i);

			t_cur += Time_increment;
		}

		if(Trigger_exceeded)
		{
//			std::cout << "\r================================" << std::endl;
			std::cout << "\rPulse: " << std::setw(8) << std::setfill('0') << Pulse << std::flush;
//			std::cout << "\r================================" << std::endl;
//			for(Int_t ii = 0;  ii < ADC_Samples_per_Pulse ; ii++ )
//				std::cout << ADC_array.at(ii) << std::endl;

			ADCmaximumAll->push_back(ADC_max);

			//ADCmaxHistAll->Fill( ADC_max );

			if( PMTPulsesData->GetPulseOrientation( ) )
				ADCmaximumVert->push_back(ADC_max);
				//ADCmaxHistVert->Fill( ADC_max );
			else
				ADCmaximumNonVert->push_back(ADC_max);
				//ADCmaxHistNonVert->Fill( ADC_max );

			if ( ADC_overflow )
				std::cout << "\r" << std::setw(8) << std::setfill('0') << Pulse << ": ADC_overflow" << std::endl;
			if ( ADC_underflow )
				std::cout << "\r" << std::setw(8) << std::setfill('0') << Pulse << ": ADC_undeflow" << std::endl;

		}
		else
		{
//			std::cout << "================================" << std::endl;
			std::cout << "\r" << std::setw(8) << std::setfill('0') << Pulse << ": Trigger not exceeded" << std::endl;
//			std::cout << "================================" << std::endl;
		}
	}

	Int_t histll = (Int_t)*std::min_element(ADCmaximumAll->begin(), ADCmaximumAll->end());
	Int_t histul = (Int_t)*std::max_element(ADCmaximumAll->begin(), ADCmaximumAll->end());
	std::cout << "\r********************************" << std::endl;
	std::cout << "Size of ADCmaximum: " << ADCmaximumAll->size() << std::endl;
	std::cout << "Min of ADCmaximun: " << histll << std::endl;
	std::cout << "Max of ADCmaximun: " << histul << std::endl;
	std::cout << "********************************" << std::endl;

	TH1 *ADCmaxHistAll 		= new TH1D("H1","ADC pulse maximum (All)",100,histll, histul);
	TH1 *ADCmaxHistVert 	= new TH1D("H2","ADC pulse maximum (Verticals)",100,histll, histul);
	TH1 *ADCmaxHistNonVert 	= new TH1D("H3","ADC pulse maximum (Non-Verticals)",100,histll, histul);

	for( auto i: *ADCmaximumAll )
		ADCmaxHistAll->Fill( i );
	
	for( auto i: *ADCmaximumVert )
		ADCmaxHistVert->Fill( i );
	
	for( auto i: *ADCmaximumNonVert )
		ADCmaxHistNonVert->Fill( i );


	TCanvas *ShowHists = new TCanvas("Algo", "otro", 600, 400);
//	ShowHists->Divide(1,3);

//	ShowHists->cd(1);
	ADCmaxHistAll->Draw();
	ShowHists->Update();

//	ShowHists->cd(2);
	ADCmaxHistVert->SetLineColor(kRed);
	ADCmaxHistVert->Draw("same");
	ShowHists->Update();

//	ShowHists->cd(3);
	ADCmaxHistNonVert->SetLineColor(kGreen);
	ADCmaxHistNonVert->Draw("same");
	ShowHists->Update();

	ShowHists->Modified();
	ShowHists->Connect("Closed()", "TApplication", gApplication, "Terminate()"); //new


	theApp.Run();
}
