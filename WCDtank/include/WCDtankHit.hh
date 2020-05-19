/*
 * WCDtankHit.hh
 *
 *  Created on: 5/09/2016
 *      Author: hepf
 */

#ifndef WCDtankHIT_HH_
#define WCDtankHIT_HH_

#include <vector>

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class WCDtankHit : public G4VHit
{
  public:

    WCDtankHit();
    virtual ~WCDtankHit();
    WCDtankHit(const WCDtankHit &right);

    const WCDtankHit& operator=(const WCDtankHit &right);

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    inline void IncPMTPhotonCount(){WCDtankTotalPMTPhotons++;}
    inline void IncPMTPhotoElectronCount(){WCDtankTotalPMTPhotoElectrons++;}
    inline void AddPhotoElectricPMTPhotonTime( G4double Time ){WCDtankPhotoElectricPMTPhotonsTime->push_back( Time );}
    inline void SetPhotoElectricPMTPhotonTime( std::vector<G4double>* aVector ){ WCDtankPhotoElectricPMTPhotonsTime = aVector;}
    inline G4int GetPMTPhotonCount(){return WCDtankTotalPMTPhotons;}
    inline G4int GetPMTPhotoElectronCount(){return WCDtankTotalPMTPhotoElectrons;}
    inline G4int GetPhotoElectricPMTPhotonTimeSize(){return WCDtankPhotoElectricPMTPhotonsTime->size();}
    inline std::vector<G4double>* GetPhotoElecticPMTPhotonTime(){return WCDtankPhotoElectricPMTPhotonsTime;}
    inline void ResetPMTPhotonCount(){WCDtankTotalPMTPhotons=0;}
    inline void ResetPMTPhotoElectronCount(){WCDtankTotalPMTPhotoElectrons=0;}
    inline void ResetPhotoElectricPMTPhotonTime(){WCDtankPhotoElectricPMTPhotonsTime->clear();}

  private:

    G4int 					WCDtankTotalPMTPhotons;
    G4int           WCDtankTotalPMTPhotoElectrons;
    std::vector<G4double>	*WCDtankPhotoElectricPMTPhotonsTime;

};

typedef G4THitsCollection<WCDtankHit> WCDtankHitsCollection;

extern G4ThreadLocal G4Allocator<WCDtankHit>* WCDtankHitAllocator;


inline void* WCDtankHit::operator new(size_t){
  if(!WCDtankHitAllocator)
      WCDtankHitAllocator = new G4Allocator<WCDtankHit>;
  return (void *) WCDtankHitAllocator->MallocSingle();
}

inline void WCDtankHit::operator delete(void *aHit){
  WCDtankHitAllocator->FreeSingle((WCDtankHit*) aHit);
}

#endif /* WCDtankHIT_HH_ */
