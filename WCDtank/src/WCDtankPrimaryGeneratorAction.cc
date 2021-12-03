
#include "WCDtankPrimaryGeneratorAction.hh"
#include "WCDtankPrimaryGeneratorMessenger.hh"
#include "WCDtankRandom.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4String.hh"

#include "Randomize.hh"

#include <cmath>
#include <map>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankPrimaryGeneratorAction::WCDtankPrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
  WaterTube(0),
  WorldBox(0),
  ParticleDirection(All),
  fGunMessenger(0),
  ParticleZenithAngle(0),
  PrimaryParticleDirectionIsVertical(FALSE)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="mu-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,-1.0));
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*m,0.0*m,2.0*m));
  fParticleGun->SetParticleEnergy(4.0*GeV);

  //create a messenger for this class
  fGunMessenger = new WCDtankPrimaryGeneratorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankPrimaryGeneratorAction::~WCDtankPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankPrimaryGeneratorAction::SetParticleDirection(G4String aDirection)
{
	std::map< G4String, Direction > aMap;

    aMap["Vertical"] 	= Vertical;
    aMap["NonVertical"] = NonVertical;
    aMap["All"] 		= All;

    ParticleDirection = aMap[ aDirection ];
}
