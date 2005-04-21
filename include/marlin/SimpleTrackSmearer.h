#ifndef SimpleTrackSmearer_h
#define SimpleTrackSmearer_h 1

#ifdef USE_CLHEP  // only if CLHEP is available !

#include "marlin/IFourVectorSmearer.h"
#include <vector>

#define ELECTRON_MASS 0.0005109989 
#define MUON_MASS     0.10565836 
#define PION_MASS     0.139570 

namespace marlin{

  /** Small helper class to store tracker  resolutions for a polar angle range
   */
  struct TrackResolution {

    TrackResolution() :
      DPP(0.) ,
      ThMin(0.) ,
      ThMax(0.) {}

    TrackResolution(float dPP, float thMin, float thMax) :
      DPP(dPP) ,
      ThMin(thMin) ,
      ThMax(thMax) {}
    
    float DPP ;
    float ThMin ;
    float ThMax ;

  } ;
  

  /** Smears the four vectors of charged tracks according to 
   *  dP/P = r for a given range of the polar angle.
   *  The resolutions r are given in the constructor as 
   *  a vector holding triplets of: 
   *  r0, th_min0, th_max0, r1, th_min1, th_max1, ...<br>
   *  Perfect electron and muon ID is assumed to set the
   *  particle energy ( mass ) - all other particles are
   *  assumed to be pions.
   */ 
  
  class SimpleTrackSmearer : public IFourVectorSmearer {
    
    typedef std::vector<TrackResolution> ResVec ;

  public:
    
    SimpleTrackSmearer(const std::vector<float>& resVec ) ;
    
    
    /** Virtual d'tor.*/
    virtual ~SimpleTrackSmearer() {} 
    
    /** Smears the given four vector according to the resolution for the 
     *  polar angle of the track. Returns a vector with all elements 0. if
     *  no resolution is defined.
     */ 
    virtual HepLorentzVector smearedFourVector( const HepLorentzVector& v, int pdgCode ) ;
    
  protected:

    ResVec _resVec ;

  } ;
  


} // end namespace 

#endif // ifdef CLHEP  
#endif



