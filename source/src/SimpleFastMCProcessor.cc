#include "marlin/MarlinConfig.h" // defines MARLIN_CLHEP / MARLIN_AIDA

#include "marlin/SimpleFastMCProcessor.h"

#include "marlin/SimpleParticleFactory.h"
#include "marlin/SimpleTrackSmearer.h"
#include "marlin/SimpleClusterSmearer.h"
#include "marlin/FastMCParticleType.h"
#include "marlin/ErrorOfSigma.h"


//--- LCIO headers 
#include "IMPL/LCCollectionVec.h"   
#include "EVENT/ReconstructedParticle.h" 
#include "UTIL/LCRelationNavigator.h"


#ifdef MARLIN_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/ICloud2D.h>
#include <AIDA/IHistogram1D.h>
#include <AIDA/IProfile1D.h>
#include <AIDA/IDataPointSetFactory.h>
#include <AIDA/IDataPointSet.h>
#include <AIDA/IDataPoint.h>
#include <AIDA/IMeasurement.h>
#include <AIDA/IManagedObject.h>
#include <AIDA/ITree.h>
#include <AIDA/IAxis.h>
#endif // MARLIN_AIDA

#include <iostream>
#include <cmath>

using namespace lcio ;


namespace marlin{
  
  
  SimpleFastMCProcessor aSimpleFastMCProcessor ;
  
  
  SimpleFastMCProcessor::SimpleFastMCProcessor() : Processor("SimpleFastMCProcessor"),
    _factory(NULL),
    _nRun(-1),
    _nEvt(-1)
    {
    
    // modify processor description
    _description = "SimpleFastMCProcessor creates ReconstrcutedParticles from MCParticles " 
      "according to the resolution given in the steering file." ;
    
    
    // register steering parameters: name, description, class-variable, default value
    
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
  
  
  void SimpleFastMCProcessor::init() { 
    
    // usually a good idea to
    printParameters() ;
    
    _nRun = 0 ;
    _nEvt = 0 ;
    


    _factory = 0 ;

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


  void SimpleFastMCProcessor::processRunHeader( LCRunHeader* ) { 
    _nRun++ ;
  } 
  

  void SimpleFastMCProcessor::processEvent( LCEvent * evt ) { 
    
    const LCCollection* mcpCol = evt->getCollection( _inputCollectionName ) ;

    LCCollectionVec* recVec = new LCCollectionVec( LCIO::RECONSTRUCTEDPARTICLE ) ;

    LCRelationNavigator relNav( LCIO::RECONSTRUCTEDPARTICLE , LCIO::MCPARTICLE ) ;

    for(int i=0; i<mcpCol->getNumberOfElements() ; i++){
      
      MCParticle* mcp = dynamic_cast<MCParticle*> ( mcpCol->getElementAt( i ) ) ;
      
      //      if( mcp->getDaughters().size() == 0 ) { // stable particles only 

      if( mcp->getGeneratorStatus() == 1 ) { // stable particles only 

	ReconstructedParticle*  rec = 0 ;
	
	if( _factory != 0 ) 
	  rec = _factory->createReconstructedParticle( mcp ) ;
      
	if( rec != 0 ) {
	  recVec->addElement( rec ) ;
	  relNav.addRelation( rec , mcp ) ;
	}
      }

    }
    recVec->setDefault( true ) ;

    evt->addCollection( recVec, _recoParticleCollectionName ) ;
    evt->addCollection( relNav.createLCCollection() , _mcTruthCollectionName ) ;


    _nEvt ++ ;
  }
  


  void SimpleFastMCProcessor::check( LCEvent * evt ) { 
    
    
#ifdef MARLIN_AIDA
    
    // define a histogram pointer
    static AIDA::ICloud1D* hChargedRes ;    
    
    static AIDA::ICloud1D* hChargedEnergy ;    
    static AIDA::ICloud1D* hPhotonEnergy ;    
    static AIDA::ICloud1D* hHadronEnergy ;    
    //~ static AIDA::ICloud1D* hLostEnergy ;    
    
    static AIDA::IHistogram1D* hPhotonRes ;    
    static AIDA::IHistogram1D* hHadronRes ;    
    
    static AIDA::IProfile1D* hGamHelperP1 ;    
    static AIDA::IProfile1D* hHadHelperP1 ;    
    
    
    if( isFirstEvent() ) { 
      
      hChargedRes = AIDAProcessor::histogramFactory(this)->
	createCloud1D( "hChargedRes", "dP/P for charged tracks", 100 ) ; 
      
      
      hChargedEnergy = AIDAProcessor::histogramFactory(this)->
       	createCloud1D( "hChargedEnergy", "E/GeV for charged particles", 100 ) ; 

      hPhotonEnergy = AIDAProcessor::histogramFactory(this)->
       	createCloud1D( "hPhotonEnergy", "E/GeV for photons", 100 ) ; 
      
      hHadronEnergy = AIDAProcessor::histogramFactory(this)->
       	createCloud1D( "hHadronEnergy", "E/GeV for neutral hadrons", 100 ) ; 

      //~ hLostEnergy = AIDAProcessor::histogramFactory(this)->
       	//~ createCloud1D( "hLostEnergy", "E/GeV for not reconstructed particles", 100 ) ; 
      
      
      hPhotonRes = AIDAProcessor::histogramFactory(this)->
	createHistogram1D( "hPhotonRes", "dE/E for photons", 100, -2.,2. ) ; 
      
      hHadronRes = AIDAProcessor::histogramFactory(this)->
	createHistogram1D( "hHadronRes", "dE/E  for neutral hadrons", 100, -2., 2. ) ;
      
      
      hGamHelperP1 = AIDAProcessor::histogramFactory(this)->
	 createProfile1D( "hGamHelperP1", " helper profile of energy spread", 10, 0.,20. ) ; 
      
      hHadHelperP1 = AIDAProcessor::histogramFactory(this)->
	createProfile1D( "hHadHelperP1", " helper profile of energy spread",  10, 0.,20. ) ; 
    }
    
    
    // fill histogram from LCIO data :
    
    LCCollection* recCol = evt->getCollection(_recoParticleCollectionName ) ;
    
    LCCollection* relCol = evt->getCollection( _mcTruthCollectionName ) ;
    
    if( relCol == 0 ) {
      log<WARNING>() << "SimpleFastMCProcessor::check no collection found: MCTruthMapping" << std::endl ;
      return ;
    }
    
    LCRelationNavigator relNav( relCol ) ;
    
    if( recCol != 0 ){
      
      int nRec = recCol->getNumberOfElements()  ;
      
      for(int i=0; i< nRec ; i++){
	
	ReconstructedParticle* rec = dynamic_cast<ReconstructedParticle*>( recCol->getElementAt( i ) ) ;
	
	const LCObjectVec&  mcps = relNav.getRelatedToObjects( rec ) ;
	
	MCParticle* mcp = dynamic_cast<MCParticle*>( mcps[0] ) ; // we have a 1-1 relation here 
	
	
	if( rec->getType() == CHARGED) { 
	  
	  double recP = std::sqrt( rec->getMomentum()[0] *  rec->getMomentum()[0] +
				   rec->getMomentum()[1] *  rec->getMomentum()[1] +
				   rec->getMomentum()[2] *  rec->getMomentum()[2] ) ;
	  
	  double mcpP = std::sqrt( mcp->getMomentum()[0] *  mcp->getMomentum()[0] +
				   mcp->getMomentum()[1] *  mcp->getMomentum()[1] +
				   mcp->getMomentum()[2] *  mcp->getMomentum()[2] ) ;
	  
	  hChargedRes->fill( ( recP - mcpP ) / recP    ) ;

	  hChargedEnergy->fill( rec->getEnergy() ) ; 

	} else {  // not charged
	  
	  double rE =  rec->getEnergy() ;
	  double mE =  mcp->getEnergy() ;

 	  double dEoverE = ( rE - mE ) /  mE  ;
	  
	  if( rec->getType() == PHOTON) { 
	    
	    hPhotonEnergy->fill( mE ) ;
	    hGamHelperP1->fill(  mE  ,   dEoverE  ) ;
	    hPhotonRes->fill(  dEoverE , mE    ) ;
	    
	  }
	  else if( rec->getType() == NEUTRAL_HADRON ) { 
	    
	    // 	    if( mE > 0.9 && mE < 1.1 ){ 

	    hHadronRes->fill(  dEoverE , mE   ) ;
	    hHadronEnergy->fill( mE ) ;
	    hHadHelperP1->fill(  mE , dEoverE  ) ;
	    
	      // 	    }


	  }	  

	} // not charged

      } // for nReco

    } // recoCol != 0  

#endif // MARLIN_AIDA
      

  }
  
  
  void SimpleFastMCProcessor::end(){ 
    
    log<MESSAGE4>()  << "SimpleFastMCProcessor::end()  " << name() 
			       << " processed " << _nEvt << " events in " << _nRun << " runs "
			       << std::endl ;
    
#ifdef MARLIN_AIDA_IGNORE_FOR_NOW
    // FIXME:
    // RAIDA does not support DataPoint sets and AIDA in generall does not allow to create a histogramm 
    // with 'faked' entries from another source (i.e. bin mean, bin error, ...)

    // create data sets for the energy resolution in the calorimeter
    
     AIDA::IProfile1D* hGamHelperP1 = 0 ;
     AIDA::IManagedObject* obj = AIDAProcessor::tree(this)->find("hGamHelperP1") ;
     if( obj != 0 )  hGamHelperP1 = static_cast<AIDA::IProfile1D*>( obj->cast("AIDA::IProfile1D") ) ;
    
     AIDA::IProfile1D* hHadHelperP1 = 0 ; 
     obj =  AIDAProcessor::tree(this)->find("hHadHelperP1");    
     if( obj != 0 )  hHadHelperP1 = static_cast<AIDA::IProfile1D*>( obj->cast("AIDA::IProfile1D") ) ;
    
    
    AIDA::IDataPointSet* dPhotonRes =  AIDAProcessor::dataPointSetFactory(this)->
      create("dPhotonRes","dE/E vs. 1./sqrt(E/GeV)",2 ) ;
    
    AIDA::IDataPointSet* dHadronRes =  AIDAProcessor::dataPointSetFactory(this)->
      create("dHadronRes","dE/E vs. 1./sqrt(E/GeV)",2 ) ;
    
    if( hHadHelperP1 != 0 ){
      
      for ( int i = 0; i< hHadHelperP1->axis().bins() ; i++ ) {
	
 	dHadronRes->addPoint() ;
	
 	double sigma =  hHadHelperP1->binRms(i) ;
 	ErrorOfSigma eos( hHadHelperP1->binEntries(i)  ) ;
	
//  	std::cout << " hadrons - sigma : " << sigma  << std::endl ;
//  	std::cout << " hadrons - dSigma- : " <<  eos.lowerError( sigma )  << std::endl ;
//  	std::cout << " hadrons - dSigma- : " <<  eos.upperError( sigma )  << std::endl ;
//  	std::cout << " hadrons - entries : " <<  hHadHelperP1->binEntries(i) << std::endl ;
//  	std::cout << " hadrons - bin mean : " <<  hHadHelperP1->binMean(i) << std::endl ;
//  	std::cout << " hadrons - 1/sqrt(e)  : " <<   1. / std::sqrt(hHadHelperP1->binMean(i)) << std::endl ;
	

   	dHadronRes->point(i)->coordinate(0)->setValue( 1. / std::sqrt( hHadHelperP1->binMean(i) )  )  ;
	
  	dHadronRes->point(i)->coordinate(1)->setValue( sigma )  ;
	
  	// use symmetric errors - needed for fitting
  	double errorMean = ( eos.upperError( sigma ) + eos.lowerError( sigma ) ) / 2 ;
	
  	dHadronRes->point(i)->coordinate(1)->setErrorPlus(  errorMean  )  ;
  	dHadronRes->point(i)->coordinate(1)->setErrorMinus( errorMean    )  ;
 	//   	dHadronRes->point(i)->coordinate(1)->setErrorPlus( eos.upperError( sigma )   )  ;
 	//   	dHadronRes->point(i)->coordinate(1)->setErrorMinus( eos.lowerError( sigma )   )  ;
	
       }
	
    }

    if( hGamHelperP1 != 0 ){
      
      for ( int i = 0; i< hGamHelperP1->axis().bins() ; i++ ) {
	
	dPhotonRes->addPoint() ;
	
  	double sigma =  hGamHelperP1->binRms(i) ;
 	ErrorOfSigma eos( hGamHelperP1->binEntries(i)  ) ;
	
//  	std::cout << " SimpleFastMCProcessor::end(): sigma : " << sigma  << std::endl ;
//  	std::cout << " SimpleFastMCProcessor::end(): dSigma- : " <<  eos.lowerError( sigma )  << std::endl ;
//  	std::cout << " SimpleFastMCProcessor::end(): dSigma- : " <<  eos.upperError( sigma )  << std::endl ;
//  	std::cout << " SimpleFastMCProcessor::end(): entries : " <<  hGamHelperP1->binEntries(i) << std::endl ;
	
  	dPhotonRes->point(i)->coordinate(0)->setValue( 1. / std::sqrt( hGamHelperP1->binMean(i) )  )  ;
	
  	dPhotonRes->point(i)->coordinate(1)->setValue( sigma )  ;
	
	// use symmetric errors - needed for fitting
 	double errorMean = ( eos.upperError( sigma ) + eos.lowerError( sigma ) ) / 2 ;
	
   	dPhotonRes->point(i)->coordinate(1)->setErrorPlus(  errorMean  )  ;
   	dPhotonRes->point(i)->coordinate(1)->setErrorMinus( errorMean    )  ;
	//   	dPhotonRes->point(i)->coordinate(1)->setErrorPlus( eos.upperError( sigma )   )  ;
	//   	dPhotonRes->point(i)->coordinate(1)->setErrorMinus( eos.lowerError( sigma )   )  ;
	
      }
    }
#endif // MARLIN_AIDA_IGNORE_FOR_NOW

  }
}

