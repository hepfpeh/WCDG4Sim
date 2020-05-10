/*
 * elecVoltageSignal.cc
 *
 *  Created on: 28/03/2020
 *      Author: Héctor Pérez
 */



#include "TRandom3.h"

#include "elecVoltageSignal.hh"


elecVoltageSignal::elecVoltageSignal(void)
{
    Entries         = 0;
    PMTEventsData   = new elecEventCollection();
	KConstant       = 0;
	VBLMean         = 0.0; // Voltios
	VBLDev          = 0.001; // Voltios
    Cache_Event     = 0;
    Cache_Time      = 0;
    Cache_Table_Pos = 0;
}

elecVoltageSignal::~elecVoltageSignal(void)
{
    if( PMTEventsData ) delete PMTEventsData;
}

Double_t    elecVoltageSignal::GetMinTime( Long_t Event )
{
    if( Event > Entries )
    {
        std::cout << "Invalid event" << std::endl;
        return 0.0;
    }

    return ((PMTEventsData->at( Event )).at(1)).Time;
}

Double_t 	elecVoltageSignal::GetVoltage( Long_t Event, Double_t Time )
{
    if( Event > Entries )
    {
        std::cout << "Invalid event" << std::endl;
        return 0.0;
    }
    
    Double_t Min_time = ((PMTEventsData->at( Event )).at(1)).Time;
    Int_t Table_pos = 0;
	Int_t Table_size = (PMTEventsData->at( Event )).size();
	Double_t Vtmp = 0;

    if( Event == Cache_Event )
    {
        if( Time > Cache_Time )
        {
            Table_pos = Cache_Table_Pos;
        }
        else
        {
            Cache_Time = Time;
        }
    }
    else
    {
        Cache_Event = Event;
    }
    

    if( Time < Min_time )
    {
        Vtmp = gRandom->Gaus( VBLMean, VBLDev );
    }
    else
    {
        while( ( (PMTEventsData->at( Event ).at( Table_pos )).Time < Time ) && ( Table_pos < ( Table_size - 1 ) ) )
				Table_pos++;
		
        if( Table_pos-- < 0 ) Table_pos = 0;
		
        Double_t V_n = ( ( PMTEventsData->at( Event ) ).at( Table_pos ) ).Voltage;
		Double_t t_n = ( ( PMTEventsData->at( Event ) ).at( Table_pos ) ).Time;

        Vtmp =  V_n * exp( -( KConstant ) * ( Time - t_n ) );
		Vtmp += gRandom->Gaus( VBLMean, VBLDev );
    }
    
    Cache_Table_Pos = Table_pos;

    return Vtmp;
}

void		elecVoltageSignal::AppendEventData( elecEventSignal* EventData )
{
    PMTEventsData->push_back( *EventData );
    Entries = PMTEventsData->size();
}
