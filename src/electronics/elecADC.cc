/*
 * elecADCsim.cc
 *
 *  Created on: 23/10/2017
 *      Author: hepf
 */

#include <cmath>
#include <algorithm>
#include <vector>
#include <boost/circular_buffer.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "Rtypes.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TRandom3.h"

#include "elecADC.hh"

elecADC::elecADC(void)
{
	ADC_bits = 14;
	ADC_Vref = 2.0; // Volts. (GND=0)
	ADC_VSignal_Offset = 1.0; // Volts. Offset a la señal de entrada del ADC.
	ADC_Sample_Rate = 125.0; // MHz.
	ADC_Samples_per_Pulse = 32;
	ADC_Trigger_Voltaje = -0.003; // Volts.
	ADC_Pre_Trigger_Samples = 5;
	ADC_VBaseLine = 0.0; // Volts. Linea base de la senal de entrada.
	ADC_VBaseLine_dev = 0.001; // Volts. Desviacion del ruido gaussiano en la señal de entrada. La media es ADC_VBaseLine. 
}

elecADC::~elecADC(void)
{}

void elecADC::DigitalizeVoltagePulses( elecRCequivalent* RCequivalentCircuit, elecPulseCollection* VoltagePulseData )
{

	//elecPulseCollection* VoltagePulseData = new elecPulseCollection();

	//RCequivalentCircuit->PMTPhotonsToVoltageSignal(PMTPulsesData, VoltagePulseData);

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
	boost::circular_buffer<Double_t> *ADC_cbuffer = new boost::circular_buffer<Double_t>( ADC_Samples_per_Pulse );
	Double_t ADC_Vtmp = 0;
	Double_t ADC_Btmp = 0;

	Long64_t NumberOfPulses = VoltagePulseData->size();
	//Long64_t NumberOfPulses = 102;
	Long_t Trigger_point=0;

	std::vector< Long_t >* ADCmaximumAll = new std::vector< Long_t >;
	
	for( Long_t Pulse = 0; Pulse < NumberOfPulses; Pulse ++)
	{
		std::cout << "\rPulse: " << std::setw(8) << std::setfill('0') << Pulse << std::flush;

		std::fill (Time_array.begin(),Time_array.end(),0.0);
		std::fill (ADC_array.begin(),ADC_array.end(),0.0);

		Double_t t_cur = ((VoltagePulseData->at( Pulse )).at(1)).Time;
		Double_t V_n=0;
		Double_t t_n=0;
		Double_t t_init = t_cur - Time_increment * ( ADC_Pre_Trigger_Samples - 1 ) - gRandom->Uniform( Time_increment );

		Long_t ADC_max = 10000000;

		bool Trigger_exceeded = false;
		bool ADC_overflow = false;
		bool ADC_underflow = false;

		Int_t Table_pos = 0;
		Int_t Table_size = (VoltagePulseData->at( Pulse )).size();
		//std::cout << "\nTable_size: " << Table_size << std::endl;

		t_cur = t_init;
		for( short i = 0; i < ADC_Pre_Trigger_Samples ; i++)
		{
			ADC_Vtmp = gRandom->Gaus( ADC_VBaseLine, ADC_VBaseLine_dev )  + ADC_VSignal_Offset ;
			ADC_Btmp = std::floor(  ADC_Vtmp / ADC_Dv );
			//std::cout << "BL+OF: " << ADC_Vtmp << " B: " << ADC_Btmp << std::endl; 

			if( ADC_Btmp < 0 )
				ADC_cbuffer->push_back( 0.0 );
			else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
				ADC_cbuffer->push_back( ADC_bins - 1 );
			else
				ADC_cbuffer->push_back( ADC_Btmp );

			Time_array.at(i) = t_cur;
			t_cur += Time_increment;
		}


		for( Long_t i = ADC_Pre_Trigger_Samples ; i < ADC_Samples_per_Pulse ; i++ )
		{

			while( ( (VoltagePulseData->at( Pulse ).at( Table_pos )).Time < t_cur ) && ( Table_pos < ( Table_size - 1 ) ) )
				Table_pos++;
			if( Table_pos-- < 0 ) Table_pos = 0;
			V_n = ( ( VoltagePulseData->at( Pulse ) ).at( Table_pos ) ).Voltage;
			t_n = ( ( VoltagePulseData->at( Pulse ) ).at( Table_pos ) ).Time;

			ADC_Vtmp =  V_n * exp( -( RCequivalentCircuit->GetConst_k() ) * ( t_cur - t_n ) );
			ADC_Vtmp += gRandom->Gaus( ADC_VBaseLine, ADC_VBaseLine_dev )  + ADC_VSignal_Offset ; 
			ADC_Btmp = std::floor(  ADC_Vtmp / ADC_Dv );
			
			if ( ADC_Vtmp < ( ADC_Trigger_Voltaje + ADC_VSignal_Offset ) && !Trigger_exceeded )
			{
				Trigger_exceeded = true;
				Trigger_point = i;
				//std::cout << "Trigger at: " << i << std::endl;
			} 
			if( ADC_Btmp > ( ADC_bins - 1 ) )
			{
				ADC_cbuffer->push_back( ADC_bins - 1 );
				ADC_overflow = true;
			}
			else if (  ADC_Btmp < 0 )
			{
				ADC_cbuffer->push_back( 0.0 );
				ADC_underflow = true;
			}
			else
				ADC_cbuffer->push_back( ADC_Btmp );

			if( ADC_Btmp < ADC_max ) ADC_max = ADC_Btmp;

			Time_array.at(i) = t_cur;
			t_cur += Time_increment;
		}

		if(Trigger_exceeded)
		{
//			std::cout << "\r================================" << std::endl;
//			std::cout << "\rPulse: " << std::setw(8) << std::setfill('0') << Pulse << std::flush;
//			std::cout << "\r================================" << std::endl;

			ADCmaximumAll->push_back(ADC_max);

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
//	TH1 *ADCmaxHistVert 	= new TH1D("H2","ADC pulse maximum (Verticals)",100,histll, histul);
//	TH1 *ADCmaxHistNonVert 	= new TH1D("H3","ADC pulse maximum (Non-Verticals)",100,histll, histul);

	for( auto i: *ADCmaximumAll )
		ADCmaxHistAll->Fill( i );

	Long_t Start_point;

	if( Trigger_point > ADC_Pre_Trigger_Samples  )
                Start_point = Trigger_point - ADC_Pre_Trigger_Samples ;
            else
                Start_point = ADC_Samples_per_Pulse - ADC_Pre_Trigger_Samples  + Trigger_point;

	for(Long_t i = 0; i < ADC_Samples_per_Pulse ; ++i)
        ADC_array[ i ] = ADC_cbuffer->at( ( Start_point + i ) % ADC_Samples_per_Pulse );

	Double_t *Ta = &Time_array[0];
	Double_t *Aa = &ADC_array[0];
	
	TGraph *Circuit_C_Carge = new TGraph( ADC_Samples_per_Pulse, Ta, Aa);


	TCanvas *ShowHists = new TCanvas("Algo", "otro", 600, 400);
	ShowHists->Divide(1,2);

	ShowHists->cd(1);
	ADCmaxHistAll->Draw();
	ShowHists->Update();

	ShowHists->cd(2);
    Circuit_C_Carge->Draw("AL*");
//	ADCmaxHistVert->SetLineColor(kRed);
//	ADCmaxHistVert->Draw("same");
	ShowHists->Update();
/*
	ShowHists->cd(3);
	ADCmaxHistNonVert->SetLineColor(kGreen);
	ADCmaxHistNonVert->Draw("same");
	ShowHists->Update();
*/
	ShowHists->Modified();
	ShowHists->Connect("Closed()", "TApplication", gApplication, "Terminate()"); //new


	theApp.Run();
}
