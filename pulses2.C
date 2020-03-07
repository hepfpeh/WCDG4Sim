// root script file

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include <cmath>
#include <algorithm>
#include <boost/circular_buffer.hpp>

void GetHunapuPulse(const char* HunapuRootFileName, Int_t PulseNumber){
	Long64_t Entries;
	std::vector<double> *PulseData = new std::vector<double>();

	TFile *HunapuRootFile = new TFile( HunapuRootFileName );
	TTree *Hunapu = (TTree*)HunapuRootFile->Get("WCDtank;1");
	Entries = Hunapu->GetEntriesFast();	
	Hunapu->SetBranchAddress("Photon_Arrival_Time",&PulseData);

	if (PulseNumber < Entries ) Hunapu->GetEntry(PulseNumber);
	else { 
		std::cout << "Invalid pulse Number" << std::endl;
		return;
	}
	
	Int_t DataEntries = PulseData->size();
	std::cout << "Number of photons: " << DataEntries << std::endl;
	
	// Ordenando por tiempo

	std::sort( PulseData->begin(), PulseData->end() );


	Double_t DataMax = PulseData->at(DataEntries-1);
	Double_t DataMin = PulseData->at(0);

// Parametros para calculo de curva de carga
	Double_t Circuit_R = 50; // en ohms.
	Double_t Circuit_C = 0.4; // en ns/ohm = nF.
	Double_t Circuit_V = 1500.0; // en V.
	Double_t Const_k = 1.0 / ( Circuit_R * Circuit_C ); // en 1/ns.
	Double_t Dep_q = -1.6e-13; // en C. ( se fija como e x 10^6)

//Parametros de captura
	Double_t Sample_Rate = 125.0; // en MHz.
	Long_t NumberOfSamples = 32;
	Long_t Trigger_ADC_value = 8100;
	Long_t Pre_trigger_points = 5;

// Parametros ADC
	Double_t ADC_Vref = 2.0; // en Volts. (GND=0)
	Int_t ADC_bits = 14;

	Long_t ADC_bins = std::pow( 2, ADC_bits );
	Long_t ADC_Zero_Level =  ADC_bins / 2;
	Double_t ADC_Dv = ADC_Vref / ADC_bins;

// Parametros de voltaje linea base. Se considera ruido gaussiano con media igual al voltaje
// de offset V_BL_offset y desviacion V_BL_desv. 
	Double_t V_BL_offset = 0; // en Volts.
	Double_t V_BL_desv = 0.001; // en Volts.

	std::vector < std::vector<Double_t> > *Data_table = new std::vector < std::vector<Double_t> >();
	std::vector<Double_t> *vect_tmp = new std::vector<Double_t>(2);
	Double_t t_cur = 0;
	Double_t t_ant;
	Double_t A_cur = 0;
	Double_t A_ant;

	vect_tmp->push_back(t_cur);
	vect_tmp->push_back(A_cur);

	Data_table->push_back( *vect_tmp );

	vect_tmp->clear();

	t_ant = t_cur;
	A_ant = A_cur;

	for( int i = 0 ; i < DataEntries ; i++ ){
		t_cur = PulseData->at( i );
		A_cur = 1.0 + A_ant * exp( -Const_k * (t_cur - t_ant) );
		vect_tmp->push_back(t_cur);
		vect_tmp->push_back(A_cur);
//		std::cout << "Time: " << t_cur << " A_cur: "<< A_cur << std::endl;
		Data_table->push_back(*vect_tmp);
		vect_tmp->clear();

//		DataHist->Fill( t_cur );
//		if ( t_cur > DataMax ) DataMax = t_cur;
//		if ( t_cur < DataMin ) DataMin = t_cur;

		t_ant = t_cur;
		A_ant = A_cur;
	}

	Int_t Data_table_size = Data_table->size();
	Int_t Data_table_size_in_Bytes = 2*sizeof(Double_t)*Data_table->size();
	std::cout << "Data table size: " << Data_table_size_in_Bytes <<  " in bytes" << std::endl;

	//TH1 *DataHist = new TH1D("H1","Photon arrival time",25,0,DataMax);
	//for( int i = 0 ; i < DataEntries ; i++ )
	//	DataHist->Fill( PulseData->at( i ) );


	std::cout << "Max time: " << DataMax  << std::endl;
	std::cout << "Min time: " << DataMin  << std::endl;

	Double_t Time_increment = 1000.0 / Sample_Rate;
	//Long_t NumberOfSamples = floor( (DataMax-DataMin) / Time_increment );
	std::cout << "Number of Samples: " << NumberOfSamples << std::endl;
	DataMax = NumberOfSamples*Time_increment;
	Double_t t_init = DataMin - Time_increment * ( Pre_trigger_points - 1 ) - gRandom->Uniform( Time_increment );

/*
	TH1 *DataHist = new TH1D("H1","Photon arrival time",25,t_init,DataMax);
	for( int i = 0 ; i < DataEntries ; i++ )
		DataHist->Fill( PulseData->at( i ) );
*/


	Double_t Time_array[ NumberOfSamples ];
	Double_t Carge_array[ NumberOfSamples ];
	boost::circular_buffer<Double_t> *ADC_cbuffer = new boost::circular_buffer<Double_t>( NumberOfSamples );
	Double_t ADC_array[ NumberOfSamples ];
	Double_t ADC_Vtmp = 0;
	Long_t ADC_Btmp = 0;

	t_cur = t_init;

	Double_t t_n=0;
	Double_t A_n=0;
	Double_t Carge_tmp;

	Int_t Table_pos = 0;

	Bool_t Triggered = false;
	Long_t Trigger_point=0;

	//Long_t Post_trigger_points = NumberOfSamples - Pre_trigger_points;

// Este ciclo genera los puntos antes del trigger.
	for( Long_t i = 0 ; i < Pre_trigger_points; i++ ){

		ADC_Vtmp = gRandom->Gaus( V_BL_offset, V_BL_desv ); 
		
		ADC_Btmp = ADC_Zero_Level + std::floor( ADC_Vtmp / ADC_Dv );
/*
		if( ADC_Btmp < 0 )
			ADC_array[ i ] = 0.0;
		else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
			ADC_array[ i ] = ADC_bins - 1;
		else
			ADC_array[ i ] = ADC_Btmp;
*/

		if( ADC_Btmp < 0 )
			ADC_cbuffer->push_back( 0.0 );
		else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
			ADC_cbuffer->push_back( ADC_bins - 1 );
		else
			ADC_cbuffer->push_back( ADC_Btmp );


		Time_array[ i ] = t_cur;
		t_cur += Time_increment;
	}

//	std::cout << "t_cur: " << t_cur << std::endl;
	
// En este ciclo se genera la curva de carga.
	for( Long_t i = Pre_trigger_points; i < NumberOfSamples ; i++ ){

		while( ( (Data_table->at( Table_pos )).at(0) < t_cur ) && ( Table_pos < Data_table_size - 1 ) ) {
			t_n = (Data_table->at( Table_pos )).at(0);
			A_n = (Data_table->at( Table_pos )).at(1);
			Table_pos++;
//			std::cout << Table_pos << std::endl;
		}

		Time_array[ i ] = t_cur;
		std::cout << "Time: " << t_cur << " t_n: " << t_n << " A_n: "<< A_n << std::endl;
		Carge_array[ i ] = -Dep_q * A_n * exp( -Const_k * (t_cur - t_n) ) + Circuit_V * Circuit_C * (1.0e-9);
		ADC_Vtmp =  1 / ( Circuit_C * 1.0e-9 ) * Dep_q * A_n * exp( -Const_k * (t_cur - t_n) );
		ADC_Vtmp += gRandom->Gaus( V_BL_offset, V_BL_desv ); 
		/*
		if( ADC_Vtmp <= 0)
			ADC_array[ i ] = 0.0;
		else if( ADC_Vtmp >= ADC_Vref )
			ADC_array[ i ] = ADC_bins - 1.0;
		else
			ADC_array[ i ] = std::floor( ADC_Vtmp / ADC_Dv );
		*/

		ADC_Btmp = ADC_Zero_Level + std::floor( ADC_Vtmp / ADC_Dv );

/*
		if( ADC_Btmp < 0 )
			ADC_array[ i ] = 0.0;
		else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
			ADC_array[ i ] = ADC_bins - 1;
		else
			ADC_array[ i ] = ADC_Btmp;
		
*/

		if( ADC_Btmp < 0 )
			ADC_cbuffer->push_back( 0.0 );
		else if( ADC_Btmp > ( ADC_bins - 1 ) ) 
			ADC_cbuffer->push_back( ADC_bins - 1 );
		else
		{
			ADC_cbuffer->push_back( ADC_Btmp );
			if( !Triggered ) 
				if( ADC_Btmp < Trigger_ADC_value )
				{
					std::cout << "Triggered at " << i << std::endl;
					Triggered = true;
					Trigger_point = i;
				}
		}


//		std::cout << "Charge: " << Carge_array[ i ] << std::endl;

		t_cur += Time_increment;
	}

	Long_t Start_point;

	if( Trigger_point >  Pre_trigger_points )
                Start_point = Trigger_point - Pre_trigger_points ;
            else
                Start_point = NumberOfSamples - Pre_trigger_points + Trigger_point;

	for(Long_t i = 0; i < NumberOfSamples; ++i)
        ADC_array[ i ] = ADC_cbuffer->at( ( Start_point + i ) % NumberOfSamples );


//	TH1 *DataHistCumulative = DataHist->GetCumulative();
	TGraph *Circuit_C_Carge = new TGraph( NumberOfSamples, Time_array, ADC_array);
	Circuit_C_Carge->GetXaxis()->SetLimits(t_init,DataMax);
	
	TCanvas *ShowHists = new TCanvas("Algo", "otro", 600, 400);
//	ShowHists->Divide(1,2);
	
//	ShowHists->cd(1);
//	DataHist->Draw();
//	ShowHists->cd(2);
//	DataHistCumulative->Draw();
//    Circuit_C_Carge->Draw("acp");
    Circuit_C_Carge->Draw("AL*");
//	ShowHists->Update();
} 

