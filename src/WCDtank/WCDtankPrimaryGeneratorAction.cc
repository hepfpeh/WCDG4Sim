
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
  ParticleAzimuthAngle(0),
  PrimaryParticleDirectionIsVertical(FALSE)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="opticalphoton");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(-1.0,-1.0,0.0));
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0*m,0.0*m,0.0*m));
  fParticleGun->SetParticleEnergy(3.0*eV);

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
	  //this function is called at the beginning of each event
	  //

	  // In order to avoid dependence of PrimaryGeneratorAction
	  // on DetectorConstruction class we get solid_WaterTube volume
	  // from G4LogicalVolumeStore.


	  G4double WaterTube_radius = 0;
	  G4double WaterTube_halfHeight = 0;

	  if (!WaterTube)
	  {
	    G4LogicalVolume* WT_LV
	      = G4LogicalVolumeStore::GetInstance()->GetVolume("WaterTube_logic");
	    if ( WT_LV ) WaterTube = dynamic_cast<G4Tubs*>(WT_LV->GetSolid());
	  }

	  if ( WaterTube ) {
	    WaterTube_radius = WaterTube->GetOuterRadius();
	    WaterTube_halfHeight = WaterTube->GetZHalfLength();
	  }
	  else  {
	    G4ExceptionDescription msg;
	    msg << "Water Tube volume of G4tubs shape not found.\n";
	    msg << "Perhaps you have changed geometry.\n";
	    msg << "The gun will be place at the center.";
	    G4Exception("WCDtankPrimaryGeneratorAction::GeneratePrimaries()",
	     "MyCode0002",JustWarning,msg);
	  }

	  // Verificando las dimensiones del mundo.

	  G4double WorldBox_halfSize = 0;

	  if (!WorldBox)
	  {
	    G4LogicalVolume* WB_LV
	      = G4LogicalVolumeStore::GetInstance()->GetVolume("WorldCube_logic");
	    if ( WB_LV ) WorldBox = dynamic_cast<G4Box*>(WB_LV->GetSolid());
	  }

	  if ( WorldBox ) {
	    WorldBox_halfSize = WorldBox->GetXHalfLength();
	  }
	  else  {
	    G4ExceptionDescription msg;
	    msg << "World volume of G4box shape not found.\n";
	    msg << "Perhaps you have changed geometry.\n";
	    msg << "The gun will be place at the center.";
	    G4Exception("WCDtankPrimaryGeneratorAction::GeneratePrimaries()",
	     "MyCode0003",JustWarning,msg);
	  }

	  /*
	   * Generacion de parametros aleatorios
	   * ===================================
	   *
	   * Se generan los siguientes numeros aleatorios uniformentente distribuidos
	   *
	   * cil_r 		: coordenada cilindrica r en el rango 0 <= r <= WaterTube_radius
	   * cil_z 		: coordenada cilindrica z en el rango -WaterTube_halfHeight <= z <= +WaterTube_halfHeight
	   * cil_phi	: coordenada cilindrica phi en el rango 0 <= phi <= 2pi
	   *
	   * Las coordenadas cilindricas son de un punto dentro del volumen de agua en el que pasara la particula.
		 * (Punto P)
	   *
	   */

	  G4double cil_r 		= ( G4UniformRand() * WaterTube_radius );
	  G4double cil_z 		= ( ( G4UniformRand() - 0.5 ) * WaterTube_halfHeight );
	  G4double cil_phi	= ( G4UniformRand() * twopi );

		/*
	   * Pistola de particulas
	   * =====================
	   *
	   * La pistola de particulas (ParticleGun) se coloca a una distancia constante a partir del 
		 * punto interno por donde pasa la particula (P). Se genera un vector e, que medido a partir
		 * de P inicara donde se coloca la pistola. Las componentes esfericas de e son:
		 * 
		 * e_r     : coordenada radial de e (magnitud). Esta es constante.
		 * e_phi   : coordenada azimutal de e. Generada aleatoriamente en el rango 0 <= phi <= 2pi
		 * e_theta : coordenada cenital de e. Este es el angulo que determina la direccion de
		 *           la particula incidente. Se distribull con cos^2.
	   *
		 */
		

	  G4double e_r      = 3.00*m;
		G4double e_phi	  = ( G4UniformRand() * twopi );
	  G4double e_theta	= CosSqrDistRand();

		/*
	   * Muones verticales
	   * =================
	   *
	   * Se consideran muones verticales a aquellos que entran por la tapa superior del cilindro de agua y salen por
	   * la inferior. La condicion que se debe verificar para esto es que ang_theta < min( theta_c0, theta_c1 ), donde
	   * theta_c0, theta_c1 son angulos criticos que dependen de las dimenciones del cilindro de agua, las coordenadas
	   * internas por donde pasa la particula (cil_r, cil_z, cil_phi) y de las coordenadas del punto de disparo e.
	   *
	   */

		G4double ang_beta = std::asin( (cil_r/WaterTube_radius)*std::sin(cil_phi-e_phi) );
		G4double wp = WaterTube_radius*std::cos(ang_beta) - cil_r*std::cos(cil_phi-e_phi);
		G4double wm = WaterTube_radius*std::cos(ang_beta) + cil_r*std::cos(cil_phi-e_phi);
	  G4double theta_c0 = std::atan( wp/(WaterTube_halfHeight - cil_z) );
	  G4double theta_c1 = std::atan( wm/(2*WaterTube_halfHeight - cil_z) );

	  G4double theta_crit = ( (theta_c0 < theta_c1 )? theta_c0 : theta_c1 );

	  switch( ParticleDirection )
	  {
	  	  case Vertical	:
					while ( e_theta > theta_crit )
									e_theta	= CosSqrDistRand();
					PrimaryParticleDirectionIsVertical = TRUE;
	  			break;
	  	  case NonVertical :
					while ( e_theta < theta_crit )
									e_theta	= CosSqrDistRand();
					PrimaryParticleDirectionIsVertical = FALSE;
					break;
	  	  case All :
					if ( e_theta < theta_crit )
						PrimaryParticleDirectionIsVertical = TRUE;
					else
						PrimaryParticleDirectionIsVertical = FALSE;
					break;
	  }

	  ParticleAzimuthAngle = e_theta;

		/*
		 * Coordenadas de la pistola de particulas
		 * =======================================
		 *  
		 * A partir de la suma vectorial de P y e, se determinan las cooedenadas cartesianas
		 * para la pistola:
		 * 
	   * ParticleGun_xCoordinate	: coordenada x de la pistola.
	   * ParticleGun_yCoordinate	: coordenada y de la pistola.
	   * ParticleGun_zCorrdinate	: coordenada z de la pistola.
	   *
	   * ParticleGun_xDirection		: componente x del vector unitario de direccion del lanzamiento.
	   * ParticleGun_yDirection		: componente y del vector unitario de direccion del lanzamiento.
	   * ParticleGun_zDirection		: componente z del vector unitario de direccion del lanzamiento.
	   *
	   */

	  G4double ParticleGun_xCoordinate = cil_r*std::sin(cil_phi) + e_r*std::sin(e_theta)*std::sin(e_phi);
	  G4double ParticleGun_yCoordinate = cil_r*std::cos(cil_phi) + e_r*std::sin(e_theta)*std::cos(e_phi);
	  G4double ParticleGun_zCoordinate = cil_z + e_r*std::cos(e_theta);
		
		if ( ( ParticleGun_xCoordinate > WorldBox_halfSize ) || ( ParticleGun_yCoordinate > WorldBox_halfSize ) || ( ParticleGun_zCoordinate > WorldBox_halfSize ) ) {
			G4ExceptionDescription msg;
			msg << "Particle can not be throw from outside the world!.\n";
			msg << "Perhaps you have changed geometry.";
			G4Exception("WCDtankPrimaryGeneratorAction::GeneratePrimaries()",
			"MyCode0003",JustWarning,msg);
	  }
	  
		G4double ParticleGun_xDirection = - std::sin(e_theta)*std::sin(e_phi);
	  G4double ParticleGun_yDirection = - std::sin(e_theta)*std::cos(e_phi);
	  G4double ParticleGun_zDirection = - std::cos(e_theta);

	  fParticleGun->SetParticlePosition( G4ThreeVector( ParticleGun_xCoordinate , ParticleGun_yCoordinate , ParticleGun_zCoordinate ) );
	  fParticleGun->SetParticleMomentumDirection( G4ThreeVector( ParticleGun_xDirection , ParticleGun_yDirection , ParticleGun_zDirection ) );

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
