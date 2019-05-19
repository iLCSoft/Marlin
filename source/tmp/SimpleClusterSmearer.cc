#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP / MARLIN_AIDA

#ifdef MARLIN_CLHEP  // only if CLHEP is available !

#include "marlin/SimpleClusterSmearer.h"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandEngine.h"
#include <cmath>

// #include "CLHEP/Vector/ThreeVector.h"


namespace marlin{


  SimpleClusterSmearer::SimpleClusterSmearer(const std::vector<float>& resVec ):
  _resVec(0)
  {
    
    // copy the resolution vector parameters into a more structured vector 
    const unsigned int size = resVec.size() / ( sizeof( ClusterResolution)  / sizeof(float) );  // ==3
    _resVec.reserve(size);

    int index = 0 ;
    
    for( unsigned int i=0 ; i <  size ; i++ ){
      
      float A     =  resVec[ index++ ] ;
      float B     =  resVec[ index++ ] ;
      float thMin =  resVec[ index++ ] ;
      float thMax =  resVec[ index++ ] ;
      
      _resVec.push_back( ClusterResolution( A, B , thMin, thMax ) );
    }
  }
  
  
  HepLorentzVector SimpleClusterSmearer::smearedFourVector( const HepLorentzVector& v, int /*pdgCode*/ ){


    // find resolution for polar angle
    double theta = v.theta() ;  

    if( theta > M_PI_2 )  theta = M_PI - theta ; // need to transform to [0,pi/2] 
      
    std::pair<double,double> resolution = std::make_pair( -1., -1. ) ; 
    
    for( unsigned int i=0 ; i <  _resVec.size() ; i++ ){

      if( theta <= _resVec[i].ThMax  &&  theta > _resVec[i].ThMin ) {
	resolution.first =  _resVec[i].A ;
	resolution.second =  _resVec[i].B ;
	break ;
      }
    }
    HepLorentzVector sv( 0., 0. , 0., 0. ) ;
    
    if( resolution.first > - 1e-10  ) {

      // do the smearing ....

      double E = v.e() ;

      double Eres  = std::sqrt( resolution.first * resolution.first + 
				resolution.second * resolution.second / E  )  ;
      
      double deltaE = RandGauss::shoot( 0.0 , E*Eres ) ;
      

      // assume massless clusters ...

      Hep3Vector n3v( v.vect() )  ;
      
      n3v.setMag( E + deltaE ) ;
      
      double mass = 0. ;

      sv.setVectM(  n3v  , mass  ) ;

    } 
      
    return sv ;

  }
  
}

#endif // MARLIN_CLHEP

