#ifndef FastMCParticleType_h
#define FastMCParticleType_h 1


#define NUMBER_OF_FASTMCPARTICLETYPES 5  // increase when adding a new type !!

namespace marlin{

  /** Enumeration that defines integer constants for various particle
   *  types used in the fast Monte Carlo.
   *
   *  @author F. Gaede, DESY
   *  @version $Id: FastMCParticleType.h,v 1.2 2005-10-11 12:56:28 gaede Exp $ 
   */ 
  enum FastMCParticleType {

    UNKNOWN = 0 ,
    CHARGED = 1 , 
    PHOTON = 2 ,
    NEUTRAL_HADRON = 3 ,
    NEUTRINO = 4
  };  

} // end namespace 
#endif



