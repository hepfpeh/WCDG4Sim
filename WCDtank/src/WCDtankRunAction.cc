#include "WCDtankRunAction.hh"
#include "WCDtankPrimaryGeneratorAction.hh"
#include "WCDtankDetectorConstruction.hh"
#include "WCDtankRun.hh"
#include "WCDtankAnalysis.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <ctime>
#include <string>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankRunAction::WCDtankRunAction()
: G4UserRunAction()
{
	G4dVectorPointer = new std::vector<G4double>;

	// Create analysis manager
	// The choice of analysis technology is done via selectin of a namespace
	// in B4Analysis.hh
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	

	analysisManager->SetVerboseLevel(1);
	analysisManager->SetFirstHistoId(0);

	// Book histograms, ntuple
	//

	// Creating histograms
	//analysisManager->CreateH1("1","Photon count at PMT", 40, 0., 400);


	// analysisManager->CreateNtuple("WCDtank", "Data");
	// analysisManager->CreateNtupleIColumn("PDG_Code");
	// analysisManager->CreateNtupleIColumn("Direction");
	// analysisManager->FinishNtuple();

	analysisManager->CreateH1("Edep","Deposited Energy",200,60.0,140*MeV);
	analysisManager->CreateH1("Tlen","Track Length",200,400.0,800*mm);
	analysisManager->CreateH1("ChFC","Cherenkov Photon Count",200,16000,25000);
	analysisManager->CreateH1("PMTFC","PMT Photoelectron Count",200,6000,14000);
	analysisManager->CreateH1("PMTCT","PMT Photoelectron Creation time",200,0,60*ns);

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*
void WCDtankRunAction::SetVectorPointer( std::vector<G4double>* aVectorPointer )
{
	G4dVectorPointer = aVectorPointer;
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankRunAction::~WCDtankRunAction()
{
//	G4cout << "~WCDtankRunAction()" << G4endl;
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* WCDtankRunAction::GenerateRun()
{
  return new WCDtankRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankRunAction::BeginOfRunAction(const G4Run*)
{
	//G4cout << "WCDtankRunAction::BeginOfRunAction" << G4endl;
	// Get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


	// Open an output file
	//
	G4String fileName = "WCDtank-";

	std::time_t TimeInSeconds;
	struct tm * TimeStruct;
	char buffer[80];
	std::time ( &TimeInSeconds );
	TimeStruct = std::localtime( &TimeInSeconds );
	std::strftime( buffer, 80, "%Y%m%d-%H%M%S", TimeStruct);

	G4String TimeString =  buffer;
	fileName.append(TimeString);
	analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankRunAction::EndOfRunAction(const G4Run*)
{
	// print histogram statistics
	//
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//	if ( analysisManager->GetH1(1) ) {
//		G4cout 	<< G4endl << " ----> print histograms statistic ";
//		G4cout 	<< "for the entire run " << G4endl << G4endl;
//		G4cout 	<< " Phothon count : mean = "
//				<< analysisManager->GetH1(1)->mean() << G4endl;
//	}

	// save histograms & ntuple
	//
	std::cout << "\n*****************************************" << std::endl;
	analysisManager->Write();
	analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
