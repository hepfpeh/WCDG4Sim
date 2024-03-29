#include "WCDtankSensitiveDetector.hh"
#include "WCDtankAnalysis.hh"
#include "WCDtankRunAction.hh"

#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankSensitiveDetector::WCDtankSensitiveDetector(G4String name)
  : G4VSensitiveDetector(name),  WCDtankSDHitsCollection(0)
{
	  //G4cout << "WCDtankSensitiveDetector::WCDtankSensitiveDetector" << G4endl;
	  collectionName.insert("pmtHitCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankSensitiveDetector::~WCDtankSensitiveDetector()
{
//	G4cout << "~WCDtankSensitiveDetector()" << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankSensitiveDetector::Initialize(G4HCofThisEvent* hitsCE)
{
//	  G4cout << "WCDtankSensitiveDetector::Initialize" << G4endl;
	  WCDtankSDHitsCollection = new WCDtankHitsCollection
	                      (SensitiveDetectorName,collectionName[0]);
	  //Store collection with event and keep ID
	  static G4int hitCID = -1;
	  if(hitCID<0){
	    hitCID = GetCollectionID(0);
	  }
	  hitsCE->AddHitsCollection( hitCID, WCDtankSDHitsCollection );

	  WCDtankHit* hit = new WCDtankHit;
	  WCDtankSDHitsCollection->insert(hit);

	  const WCDtankRunAction* runAction = static_cast<const WCDtankRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
	  hit->SetPhotoElectricPMTPhotonTime( runAction->GetVectorPointer() );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool WCDtankSensitiveDetector::ProcessHits(G4Step* theStep,G4TouchableHistory* ){
	//G4cout << "WCDtankSensitiveDetector::ProcessHits" << G4endl;
	//G4cout << "WCDtankSDHitsCollection value: " << WCDtankSDHitsCollection << G4endl;
	WCDtankHit* hit;// = new WCDtankHit;
	hit = (*WCDtankSDHitsCollection)[0];
	hit->IncPMTPhotonCount();
//	hit->IncPhotonCount();


	G4Track* theTrack = theStep->GetTrack();
	G4double ParticleEnergy = theTrack->GetKineticEnergy();
	G4double ArrivalTime = theTrack->GetGlobalTime();


	// G4ParticleDefinition* particleType = theTrack->GetDefinition();
	// G4String ParticleName = particleType->GetParticleName();
	// G4cout<< hit->GetPMTPhotonCount() << "\t" << ParticleName << "\t" << G4BestUnit(ParticleEnergy,"Energy");

	/*
	 * Implementación de la eficiencia cuántica en el PMT
	 *
	 * Esta es una forma bastante fea, inflexible e inentendible de hacer esto,
	 * pero es una primera implementación. Se compara la energía del fotón y
	 * se asigna un valor de QE que debe ser comparado con un número aleatorio
	 * generado de forma uniforme entre 0 y 1. Solo si el número aleatorio es menor
	 * que QE, se cuenta el hit.
	 */


	G4double QuantumEfficiency;

	if (ParticleEnergy < 1.95*eV )
		QuantumEfficiency = 0;
	else if (ParticleEnergy < 2.05*eV )
		QuantumEfficiency = 0.0047;
	else if (ParticleEnergy < 2.19*eV )
		QuantumEfficiency = 0.023;
	else if (ParticleEnergy < 2.34*eV )
		QuantumEfficiency = 0.056;
	else if (ParticleEnergy < 2.51*eV )
		QuantumEfficiency = 0.10;
	else if (ParticleEnergy < 2.72*eV )
		QuantumEfficiency = 0.16;
	else if (ParticleEnergy < 2.96*eV )
		QuantumEfficiency = 0.21;
	else if (ParticleEnergy < 3.25*eV )
		QuantumEfficiency = 0.23;
	else if (ParticleEnergy < 3.59*eV )
		QuantumEfficiency = 0.21;
	else if (ParticleEnergy < 4.03*eV )
		QuantumEfficiency = 0.15;
	else if (ParticleEnergy < 4.58*eV )
		QuantumEfficiency = 0.058;
	else QuantumEfficiency = 0;

	G4double Nrand = G4UniformRand();

	// G4cout << "\tQE: " << QuantumEfficiency;

	if ( Nrand < QuantumEfficiency ){
		hit->AddPhotoElectricPMTPhotonTime( ArrivalTime );
		hit->IncPMTPhotoElectronCount();
		// G4cout << "\tHit counted";
	}

	// G4cout << G4endl;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankSensitiveDetector::EndOfEvent(G4HCofThisEvent* ) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankSensitiveDetector::clear() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankSensitiveDetector::DrawAll() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankSensitiveDetector::PrintAll() {}
