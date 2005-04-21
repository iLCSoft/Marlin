#ifndef SimpleFastMCProcessor_h
#define SimpleFastMCProcessor_h 1

#include "marlin/Processor.h"
#include "marlin/IRecoParticleFactory.h"

#include "lcio.h"
#include <string>



using namespace lcio ;

namespace marlin{
 

  /** A simple smearing "Monte Carlo" that creates ReconstructedParticles from MCParticles according to
   *  the resolution that is specified for the particle type, one of:
   *  <ul>
   *  <li>photon</li>
   *  <li>charged: e+-,mu+-,pi+-,K+-,p+-,....</li>
   *  <li>neutral hadron: K0L, n, Lambda0,...</li>
   *  </ul>
   *  The resolutions for charged particles are given as delta_P/P for a certain polar angle 
   *  range (mapped to [0.,pi/2.]), e.g. <br>
   *  <b>ChargedResolution  &nbsp; .7e-5   &nbsp; 0.   &nbsp; 3.141593/2. </b><br>
   *  sets the resolution for all charged particles to 0.7*10^-5.<br> 
   *  Specifying different resolutions for different polar angle ranges allows to mimic degrading 
   *  detector performance in the very forward region. <br>
   *  The energy of the charged ReconstructedParticles is set from the "measured"
   *  momentum using the energy momentum relation for electrons and muons and assuming the pion mass
   *  for all other charged tracks.<br>
   *  The resolutions for neutral particles are given as A and B for a certain polar angle range,
   *  where dE/E = A "+" B / sqrt(E/GeV), e.g. <br>
   *  <b>NeutralHadronResolution   &nbsp; 0.03  &nbsp; .30  &nbsp;  0.  &nbsp;  3.141593/2. </b><br>
   *  sets the resolution for all neutral hadrons to 3% "+" 30% / sqrt( E /GeV ).
   *  The resolution for gammas is specified in <b>PhotonResolution</b><br> 
   *  No ReconstructedParticles are created if there is no resolution defined at that polar angle, e.g.<br>
   *  <b>PhotonResolution  &nbsp; .7e-5   &nbsp; 0.083   &nbsp; 3.141593/2. </b><br>
   *  effectively limits the acceptance region for photons to theta > 83mrad.<br>
   *  Output collections:
   *  <ul>
   *  <li><b>ReconstructedParticles</b>: the list of reconstrcuted particles</li>
   *  <li><b>MCTruthMapping</b>:  holds LCRelations  that map the  ReconstructedParticles to their
   *       proper MCParticles </li>
   *  <li><b>TrueJets</b>: a list of perfect parton (quark, gluon) jets <b>NOT YET !!!</b></li>
   *  </ul>
   *  
   *  A collection of LCRelations, called "MCTruthMapping" holds the relation between the 
   *  ReconstructedParticles and their proper MCParticles.
   */
  
  class SimpleFastMCProcessor : public Processor {
    
  public:
    
    /** Returns a new instance of the processor.*/
    virtual Processor*  newProcessor() { return new SimpleFastMCProcessor ; }
    
    
    SimpleFastMCProcessor() ;
    
        
    /** Initializes ...
     */
    virtual void init() ;
    
    /** Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;
    
    /** Updates all registered conditions handlers and adds the data to the event.
     */
    virtual void processEvent( LCEvent * evt ) ; 
    
    /** Creates some checkplots.
     */
    virtual void check( LCEvent * evt ) ; 
    
    /** Called after data processing for clean up.
     */
    virtual void end() ;
    
    
  protected:
    
    /**  Input collection name */
    std::string _inputCollectionName ;

    /** Momentum cut in GeV */
    float _momentumCut ;

    /** Resolutions of charged particles */
    FloatVec _initChargedRes ;

    /** Resolutions of photons */
    FloatVec _initPhotonRes ;

    /** Resolutions of photons */
    FloatVec _initNeutralHadronRes ;

    /** The particle factory */
    IRecoParticleFactory* _factory ;

    int _nRun ;
    int _nEvt ;
    
  } ;
  
} // end namespace 

#endif


