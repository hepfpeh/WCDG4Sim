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

	std::cout << "\rEvent ID: " << std::setw(5) << std::setfill('0') << anEvent->GetEventID() << std::flush;

	G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent(); //Get the hit collections
	WCDtankHitsCollection* pmtHC = 0;

	if(hitsCE){
		if(WCDtankSDHitsCollectionId>=0) pmtHC = (WCDtankHitsCollection*)(hitsCE->GetHC(WCDtankSDHitsCollectionId));
	}

	G4int NumberOfPMTPhotons = (*pmtHC)[0]->GetPMTPhotonCount();
	

	// get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// fill ntuple
	// analysisManager->FillNtupleIColumn(0, PrimaryParticlePDGcode);

	// if( PrimaryParticleIsVertical )
	// 	analysisManager->FillNtupleIColumn(1, 1);
	// else
	// 	analysisManager->FillNtupleIColumn(1, 0);

	// analysisManager->AddNtupleRow();

	analysisManager->FillH1(0, DepositedEnergy);
	analysisManager->FillH1(1, TrackLength);
	analysisManager->FillH1(2, CherenkovPhotonsEmitted);
	analysisManager->FillH1(3, NumberOfPMTPhotons);

	(*pmtHC)[0]->ResetPMTPhotonCount();
	(*pmtHC)[0]->ResetPMTPhotoElectronCount();
	(*pmtHC)[0]->ResetPhotoElectricPMTPhotonTime();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
