/*
 * elecVoltageSignal.hh
 *
 *  Created on: 27/03/2020
 *      Author: Héctor Pérez
 */

#ifndef INCLUDE_ELECTRONICS_ELECVOLTAGESIGNAL_HH_
#define INCLUDE_ELECTRONICS_ELECVOLTAGESIGNAL_HH_


#include "elecTypes.hh"


class elecVoltageSignal {
public:
	elecVoltageSignal(void);
	~elecVoltageSignal(void);

	Long64_t 	GetNumberOfEvents(void){ return Entries; };
	Double_t 	GetMinTime( Long_t Event );
	Double_t 	GetVoltage( Long_t Event, Double_t Time );

	void		SetKConstant( Double_t K ) { KConstant = K; };
	void		SetBaseLineVoltageMean( Double_t VMean ) { VBLMean = VMean; };
	void		SetBaseLineVoltageDev( Double_t VDev ) { VBLDev = VDev; };

	void		AppendEventData( elecEventSignal* EventData );

private:
	Long64_t 				Entries;
    elecEventCollection*     PMTEventsData;
	Double_t				KConstant;
	/* 
	 * VBLMean es el voltaje de linea (baseline)
	 * VBLdev es la desviación estándar en este voltaje.
	 * Con esto se genera ruido gaussiano
	 */
	Double_t				VBLMean;
	Double_t				VBLDev;
	/*
	 * Cache: Estas variables son para recordar
	 * los parámetros con que se llamó por última
	 * vez a la funcion GetVoltage para evitar 
	 * recorer toda la tabla de valores en cada
	 * llamada secuencial del mismo evento.
	 */
	Long_t					Cache_Event;
	Double_t				Cache_Time;
	Int_t					Cache_Table_Pos;

};



#endif /* INCLUDE_ELECTRONICS_ELECVOLTAGESIGNAL_HH_ */
