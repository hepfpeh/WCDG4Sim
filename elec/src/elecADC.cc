/*
 * elecADCsim.cc
 *
 *  Created on: 23/10/2017
 *      Author: hepf
 */

#include <cmath>
#include <algorithm>
#include <vector>
#include <fstream>
#include <ctime>
#include <boost/circular_buffer.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
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
	ADC_Sample_Rate = 500.0; // MHz.
	ADC_Samples_per_Pulse = 256;
	ADC_Trigger_Voltage = -0.003; // Volts.
	ADC_Pre_Trigger_Samples = 60;
}

elecADC::~elecADC(void)
{}

void elecADC::DigitalizeVoltageSignal( elecVoltageSignal* VoltageSignalData, elecADCoutput outputType )
{

	Long_t ADC_bins = std::pow( 2, ADC_bits );
	Double_t ADC_Dv = ADC_Vref / ADC_bins;

	Double_t Time_increment = 1000.0 / ADC_Sample_Rate;

	std::vector< Double_t > Time_array ( ADC_Samples_per_Pulse );
	std::vector< Double_t > ADC_array ( ADC_Samples_per_Pulse );
	Double_t ADC_Vtmp = 0;
	Double_t ADC_Btmp = 0;

	Long64_t NumberOfEvents = VoltageSignalData->GetNumberOfEvents();
	Long_t Trigger_point=0;
	Long_t Start_point;

	boost::circular_buffer<Double_t>* ADC_cbuffer = 0;

	/* Archivo y árboles para la salida ROOTfile */
	TFile *ROOTOutputFile;
	TNtuple *ADC_Parameters;
	TTree *ADC_Output;
	WCDtankEventInfo *PMTEventInfo;
	if( outputType == elecADCoutput::ROOTfile )
	{
		ROOTOutputFile = new TFile("ElecOutput.root","recreate");
		ADC_Parameters = new TNtuple("ElecSim_info",
									 "Digitalization parameters",
									 "ADC_Bits:ADC_Vref:ADC_Vin_Offset:Sample_Rate:Trigger_Voltage:Samples_per_Pulse:Pre_Trigger_Samples"
									);
		ADC_Parameters->Fill(ADC_bits,
							 ADC_Vref,
							 ADC_VSignal_Offset,
							 ADC_Sample_Rate,
							 ADC_Trigger_Voltage,
							 ADC_Samples_per_Pulse,
							 ADC_Pre_Trigger_Samples
							);
		ADC_Parameters->Write();

		PMTEventInfo = new WCDtankEventInfo;
		ADC_Output = new TTree("ElecSim_Output","WCDtankSim info and digitalized output");
		ADC_Output->Branch("PDG_Code",&(PMTEventInfo->PDG_Code));
		ADC_Output->Branch("Energy",&(PMTEventInfo->Energy));
		ADC_Output->Branch("Zenith_angle",&(PMTEventInfo->Zenith_angle));
		ADC_Output->Branch("Direction",&(PMTEventInfo->Direction));
		ADC_Output->Branch("Deposited_Energy",&(PMTEventInfo->Deposited_Energy));
		ADC_Output->Branch("Track_Length",&(PMTEventInfo->Track_Length));
		ADC_Output->Branch("Cherenkov_Photon_Count",&(PMTEventInfo->Cherenkov_Photon_Count));
		ADC_Output->Branch("PMT_Photon_Count",&(PMTEventInfo->PMT_Photon_Count));
		ADC_Output->Branch("Digitalized_Data",&ADC_array);

 		ADC_cbuffer = new boost::circular_buffer<Double_t>( ADC_Samples_per_Pulse );

	}

	/* Datos para la salida histogram*/
	std::vector< Long_t >* ADCmaximumAll = 0;
	if( outputType == elecADCoutput::histogram ) ADCmaximumAll = new std::vector< Long_t >;

	/* Archivo de salida para PAAfile */
	std::ofstream* PAAOutputFile = 0;
	if( outputType == elecADCoutput::PAAfile )
	{
		PAAOutputFile = new std::ofstream("ElecOutput.paa", std::ios::out | std::ios::binary);
   		if(!PAAOutputFile) 
		{
      		std::cout << "Cannot open file for output!" << std::endl;
     		return;
   		}
		
		// Coloca el identificador de archivo
		PAAOutputFile->write("PAA 01\n", 7);
		
		// Headers en modo texto
		PAAOutputFile->seekp(8);
		PAAOutputFile->write("Elec: WCD tank electronics simulation program\n", 46);
		PAAOutputFile->write("Version: 0.3-beta\n", 18);
		PAAOutputFile->write("Build: Unknown\n", 15);

		time_t now = time(NULL);
		char itDateTime[30];
        struct tm *it = localtime(&now);
        strftime(itDateTime, sizeof(itDateTime)-1, "%c %Z", it);

		PAAOutputFile->write(itDateTime, 28);
		PAAOutputFile->write("\n", 1);

		PAAOutputFile->write("Digitalized results from WCDTankSim data\n", 41);
		
		// Endianness check number 
		PAAOutputFile->seekp (520);
		uint32_t eci = 0x10203040;
		PAAOutputFile->write( (char *)&eci, 4);

		// Número de puntos por pulso (ps)
		uint32_t ps = (uint32_t)ADC_Samples_per_Pulse;
		PAAOutputFile->write( (char *)&ps, 4);

		// Número de puntos por archivo (pc)
		uint32_t pc = (uint32_t)NumberOfEvents;
		PAAOutputFile->write( (char *)&pc, 4);

		// Nivel de trigger (tk)
		int32_t tl = (int32_t)std::floor( ( ADC_Trigger_Voltage + ADC_VSignal_Offset ) / ADC_Dv ) ;
		PAAOutputFile->write( (char *)&tl, 4);

		// Nueve el puntero de incerción al pundo donde deben inciar los datos
		PAAOutputFile->seekp( 640 );

 		ADC_cbuffer = new boost::circular_buffer<Double_t>( ADC_Samples_per_Pulse );
	}


	for( Long_t Event = 0; Event < NumberOfEvents; Event ++)
	{
		std::cout << "\rEvent: " << std::setw(8) << std::setfill('0') << Event << std::flush;

		std::fill (Time_array.begin(),Time_array.end(),0.0);
		std::fill (ADC_array.begin(),ADC_array.end(),0.0);

		Double_t t_cur = VoltageSignalData->GetMinTime( Event );
		Double_t t_init = t_cur - Time_increment * ( ADC_Pre_Trigger_Samples - 1 ) - gRandom->Uniform( Time_increment );

		Long_t ADC_max = 10000000;

		bool Trigger_exceeded = false;
		bool ADC_overflow = false;
		bool ADC_underflow = false;

		t_cur = t_init;
		for( short i = 0; i < ADC_Pre_Trigger_Samples ; i++)
		{
			ADC_Vtmp = VoltageSignalData->GetVoltage( Event, t_cur) + ADC_VSignal_Offset;
			ADC_Btmp = std::floor(  ADC_Vtmp / ADC_Dv ); 

			if( ADC_Btmp < 0 )
				ADC_Btmp = 0.0;
			else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
				ADC_Btmp = ADC_bins - 1;

			Time_array.at(i) = t_cur;
			t_cur += Time_increment;

			if( outputType == elecADCoutput::histogram ) 
				if( ADC_Btmp < ADC_max ) ADC_max = ADC_Btmp;
			
			if( outputType == elecADCoutput::PAAfile ) 
				ADC_cbuffer->push_back( ADC_Btmp );

			if( outputType == elecADCoutput::ROOTfile ) 
				ADC_cbuffer->push_back( ADC_Btmp );
		}


		for( Long_t i = ADC_Pre_Trigger_Samples ; i < ADC_Samples_per_Pulse ; i++ )
		{

			ADC_Vtmp = VoltageSignalData->GetVoltage( Event, t_cur) + ADC_VSignal_Offset; 
			ADC_Btmp = std::floor(  ADC_Vtmp / ADC_Dv );
			
			if ( ADC_Vtmp < ( ADC_Trigger_Voltage + ADC_VSignal_Offset ) && !Trigger_exceeded )
			{
				Trigger_exceeded = true;
				Trigger_point = i;
			} 
			if( ADC_Btmp > ( ADC_bins - 1 ) )
			{
				ADC_Btmp = ADC_bins - 1;
				ADC_overflow = true;
			}
			else if (  ADC_Btmp < 0 )
			{
				ADC_Btmp =  0.0;
				ADC_underflow = true;
			}

			Time_array.at(i) = t_cur;
			t_cur += Time_increment;

			if( outputType == elecADCoutput::histogram ) 
				if( ADC_Btmp < ADC_max ) ADC_max = ADC_Btmp;
			
			if( outputType == elecADCoutput::PAAfile ) 
				ADC_cbuffer->push_back( ADC_Btmp );

			if( outputType == elecADCoutput::ROOTfile ) 
				ADC_cbuffer->push_back( ADC_Btmp );
		}

		if(Trigger_exceeded)
		{
			if( outputType == elecADCoutput::histogram ) ADCmaximumAll->push_back(ADC_max);

			if( outputType == elecADCoutput::PAAfile ) 
			{
				if( Trigger_point > ADC_Pre_Trigger_Samples  )
					Start_point = Trigger_point - ADC_Pre_Trigger_Samples ;
				else
					Start_point = ADC_Samples_per_Pulse - ADC_Pre_Trigger_Samples  + Trigger_point;

				for(Long_t i = 0; i < ADC_Samples_per_Pulse ; ++i)
				{
					ADC_array[ i ] = ADC_cbuffer->at( ( Start_point + i ) % ADC_Samples_per_Pulse );
					uint16_t cp = (uint16_t)ADC_array[ i ];
					PAAOutputFile->write( (char *)&cp, 2);
				}
			}

			if( outputType == elecADCoutput::ROOTfile ) 
			{
				if( Trigger_point > ADC_Pre_Trigger_Samples  )
					Start_point = Trigger_point - ADC_Pre_Trigger_Samples ;
				else
					Start_point = ADC_Samples_per_Pulse - ADC_Pre_Trigger_Samples  + Trigger_point;

				for(Long_t i = 0; i < ADC_Samples_per_Pulse ; ++i)
				{
					ADC_array[ i ] = ADC_cbuffer->at( ( Start_point + i ) % ADC_Samples_per_Pulse );
				}
				*PMTEventInfo = VoltageSignalData->GetEventInfo(Event);
				ADC_Output->Fill();
			}

			if ( ADC_overflow )
				std::cout << "\r" << std::setw(8) << std::setfill('0') << Event << ": ADC_overflow" << std::endl;
			if ( ADC_underflow )
				std::cout << "\r" << std::setw(8) << std::setfill('0') << Event << ": ADC_undeflow" << std::endl;

		}
		else
		{
			std::cout << "\r" << std::setw(8) << std::setfill('0') << Event << ": Trigger not exceeded" << std::endl;
		}
	}

	if( outputType == elecADCoutput::histogram )
	{
		int argc = 1;
		char* argv[] = { (char*)"elec" };

		TApplication theApp("ElecSim", &argc, argv);
		if (gROOT->IsBatch()) 
		{
	  		fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
	  		return;
		}

		Int_t histll = (Int_t)*std::min_element(ADCmaximumAll->begin(), ADCmaximumAll->end());
		Int_t histul = (Int_t)*std::max_element(ADCmaximumAll->begin(), ADCmaximumAll->end());
		std::cout << "\r********************************" << std::endl;
		std::cout << "Size of ADCmaximum: " << ADCmaximumAll->size() << std::endl;
		std::cout << "Min of ADCmaximun: " << histll << std::endl;
		std::cout << "Max of ADCmaximun: " << histul << std::endl;
		std::cout << "********************************" << std::endl;

		TH1 *ADCmaxHistAll 		= new TH1D("H1","ADC pulse maximum (All)",100,histll, histul);

		for( auto i: *ADCmaximumAll )
			ADCmaxHistAll->Fill( i );

		TCanvas *ShowHists = new TCanvas("C1", "ElecSim", 600, 400);

		ADCmaxHistAll->Draw();
		ShowHists->Update();

		ShowHists->Modified();
		ShowHists->Connect("Closed()", "TApplication", gApplication, "Terminate()"); //new


		theApp.Run();
	}

	if( outputType == elecADCoutput::PAAfile ) 
	{
		std::cout<< "\rFile ElecOutput.paa written with " << NumberOfEvents << " events data " << std::endl;
		PAAOutputFile->close();
	}

	if( outputType == elecADCoutput::ROOTfile ) 
	{
		ADC_Output->Write();
		ROOTOutputFile->Close();
		std::cout<< "\rFile ElecOutput.root written with " << NumberOfEvents << " events data " << std::endl;
	}
	
	if( outputType == elecADCoutput::none ) 
	{
		std::cout<< "\r----- :) ----- " << std::endl;
	}

}
