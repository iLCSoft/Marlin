#ifndef SimpleClusterSmearer_h
#define SimpleClusterSmearer_h 1

#include "marlin/MarlinConfig.h"

#ifdef MARLIN_CLHEP  // only if CLHEP is available !

#include "marlin/IFourVectorSmearer.h"
#include <vector>

namespace marlin{

  /** Small helper class to store calorimeter resolutions for a polar angle range
   *
   *  @author F. Gaede, DESY
   *  @version $Id: SimpleClusterSmearer.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
   */
  struct ClusterResolution {
    
    ClusterResolution() :
      A(0.) ,
      B(0.) ,
      ThMin(0.) ,
      ThMax(0.) {}
    
    ClusterResolution(float a, float b, float thMin, float thMax) :
      A(a) ,
      B(b) ,
      ThMin(thMin) ,
      ThMax(thMax) {}
    
    float A ;
    float B ;
    float ThMin ;
    float ThMax ;
    
  } ;

  /** Smears the four vectors of (neutral) clusters according to 
   *  dE/E = A "+" B / sqrt( E/GeV ) for a given range of the polar angle.
   *  The resolution parameters A and B are given in the constructor as 
   *  a vector holding quadruples of: 
   *  A0, B0, th_min0, th_max0, A0, B0, th_min1, th_max1, ...<br>
   *  Momenta are assigned assumig massless particles.
   */ 
  
  class SimpleClusterSmearer : public IFourVectorSmearer {
    
  
    typedef std::vector<ClusterResolution> ResVec ;

  public:
    
    SimpleClusterSmearer(const std::vector<float>& resVec ) ;
    
    
    /** Virtual d'tor.*/
    virtual ~SimpleClusterSmearer() {} 
    
    /** Smears the given four vector according to the resolution for the 
     *  polar angle of the cluster. Returns a vector with all elements 0. if
     *  no resolution is defined.
     */ 
    virtual HepLorentzVector smearedFourVector( const HepLorentzVector& v, int pdgCode ) ;
    
  protected:

    ResVec _resVec ;

  } ;
  


} // end namespace 

#endif // MARLIN_CLHEP  
#endif // SimpleClusterSmearer_h

