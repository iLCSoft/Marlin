#ifndef IFourVectorSmearer_h
#define IFourVectorSmearer_h 1

#ifdef USE_CLHEP  // only if CLHEP is available !

#include "CLHEP/Vector/LorentzVector.h"


namespace marlin{


  /** Interface for smearing of four vectors - based on CLHEP::HepLorentzVector
   *
   *  @author F. Gaede, DESY
   *  @version $Id: IFourVectorSmearer.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
   */ 
  
  class IFourVectorSmearer {
    
  public:
    
    /** Virtual d'tor.*/
    virtual ~IFourVectorSmearer() {} 
    
    /** Smears the given four vector 
     */ 
    virtual HepLorentzVector smearedFourVector( const HepLorentzVector& v, int pdgCode ) = 0 ;
    
  } ;
  
  
} // end namespace 

#endif // ifdef CLHEP  
#endif



