#include "MyProcessor.h"
#include <iostream>

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>


using namespace lcio ;
using namespace marlin ;


MyProcessor aMyProcessor ;


MyProcessor::MyProcessor() : Processor("MyProcessor") {
  
  // modify processor description
  _description = "MyProcessor does whatever it does ..." ;
  

  // register steering parameters: name, description, class-variable, default value

  registerProcessorParameter( "CollectionName" , 
			      "Name of the MCParticle collection"  ,
			      _colName ,
			      std::string("MCParticle") ) ;
}


void MyProcessor::init() { 

  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;
  
}

void MyProcessor::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;
} 

void MyProcessor::processEvent( LCEvent * evt ) { 

  static bool firstEvent = true ;
    
  // this gets called for every event 
  // usually the working horse ...

#ifdef MARLIN_USE_AIDA
  
  // define a histogram pointer
  static AIDA::ICloud1D* hMCPEnergy ;    
  
  if( firstEvent ) { 
    
    hMCPEnergy = 
      AIDAProcessor::histogramFactory(this)->
      createCloud1D( "hMCPEnergy", "energy of the MCParticles", 100 ) ; 
  }
  
  
  // fill histogram from LCIO data :

  LCCollection* col = evt->getCollection( _colName ) ;
  
  if( col != 0 ){
    
    int nMCP = col->getNumberOfElements()  ;
    
    for(int i=0; i< nMCP ; i++){
      
      MCParticle* p = dynamic_cast<MCParticle*>( col->getElementAt( i ) ) ;
      
      hMCPEnergy->fill( p->getEnergy() ) ;
      
    } 
  }
  
#endif


  firstEvent = false ;
  
  _nEvt ++ ;
}



void MyProcessor::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void MyProcessor::end(){ 
  
//   std::cout << "MyProcessor::end()  " << name() 
// 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
// 	    << std::endl ;

}

