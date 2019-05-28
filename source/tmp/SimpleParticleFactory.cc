#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP

#ifdef MARLIN_CLHEP  // only if CLHEP is available !

#include <cstdlib>

#include "marlin/SimpleParticleFactory.h"

#include "IMPL/ReconstructedParticleImpl.h"

// #if LCIO_VERSION_GE( 1,9) 
// // lcio v01-09 properly fills the charge
// #include "UTIL/LCStdHepRdr.h"
// #else
// #ifdef USE_SEPARATE_HEPPDT
// #include "HepPDT/ParticleID.hh"
// #else
// #include "CLHEP/HepPDT/ParticleID.hh"
// #endif
// #endif

#if ! LCIO_PATCHVERSION_GE( 1,51,3 )
    //#warning "have to #define USE_CLHEP to activate code from LCIO header <UTIL/LCFourVector.h>"
    #define USE_CLHEP // to activate code from LCIO header <UTIL/LCFourVector.h>
#endif

#include "UTIL/LCFourVector.h"

using namespace lcio ;


namespace marlin{
  
  SimpleParticleFactory::SimpleParticleFactory() :
    _smearingVec(NUMBER_OF_FASTMCPARTICLETYPES, NULL), _momentumCut( 0.0000000001 )   {
  }
  

  void SimpleParticleFactory::setMomentumCut( double mCut ) {
    _momentumCut = mCut ;
  }

  void SimpleParticleFactory::registerIFourVectorSmearer( IFourVectorSmearer* sm , 
							  FastMCParticleType type ) {
    _smearingVec[ type ] = sm ;
    
  }
  
  
  FastMCParticleType SimpleParticleFactory::getParticleType( const lcio::MCParticle* mcp ) {


    // assumes that mcp is a stable particle !

    FastMCParticleType type( UNKNOWN )  ;
    
    //     if( mcp->getCharge() > 0.001 ||  mcp->getCharge() < -.001 ){  // mcp->getCharge() != 0.00 
    //     NOTE: the charge is currently (LCIO v01-04) not filled when reading StdHep

// #if LCIO_VERSION_GE( 1,9) 
     float charge =  mcp->getCharge()  ;
// #else
//      float charge = getCharge( mcp->getPDG() ) ;
// #endif

    if( charge > 1e-10 || charge < -1e-10  ){  

      type = CHARGED ;
      
    } else if(  mcp->getPDG() == 22 )  { // photon
      
      type = PHOTON ;
      
    } else if(  std::abs( mcp->getPDG() ) == 12 || std::abs( mcp->getPDG() ) == 14 ||
		std::abs( mcp->getPDG() ) == 16 || std::abs( mcp->getPDG() ) == 18 )  { // neutrinos - 18 is tau-prime

      type = NEUTRINO ;


    } else {  // treat everything else neutral hadron  
      
      type = NEUTRAL_HADRON ;
    }
    
    return type ;
  }
  
//   float SimpleParticleFactory::getCharge( int pdgCode ) {
// #if LCIO_VERSION_GE( 1,9) 
//     static LCStdHepRdr rdr("")  ;  // threeCharge should be a static method really ...
//     return rdr.threeCharge( pdgCode ) / 3.00 ;
// #else
//     return  double( HepPDT::ParticleID( pdgCode ).threeCharge() / 3.00 )   ;
// #endif
//   }



  ReconstructedParticle* SimpleParticleFactory::createReconstructedParticle( const MCParticle* mcp ) {
    
    // this is where we do the fast Monte Carlo ....
    

#ifdef LCIO_PATCHVERSION_GE  // has been defined in 1.4.1 which has a bug fix in  LCFourVector<T>
    LCFourVector<MCParticle>  mc4V( mcp ) ;
#else
    HepLorentzVector mc4V( mcp->getMomentum()[0], mcp->getMomentum()[1],
			   mcp->getMomentum()[2], mcp->getEnergy() )  ;
#endif


    
    FastMCParticleType type = getParticleType(mcp ) ;
    
  
    IFourVectorSmearer* sm = _smearingVec[ type ] ;

    HepLorentzVector reco4v(0.,0.,0.,0.)  ;
    
    if( sm == 0 ){
      // if we don't have a smearer registered we don't reconstruct the particle, e.g for neutrinos
      
      return 0 ;
    }

    reco4v = sm->smearedFourVector( mc4V , mcp->getPDG() ) ;
   
    
    ReconstructedParticleImpl* rec = 0 ;
    
    if(  reco4v.vect().mag() >  _momentumCut  ) {  
      
      rec = new ReconstructedParticleImpl ;
      
      float p[3] ;
      p[0] = reco4v.px() ;
      p[1] = reco4v.py() ;
      p[2] = reco4v.pz() ;
      
      rec->setMomentum( p ) ;
      rec->setEnergy( reco4v.e() ) ;
      rec->setMass( reco4v.m() ) ;

      rec->setCharge( mcp->getCharge() ) ;
    
      float vtx[3] ;
      vtx[0] = mcp->getVertex()[0] ;
      vtx[1] = mcp->getVertex()[1] ;
      vtx[2] = mcp->getVertex()[2] ;
      rec->setReferencePoint( vtx ) ;

      rec->setType( type ) ;
    }
    
    return  rec ;
  }


} // namespace

#endif // MARLIN_CLHEP

