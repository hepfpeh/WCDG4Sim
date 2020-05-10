/*
 * WCDtankStackingAction.cc
 *
 *  Created on: 17/10/2016
 *      Author: hepf
 */


#include "WCDtankStackingAction.hh"
#include "WCDtankEventAction.hh"

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//WCDtankStackingAction::WCDtankStackingAction()
//  : G4UserStackingAction(),
//    fScintillationCounter(0), fCerenkovCounter(0)
//{}

/* Modification to get total number of Cherenkov photons */
WCDtankStackingAction::WCDtankStackingAction(WCDtankEventAction* eventAction)
  : G4UserStackingAction(),
  EventAction(eventAction)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WCDtankStackingAction::~WCDtankStackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
WCDtankStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  { // particle is optical photon
    if(aTrack->GetParentID()>0)
    { // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
        EventAction->IncrementCherekovPhotons();
    }
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankStackingAction::NewStage()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WCDtankStackingAction::PrepareNewEvent()
{
  EventAction->ResetCherekovPhotons();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


