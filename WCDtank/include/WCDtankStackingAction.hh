/*
 * WCDtankStakingAction.hh
 *
 *  Created on: 17/10/2016
 *      Author: hepf
 */

#ifndef WCDtankSTACKINGACTION_HH_
#define WCDtankSTACKINGACTION_HH_

#include "globals.hh"
#include "G4UserStackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/* Modification to get total number of Cherenkov photons */
class WCDtankEventAction;
class G4LogicalVolume;

class WCDtankStackingAction : public G4UserStackingAction
{
  public:
    //WCDtankStackingAction();
    /* Modification to get total number of Cherenkov photons */
    WCDtankStackingAction(WCDtankEventAction* eventAction);
    virtual ~WCDtankStackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
    WCDtankEventAction*  EventAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif /* WCDtankSTACKINGACTION_HH_ */
