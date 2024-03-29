#include <iostream>
#include <iomanip>

#include "WCDtankEventAction.hh"
#include "WCDtankRun.hh"
#include "WCDtankRunAction.hh"
#include "WCDtankPrimaryGeneratorAction.hh"
#include "WCDtankAnalysis.hh"
#include "WCDtankHit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SDManager.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankEventAction::WCDtankEventAction()
: G4UserEventAction(),
  DepositedEnergy(0.),
  TrackLength(0.),
  PrimaryParticlePDGcode(0.),
  PrimaryParticleEnergy(0.),
  PrimaryParticleZenithAngle(0.),
  PrimaryParticleIsVertical(FALSE),
  WCDtankSDHitsCollectionId(-1)

{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankEventAction::~WCDtankEventAction()
{
//	G4cout << "~WCDtankEventAction()" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankEventAction::BeginOfEventAction(const G4Event*)
{
	//G4cout << "WCDtankEventAction::BeginOfEventAction" << G4endl;
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	if(WCDtankSDHitsCollectionId<0)
		WCDtankSDHitsCollectionId=SDman->GetCollectionID("pmtHitCollection");

	//G4cout << "WCDtankEventAction:  WCDtankSDHitsCollectionId value: " << WCDtankSDHitsCollectionId << G4endl;
	DepositedEnergy = 0;
	TrackLength = 0;
	// Run conditions
	//  note: There is no primary generator action object for "master"
	//        run manager for multi-threaded mode.
	const WCDtankPrimaryGeneratorAction* generatorAction
		= static_cast<const WCDtankPrimaryGeneratorAction*>
	 	 (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
	if (generatorAction)
	{
		const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
		PrimaryParticleName = particleGun->GetParticleDefinition()->GetParticleName();
		PrimaryParticlePDGcode = particleGun->GetParticleDefinition()->GetPDGEncoding();
		PrimaryParticleEnergy = particleGun->GetParticleEnergy();
		PrimaryParticleZenithAngle = generatorAction->GetParticleZenithAngle();
		PrimaryParticleIsVertical = generatorAction->GetParticleVerticalDirectionFlag();
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankEventAction::EndOfEventAction(const G4Event* anEvent)
{

	// Print the total amount of energy deposited in this event
//	G4cout << "-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-" << G4endl;
	std::cout << "\rEvent ID: " << std::setw(5) << std::setfill('0') << anEvent->GetEventID() << std::flush;
/*	G4cout << "Primary particle PDG code: " << PrimaryParticlePDGcode << G4endl;
	G4cout << "Primary particle name: " << PrimaryParticleName << G4endl;
	G4cout << "Primary particle energy: " << G4BestUnit(PrimaryParticleEnergy,"Energy") << G4endl;
	G4cout << "Primary particle incident Zenith angle: " << G4BestUnit(PrimaryParticleZenithAngle,"Angle") << G4endl;
	G4cout << "Primary particle direction is vertical?: ";

	if( PrimaryParticleIsVertical )
		G4cout << "True" << G4endl;
	else
		G4cout << "False" << G4endl;
	G4cout << "Energy deposited: " << G4BestUnit(DepositedEnergy,"Energy") << G4endl;
	G4cout << "Track length: " << G4BestUnit(TrackLength,"Length") << G4endl;
*/
	G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent(); //Get the hit collections
	WCDtankHitsCollection* pmtHC = 0;

	if(hitsCE){
		if(WCDtankSDHitsCollectionId>=0) pmtHC = (WCDtankHitsCollection*)(hitsCE->GetHC(WCDtankSDHitsCollectionId));
	}

	G4int NumberOfPMTPhotons = (*pmtHC)[0]->GetPMTPhotonCount();
	// G4int NumberOfPMTPhotoElectrons = (*pmtHC)[0]->GetPMTPhotoElectronCount();

	// G4cout << "Cherenkov Photons in this event: " << CherenkovPhotonsEmitted << G4endl;

	// get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// fill ntuple
	analysisManager->FillNtupleIColumn(0, PrimaryParticlePDGcode);
	analysisManager->FillNtupleDColumn(1, PrimaryParticleEnergy);
	analysisManager->FillNtupleDColumn(2, PrimaryParticleZenithAngle);

	if( PrimaryParticleIsVertical )
		analysisManager->FillNtupleIColumn(3, 1);
	else
		analysisManager->FillNtupleIColumn(3, 0);

	analysisManager->FillNtupleDColumn(4, DepositedEnergy);
	analysisManager->FillNtupleDColumn(5, TrackLength);
	analysisManager->FillNtupleIColumn(6, CherenkovPhotonsEmitted);
	analysisManager->FillNtupleIColumn(7, NumberOfPMTPhotons);
	// analysisManager->FillNtupleIColumn(8, NumberOfPMTPhotoElectrons);


	// const WCDtankRunAction* runAction = static_cast<const WCDtankRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());

	// G4cout << "Photon time size: " << (*pmtHC)[0]->GetPhotonTimeSize() << " pointer: " << runAction->GetVectorPointer() << G4endl;
	
	// G4cout << "\n"
	// 	   << anEvent->GetEventID()
	//        << " PMT PhotoElectric count: "
	//        << (*pmtHC)[0]->GetPMTPhotoElectronCount()
	// 	   << " PhotoElectricTime size: "
	// 	   << (*pmtHC)[0]->GetPhotoElectricPMTPhotonTimeSize()
	// 	   << " Arrived: "
	// 	   << NumberOfPMTPhotons
	// 	   << " ratios: "
	// 	   << ( (*pmtHC)[0]->GetPhotoElectricPMTPhotonTimeSize() + 0.0 ) / NumberOfPMTPhotons
	// 	   << " "
	//        << ( (*pmtHC)[0]->GetPMTPhotoElectronCount() + 0.0 ) / NumberOfPMTPhotons
	// 	   << G4endl;
	
	analysisManager->AddNtupleRow();

	(*pmtHC)[0]->ResetPMTPhotonCount();
	(*pmtHC)[0]->ResetPMTPhotoElectronCount();
	(*pmtHC)[0]->ResetPhotoElectricPMTPhotonTime();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
