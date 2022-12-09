#include "marlin/PatchCollections.h"
#include "marlin/Global.h"
//#include "marlin/StringParameters.h"

#include "UTIL/CheckCollections.h"

#include <iostream>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"


using namespace lcio ;
using namespace marlin ;


PatchCollections aPatchCollections ;


PatchCollections::PatchCollections() : Processor("PatchCollections") {
  
  // modify processor description
  _description = "PatchCollections adds missing collections in 'PatchCollections' to the event - or all missing in inut files (if PatchCollections not given)" ;
  
  std::vector<std::string> colsExample ;
  // colsExample.push_back( "BcalCollection" );
  // colsExample.push_back( "SimCalorimeterHit" );
  
  
  registerProcessorParameter( "PatchCollections" , 
			      "list of collections to patch  - pairs of CollectionName CollectionType"  ,
			      _colList ,
			      colsExample ) ;
  _nEvt = -1;
  _nRun = -1;  
}


void PatchCollections::init() { 
  
  // usually a good idea to
  printParameters() ;
  
  _nRun = 0 ;
  _nEvt = 0 ;
  
  _colCheck = new CheckCollections() ;

  
  unsigned nCols = _colList.size() ;
  
  
  if( nCols % 2 ){
    throw Exception("Parameter PatchCollections needs to have pairs of 'name type'") ;
  }


  if( nCols > 0 ) { 
    
    for( unsigned i=0 ; i < nCols ; i+=2 ) {
      _patchCols.insert( std::make_pair( _colList[i] , _colList[ i+1 ] ) ) ;
    }

  } else {

    streamlog_out( MESSAGE ) << " no patch collections defined in steering - "
			     << " will check all inputfiles for missing collections : "
			     << std::endl ;

    StringVec inFiles ;
    Global::parameters->getStringVals("LCIOInputFiles" , inFiles ) ;

    for(auto fn : inFiles ){
      streamlog_out( MESSAGE ) << "  " << fn << std::endl ;
    }

    streamlog_out( MESSAGE ) << "   ... reading .. "  ;

    _colCheck->checkFiles( inFiles ) ;

    _patchCols.merge( _colCheck->getMissingCollections() ) ;
    
    streamlog_out( MESSAGE ) << "   ... done " << std::endl  ;
  } 

  
  streamlog_out( MESSAGE ) << " will patch the following collections to all events where they are missing : " << std::endl ;
  
  for( auto c : _patchCols ){

    streamlog_out( MESSAGE ) << "   " <<   c.first << "   " << c.second << std ::endl ;
  }
  
  _colCheck->addPatchCollections( _patchCols ) ;


}

void PatchCollections::processRunHeader( LCRunHeader* ) { 
  
  _nRun++ ;
} 


// void PatchCollections::modifyEvent( LCEvent *evt ) {
//   processEvent( evt );
// }

void PatchCollections::processEvent( LCEvent * evt ) { 


  //-- note: this will not be printed if compiled w/o MARLINDEBUG=1 !

  streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
		       << "   in run:  " << evt->getRunNumber() 
		       << std::endl ;


  _colCheck->patchCollections( evt ) ;

  _nEvt ++ ;
}



void PatchCollections::check( LCEvent *  ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void PatchCollections::end(){ 

  delete _colCheck ;  
//   std::cout << "PatchCollections::end()  " << name() 
// 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
// 	    << std::endl ;

}

