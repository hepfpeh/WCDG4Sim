/*
*
*/

#include "elecCommandLine.hh"
#include "elecWCDtankPMTdata.hh"
#include "elecRCequivalent.hh"
#include "elecVoltageSignal.hh"
#include "elecADC.hh"


#include <vector>
#include <iostream>

int main(int argc, char **argv){

	elecCommandLine *myelecCommandLine = new elecCommandLine(argc, argv);

	std::string HunapuRootFileName;
	if( myelecCommandLine->GetGlobalArgumentValue("in_file", HunapuRootFileName) == 0 )
		std::cout << "Root data file name: " << HunapuRootFileName << std::endl;

	elecWCDtankPMTdata* HunapuPMTData = new elecWCDtankPMTdata();

	HunapuPMTData->LoadRootFile( HunapuRootFileName.c_str () );

	elecRCequivalent *HunapuRCequivalentCircuit = new elecRCequivalent();
	
	elecVoltageSignal* HunapuVoltageSignal = new elecVoltageSignal();
	
	HunapuRCequivalentCircuit->PMTPhotonsToVoltageSignal(HunapuPMTData, HunapuVoltageSignal);

	std::cout << "HunapuVoltageSignal Events: " << HunapuVoltageSignal->GetNumberOfEvents() << std::endl;

	elecADC* HunapuADC = new elecADC;

	HunapuADC->DigitalizeVoltageSignal(HunapuVoltageSignal);

	/*
	char aChar;

	std::cout << "Press any key and enter: " ;
	std::cin >> aChar;
	std::cout << std::endl;
	*/

	return 0;
}
