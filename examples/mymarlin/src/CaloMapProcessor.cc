#include "CaloMapProcessor.h"

#include "lccd/LCConditionsMgr.hh"
#include <marlin/ConditionsProcessor.h>

#include <iostream>

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/ICloud1D.h>
//#include <AIDA/IHistogram1D.h>
#endif

#include "EVENT/LCCollection.h"
#include "EVENT/RawCalorimeterHit.h"



using namespace lcio ;
using namespace marlin ;


CaloMapProcessor aCaloMapProcessor ;


CaloMapProcessor::CaloMapProcessor() : Processor("CaloMapProcessor") {
  
  // modify processor description
  _description = "CaloMapProcessor tests channel to cell ID mapping" ;
  

  // register steering parameters: name, description, class-variable, default value

  registerProcessorParameter( "ConditionsName" , 
 			      "Name of the conditions collection"  ,
 			      _colName ,
 			      std::string("channelmap") ) ;
}


void CaloMapProcessor::init() { 

  // usually a good idea to
  printParameters() ;

  // create a conditions map with channel to cell id mapping 
  _chMap = new ChMap( &ChannelToCellID::getChannelID ) ;

  ConditionsProcessor::registerChangeListener( _chMap , _colName ) ;

//   lccd::LCConditionsMgr::instance()->registerChangeListener( _chMap , _colName ) ;
  


  _nRun = 0 ;
  _nEvt = 0 ;
  
}

void CaloMapProcessor::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;
} 

void CaloMapProcessor::processEvent( LCEvent * evt ) { 

  LCCollection*  calVec = evt->getCollection( "RawCaloHits" ) ;

  //  We could also access the channel mapping through the event collection
  //   LCCollection*  condVec =  evt->getCollection( _colName ) ;

  int nError  = 0 ;
  for(int i=0 ; i< calVec->getNumberOfElements() ; i++ ){
    
    RawCalorimeterHit* rh = dynamic_cast<RawCalorimeterHit*>( calVec->getElementAt(i) ) ;
    
    
    int channelID = rh->getCellID0() ; // for raw hits the cellID is a channel id realy

    int cellID = 0xffffffff ;

    try{  
      cellID = _chMap->find( channelID ).getCellID() ;  
    }
    catch(Exception& e ){
//       _chMap->print( std::cout ) ;
      std::cout << " Exception: " << e.what() << std::endl ;
    }
    
    // the following code would simply rely on the order of the conditions elements:
    // interestingly enough it is not realy faster than using the map !
    //  ChannelToCellID cid( condVec->getElementAt(i) ) ;
    //  cellID = cid.getCellID() ;
    

    // createmaps created cellid == cellnumber (order in collection)
    if( cellID != i ) 
      nError ++ ;

  }

  if( nError != 0 ) {
    std::cout  << "ERROR --- run: " << evt->getRunNumber() 
	       << " event: "        << evt->getEventNumber() 
	       << " :  false channel to cell id mappings: " 
	       << nError << std::endl ;
  }
  else 
    std::cout  << "OK    --- run: " << evt->getRunNumber() 
	       << " event: "        << evt->getEventNumber() 
	       << "  " << calVec->getNumberOfElements() 
	       << " channel to cell id mappings correct " 
	       << std::endl ;
      
  _nEvt ++ ;
}



void CaloMapProcessor::check( LCEvent * evt ) { 
}

void CaloMapProcessor::end(){ 
}

