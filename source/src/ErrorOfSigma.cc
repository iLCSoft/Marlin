#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP / MARLIN_AIDA

#include "marlin/ErrorOfSigma.h"

#include <iostream>
#include <cmath>

namespace marlin{
  
  ErrorOfSigma::ErrorOfSigma( unsigned n ) : 
    _n( n ) {
    
    if( n < 30 ) { // FIXME - implement proper errors for small n

      std::cout << "ErrorOfSigma::ErrorOfSigma: errors will be inaccurate for n = " << n
		<< " - should be > 30 " 
		<< std::endl ;
    }



  }
  
  double ErrorOfSigma::lowerError( double sigma ) {
    
    return ( 1. - std::sqrt( _n - 1.0 ) / std::sqrt( getChiSquaredPlus()  ) ) * sigma ;
    
  }
  
  double ErrorOfSigma::upperError( double sigma ) {

    return ( std::sqrt( _n - 1.0 ) / std::sqrt( getChiSquaredMinus() ) - 1.  ) * sigma ;

  }
  
  
  double ErrorOfSigma::getChiSquaredPlus() {

    // FIXME: only for n>30 ...
    double t = 2./ ( 9. * _n )  ;
    double chiPlus = _n  * std::pow( ( 1. - t + std::sqrt( t) ) , 3. ) ; 
    
//     std::cout << " getChiSquaredPlus( " << _n << ") :" << chiPlus << std::endl ;

#ifdef MARLIN_CLHEP  // only if CLHEP is available !
    // here we can use CLHEP::CumulativeChiSquared to compute
    // the exact chiPlus
#endif
    
    

    return chiPlus ;

  }
  
  double ErrorOfSigma::getChiSquaredMinus() {
    
    
    // FIXME: only for n>30 ...
    double t = 2./ ( 9. * _n )  ;
    double chiMinus = _n  * std::pow( ( 1. - t -  std::sqrt( t) ) , 3. ) ; 
    
//     std::cout << " getChiSquaredMinus( " << _n << ") :" << chiMinus << std::endl ;
    
#ifdef MARLIN_CLHEP  
    
    // here we can use CLHEP::CumulativeChiSquared to compute
    // the exact chiMinus
#endif
    
    return chiMinus ;
    
  }
  
}

