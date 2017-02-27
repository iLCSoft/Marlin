#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP / MARLIN_AIDA

#ifdef MARLIN_CLHEP  // only if CLHEP is available !

#include "marlin/SimpleTrackSmearer.h"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandEngine.h"


#include <cmath>
#include <cstdlib>

namespace CLHEP{} 
using namespace CLHEP ;

// #include "CLHEP/Vector/ThreeVector.h"


namespace marlin{


  SimpleTrackSmearer::SimpleTrackSmearer(const std::vector<float>& resVec ):
    _resVec(0)
  {

    const unsigned int size = resVec.size() / ( sizeof(TrackResolution)  / sizeof(float) );  // ==3
    _resVec.reserve(size);
    // copy the resolution vector parameters into a more structured vector 
    int index = 0 ;
    
    for( unsigned int i=0 ; i < size ; i++ ){
      
      float dPP   =  resVec[ index++ ] ;
      float thMin =  resVec[ index++ ] ;
      float thMax =  resVec[ index++ ] ;
      
      _resVec.push_back( TrackResolution( dPP, thMin, thMax ) );
    }
  }
  
  
  HepLorentzVector SimpleTrackSmearer::smearedFourVector( const HepLorentzVector& v, int pdgCode ){


    // find resolution for polar angle
    double theta = v.theta() ;  

    if( theta > M_PI_2 )  theta = M_PI - theta ; // need to transform to [0,pi/2] 
      
    double resolution = -1. ; 

    for( unsigned int i=0 ; i <  _resVec.size() ; i++ ){

      if( theta <= _resVec[i].ThMax  &&  theta > _resVec[i].ThMin ) {
	resolution =  _resVec[i].DPP ;
	break ;
      }
    }
    HepLorentzVector sv( 0., 0. , 0., 0. ) ;
    
    if( resolution > - 1e-10  ) {

      // do the smearing ....

      double P = v.vect().mag() ;

      double deltaP = RandGauss::shoot( 0.0 , P*P*resolution ) ;
      
      Hep3Vector n3v( v.vect() )  ;
      
      n3v.setMag( P + deltaP  ) ;
      
//       std::cout << " SimpleTrackSmearer::smearedFourVector P0: " 
// 		<< P << " - P1 : " << P + deltaP 
// 		<< " resolution: " << resolution 
// 		<< std::endl ;


      // assume perfect electron and muon ID and
      // assign pion mass to everything else

      double mass = PION_MASS ; 

      if( std::abs( pdgCode ) == 12  )  // electron

	mass = ELECTRON_MASS ;

      else if( std::abs( pdgCode ) == 13  )  // muon

	mass = MUON_MASS ;
      
      sv.setVectM(  n3v  , mass  ) ;

    } 
      
    return sv ;

  }
  
}

#endif // MARLIN_CLHEP
