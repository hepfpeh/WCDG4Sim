/*
 * elecADCsim.hh
 *
 *  Created on: 23/10/2017
 *      Author: hepf
 */

#ifndef INCLUDE_ELECTRONICS_ELECADC_HH_
#define INCLUDE_ELECTRONICS_ELECADC_HH_

#include "elecRCequivalent.hh"
#include "elecVoltageSignal.hh"

class elecVoltageSignal;

enum class elecADCoutput { file, histogram, none };

class elecADC {
public:
	elecADC(void);
	~elecADC(void);

	void DigitalizeVoltageSignal( elecVoltageSignal* VoltageSignalData, elecADCoutput outputType );

private:
	Int_t		ADC_bits;
	Double_t	ADC_Vref;
	Double_t	ADC_VSignal_Offset;
	Double_t	ADC_Sample_Rate;
	Int_t		ADC_Samples_per_Pulse;
	Double_t	ADC_Trigger_Voltaje;
	Int_t		ADC_Pre_Trigger_Samples;
};



#endif /* INCLUDE_ELECTRONICS_ELECADC_HH_ */
