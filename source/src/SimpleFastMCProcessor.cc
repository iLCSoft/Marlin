#include <marlin/MarlinConfig.h> // defines MARLIN_CLHEP / MARLIN_AIDA

// -- marlin headers
#include <marlin/SimpleFastMCProcessor.h>
#include <marlin/SimpleParticleFactory.h>
#include <marlin/SimpleTrackSmearer.h>
#include <marlin/SimpleClusterSmearer.h>
#include <marlin/FastMCParticleType.h>
#include <marlin/ErrorOfSigma.h>

// -- lcio headers 
#include <IMPL/LCCollectionVec.h>
#include <EVENT/ReconstructedParticle.h>
#include <UTIL/LCRelationNavigator.h>

// -- std headers
#include <iostream>
#include <cmath>

namespace marlin {
    
  SimpleFastMCProcessor aSimpleFastMCProcessor ;
  
  SimpleFastMCProcessor::SimpleFastMCProcessor() : 
    Processor("SimpleFastMCProcessor") {
    
    // modify processor description
    _description = "SimpleFastMCProcessor creates ReconstrcutedParticles from MCParticles " 
      "according to the resolution given in the steering file." ;
        
    registerInputCollection( LCIO::MCPARTICLE,
			     "InputCollectionName" , 
			     "Name of the MCParticle input collection"  ,
			     _inputCollectionName ,
			     std::string("MCParticle") ) ;
    
    registerOutputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			     "RecoParticleCollectionName" , 
			     "Name of the ReconstructedParticles output collection"  ,
			     _recoParticleCollectionName ,
			     std::string("ReconstructedParticles") ) ;

    registerOutputCollection( LCIO::LCRELATION,
			     "MCTruthMappingCollectionName" , 
			     "Name of the MCTruthMapping output collection"  ,
			     _mcTruthCollectionName ,
			     std::string("MCTruthMapping") ) ;

    registerProcessorParameter( "MomentumCut" , 
				"No reconstructed particles are produced for smaller momenta (in [GeV])"  ,
				_momentumCut ,
				float( 0.001 ) ) ;

    FloatVec chResDefault ;
    chResDefault.push_back( 5e-5 ) ;
    chResDefault.push_back( 0.00 ) ;
    chResDefault.push_back( 3.141593/2. ) ;
    
    registerProcessorParameter( "ChargedResolution" , 
				"Resolution of charged particles in polar angle range:  d(1/P)  th_min  th_max"  ,
				_initChargedRes ,
				chResDefault ,
				chResDefault.size() ) ;
    
    FloatVec gammaResDefault ;
    gammaResDefault.push_back( 0.01 ) ;
    gammaResDefault.push_back( 0.10 ) ;
    gammaResDefault.push_back( 0.00 ) ;
    gammaResDefault.push_back( 3.141593/2. ) ;
    
    registerProcessorParameter( "PhotonResolution" , 
				"Resolution dE/E=A+B/sqrt(E/GeV) of photons in polar angle range: A  B th_min  th_max"  ,
				_initPhotonRes ,
				gammaResDefault ,
				gammaResDefault.size() ) ;
    
    FloatVec hadronResDefault ;
    hadronResDefault.push_back( 0.04 ) ;
    hadronResDefault.push_back( 0.50 ) ;
    hadronResDefault.push_back( 0.00 ) ;
    hadronResDefault.push_back( 3.141593/2. ) ;
    
    registerProcessorParameter( "NeutralHadronResolution" , 
				"Resolution dE/E=A+B/sqrt(E/GeV) of neutral hadrons in polar angle range: A  B th_min  th_max"  ,
				_initNeutralHadronRes ,
				hadronResDefault ,
				hadronResDefault.size() ) ;
  }
  
  //--------------------------------------------------------------------------
  
  Processor *SimpleFastMCProcessor::newProcessor() { 
    return new SimpleFastMCProcessor() ; 
  }
  
  //--------------------------------------------------------------------------
  
  void SimpleFastMCProcessor::init() { 
    // usually a good idea to
    printParameters() ;

#ifdef MARLIN_CLHEP
    SimpleParticleFactory* simpleFactory  =  new SimpleParticleFactory() ;
    simpleFactory->registerIFourVectorSmearer(  new SimpleTrackSmearer( _initChargedRes ), CHARGED ) ;
    simpleFactory->registerIFourVectorSmearer(  new SimpleClusterSmearer( _initPhotonRes ), PHOTON ) ;
    simpleFactory->registerIFourVectorSmearer(  new SimpleClusterSmearer( _initNeutralHadronRes ), NEUTRAL_HADRON ) ;
    simpleFactory->setMomentumCut( _momentumCut ) ;
    _factory = simpleFactory ;    
    log<MESSAGE>() << " SimpleFastMCProcessor::init() : registering SimpleParticleFactory " << std::endl ;
#endif // MARLIN_CLHEP
  }
  
  //--------------------------------------------------------------------------

  void SimpleFastMCProcessor::processEvent( EVENT::LCEvent * evt ) { 
    
    const EVENT::LCCollection* mcpCol = evt->getCollection( _inputCollectionName ) ;
    IMPL::LCCollectionVec* recVec = new LCCollectionVec( EVENT::LCIO::RECONSTRUCTEDPARTICLE ) ;
    UTIL::LCRelationNavigator relNav( EVENT::LCIO::RECONSTRUCTEDPARTICLE , EVENT::LCIO::MCPARTICLE ) ;

    for(int i=0 ; i<mcpCol->getNumberOfElements() ; i++ ) {
      EVENT::MCParticle* mcp = dynamic_cast<EVENT::MCParticle*> ( mcpCol->getElementAt( i ) ) ;
      // stable particles only 
      if( mcp->getGeneratorStatus() == 1 ) {
	      EVENT::ReconstructedParticle *rec = nullptr ;
        if( _factory != nullptr ) {
          rec = _factory->createReconstructedParticle( mcp ) ;
        }
        if( rec != nullptr ) {
          recVec->addElement( rec ) ;
          relNav.addRelation( rec , mcp ) ;
        }
      }
    }
    recVec->setDefault( true ) ;
    evt->addCollection( recVec, _recoParticleCollectionName ) ;
    evt->addCollection( relNav.createLCCollection() , _mcTruthCollectionName ) ;
  }
    
}

