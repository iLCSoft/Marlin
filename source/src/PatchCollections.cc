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
  _description = "add missing collections to the event - either parsed from inputfiles or as given in PatchCollections" ;
  

  registerProcessorParameter( "ParseFiles" ,
			      "if true, all inputfiles are parsed for missing collections on init",
			      _parseFiles,
			      false ) ;


  std::vector<std::string> colsExample ;
  colsExample.push_back( "BcalCollection" );
  colsExample.push_back( "SimCalorimeterHit" );
  
  
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
  
  _colCheck = std::make_unique<CheckCollections>() ;


  if( ! _parseFiles ) { // ------ patch only user defined collections

    unsigned nCols = _colList.size() ;

    if( nCols % 2 ){
      throw Exception("Parameter PatchCollections needs to have pairs of 'name type'") ;
    }
    for( unsigned i=0 ; i < nCols ; i+=2 ) {
      _patchCols.push_back( { _colList[i] , _colList[ i+1 ] } ) ;
    }

  } else { // ---------------- parse all input files to get set of missing collections

    streamlog_out( MESSAGE ) << " ------ " << std::endl
			     << "    will check all inputfiles for missing collections : "
			     << std::endl ;

    StringVec inFiles ;
    Global::parameters->getStringVals("LCIOInputFiles" , inFiles ) ;

    for(auto fn : inFiles ){
      streamlog_out( MESSAGE ) << "  " << fn << std::endl ;
    }

    streamlog_out( MESSAGE ) << "   parsing files ... "  ;

    _colCheck->checkFiles( inFiles ) ;

    auto mcols =  _colCheck->getMissingCollections() ;
    std::copy( mcols.begin() , mcols.end() , std::back_inserter( _patchCols ) ) ;
    
    streamlog_out( MESSAGE ) << "   ... done !" << std::endl  ;
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

//   std::cout << "PatchCollections::end()  " << name() 
// 	    << " processed " << _nEvt << " events in " << _nRun << " runs "
// 	    << std::endl ;

}

