#ifndef SimpleParticleFactory_h
#define SimpleParticleFactory_h 1

#include "marlin/MarlinConfig.h"

#ifdef MARLIN_CLHEP  // only if CLHEP is available !

#include "marlin/IRecoParticleFactory.h"
#include "marlin/IFourVectorSmearer.h"
#include "marlin/FastMCParticleType.h"


namespace marlin {

/** Implementation of IRecoParticleFactory that implements the default behaviour 
 *  as described in SimpleFastMCProcessor, i.e. have polar angle ranges with different resolutions
 *  for charged tracks, photons and neutral hadrons.
 *
 *  @author F. Gaede, DESY
 *  @version $Id: SimpleParticleFactory.h,v 1.3 2007-11-23 20:09:12 gaede Exp $ 
 */ 

  class SimpleParticleFactory : public IRecoParticleFactory {
    
  public:
    
    SimpleParticleFactory() ;

    
    /** Virtual d'tor.*/
    virtual ~SimpleParticleFactory() {} 
    

    /** The actual factory method that creates a new ReconstructedParticle
     */ 
    virtual lcio::ReconstructedParticle* createReconstructedParticle( const lcio::MCParticle* mcp ) ;
    
    
    /** Register a particle four vector smearer for the given type.
     */
    virtual void registerIFourVectorSmearer( IFourVectorSmearer* sm , FastMCParticleType type ) ; 


    /** Returns the type of the MCParticle. 
     */
    virtual FastMCParticleType getParticleType( const lcio::MCParticle* mcp ) ;


//     /** Helper function to determine the charge from the PDG (charge is missing from stdhep files) 
//      */
//     virtual float getCharge( int pdgCode ) ;


    /** Set the momentum cut in GeV - no particles are produced for ower momenta. Default is 0.1 eV.
     */
    virtual void setMomentumCut( double mCut ) ;

  protected:
    
    std::vector<IFourVectorSmearer*> _smearingVec ;
    double _momentumCut ;

  };
  
}

#endif // MARLIN_CLHEP

#endif // SimpleParticleFactory_h
